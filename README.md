**简体中文** | [English](README_EN.md)

---

![gui_logo](https://github.com/user-attachments/assets/76f93b08-3efa-40a8-96b9-b64b17c14b3f)

# Entity-Client-DDNet (中文汉化增强版)

本项目是基于经典开源游戏 [DDNet](https://github.com/ddnet/ddnet) (DDRaceNetwork) 的深度定制客户端 **Entity-Client** 的中文增强与汉化维护分支。

在继承原版 Entity-Client 强大功能（如快捷绑定轮盘、仇人列表、状态栏、玩家动作菜单、ChaiScript 脚本引擎等）的基础上，**打通了完整的本地化系统，并内嵌了开箱即用的简体中文词条支持**。

---

## 🌟 中文版主要特性

- **✨ 零配置开箱即用中文**：源码内置了 280+ 条 Entity 专属设置的中文翻译对照表。无论是否加载外部语言包，只要客户端语言设置为中文，所有专有功能面板**全自动显示为中文**。
- **🌐 外部语言包热重载**：同时保留了外部语言文件扩展能力。可在 `data/languages/simplified_chinese.txt` 中自由修改或补充词条，**修改后重启游戏即刻生效，无需重新编译**。
- **💬 可滚动与选中复制聊天**：支持在聊天框中直接用鼠标高亮选中文字并复制，聊天历史可向上平滑滚动。
- **⚡ 进程优先级优化**：自动提高 DDNet 进程优先级并优化后台软件（如 Discord 等）的优先级，大幅降低直播/语音通话时的画面卡顿。
- **🧱 实体层移动图块渲染**：支持在 Entity 视图中正常显示并渲染动态移动图块。
- **🎨 深度视觉与个性化**：包含玩家方位指示器、彩虹身体/脚部/钩爪、纯白脚、冰冻武士刀、小地图全景概览、灵动岛媒体展示等丰富特效。
- **📜 ChaiScript 脚本引擎**：支持直接编写 ChaiScript 脚本实现客户端自动化、游戏状态查询与快捷操作。

---

## 📥 获取与下载

你可以直接从本仓库的 **GitHub Actions** 中下载由微软云端虚拟机自动编译打包的最新版本：

1. 进入 [Actions 页面](../../actions)。
2. 点击最新的 **Build** 构建记录。
3. 在页面最底部的 **Artifacts** 区域下载 `DDNet-*-win64.zip`。
4. 解压后直接运行 `DDNet.exe` 即可享受完整的汉化体验！

---

## 📜 ChaiScript 脚本功能说明

Entity-Client 内置了 [ChaiScript](https://chaiscript.com/) 脚本引擎，可用于编写轻量级自定义功能与逻辑。

* **使用方式**：将 `.chai` 脚本文件放置在客户端配置目录（Config Dir）中，然后在控制台执行 `chai [脚本名] [参数...]`。

> [!CAUTION]
> 脚本引擎没有运行环境限制，请勿编写死循环 `while (true) {}` 以免游戏卡死或内存溢出。

<details>
<summary><b>点击展开 ChaiScript 基础语法与 API 列表</b></summary>

### 基础语法示例

```js
var a // 声明变量
a = 1 // 赋值
var b = 2 // 声明并赋值
var c = "strings"
var d = ["lists", 2] // 列表
print(d[0] + to_string(d[1])) // 字符串拼接需显式转换 to_string
var bass = "ba" + "s" + "s"
var ass = bass.substr(1, -1) // 截取子串
if (a == b) {
	print("this will never happen")
} else if (c == "strings") {
	exec("echo hello world") // 执行控制台指令
}
var current_game_mode = state("game_mode") // 获取当前游戏模式

def myfunc(a, b, c) { // 定义函数
	print(a, b, c)
	if (a == b) { return "early" }
	c // 最后一条语句作为返回值
}
print(myfunc(1, 2, 3))
for (var i = 0; i < 10; i = i+1) { // 循环
	print(i)
}
```

### 状态查询函数 `state(...)`

| 返回类型 | 调用方式 | 说明 |
| --- | --- | --- |
| `string` | `to_lower(<string>)` | 转换为小写 |
| `string` | `to_upper(<string>)` | 转换为大写 |
| `int` | `state("client_id")` | 获取当前玩家客户端 ID |
| `int` | `state("dummy_id")` | 获取分身 (Dummy) ID |
| `string` | `state("game_mode")` | 当前游戏模式（如 'DM', 'TDM', 'CTF'） |
| `bool` | `state("game_mode_pvp")` | 是否为 PvP 模式 |
| `bool` | `state("game_mode_race")` | 是否为跑图 (Race) 模式 |
| `bool` | `state("eye_wheel_allowed")` | 当前服务器是否允许眼神表情轮盘 |
| `bool` | `state("zoom_allowed")` | 当前服务器是否允许视野缩放 |
| `bool` | `state("dummy_allowed")` | 当前服务器是否允许使用分身 |
| `bool` | `state("dummy_connected")` | 分身是否已连接 |
| `bool` | `state("rcon_authed")` | 是否已获得 RCON 管理员认证 |
| `int` | `state("team")` | 玩家当前队伍编号 |
| `int` | `state("ddnet_team")` | 玩家 DDNet 跑图队伍编号 |
| `string` | `state("map")` | 当前地图名称 |
| `string` | `state("server_ip")` | 当前服务器 IP 地址 |
| `int` | `state("players_connected")` | 当前在线玩家数量 |
| `int` | `state("players_cap")` | 服务器最大玩家承载数 |
| `string` | `state("server_name")` | 服务器名称 |
| `string` | `state("community")` | 服务器社区标识 |
| `string` | `state("location")` | 玩家大致方位（'NW', 'C', 'SE' 等） |
| `string` | `state("state")` | 连接状态（'online', 'offline', 'loading', 'demo'） |
| `int` | `state("id", string Name)` | 通过玩家名字查询对应 ID |
| `string` | `state("name", int Id)` | 根据 ID 查询玩家名字 |
| `string` | `state("clan", int Id)` | 根据 ID 查询战队名字 |
| `string` | `state("player_name")` | 获取配置中的玩家名称 |
| `string` | `state("dummy_name")` | 获取配置中的分身名称 |
| `bool` | `client_info("exists", int Id)` | 检查 ID 是否存在 |
| `int` | `client_info("team", int Id)` | 对应 ID 的队伍编号 |
| `int` | `client_info("ddnet_team", int Id)` | 对应 ID 的跑图队伍 |
| `string` | `client_info("name", int Id)` | 对应 ID 的玩家名字 |
| `string` | `client_info("clan", int Id)` | 对应 ID 的战队名字 |
| `string` | `client_info("skin_name", int Id)` | 对应 ID 的皮肤名称 |
| `int` | `client_info("skin_custom_color", int Id)` | 是否使用自定义颜色 |
| `int` | `client_info("skin_color_feet", int Id)` | 对应 ID 脚部颜色 |
| `int` | `client_info("skin_color_body", int Id)` | 对应 ID 身体颜色 |
| `bool` | `client_info("afk", int Id)` | 是否挂机 (AFK) |
| `bool` | `client_info("friend", int Id)` | 是否为好友 |
| `bool` | `client_info("foe", int Id)` | 是否为仇人 |
| `int` | `client_info("warlist_type", int Id)` | 仇人列表类型 ID |
| `string` | `client_info("warlist_type_name", int Id)` | 仇人列表类型名称 |
| `bool` | `client_info("muted", int Id)` | 是否已静音 |
| `int` | `client_info("auth_level", int Id)` | 对应 ID 的权限等级 |

### 辅助模块：数学运算 (`math`) 与正则表达式 (`re`)

```js
import("math")
math.pi
math.min(1, 10)
math.random(1, 100)

import("re")
if(re.test(re.compile(".+?ello.+?"), "hello")) {
	print("匹配成功")
}
```

</details>

---

## 🖼️ 设置界面预览

<details open>
<summary><b>点击展开界面截图</b></summary>

### 常规设置 (Main Settings)
<img width="1920" height="1854" alt="Settings" src="https://github.com/user-attachments/assets/9a9ea2cc-96bd-44b0-a4e5-fe68ed319ca1" />

### 视觉特效 (Visuals)
<img width="1920" height="2792" alt="Visuals" src="https://github.com/user-attachments/assets/91d2b82f-d013-476a-af10-2fd31de2869f" />

### 仇人与目标列表 (Warlist)
<img width="1920" height="1080" alt="Warlist" src="https://github.com/user-attachments/assets/4a8138c0-817b-452e-b29e-dcdd96643ce6" />

### 底部状态栏 (Status bar)
<img width="1920" height="1080" alt="Status bar" src="https://github.com/user-attachments/assets/26d4ec2f-2e23-4c78-98e5-c96bde360418" />

### 快捷绑定轮盘 (Bindwheel)
<img width="1920" height="1080" alt="Bindwheel" src="https://github.com/user-attachments/assets/942384fe-32a7-44c1-af8b-e54a5ccf01f9" />

### 玩家动作菜单 (Player actions)
<img width="1920" height="1080" alt="Player actions" src="https://github.com/user-attachments/assets/46436731-b867-4e71-a496-eaaf2b9b0bf7" />

### 关于与信息 (Info)
<img width="1920" height="1080" alt="Info" src="https://github.com/user-attachments/assets/09b589d4-3472-4ddd-98bd-04e0d5d7f7b9" />

</details>

---

## ⌨️ 常用控制台指令

<details>
<summary><b>点击展开常用命令清单</b></summary>

```text
votekick "<玩家名> <原因>"        # 发起踢人投票
onlineinfo                       # 查看在线玩家简报
saveskin                         # 保存当前皮肤方案
restoreskin                      # 恢复已保存的皮肤
playerinfo "<玩家名>"             # 查询玩家详细信息
addtempwar "<玩家名>"             # 临时添加仇人
deltempwar "<玩家名>"             # 临时移除仇人
addtemphelper "<玩家名>"          # 临时添加助手
deltemphelper "<玩家名>"          # 临时移除助手
addtempmute "<玩家名>"            # 临时屏蔽玩家聊天
deltempmute "<玩家名>"            # 取消临时屏蔽
view_link <链接>                 # 在默认浏览器中打开网址
server_rainbow_speed "<速度>"     # 调整服务器端彩虹速度
server_rainbow_both_players "<0|1>" # 主分身是否同时开启彩虹
server_rainbow_sat <饱和度> <0|1>  # 彩虹饱和度
server_rainbow_lht <亮度> <0|1>    # 彩虹亮度
server_rainbow_body <0|1> <0|1>   # 彩虹身体开关
server_rainbow_feet <0|1> <0|1>   # 彩虹脚部开关
reply_last <?消息内容>            # 快捷回复上一条消息
specid <ID>                      # 快速旁观指定 ID 的玩家
```
</details>

---

## 🤝 致谢与鸣谢 (Credits)

- **Entity-Client 原作者**：[qxdFox](https://github.com/qxdFox) 与 [Miro8D](https://github.com/Miro8D)
- **底层引擎与游戏**：[DDNet 团队](https://github.com/ddnet/ddnet)
- **中文汉化与本地化支持维护**：[gradiaun](https://github.com/gradiaun)
