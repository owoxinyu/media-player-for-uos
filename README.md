# Media Player for UOS

一个基于 Qt 框架开发的智能多媒体播放器，专为 UOS（统信操作系统）设计。

## 项目概述

Media Player for UOS 是一个功能丰富的多媒体播放器，具有以下核心特性：

- **多媒体播放**: 支持音频和视频文件的播放
- **智能推荐**: 基于机器学习的音乐推荐系统
- **现代化 UI**: 支持浅色、深色、跟随系统主题切换
- **播放列表管理**: 完整的播放列表功能
- **快捷键支持**: 可自定义的快捷键设置

### 核心播放功能

- 正常在 UOS 系统运行
- 支持常见的音频视频格式：mp3、wav、mp4、avi、mkv 等格式的播放
- 支持音量调节
- 支持进度条显示和进度调整
- 支持倍速播放，倍速播放时音视频需同步
- 支持历史播放列表和搜索功能
- 主题切换（浅色、深色、跟随系统）
- 默认播放模式设置（顺序播放、随机播放、单个播放、单个循环、列表循环）
- 快捷键设置（默认或自定义设置）

### 音乐推荐系统

- 结合歌曲音频特征与用户播放行为进行推荐
- 融合 K-Means 聚类、自动编码器和协同过滤算法
- 生成个性化音乐推荐列表

### 用户界面
- 支持窗口大小调整
- 浅色、深色、跟随系统主题切换
- 播放控制栏
- 播放列表面板
- 设置面板
- 情感分析界面

## 项目结构

```text
media-player-for-uos/
├── CMakeLists.txt                    # 项目构建配置
├── README.md                         # 项目说明
├── .gitignore                        # Git 忽略文件
├── src/                              # 播放器源码
│   ├── CMakeLists.txt                # 播放器构建配置
│   ├── main.cpp                      # 程序入口
│   ├── mainwindow.[cpp/h/ui]         # 主窗口
│   ├── playbar.[cpp/h/ui]            # 播放控制栏
│   ├── playlist.[cpp/h/ui]           # 播放列表
│   ├── setting.[cpp/h/ui]            # 设置界面
│   ├── shortcutkeyedititem.[cpp/h/ui] # 快捷键组件
│   ├── modelchat.[cpp/h/ui]          # 模型交互
│   ├── grableprogressbar.[cpp/h]     # 可拖动进度条
│   └── mytimeformater.[cpp/h]        # 时间格式化
├── resources/                        # 资源文件
│   ├── qss.qrc                       # Qt 资源配置
│   ├── images/                       # 界面图标
│   ├── qss/
│   │   ├── dark/                     # 深色主题
│   │   └── white/                    # 浅色主题
│   └── icons/                        # 应用图标
├── modules/
│   └── music_recommendation/         # 音乐推荐模块
│       ├── recommender.py            # 主推荐算法
│       ├── requirements.txt          # Python 依赖
│       ├── data*.csv                 # 歌曲数据集
│       └── user_data*.csv            # 用户数据集
├── packaging/linux/
│   └── media-player-for-uos.desktop  # 桌面入口
└── scripts/
    ├── configure-uos.sh              # 依赖安装
    └── install-uos.sh                # 构建安装
```

## 技术栈

- **Qt 6.7.2**：跨平台应用框架
- **C++17**：主要编程语言
- **Qt Widgets**：用户界面框架
- **Qt Multimedia**：多媒体播放框架
- **Qt WebSockets**：网络通信组件
- **QSS**：Qt 样式表
- **CMake**：项目构建工具
- **Python 3.8+**：推荐算法实现
- **TensorFlow / Keras**：深度学习框架
- **scikit-learn**：机器学习库
- **pandas / NumPy**：数据处理库
- **FFmpeg**：多媒体处理工具

## 安装与部署
1. 在 Release 中下载并解压源码包
2. 执行 `scripts/configure-uos.sh`
3. 执行 `scripts/install-uos.sh` 完成构建和安装

### 前置要求
- UOS操作系统或兼容的Linux发行版
- Qt 6 开发环境
- Python 3.8+
- FFmpeg

### 编译安装

1. **克隆项目**
```bash
git clone https://github.com/owoxinyu/media-player-for-uos.git
cd media-player-for-uos
```

2. **配置项目**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

3. **编译项目**
```bash
cmake --build build --parallel
```

4. **安装**
```bash
sudo cmake --install build
```

### Python依赖安装
```bash
cd modules/music_recommendation
pip install -r requirements.txt
```

## 使用方法

### 启动播放器
```bash
media-player-for-uos
```

### 基本操作
- **播放/暂停**: 空格键或播放按钮
- **音量调节**: 鼠标滚轮或滑块控制
- **切换歌曲**: 上一曲/下一曲按钮
- **主题切换**: 设置面板中选择主题

### 音乐推荐
1. 在播放列表中右键歌曲
2. 选择"推荐相似歌曲"
3. 系统会基于当前歌曲推荐相似音乐
