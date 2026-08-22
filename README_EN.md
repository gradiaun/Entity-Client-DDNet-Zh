[简体中文](README.md) | **English**

---

![gui_logo](https://github.com/user-attachments/assets/76f93b08-3efa-40a8-96b9-b64b17c14b3f)

# Entity-Client-DDNet (Chinese Localization & Extended Fork)

This project is a customized, feature-rich DDNet (DDRaceNetwork) client fork with extended features and full Chinese localization support (with built-in fallback translations).

### Key Features
- **Full Chinese Localization**: Built-in Chinese fallback dictionary, no extra language configuration needed.
- **Scrollable Chat & Direct Copy**: Select and copy text directly from the chat log.
- **Process Priority Management**: Set DDNet process priority to high and reduce Discord's priority to mitigate stream lag.
- **Moving Tiles in Entities**: Support rendering moving tiles in entity mode.
- **Extensive UI & Visual Tweaks**: Warlist, Status Bar, Bindwheel, Player Actions, Rainbow effects, and more.
- **Embedded ChaiScript Engine**: Easy scripting for client automation and state querying.

---

### Scripting (ChaiScript)

Entity Client supports the [ChaiScript](https://chaiscript.com/) language for simple custom tasks.

Add scripts to your config dir then run them with `chai [scriptname] [args]`

> [!CAUTION]
> There are no runtime restrictions, you can easily `while (true) {}` yourself or run out of memory, be careful!

<details>
<summary>ChaiScript Capabilities & API</summary>

```js
var a // Declare a variable
a = 1 // Set it
var b = 2 // Do both at once
var c = "strings"
var d = ["lists", 2] // not strongly typed
// var e, f = d // no list deconstruction
print(d[0] + to_string(d[1])) // explicit to_string required for string concat
var bass = "ba" + "s" + "s"
var ass = bass.substr(1, -1) // both indices required, use -1 for end
if (a == b) { // brackets required
	print("this will never happen") // output
} else if (c == "strings") { // string comparison
	exec("echo hello world") // run console stuff
}
var current_game_mode = state("game_mode") // Get the current game mode, all states you can get are listed below
def myfunc(a, b, c) { // function definition
	print(a, b, c)
	if (a == b) { return "early" }
	c // last statement returns
}
print(myfunc(1, 2, 3)) // prints "early"
for (var i = 0; i < 10; i = i+1) { // for loops (c style)
	print(i) // auto converts to string, will throw if it cant
}
return "top level return"
```

#### Available States (`state(...)`)

| Return type | Call | Description |
| --- | -- | --- |
| `string` | `to_lower(<string>)` | Converts the input string to lowercase. |
| `string` | `to_upper(<string>)` | Converts the input string to uppercase. |
| `int` | `state("client_id")` | Returns the current client ID. |
| `int` | `state("dummy_id")` | Returns the dummy client ID if connected. |
| `string` | `state("game_mode")` | Current game mode name ('DM', 'TDM', 'CTF', etc.). |
| `bool` | `state("game_mode_pvp")` | Whether the current mode is PvP. |
| `bool` | `state("game_mode_race")` | Whether the current mode is a race mode. |
| `bool` | `state("eye_wheel_allowed")` | Whether 'eye wheel' is allowed on this server. |
| `bool` | `state("zoom_allowed")` | Whether camera zoom is allowed. |
| `bool` | `state("dummy_allowed")` | Whether using a dummy client is allowed. |
| `bool` | `state("dummy_connected")` | Whether dummy client is currently connected. |
| `bool` | `state("rcon_authed")` | Whether authenticated with RCON. |
| `int` | `state("team")` | Current team number. |
| `int` | `state("ddnet_team")` | Current DDNet team number. |
| `string` | `state("map")` | Name of the current/connecting map. |
| `string` | `state("server_ip")` | IP address of current server. |
| `int` | `state("players_connected")` | Number of connected players. |
| `int` | `state("players_cap")` | Max players server supports. |
| `string` | `state("server_name")` | Server's name. |
| `string` | `state("community")` | Community identifier. |
| `string` | `state("location")` | Approximate map location ('NW', 'C', 'SE', etc.). |
| `string` | `state("state")` | Connection state ('online', 'offline', 'loading', 'demo'). |
| `int` | `state("id", string Name)` | Finds client ID by player name. |
| `string` | `state("name", int Id)` | Player name for given client ID. |
| `string` | `state("clan", int Id)` | Clan name for given client ID. |
| `string` | `state("player_name")` | Value of `player_name` config. |
| `string` | `state("dummy_name")` | Value of `dummy_name` config. |
| `bool` | `client_info("exists", int Id)` | Whether client ID exists. |
| `int` | `client_info("team", int Id)` | Team of ID. |
| `int` | `client_info("ddnet_team", int Id)` | DDRace team of ID. |
| `string` | `client_info("name", int Id)` | Name of ID. |
| `string` | `client_info("clan", int Id)` | Clan of ID. |
| `string` | `client_info("skin_name", int Id)` | Skin name of ID. |
| `int` | `client_info("skin_custom_color", int Id)` | Custom color flag. |
| `int` | `client_info("skin_color_feet", int Id)` | Feet color of ID. |
| `int` | `client_info("skin_color_body", int Id)` | Body color of ID. |
| `bool` | `client_info("afk", int Id)` | Whether ID is AFK. |
| `bool` | `client_info("friend", int Id)` | Whether ID is friend. |
| `bool` | `client_info("foe", int Id)` | Whether ID is foe. |
| `int` | `client_info("warlist_type", int Id)` | Warlist type if has entry. |
| `string` | `client_info("warlist_type_name", int Id)` | Warlist type name. |
| `bool` | `client_info("muted", int Id)` | Whether ID is muted. |
| `int` | `client_info("auth_level", int Id)` | Auth level of ID. |

```js
var what = include("thatscript.chai") // include other scripts (relative to config dir)
print(what)
if (!file_exists("file")) {
	throw("file missing")
}
```

#### Modules: `math` & `re`

```js
import("math")
math.pi
math.min(1, 10)
math.random(1, 100)

import("re")
if(re.test(re.compile(".+?ello.+?"), "hello")) {
	print("matched")
}
```

</details>

---

<details open>
<summary>Setting Pages Preview</summary>

### Main Settings
<img width="1920" height="1854" alt="Settings" src="https://github.com/user-attachments/assets/9a9ea2cc-96bd-44b0-a4e5-fe68ed319ca1" />

### Visuals
<img width="1920" height="2792" alt="menu_2026-07-12_13-12-28" src="https://github.com/user-attachments/assets/91d2b82f-d013-476a-af10-2fd31de2869f" />

### Warlist
<img width="1920" height="1080" alt="menu_2026-07-12_13-44-18" src="https://github.com/user-attachments/assets/4a8138c0-817b-452e-b29e-dcdd96643ce6" />

### Status bar
<img width="1920" height="1080" alt="menu_2026-07-12_13-12-30" src="https://github.com/user-attachments/assets/26d4ec2f-2e23-4c78-98e5-c96bde360418" />

### Bindwheel
<img width="1920" height="1080" alt="menu_2026-07-12_13-12-32" src="https://github.com/user-attachments/assets/942384fe-32a7-44c1-af8b-e54a5ccf01f9" />

### Player actions
<img width="1920" height="1080" alt="menu_2026-07-12_13-12-33" src="https://github.com/user-attachments/assets/46436731-b867-4e71-a496-eaaf2b9b0bf7" />

### Info
<img width="1920" height="1080" alt="menu_2026-07-12_13-45-47" src="https://github.com/user-attachments/assets/09b589d4-3472-4ddd-98bd-04e0d5d7f7b9" />

</details>

---

<details>
<summary>Console Commands</summary>

```
votekick "<Name> <Reason>"
onlineinfo
saveskin
restoreskin
playerinfo "<Name>"
addtempwar "<Name>"
deltempwar "<Name>"
addtemphelper "<Name>"
deltemphelper "<Name>"
addtempmute "<Name>"
deltempmute "<Name>"
view_link <url>
server_rainbow_speed "<speed>"
server_rainbow_both_players "<0 | 1>"
server_rainbow_sat <Saturation> <0 | 1 (Dummy)>
server_rainbow_lht <Lightness> <0 | 1 (Dummy)>
server_rainbow_body <0 | 1> <0 | 1 (Dummy)>
server_rainbow_feet <0 | 1> <0 | 1 (Dummy)>
reply_last <?Message>
specid <id>
```
</details>

---

### Credits
- Original Entity-Client by [qxdFox](https://github.com/qxdFox) and [Miro8D](https://github.com/Miro8D).
- Chinese Localization & Embedded fallback translation maintained by [gradiaun](https://github.com/gradiaun).
