#include "entity.h"

#include <base/log.h>
#include <base/math.h>
#include <base/str.h>
#include <base/system.h>
#include <base/thread.h>
#include <base/vmath.h>

#include <engine/client.h>
#include <engine/client/client.h>
#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>
#include <engine/shared/protocol.h>
#include <engine/textrender.h>

#include <generated/protocol.h>

#include <game/client/components/binds.h>
#include <game/client/components/chat.h>
#include <game/client/components/controls.h>
#include <game/client/gameclient.h>
#include <game/gamecore.h>
#include <game/teamscore.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#if defined(CONF_FAMILY_WINDOWS)
#include <Windows.h>
#endif

#if defined(CONF_FAMILY_WINDOWS)
#include <TlHelp32.h>
#include <processthreadsapi.h>

static bool IsDiscordProcessName(const wchar_t *pProcessName)
{
	return _wcsicmp(pProcessName, L"Discord.exe") == 0 ||
	       _wcsicmp(pProcessName, L"DiscordCanary.exe") == 0 ||
	       _wcsicmp(pProcessName, L"DiscordPTB.exe") == 0 ||
	       _wcsicmp(pProcessName, L"DiscordSystemHelper.exe") == 0;
}
#endif

void CEClient::OnChatMessage(int ClientId, int Team, const char *pMsg)
{
	if(ClientId < 0 || ClientId > MAX_CLIENTS)
		return;

	bool Highlighted = GameClient()->m_Chat.LineHighlighted(ClientId, pMsg);

	if(Team == TEAM_WHISPER_RECV)
		Highlighted = true;

	if(!Highlighted)
		return;
	char aName[16];
	str_copy(aName, GameClient()->m_aClients[ClientId].m_aName, sizeof(aName));

	if(!str_comp(aName, GameClient()->m_aClients[GameClient()->m_aLocalIds[0]].m_aName))
		return;
	if(Client()->DummyConnected() && !str_comp(aName, GameClient()->m_aClients[GameClient()->m_aLocalIds[1]].m_aName))
		return;

	bool HiddenMessage = GameClient()->m_WarList.m_WarPlayers[ClientId].m_IsMuted || (g_Config.m_ClHideEnemyChat && (GameClient()->m_WarList.GetWarData(ClientId).m_WarGroupMatches[1]));

	if(!HiddenMessage)
	{
		str_copy(m_aLastPing.m_aName, aName);
		str_copy(m_aLastPing.m_aMessage, pMsg);
		m_aLastPing.m_Team = Team;
	}

	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;

	if(ClientId != m_LastReplyId)
	{
		char Reply[MAX_LINE_LENGTH];
		if(g_Config.m_ClReplyMuted && GameClient()->m_WarList.m_WarPlayers[ClientId].m_IsMuted)
		{
			str_format(Reply, sizeof(Reply), "%s: %s", aName, g_Config.m_ClAutoReplyMutedMsg);

			if(Team == TEAM_WHISPER_RECV) // whisper recv
				str_format(Reply, sizeof(Reply), "/w %s %s", aName, g_Config.m_ClAutoReplyMutedMsg);

			GameClient()->m_Chat.SendChat(TEAM_FLOCK, Reply);
			m_LastReplyId = ClientId;
		}
		else if(g_Config.m_ClTabbedOutMsg)
		{
			IEngineGraphics *pGraphics = ((IEngineGraphics *)Kernel()->RequestInterface<IEngineGraphics>());
			if(pGraphics && !pGraphics->WindowActive() && Graphics())
			{
				if(Team == TEAM_WHISPER_RECV) // whisper recv
				{
					str_format(Reply, sizeof(Reply), "/w %s ", aName);
					str_append(Reply, g_Config.m_ClAutoReplyMsg);
					GameClient()->m_Chat.SendChat(TEAM_FLOCK, Reply);
				}
				else
				{
					str_format(Reply, sizeof(Reply), "%s: ", aName);
					str_append(Reply, g_Config.m_ClAutoReplyMsg);
					GameClient()->m_Chat.SendChat(TEAM_FLOCK, Reply);
				}
				m_LastReplyId = ClientId;
			}
		}
	}
}

