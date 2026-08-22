**简体中文** | [English](README_EN.md)

---

![gui_logo](https://github.com/user-attachments/assets/76f93b08-3efa-40a8-96b9-b64b17c14b3f)

# Entity-Client-DDNet (个人需求定制改版)

本项目是基于经典 DDNet 扩展客户端 [Entity-Client (E-Client)](https://github.com/qxdFox/Entity-Client-DDNet) 根据**个人使用与跑图需求深度定制**的改版分支。

在完整继承原版 Entity-Client 全部强大特性的前提下，重构打通了本地化系统并内置中文支持，同时针对 Gores 模式、跑图观战以及日常操作体验进行了专属的自定义扩展。

---

## 📥 获取与下载

无需本地编译，直接从本仓库的 **GitHub Actions** 中下载由微软云端虚拟机自动编译打包的最新成品：

1. 打开本仓库的 [Actions 页面](../../actions)。
2. 点击最新的 **Build** 构建记录。
3. 在页面最底部的 **Artifacts** 区域下载 `DDNet-*-win64.zip`。
4. 解压后直接双击运行 `DDNet.exe` 即可使用。

---

## ✨ 本分支专属定制与新增功能 (Custom Features)

以下为本分支根据个人需求独立开发并补充的专属改动：

### 1. 🇨🇳 深度汉化与内嵌零配置中文支持
* **内嵌零配置中文**：源码内置了 280+ 条 Entity 专属设置的中文翻译对照表。无需额外配置任何外部语言包，只要游戏语言设置为简体中文，所有专有功能面板**全自动显示为中文**。
* **底层系统修复**：打通了原版中未生效的 `EcLocalize` 本地化接口，修复了数十处硬编码英文 UI。
* **外部语言热更新**：依然支持通过 `data/languages/simplified_chinese.txt` 进行动态热重载，修改后重启游戏即刻生效。

### 2. ⚡ Gores 模式一键快捷开关 (`toggle_gores_mode`)
* **痛点解决**：在开启 Gores 自动切锤敲击模式时，无法正常使用手枪开火提醒队友。
* **新增指令**：新增 `toggle_gores_mode` 控制台命令，可在聊天框实时输出开启/关闭提示。
* **按键设置集成**：在 **「设置」 -> 「控制」 -> 「武器 (Weapon)」** 菜单中直接加入了 **`切换 Gores 自动切枪模式`** 绑定项，支持鼠标点击直接按键绑定。
* **轮盘集成**：支持直接添加至快捷绑定轮盘：`add_bindwheel "Gores模式" "toggle_gores_mode"`。

### 3. 👁️ 观战/暂停模式「仅看同队成员」纯净视野 (`ec_spec_team_only`)
* **痛点解决**：在跑图中使用 `/pause`、`/spec` 或 `smartspec.chai` 观战查看队友路线时，全图其他队伍的玩家、激光与名字牌会造成严重视觉干扰。
* **纯净过滤**：开启此功能后，无论处于**自由漫游视角 (Freeview)** 还是**锁定队友视角**，客户端将**全自动 100% 隐藏所有非同队玩家**（包括 Tee 模型、名字牌、钩爪、激光、投掷物与冰冻状态条），仅呈现同队伍队员。
* **配置开关**：
  * 设置面板：在 **「Entity 设置」 -> 「常规设置」 -> 「菜单设置」** 中勾选 **`观战/暂停时仅显示同队成员`**。
  * 控制台变量：`ec_spec_team_only 1`（`0` 为关闭，`1` 为开启）。

---

## 🛠️ 原版 Entity-Client (E-Client) 核心基础功能

以下为原版 Entity-Client 客户端自带的核心功能与使用说明：

### 1. 快捷绑定轮盘 (Bindwheel)
按住设定按键即可呼出可视化径向轮盘，通过鼠标快速选择执行预设的各类指令。
* `+bindwheel`：呼出绑定轮盘（在“设置 -> 控制”或控制台绑定按键，如 `bind x +bindwheel`）。
* `add_bindwheel "<显示名称>" "<执行指令>"`：向轮盘添加快捷项。
* `remove_bindwheel "<显示名称>" "<执行指令>"`：移除指定项。
* `delete_all_bindwheel_binds`：清空轮盘。

#### 💡 实用轮盘一键配置范例（按 F1 在控制台执行）：
```text
# 队伍加入与退出
add_bindwheel "队伍 1" "team 1"
add_bindwheel "队伍 2" "team 2"
add_bindwheel "队伍 3" "team 3"
add_bindwheel "退出队伍" "team 0"

# 快速重开 / 自杀
add_bindwheel "自杀" "kill"

# 切换控制分身 (Dummy)
add_bindwheel "切换分身" "toggle cl_dummy 0 1"

# 常用聊天命令
add_bindwheel "Top5 榜单" "say /top5"
add_bindwheel "个人排名" "say /rank"
add_bindwheel "暂停/继续" "say /pause"

# 特效与模式开关
add_bindwheel "彩虹开关" "toggle ec_server_rainbow 0 1"
add_bindwheel "Gores开关" "toggle_gores_mode"
```

---

### 2. Gores 自动化与 Anti-Ping 优化
* `ec_gores_mode 1`：自动切枪模式（默认持手枪，按下开火键瞬间切锤敲击并切回手枪）。
* `ec_gores_mode_disable_weapons 1`：拥有激光/榴弹/霰弹等重武器时自动暂停切锤，无重武器时自动恢复。
* `ec_gores_mode_auto_enable 1`：进入 Gores 模式服务器时自动激活，离开时自动关闭。
* `ec_antiping_negative_buffer 1`：专为 Gores 模式优化的负缓冲区 Anti-Ping 预测，大幅减少救援与被敲击时的画面拉扯瞬移。

---

### 3. 玩家快捷动作菜单 (Player Actions)
将准星对准某位玩家按住动作菜单按键，一键针对该目标执行指令。
* 支持动态占位符：`%s`（目标玩家名字）、`%d`（目标玩家 Client ID）。
* 范例：配置动作为 `/whisper %s 来我队` 或 `votekick "%s" "挂机"`。

---

### 4. 目标与仇人管理 (Warlist)
多维度标记玩家与战队（好友、仇人、助手等），在聊天栏、计分板、旁观菜单全方位高亮：
* `onlineinfo`：在聊天栏输出当前服务器各分组玩家的在线与挂机 (AFK) 数量。
* `playerinfo "<玩家名>"`：快速查询玩家所属战队、颜色、皮肤及权限等级。
* `addtempwar "<玩家名>"` / `deltempwar "<玩家名>"`：临时增删仇人名单。
* `addtempmute "<玩家名>"` / `deltempmute "<玩家名>"`：临时增删屏蔽名单。

---

### 5. 皮肤快照与个性化外观
* `saveskin` / `restoreskin`：一键保存与恢复当前账号（或分身）的皮肤、名字、战队、颜色与旗帜快照。
* 服务端彩虹模式：`server_rainbow_body`、`server_rainbow_feet`、`server_rainbow_speed`、`server_rainbow_both_players`、`server_rainbow_sat`、`server_rainbow_lht`。
* 视觉增强：纯白脚、冰冻武士刀、轨迹拖尾、地图全景概览、右上角冰冻队友状态栏 (`ec_frozen_tees_hud`)。

---

### 6. 聊天与便捷工具
* **可框选复制聊天**：聊天栏支持鼠标直接高亮选中文字并按 `Ctrl+C` 复制，聊天记录可向上平滑滚动。
* `reply_last <?消息内容>`：一键向上一位私聊或 `@` 提及你的玩家快捷回复。
* `calc "<数学表达式>"`：内置轻量计算器（如 `calc 1920/1080`）。
* 头顶聊天气泡（Chat Bubbles）：实时在 Tee 头顶以气泡展示聊天文字。

---

### 7. 竞技操作与性能优化
* `ec_fast_input 1`：极速输入模式（大幅降低画面操作响应延迟）。
* `ec_high_process_priority 1`：自动提升游戏进程为高优先级，确保帧率极致平稳。
* `ec_freeze_kill 1`：进冰自杀辅助，支持仅完全深冰自杀、视野内有队友时不自杀等智能保护。
* `ec_anti_spawn_block 1`：防止出生点卡位。

---

## 📜 ChaiScript 脚本引擎

Entity-Client 内置了 [ChaiScript](https://chaiscript.com/) 脚本引擎。将 `.chai` 脚本放置于配置目录下，在控制台通过 `chai [脚本名] [参数...]` 即可运行。

<details>
<summary><b>点击展开 ChaiScript 基础语法与 API 状态表</b></summary>

```js
var a = 1
var b = 2
var c = "strings"
print("hello " + to_string(a)) // 字符串拼接需显式 to_string
if (c == "strings") {
    exec("echo hello world") // 执行控制台指令
}
var current_mode = state("game_mode") // 获取游戏状态

def myfunc(x, y) {
    x + y // 表达式作为返回值
}
```

### 状态查询函数 `state(...)` 常用项

| 返回类型 | 调用方式 | 说明 |
| :--- | :--- | :--- |
| `int` | `state("client_id")` | 获取当前玩家客户端 ID |
| `int` | `state("dummy_id")` | 获取分身 (Dummy) ID |
| `string` | `state("game_mode")` | 当前游戏模式（'DM', 'TDM', 'CTF' 等） |
| `bool` | `state("in_freeze")` | 当前是否处于冰冻状态 |
| `int` | `state("team")` | 当前队伍编号 |
| `string` | `state("map")` | 当前地图名称 |
| `string` | `state("server_ip")` | 当前服务器 IP 地址 |
| `int` | `state("players_connected")`| 当前在线人数 |
| `bool` | `client_info("afk", int Id)` | 目标 ID 是否挂机 |
| `bool` | `client_info("friend", int Id)` | 目标 ID 是否为好友 |

</details>

---

## 🤝 致谢与鸣谢 (Credits)

- **Entity-Client 原作者**：[qxdFox](https://github.com/qxdFox) 与 [Miro8D](https://github.com/Miro8D)
- **底层引擎与游戏**：[DDNet 团队](https://github.com/ddnet/ddnet)
- **个人定制改版维护**：[gradiaun](https://github.com/gradiaun)
