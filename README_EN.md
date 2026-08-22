[简体中文](README.md) | **English**

---

![gui_logo](https://github.com/user-attachments/assets/76f93b08-3efa-40a8-96b9-b64b17c14b3f)

# Entity-Client-DDNet (Personal Custom Fork)

This project is a personal custom fork based on the [Entity-Client (E-Client)](https://github.com/qxdFox/Entity-Client-DDNet) modification for [DDNet](https://github.com/ddnet/ddnet) (DDRaceNetwork), tailored specifically for personal racing and daily gameplay needs.

While retaining all the powerful capabilities of the original Entity-Client, this fork adds comprehensive Chinese localization with zero-configuration fallback, Gores mode quality-of-life toggles, team-only spectator filtering, and various usability enhancements.

---

## 📥 Downloads

Pre-built binaries generated via **GitHub Actions** are available directly:

1. Navigate to the repository's [Actions tab](../../actions).
2. Click the latest **Build** workflow run.
3. Download `DDNet-*-win64.zip` from the **Artifacts** section at the bottom.
4. Extract and run `DDNet.exe`.

---

## ✨ Custom Features in this Fork

The following features were independently implemented and customized in this fork:

### 1. 🇨🇳 Complete Chinese Localization with Zero-Config Fallback
* **Built-in Fallback Dictionary**: Over 280+ Entity-Client specific UI strings are embedded directly in the source code. The settings interface will **automatically display in Chinese** out of the box whenever the client language is set to Simplified Chinese.
* **Underlying Fixes**: Fixed the `EcLocalize` interface and wrapped dozens of previously hardcoded English strings.
* **Hot-Reloadable**: Retains support for updating translations via `data/languages/simplified_chinese.txt` without recompilation.

### 2. ⚡ Gores Mode Toggle & Keybinding (`toggle_gores_mode`)
* **Problem Solved**: When automated Gores hammer-switching is enabled, firing pistols to ping/alert teammates was impossible because every shot became a hammer swing.
* **Console Command**: Added `toggle_gores_mode` with on-screen chat feedback (`Gores Mode: Enabled / Disabled`).
* **Settings Integration**: Added a dedicated keybinding slot in **Settings -> Controls -> Weapon** (`Toggle Gores mode`).
* **Bindwheel Support**: Easily add it to your radial menu: `add_bindwheel "Gores Mode" "toggle_gores_mode"`.

### 3. 👁️ Team-Only Spectator / Pause Filter (`ec_spec_team_only`)
* **Problem Solved**: When using `/pause`, `/spec`, or `smartspec.chai` to inspect teammate positions and map routes, other players and their projectiles across the map cluttered the screen.
* **Clean Team Vision**: When enabled, the client **automatically hides all players from other teams** (including Tee bodies, nameplates, hooks, lasers, projectiles, and freeze bars) in both **Freeview** and **Follow/Spectate** modes.
* **Configuration**:
  * In GUI: **Entity Settings -> General -> Menu Settings -> `Only Show Team Members in Spectator Mode`**.
  * Console variable: `ec_spec_team_only 1` (default `0`).

---

## 🛠️ Original Entity-Client (E-Client) Core Features

The following core features are inherited from the original Entity-Client:

### 1. Radial Bindwheel
Hold a key to open a visual radial menu and execute bound commands via mouse direction.
* `+bindwheel`: Open radial bindwheel (bind in Controls settings, e.g. `bind x +bindwheel`).
* `add_bindwheel "<Name>" "<Command>"`: Add a bind entry.
* `remove_bindwheel "<Name>" "<Command>"`: Remove an entry.
* `delete_all_bindwheel_binds`: Clear all wheel binds.

#### 💡 Example Binds (run in console via F1):
```text
# Join / leave teams
add_bindwheel "Team 1" "team 1"
add_bindwheel "Team 2" "team 2"
add_bindwheel "Team 3" "team 3"
add_bindwheel "Leave Team" "team 0"

# Quick suicide / restart
add_bindwheel "Kill" "kill"

# Toggle dummy control
add_bindwheel "Toggle Dummy" "toggle cl_dummy 0 1"

# Chat commands
add_bindwheel "Top 5" "say /top5"
add_bindwheel "Rank" "say /rank"
add_bindwheel "Pause" "say /pause"

# Feature toggles
add_bindwheel "Toggle Rainbow" "toggle ec_server_rainbow 0 1"
add_bindwheel "Toggle Gores" "toggle_gores_mode"
```

---

### 2. Gores Mode Automation & Anti-Ping
* `ec_gores_mode 1`: Automatically holds gun and performs instantaneous hammer swings upon firing.
* `ec_gores_mode_disable_weapons 1`: Automatically pauses Gores hammer switching when holding laser, grenade, or shotgun.
* `ec_gores_mode_auto_enable 1`: Auto-enables when connected to a "Gores" gametype server.
* `ec_antiping_negative_buffer 1`: Negative buffer prediction algorithm for smoother anti-ping handling during saves and hammer fly.

---

### 3. Player Actions Menu
Hold a key to trigger actions targeting the player under your cursor.
* Dynamic placeholders: `%s` (Player name), `%d` (Client ID).

---

### 4. Warlist & Player Tracking
* `onlineinfo`: Summary of tracked and AFK players on current server.
* `playerinfo "<Name>"`: Query player clan, skin, custom colors, and auth level.
* `addtempwar` / `deltempwar` / `addtempmute` / `deltempmute`: Temporary war/mute list controls.

---

### 5. Skin Profiles & Visual Enhancements
* `saveskin` / `restoreskin`: Snapshot and restore player / dummy skin, name, clan, flag, and colors.
* Server-Side Rainbow: `server_rainbow_body`, `server_rainbow_feet`, `server_rainbow_speed`, etc.
* Visual tweaks: White feet, frozen katana, tee trails, map overview, frozen tee status bar (`ec_frozen_tees_hud`).

---

### 6. Chat & Usability
* **Selectable Chat**: Select and copy text directly from chat history with smooth scrolling.
* `reply_last <?Message>`: Fast reply to the last player who pinged or whispered you.
* `calc "<expression>"`: In-client mathematical expression evaluator.
* Chat Bubbles: Floating speech bubbles over player heads.

---

### 7. Performance & Competitive Helpers
* `ec_fast_input 1`: Reduced visual input delay mode.
* `ec_high_process_priority 1`: Run DDNet process at high OS priority.
* `ec_freeze_kill 1`: Smart freeze kill helper.
* `ec_anti_spawn_block 1`: Prevent spawn blocking.

---

## 📜 ChaiScript Scripting Engine

Entity-Client embeds the [ChaiScript](https://chaiscript.com/) engine. Place `.chai` scripts in your config directory and run them via `chai [scriptname] [args]`.

<details>
<summary><b>Click to view ChaiScript Syntax & Available States</b></summary>

```js
var a = 1
var b = 2
var c = "strings"
print("hello " + to_string(a)) // explicit to_string required for concatenation
if (c == "strings") {
    exec("echo hello world") // run console command
}
var current_mode = state("game_mode") // query game state
```

### Common `state(...)` Query Functions

| Return Type | Query | Description |
| :--- | :--- | :--- |
| `int` | `state("client_id")` | Current client ID |
| `int` | `state("dummy_id")` | Dummy client ID |
| `string` | `state("game_mode")` | Current game mode ('DM', 'TDM', 'CTF', etc.) |
| `bool` | `state("in_freeze")` | Whether currently frozen |
| `int` | `state("team")` | Current team ID |
| `string` | `state("map")` | Current map name |
| `string` | `state("server_ip")` | Current server IP |
| `int` | `state("players_connected")`| Connected player count |

</details>

---

## 🤝 Credits

- **Original Entity-Client**: [qxdFox](https://github.com/qxdFox) and [Miro8D](https://github.com/Miro8D)
- **Engine & Game**: [DDNet Team](https://github.com/ddnet/ddnet)
- **Personal Custom Fork & Chinese Localization**: [gradiaun](https://github.com/gradiaun)