void CEClient::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		OnChatMessage(pMsg->m_ClientId, pMsg->m_Team, pMsg->m_pMessage);
	}
}

void CEClient::AutoJoinTeam()
{
	if(m_JoinTeam > time_get())
		return;

	if(!g_Config.m_ClAutoJoinTest)
		return;

	if(GameClient()->m_Chat.IsActive())
		return;

	if(GameClient()->CurrentRaceTime())
		return;

	int Local = GameClient()->m_Snap.m_LocalClientId;

	for(int ClientId = 0; ClientId < MAX_CLIENTS; ClientId++)
	{
		if(GameClient()->m_Teams.Team(ClientId))
		{
			if(str_comp(GameClient()->m_aClients[ClientId].m_aName, g_Config.m_ClAutoJoinTeamName) == 0)
			{
				int LocalTeam = -1;

				if(ClientId == Local)
					return;

				int Team = GameClient()->m_Teams.Team(ClientId);
				char TeamChar[8];
				str_format(TeamChar, sizeof(TeamChar), "%d", Team);

				int PrevTeam = -1;

				if(!GameClient()->m_Teams.SameTeam(Local, ClientId) && (Team > 0) && !m_JoinedTeam)
				{
					char aBuf[48] = "/team ";
					str_append(aBuf, TeamChar);
					GameClient()->m_Chat.SendChat(0, aBuf);

					char Joined[48] = "attempting to auto Join ";
					str_append(Joined, GameClient()->m_aClients[ClientId].m_aName);
					GameClient()->ClientMessage(Joined);

					m_JoinedTeam = true;
					m_AttemptedJoinTeam = true;
				}
				if(GameClient()->m_Teams.SameTeam(Local, ClientId) && m_JoinedTeam)
				{
					char Joined[48] = "Successfully Joined The Team of ";
					str_append(Joined, GameClient()->m_aClients[ClientId].m_aName);
					GameClient()->ClientMessage(Joined);

					LocalTeam = GameClient()->m_Teams.Team(Local);

					PrevTeam = Team;

					m_JoinedTeam = false;
				}
				if(!GameClient()->m_Teams.SameTeam(Local, ClientId) && m_AttemptedJoinTeam)
				{
					char Joined[48] = "Couldn't Join The Team of ";
					str_append(Joined, GameClient()->m_aClients[ClientId].m_aName);
					GameClient()->ClientMessage(Joined);

					m_AttemptedJoinTeam = false;
				}
				if(PrevTeam != Team && m_AttemptedJoinTeam)
				{
					GameClient()->ClientMessage("team has changed");
					m_JoinedTeam = false;
				}
				if(LocalTeam > 0)
				{
					GameClient()->ClientMessage("self team is bigger than 0");
					m_JoinedTeam = false;
					LocalTeam = GameClient()->m_Teams.Team(Local);
				}
				if(LocalTeam != Team)
				{
					m_AttemptedJoinTeam = false;
				}
				return;
			}
		}
		m_JoinTeam = time_get() + time_freq() * 0.25;
	}
}

