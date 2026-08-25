# Entity-Client-DDNet-Zh (中文增强定制版)

本分支基于经典 DDNet 客户端扩展 [FoxNet-DDNet/Entity-Client-DDNet](https://github.com/FoxNet-DDNet/Entity-Client-DDNet) 进行**深度定制与中文本地化**。

在完整继承原版 E-Client 特性的基础上，重构了本地化系统、独立拆分出「扩展设置」面板、加入了「内置指令列表与一键复制」、皮肤窃取、优化了 Gores 自动切枪与分身不断钩锤击等实用功能。

---

## 📥 获取与下载方式

1. **Releases 页面直接下载**（推荐）：
   - 点击进入本仓库右侧的 [Releases 页面](../../releases)，下载最新的发布包压缩文件（如 `DDNet-*-win64.zip`）。
2. **GitHub Actions 云端最新构建**：
   - 进入本仓库的 [Actions 页面](../../actions) -> 点击最新的 **Fast Build** -> 在底部的 **Artifacts** 区域下载 `ddnet-windows-latest`。

解压后直接双击运行 `DDNet.exe` 即可畅玩。

---

## ✨ 本分支专属增强特性与使用说明

### 1. 📋 内置指令列表页与一键复制 (Commands List)
* **位置**：游戏主界面点击 **「设置」 -> 「扩展 (Extensions)」 -> 「指令列表」**。
* **特性**：
  * **实时搜索**：输入关键字（如 `steal`、`gores`、`dummy`、`开火`、`武器`）即时筛选过滤。
  * **一键复制**：点击任意指令右侧的 **「复制」** 按钮，指令会自动复制到操作系统剪贴板，并在游戏内提示。
  * **宏绑定利器**：方便玩家在 F1 控制台组合复杂的 `bind` 快捷指令（例如：`bind mouse1 "+fire; steal_skin"`）。

### 2. 🎭 皮肤窃取 / 快速复制 (`steal_skin` / `copy_skin`)
* **功能**：自动检索距离自己最近的玩家，一键将自己的皮肤名称、身体颜色、脚部颜色完全同步为其外观。
* **使用方式**：
  * **控制台指令**：按 F1 输入 `steal_skin`（或 `copy_skin`）。
  * **一键绑定快捷键**：在 F1 中输入 `bind <按键> steal_skin`（例如 `bind k steal_skin`）。
  * **图形化设置**：进入 **「设置」 -> 「扩展」 -> 「皮肤窃取」**，直接点击录制快捷键，或点击「立即复制最近玩家的皮肤」按钮。

### 3. 🔨 Gores 模式深度优化 (开火固定切锤)
* **开火绝对切锤**：原版采用 `+prevweapon`（切上一个武器）极易因按键顺序错位导致“开火变成切枪开火，导致挥锤挥空”。本分支改为 **固定切锤（`+weapon1`）**，彻底杜绝打空反转。
* **智能单次禁用**：在开启「持有重武器时自动暂停」后，当玩家刚刚拾取激光/散弹/榴弹时，仅执行一次自动关闭；如果玩家手动重新开启 Gores 模式，仍可在持有重武器的状态下强制切锤挥击。
* **快捷按键开关**：新增控制台命令 `toggle_gores_mode`，可在 **「设置」 -> 「扩展」 -> 「Gores Mode」** 直接录制开关按键。

### 4. 🪝 分身锤击保持钩爪抓取 (`ec_dummy_hammer_keep_hook`)
* **痛点解决**：原版在启用分身锤击（Hammerfly / `cl_dummy_hammer 1`）时，分身出锤前会被强制重置松开钩爪。
* **开启后效果**：分身在执行自动锤击时**全程保持钩爪抓取状态**，不再松钩，双人锤飞/飞图控制更加稳定。
* **设置方法**：进入 **「设置」 -> 「扩展」 -> 「分身锤击设置」** 勾选 **`分身锤击时保持钩爪抓取 (不松钩)`**，或绑定快捷键 `toggle ec_dummy_hammer_keep_hook 0 1`。

### 5. 👁️ 观战/暂停模式「仅看同队成员」纯净视野 (`ec_spec_team_only`)
* **功能**：在跑图中使用 `/pause` 或 `/spec` 观战查看队友路线时，客户端**全自动隐藏所有非同队玩家**（包括人物模型、名字、钩爪、激光、弹幕及状态条），彻底消除其他路人队伍的视觉干扰。
* **设置方法**：在 **「设置」 -> 「扩展」** 勾选 **`旁观模式仅显示同队成员`**，或输入 `toggle cl_spec_team_only 0 1`。

### 6. 🇨🇳 深度汉化与外部语言包自定义指南
* **内嵌零配置汉化**：源码内嵌了完整的中文对照字典，只要游戏语言选择简体中文，所有 E-Client 及扩展设置**全自动显示为中文**。
* **如何自定义/修改或制作其他语言包**：
  1. 游戏运行后，配置与用户数据默认保存在系统应用目录：
     * **Windows**：按 `Win + R` 输入 `%appdata%\DDNet`（即 `C:\Users\<用户名>\AppData\Roaming\DDNet`）。
     * **Linux / macOS**：`~/.local/share/ddnet` 或 `~/Library/Application Support/DDNet`。
  2. 如果想自行调整中文词条或制作其他语言翻译，可在游戏根目录下的 `data/languages/` 文件夹中找到对应语言文件（例如 `simplified_chinese.txt`）。
  3. 按照格式添加或修改翻译映射：
     ```text
     英文原词条
     == 你的自定义中文翻译
     ```
  4. 修改保存后，在游戏主菜单重新切换一次语言即可热重载生效。

---

## 🛠️ 常用扩展指令速查

| 指令 | 默认参数/类型 | 说明 |
| :--- | :--- | :--- |
| `steal_skin` / `copy_skin` | 指令 | 复制/窃取最近玩家的皮肤与颜色 |
| `toggle_gores_mode` | 指令 | 一键切换开启/关闭 Gores 自动切枪模式 |
| `toggle ec_dummy_hammer_keep_hook 0 1` | 开关 (0/1) | 切换分身锤击是否保持抓钩状态 |
| `toggle cl_spec_team_only 0 1` | 开关 (0/1) | 切换观战是否仅显示同队队友 |
| `bindwheel` | 指令 | 呼出自定义快捷绑定轮盘 |
| `playeractions` | 指令 | 呼出针对目标玩家的动作菜单 |
| `toggle cl_dummy 0 1` | 开关 (0/1) | 切换当前操作本体还是控制分身 |
| `toggle cl_dummy_hammer 0 1` | 开关 (0/1) | 切换分身自动锤击 |

---

## 🤝 致谢与鸣谢 (Credits)

- **E-Client 原作者**：[qxdFox](https://github.com/qxdFox) & [Miro8D](https://github.com/Miro8D)
- **底层架构**：[DDNet 官方开发团队](https://github.com/ddnet/ddnet)
- **中文分支维护与定制**：[gradiaun](https://github.com/gradiaun)

