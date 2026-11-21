# CodeConvert

基于 C++ + Qt6 开发的文件编码转换工具

## 功能特性

- ✅ 批量转换文件编码（UTF-8 BOM / UTF-8 / GB2312），支持 UTF-8 与 UTF-8 BOM 互相转换
- ✅ 自动检测源编码（uchardet），支持 UTF-16LE/BE、GB2312/GBK、Big5、Shift_JIS 等
- ✅ 支持单文件或文件夹递归处理
- ✅ 支持文件/文件夹拖拽导入
- ✅ 支持多种文件类型（.c, .h, .cpp, .hpp, .txt, .bat, .py, .java）及自定义文件后缀
- ✅ 多线程处理，不阻塞 UI
- ✅ 实时显示处理进度，结果表格支持筛选（全部 / 仅成功 / 仅失败）
- ✅ 原子写入：转换失败不会损坏原文件；存在无法转换的字符时会在结果中提示

## 技术栈

| 组件 | 用途 |
|------|------|
| **Qt 6** | GUI 框架 |
| **uchardet** | 编码检测 |
| **QTextCodec** | 编码转换 |
| **CMake** | 构建系统 |

## 项目结构

```
CodeConvert/
├── CMakeLists.txt          # 构建配置（版本号唯一来源）
├── version.h.in            # 版本头文件模板（由 CMake 生成 version.h）
├── package.nsi             # NSIS 便携单文件打包脚本
├── README.md               # 说明文档
├── src/
│   ├── main.cpp            # 程序入口
│   ├── mainwindow.h/cpp    # 主窗口
│   ├── mainwindow.ui       # 界面文件
│   ├── worker.h/cpp        # 工作线程
│   └── encodedetector.h/cpp # 编码检测器
└── test/                   # 手工测试样例（各编码的样例文件）
```

## 依赖

- **Qt 6** (6.3+)
- **uchardet**：未安装时会通过 CMake FetchContent 自动下载源码构建，无需手工安装
- **CMake** (3.16+)
- **C++17 编译器** (GCC 9+, Clang 10+, MSVC 2019+)

## 构建

```bash
# 创建构建目录
mkdir build
cd build

# 配置
cmake ..

# 编译
cmake --build .

# 运行（Windows GUI 程序）
CodeConvert.exe
```

## 使用方法

1. 选择「文件」或「文件夹」标签页，点击按钮选择路径，或直接把文件/文件夹拖入窗口
2. 文件夹模式下勾选需要处理的文件后缀，或在输入框中添加自定义后缀（如 `.log`）
3. 选择目标编码（UTF-8 BOM / UTF-8 / GB2312），点击「转换」
4. 结果表格可按列排序、按结果筛选，日志区可折叠查看详细过程

## 打包便携版（Windows，可选）

1. 构建完成后，把 `CodeConvert.exe` 及其依赖的 Qt DLL 收集到 `dist\` 目录
2. 安装 [NSIS](https://nsis.sourceforge.io/)，执行：

```bash
makensis package.nsi
```

生成 `CodeConvert-Portable.exe`：双击静默解压到 `%LOCALAPPDATA%\CodeConvert` 并启动，重复打包后自动刷新缓存。

## 编码检测

使用 **uchardet** 进行编码检测：
- 来自 Mozilla 项目，与 Python chardet 同源
- 支持 UTF-8、GB2312、Big5、Shift_JIS 等常见编码

## 许可证

[MIT License](LICENSE)