void CEClient::GoresMode()
{
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;

	CCharacterCore Core = GameClient()->m_PredictedPrevChar;
	bool HasExtraWeapons = (Core.m_aWeapons[WEAPON_GRENADE].m_Got || Core.m_aWeapons[WEAPON_LASER].m_Got || Core.m_aWeapons[WEAPON_SHOTGUN].m_Got);

	if(g_Config.m_ClGoresModeDisableIfWeapons)
	{
		// Check on newly acquired weapons: if player just got a heavy weapon, execute a one-time disable
		if(HasExtraWeapons && !m_HadExtraWeapons)
		{
			if(g_Config.m_ClGoresMode)
			{
				g_Config.m_ClGoresMode = 0;
				GameClient()->ClientMessage("Gores Mode: Disabled (acquired heavy weapon)");
			}
		}
	}
	m_HadExtraWeapons = HasExtraWeapons;
	m_WeaponsGot = false;

	if(!g_Config.m_ClGoresMode)
	{
		m_GoresState[0] = EGoresState::IDLE;
		m_GoresState[1] = EGoresState::IDLE;
		m_GoresPendingFire[0] = false;
		m_GoresPendingFire[1] = false;
		return;
	}

	const int Dummy = g_Config.m_ClDummy;
	int CurrentActiveWeapon = GameClient()->m_Snap.m_pLocalCharacter->m_Weapon;
	int &RawFire = GameClient()->m_Controls.m_aInputData[Dummy].m_Fire;
	int64_t Now = time_get();

	// Timeout safeguard (if transaction takes > 400ms, unlock and reset)
	if(m_GoresState[Dummy] != EGoresState::IDLE && (Now - m_GoresLockTime[Dummy]) > time_freq() * 0.4f)
	{
		m_GoresState[Dummy] = EGoresState::IDLE;
		m_GoresPendingFire[Dummy] = false;
	}

	switch(m_GoresState[Dummy])
	{
	case EGoresState::IDLE:
	{
		// Player triggers fire while not holding hammer
		if((RawFire & 1) && CurrentActiveWeapon != WEAPON_HAMMER)
		{
			m_GoresPrevWeapon[Dummy] = CurrentActiveWeapon;
			m_GoresState[Dummy] = EGoresState::SWITCH_TO_HAMMER;
			m_GoresLockTime[Dummy] = Now;
			m_GoresPendingFire[Dummy] = true;

			// Suppress immediate fire, send switch to hammer first
			if((RawFire & 1) != 0)
				RawFire++;
			RawFire &= INPUT_STATE_MASK;
			GameClient()->m_Controls.m_aInputData[Dummy].m_WantedWeapon = WEAPON_HAMMER + 1;
		}
		break;
	}
	case EGoresState::SWITCH_TO_HAMMER:
	{
		// Request hammer switch
		GameClient()->m_Controls.m_aInputData[Dummy].m_WantedWeapon = WEAPON_HAMMER + 1;

		// Check if switch to hammer completed
		if(CurrentActiveWeapon == WEAPON_HAMMER)
		{
			m_GoresState[Dummy] = EGoresState::HAMMER_FIRING;
			m_GoresLockTime[Dummy] = Now;

			// Trigger fire with hammer
			RawFire = (RawFire + 1) | 1;
			RawFire &= INPUT_STATE_MASK;
		}
		else
		{
			// Suppress fire while waiting for switch
			if((RawFire & 1) != 0)
				RawFire++;
			RawFire &= INPUT_STATE_MASK;
		}
		break;
	}
	case EGoresState::HAMMER_FIRING:
	{
		// Hammer has fired, now switch back to previous weapon
		GameClient()->m_Controls.m_aInputData[Dummy].m_WantedWeapon = m_GoresPrevWeapon[Dummy] + 1;

		// Release fire button
		if((RawFire & 1) != 0)
			RawFire++;
		RawFire &= INPUT_STATE_MASK;

		m_GoresState[Dummy] = EGoresState::RESTORE_WEAPON;
		m_GoresLockTime[Dummy] = Now;
		break;
	}
	case EGoresState::RESTORE_WEAPON:
	{
		// Continue requesting restore until weapon is restored or timeout
		GameClient()->m_Controls.m_aInputData[Dummy].m_WantedWeapon = m_GoresPrevWeapon[Dummy] + 1;

		if(CurrentActiveWeapon == m_GoresPrevWeapon[Dummy] || CurrentActiveWeapon != WEAPON_HAMMER)
		{
			m_GoresState[Dummy] = EGoresState::IDLE;
			m_GoresPendingFire[Dummy] = false;
		}
		break;
	}
	}
}

