#include "info.h"

#include <engine/shared/json.h>

#include <game/client/gameclient.h>
#include <game/version.h>

#include <tuple>

static constexpr const char *ECLIENT_INFO_FILE = "eclient-info.json";
static constexpr const char *ECLIENT_INFO_URL[2] = {"https://raw.githubusercontent.com/qxdFox/FoxSite/refs/heads/main/docs/info.json", "https://www.entityclient.net/info.json"};

static const char *s_pCustomChangelogZh =
	"#1 Entity-Client-DDNet-Zh 中文增强分支\n"
	"-# 本分支基于 FoxNet-DDNet/Entity-Client-DDNet，由 gradiaun 维护定制\n"
	"\n"
	"## v1.0.0 (Custom)\n"
	"- 修复聊天自动追加 [ERR] 翻译后缀问题（默认关闭自动翻译）\n"
	"- 新增「扩展 (Extensions)」专属设置标签页，收录分支自实现功能\n"
	"- 新增「指令列表 (Commands List)」内置查询页，支持实时搜索与一键复制指令到剪贴板\n"
	"- 新增皮肤窃取功能（steal_skin / copy_skin），一键同步最近玩家的皮肤及颜色\n"
	"- 优化 Gores 模式自动切枪逻辑：开火固定切锤（+weapon1），彻底避免切枪状态反转导致打不到人\n"
	"- 优化重武器自动禁用：拾取喷子/激光/榴弹时单次触发关闭，重新手动开启依然可用\n"
	"- 新增分身锤击保持抓钩抓取功能（ec_dummy_hammer_keep_hook），锤击不断钩\n"
	"- 全面汉化与润色：重构中文词条表，将 Warlist 词条规范化为「对手/敌人」，补全所有设置项中文\n"
	"- 修复 GitHub Actions 云端打包多处编译错误与 Release 自动发布工作流\n"
	"\n"
	"----------------------------------------\n"
	"#2 官方 Entity-Client 历史更新日志\n"
	"\n";

void CEntityInfo::OnInit()
{
	str_copy(m_aNews, s_pCustomChangelogZh);

	void *pBuf;
	unsigned Length;
	if(!Storage()->ReadFile(ECLIENT_INFO_FILE, IStorage::TYPE_SAVE, &pBuf, &Length))
		return;

	json_value *pJson = json_parse((const char *)pBuf, Length);
	if(!pJson)
		return;
	const json_value &Json = *pJson;
	const json_value &CurrentNews = Json["news"];

	if(CurrentNews.type == json_string)
	{
		char aCombined[sizeof(m_aNews)];
		str_copy(aCombined, s_pCustomChangelogZh);
		str_append(aCombined, CurrentNews);

		if(m_aNews[0] && !str_find(m_aNews, (const char *)CurrentNews))
			g_Config.m_EcUnreadNews = true;

		str_copy(m_aNews, aCombined);
	}
	json_value_free(pJson);
}

void CEntityInfo::OnRender()
{
	if(m_pEClientInfoTask)
	{
		if(m_pEClientInfoTask->State() == EHttpState::DONE)
		{
			FinishEClientInfo();
			ResetEClientInfoTask();
		}
		else if(m_pEClientInfoTask->State() == EHttpState::ERROR && !m_Retried)
		{
			g_Config.m_ClInfoUrlType = !g_Config.m_ClInfoUrlType;
			ResetEClientInfoTask();
			FetchEClientInfo();
			m_Retried = true;
		}
	}
}
void CEntityInfo::ResetEClientInfoTask()
{
	if(m_pEClientInfoTask)
	{
		m_pEClientInfoTask->Abort();
		m_pEClientInfoTask = nullptr;
	}
}

void CEntityInfo::FetchEClientInfo()
{
	if(m_pEClientInfoTask && !m_pEClientInfoTask->Done())
		return;
	const char *aUrl = ECLIENT_INFO_URL[g_Config.m_ClInfoUrlType];

	m_pEClientInfoTask = HttpGetFile(aUrl, Storage(), ECLIENT_INFO_FILE, IStorage::TYPE_SAVE);
	m_pEClientInfoTask->Timeout(CTimeout{10000, 0, 500, 10});
	m_pEClientInfoTask->IpResolve(IPRESOLVE::V4);
	m_pEClientInfoTask->SkipByFileTime(false);
	Http()->Run(m_pEClientInfoTask);
}

typedef std::tuple<int, int, int> EcVersion;
static const EcVersion gs_InvalidECVersion = std::make_tuple(-1, -1, -1);

static EcVersion ToECVersion(char *pStr)
{
	int aVersion[3] = {0, 0, 0};
	const char *p = strtok(pStr, ".");

	for(int i = 0; i < 3 && p; ++i)
	{
		if(!str_isallnum(p))
			return gs_InvalidECVersion;

		aVersion[i] = str_toint(p);
		p = strtok(nullptr, ".");
	}

	if(p)
		return gs_InvalidECVersion;

	return std::make_tuple(aVersion[0], aVersion[1], aVersion[2]);
}

void CEntityInfo::FinishEClientInfo()
{
	void *pBuf;
	unsigned Length;
	json_value *pJson = nullptr;
	if(Storage()->ReadFile(ECLIENT_INFO_FILE, IStorage::TYPE_SAVE, &pBuf, &Length))
		pJson = json_parse((const char *)pBuf, Length);
	if(!pJson)
		return;
	const json_value &Json = *pJson;
	const json_value &CurrentVersion = Json["version"];

	if(CurrentVersion.type == json_string)
	{
		char aNewVersionStr[64];
		str_copy(aNewVersionStr, CurrentVersion);
		char aCurVersionStr[64];
		str_copy(aCurVersionStr, ECLIENT_VERSION);
		if(ToECVersion(aNewVersionStr) > ToECVersion(aCurVersionStr))
		{
			str_copy(m_aVersionStr, CurrentVersion);
		}
		else
		{
			m_aVersionStr[0] = '0';
			m_aVersionStr[1] = '\0';
		}
	}
	const json_value &CurrentNews = Json["news"];

	if(CurrentNews.type == json_string)
	{
		char aCombined[sizeof(m_aNews)];
		str_copy(aCombined, s_pCustomChangelogZh);
		str_append(aCombined, CurrentNews);

		if(m_aNews[0] && !str_find(m_aNews, (const char *)CurrentNews))
			g_Config.m_EcUnreadNews = true;

		str_copy(m_aNews, aCombined);
	}

	json_value_free(pJson);
}