void CEClient::OnConnect(int Conn)
{
	if(Conn != IClient::CONN_MAIN)
		return;

	static bool s_SentInfoMessage = false;
	if(m_FirstLaunch && !s_SentInfoMessage)
	{
		GameClient()->ClientMessage("╭──                 客户端信息");
		GameClient()->ClientMessage("│ 这应该是你第一次运行使用客户端，欢迎使用!");
		GameClient()->ClientMessage("│");
		GameClient()->ClientMessage("│ 输入 \".help\" 查看默认聊天命令列表");
		GameClient()->ClientMessage("│");
		GameClient()->ClientMessage("│ 如果发现 bug 或有功能需求，请发送到:\".github\"");
		GameClient()->ClientMessage("│");
		GameClient()->ClientMessage("│ 以 \".\" 开头的聊天命令默认是静默的，意味着没人会看到");
		GameClient()->ClientMessage("│以 \"!\" 开头的聊天命令默认是公开的，这意味着所有人都会看到");
		GameClient()->ClientMessage("╰───────────────────────");
		s_SentInfoMessage = true;
	}
	else
	{
		// if current server is type "Gores", turn the config on, else turn it off
		if(g_Config.m_ClAutoEnableGoresMode)
		{
			if(str_find_nocase(Client()->ServerInfo().m_aGameType, "Gores"))
			{
				m_GoresServer = true;
				g_Config.m_ClGoresMode = 1;
			}
			else
			{
				m_GoresServer = false;
				g_Config.m_ClGoresMode = 0;
			}
		}
	}
}

void CEClient::NotifyOnMove()
{
	if(!g_Config.m_ClNotifyOnMove)
		return;
	if(Client()->State() != IClient::STATE_ONLINE)
		return;
	IEngineGraphics *pGraphics = ((IEngineGraphics *)Kernel()->RequestInterface<IEngineGraphics>());
	if(!pGraphics || !Graphics())
		return;
	const CNetObj_Character *pLocalChar = GameClient()->m_Snap.m_pLocalCharacter;
	if(!pLocalChar)
		return;
	if(GameClient()->m_Snap.m_SpecInfo.m_Active)
		return;

	int LocalId = GameClient()->m_Snap.m_LocalClientId;

	vec2 LocalPos = GameClient()->m_aClients[LocalId].m_RenderPos;
	if(!pGraphics->WindowActive())
	{
		const float MaxDist = 27.5f;

		bool Moved = false;
		for(int ClientId = 0; ClientId < MAX_CLIENTS; ClientId++)
		{
			if(m_LastPos == LocalPos)
				continue;
			const CGameClient::CClientData &ClientData = GameClient()->m_aClients[ClientId];
			if(ClientId == LocalId || !ClientData.m_Active)
				continue;
			if(ClientData.m_Solo)
				continue;
			if(!GameClient()->m_Teams.SameTeam(LocalId, ClientId))
				continue;
			if(!GameClient()->m_Snap.m_aCharacters[ClientId].m_HasExtendedData)
				continue;
			const CNetObj_Character *pOtherChar = &GameClient()->m_Snap.m_aCharacters[ClientId].m_Cur;
			vec2 OtherPos = GameClient()->m_aClients[ClientId].m_RenderPos;

			float Dist = distance(LocalPos, OtherPos);
			if(Dist < MaxDist + MaxDist)
				Moved = true;

			// check if the player is hooked to the local player
			if(GameClient()->m_aClients[ClientId].m_RenderCur.m_HookedPlayer == LocalId)
				Moved = true;

			// Check for hammer firing
			bool Hammering = (pOtherChar->m_Weapon == WEAPON_HAMMER) && (pOtherChar->m_AttackTick + 2 > Client()->GameTick(g_Config.m_ClDummy));
			Dist = distance(vec2(pOtherChar->m_X, pOtherChar->m_Y), vec2(pLocalChar->m_X, pLocalChar->m_Y));
			if(Hammering && Dist < 70.0f)
				Moved = true;
		}

		if(Moved)
		{
			Client()->Notify("E-Client", "current tile changed");
			Graphics()->NotifyWindow();
		}
	}
	m_LastPos = LocalPos;
}

void CEClient::UpdateVolleyball()
{
	bool IsVolleyBall = false;
	if(g_Config.m_EcVolleyBallBetterBall > 0 && g_Config.m_EcVolleyBallBetterBallSkin[0] != '\0')
	{
		if(g_Config.m_EcVolleyBallBetterBall > 1)
			IsVolleyBall = true;
		else
			IsVolleyBall = str_startswith_nocase(GameClient()->Map()->BaseName(), "volleyball");
	};
	for(auto &Client : GameClient()->m_aClients)
	{
		Client.m_IsVolleyBall = IsVolleyBall && Client.m_DeepFrozen;
	}
}

void CEClient::UpdateRainbow()
{
	static bool s_RainbowWasOn = false;

	if(g_Config.m_ClServerRainbow && !s_RainbowWasOn)
	{
		s_RainbowWasOn = true;
	}
	if(s_RainbowWasOn && !g_Config.m_ClServerRainbow)
	{
		GameClient()->SendInfo(false);
		GameClient()->SendDummyInfo(false);
		s_RainbowWasOn = false;
	}
	// Makes the slider look smoother
	static float s_Speed = 1.0f;
	s_Speed = s_Speed + m_RainbowSpeed * Client()->FrameTimeAverage() * 0.1f;

	if(s_Speed > 255.f * 10) // Reset if Value gets highish, why? why not :D
		s_Speed = 1.0f;
	float h = round_to_int(s_Speed) % 255 / 255.f;
	float s = abs(m_RainbowSat[g_Config.m_ClDummy] - 255);
	float l = abs(m_RainbowLht[g_Config.m_ClDummy] - 255);

	m_PreviewRainbowColor[g_Config.m_ClDummy] = getIntFromColor(h, s, l);

	if(Client()->State() == IClient::STATE_ONLINE)
	{
		const int64_t Now = time_get();
		if(g_Config.m_ClServerRainbow && m_RainbowDelay < Now && m_LastMovement + time_freq() * 30 > Now && !GameClient()->m_aClients[GameClient()->m_Snap.m_LocalClientId].m_Afk)
		{
			if(m_RainbowBody[g_Config.m_ClDummy] || m_RainbowFeet[g_Config.m_ClDummy])
			{
				if(m_BothPlayers)
				{
					GameClient()->SendDummyInfo(false);
					GameClient()->SendInfo(false);
				}
				else if(g_Config.m_ClDummy)
					GameClient()->SendDummyInfo(false);
				else
					GameClient()->SendInfo(false);
			}
			m_RainbowDelay = time_get() + time_freq() * g_Config.m_SvInfoChangeDelay;
			m_RainbowColor[0] = m_RainbowColor[1] = getIntFromColor(h, s, l);
		}
	}
}

void CEClient::OnShutdown()
{
	// str_copy(g_Config.m_ClDummySkin, g_Config.m_ClSavedDummySkin, sizeof(g_Config.m_ClDummySkin));
	// str_copy(g_Config.m_ClDummyName, g_Config.m_ClSavedDummyName, sizeof(g_Config.m_ClDummyName));
	// str_copy(g_Config.m_ClDummyClan, g_Config.m_ClSavedDummyClan, sizeof(g_Config.m_ClDummyClan));
	// g_Config.m_ClDummyCountry = g_Config.m_ClSavedDummyCountry;
	// g_Config.m_ClDummyColorFeet = g_Config.m_ClSavedDummyColorFeet;
	if(g_Config.m_ClServerRainbow)
	{
		g_Config.m_ClDummyUseCustomColor = g_Config.m_ClSavedDummyUseCustomColor;
		g_Config.m_ClDummyColorBody = g_Config.m_ClSavedDummyColorBody;
	}

	// str_copy(g_Config.m_ClPlayerSkin, g_Config.m_ClSavedPlayerSkin, sizeof(g_Config.m_ClPlayerSkin));
	// str_copy(g_Config.m_PlayerName, g_Config.m_ClSavedName, sizeof(g_Config.m_PlayerName));
	// str_copy(g_Config.m_PlayerClan, g_Config.m_ClSavedClan, sizeof(g_Config.m_PlayerClan));
	// g_Config.m_PlayerCountry = g_Config.m_ClSavedCountry;
	// g_Config.m_ClPlayerColorFeet = g_Config.m_ClSavedPlayerColorFeet;
	if(g_Config.m_ClServerRainbow)
	{
		g_Config.m_ClPlayerUseCustomColor = g_Config.m_ClSavedPlayerUseCustomColor;
		g_Config.m_ClPlayerColorBody = g_Config.m_ClSavedPlayerColorBody;
	}
}

void CEClient::SetDDNetProcessPriority(bool Set)
{
#if defined(CONF_FAMILY_WINDOWS)
	if(!SetPriorityClass(GetCurrentProcess(), Set ? HIGH_PRIORITY_CLASS : NORMAL_PRIORITY_CLASS))
	{
		log_info("entity-client", Set ? "Failed to set process priority" : "Failed to reset process priority");
		return;
	}
	if(!SetThreadPriority(GetCurrentThread(), Set ? THREAD_PRIORITY_HIGHEST : THREAD_PRIORITY_NORMAL))
	{
		log_info("entity-client", Set ? "Failed to set thread priority" : "Failed to reset thread priority");
		return;
	}
#endif
}

void CEClient::DiscordPriorityThread(void *pUserData)
{
	CEClient *pSelf = (CEClient *)pUserData;
	pSelf->SetDiscordProcessesNormalPriority();
	pSelf->m_DiscordPriorityDelay.store(time_get() + time_freq() * 30);
	pSelf->m_DiscordPriorityThreadRunning.store(false);
}

void CEClient::StartDiscordPriorityThread()
{
	// Don't start the thread if it's already running
	if(m_DiscordPriorityThreadRunning.load())
		return;

	if(m_pDiscordPriorityThread)
	{
		thread_wait(m_pDiscordPriorityThread);
		m_pDiscordPriorityThread = nullptr;
	}

	m_DiscordPriorityDelay.store(0);
	m_DiscordPriorityThreadRunning.store(true);
	m_pDiscordPriorityThread = thread_init(DiscordPriorityThread, this, "discord-priority");
}

void CEClient::SetDiscordProcessesNormalPriority()
{
#if defined(CONF_FAMILY_WINDOWS)
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		log_info("entity-client", "Failed to create process snapshot");
		return;
	}

	PROCESSENTRY32 Entry;
	mem_zero(&Entry, sizeof(Entry));
	Entry.dwSize = sizeof(Entry);

	int Changed = 0;
	int Failed = 0;

	if(Process32First(hSnapshot, &Entry))
	{
		do
		{
			if(!IsDiscordProcessName(Entry.szExeFile))
				continue;

			HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Entry.th32ProcessID);
			if(!hProcess)
			{
				Failed++;
				continue;
			}

			if(SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS))
				Changed++;
			else
				Failed++;

			CloseHandle(hProcess);
		} while(Process32Next(hSnapshot, &Entry));
	}

	CloseHandle(hSnapshot);
#endif
}

void CEClient::OnInit()
{
	SetDDNetProcessPriority(g_Config.m_ClHighProcessPriority);
	if(g_Config.m_ClDiscordNormalProcessPriority)
		StartDiscordPriorityThread();

	// On client load
	TextRender()->SetCustomFace(g_Config.m_ClCustomFont);

	m_LastMovement = 0;

	m_JoinedTeam = false;
	m_AttemptedJoinTeam = false;

	// Rainbow
	m_RainbowColor[0] = g_Config.m_ClPlayerColorBody;

	// Dummy Rainbow
	m_RainbowColor[1] = g_Config.m_ClDummyColorBody;

	// First Launch
	if(g_Config.m_ClFirstLaunch)
	{
		m_FirstLaunch = true;
		g_Config.m_ClFirstLaunch = 0;
	}

	if(g_Config.m_ClMouseSensXIngame == -1 || g_Config.m_ClMouseSensYIngame == -1)
	{
		g_Config.m_ClMouseSensXIngame = g_Config.m_InpMousesens;
		g_Config.m_ClMouseSensYIngame = g_Config.m_InpMousesens;
	}
	if(g_Config.m_ClMouseSensXUi == -1 || g_Config.m_ClMouseSensYUi == -1)
	{
		g_Config.m_ClMouseSensXUi = g_Config.m_UiMousesens;
		g_Config.m_ClMouseSensYUi = g_Config.m_UiMousesens;
	}
}

void CEClient::OnNewSnapshot()
{
	NotifyOnMove();
	UpdateVolleyball();
}

void CEClient::OnStateChange(int NewState, int OldState)
{
	if(NewState != OldState)
	{
		m_JoinedTeam = false;
		m_AttemptedJoinTeam = false;
		m_LastReplyId = -1;
		m_aLastPing = CLastPing();
	}
}

void CEClient::OnRender()
{
	// Every minute
	static int64_t LastTime = 0;
	if(time_freq() * 60 < time_get() - LastTime)
	{
		m_Playtime++;
		LastTime = time_get();
	}

	const int64_t DiscordPriorityDelay = m_DiscordPriorityDelay.load();
	if(g_Config.m_ClDiscordNormalProcessPriority && !m_DiscordPriorityThreadRunning.load() && DiscordPriorityDelay < time_get())
	{
		StartDiscordPriorityThread();
	}

	if(Client()->State() == CClient::STATE_DEMOPLAYBACK)
		return;

	UpdateRainbow();
	GoresMode();
	UpdateTaskScheduler();

	if(GameClient()->m_Controls.m_aInputData[g_Config.m_ClDummy].m_Jump || (GameClient()->m_Controls.m_aInputDirectionLeft[g_Config.m_ClDummy] || GameClient()->m_Controls.m_aInputDirectionRight[g_Config.m_ClDummy]))
	{
		m_LastMovement = time_get();
	}
}

void CEClient::UpdateTaskScheduler()
{
	int64_t Now = time_get();
	for(size_t i = 0; i < m_vScheduledTasks.size(); )
	{
		auto &Task = m_vScheduledTasks[i];
		if(!Task.m_Active)
		{
			++i;
			continue;
		}

		if(Now >= Task.m_NextExecTime)
		{
			if(Task.m_aCommand[0] != '\0')
			{
				Console()->ExecuteLine(Task.m_aCommand, -1);
			}

			if(Task.m_Repeat)
			{
				Task.m_NextExecTime = Now + (time_freq() * Task.m_IntervalMs) / 1000;
				++i;
			}
			else
			{
				// One-shot timeout finished, erase task
				m_vScheduledTasks.erase(m_vScheduledTasks.begin() + i);
			}
		}
		else
		{
			++i;
		}
	}
}

int CEClient::AddScheduledTask(const char *pName, const char *pCommand, int IntervalMs, bool Repeat)
{
	if(!pCommand || pCommand[0] == '\0')
		return -1;

	CScheduledTask Task;
	Task.m_Id = m_NextTaskId++;
	if(pName && pName[0] != '\0')
		str_copy(Task.m_aName, pName, sizeof(Task.m_aName));
	else
		str_format(Task.m_aName, sizeof(Task.m_aName), "Task #%d", Task.m_Id);

	str_copy(Task.m_aCommand, pCommand, sizeof(Task.m_aCommand));
	Task.m_IntervalMs = std::max(10, IntervalMs);
	Task.m_Repeat = Repeat;
	Task.m_Active = true;
	Task.m_NextExecTime = time_get() + (time_freq() * Task.m_IntervalMs) / 1000;

	m_vScheduledTasks.push_back(Task);
	return Task.m_Id;
}

bool CEClient::RemoveScheduledTask(int Id)
{
	for(auto it = m_vScheduledTasks.begin(); it != m_vScheduledTasks.end(); ++it)
	{
		if(it->m_Id == Id)
		{
			m_vScheduledTasks.erase(it);
			return true;
		}
	}
	return false;
}

void CEClient::ClearScheduledTasks()
{
	m_vScheduledTasks.clear();
}

void CEClient::OnSelfDeath(bool Dummy)
{
	m_HadExtraWeapons = false;

	// only count deaths of the tee we are playing on, like before dummy support
	if(Dummy != (bool)g_Config.m_ClDummy)
		return;

	m_DeathCounter++;
}

void CEClient::OnFocusChange(bool IsFocused)
{
	SetDDNetProcessPriority(g_Config.m_ClHighProcessPriority);
}
