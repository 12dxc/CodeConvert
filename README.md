# CodeConvert

基于 C++ + Qt6 开发的文件编码转换工具

## 功能特性

- ✅ 批量转换文件编码（UTF-8 BOM / UTF-8 / GB2312）
- ✅ 支持单文件或文件夹递归处理
- ✅ 支持多种文件类型（.c, .h, .cpp, .hpp, .txt, .bat, .py, .java）
- ✅ 支持自定义文件后缀
- ✅ 多线程处理，不阻塞 UI
- ✅ 实时显示处理进度和结果

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
├── CMakeLists.txt          # 构建配置
├── README.md               # 说明文档
└── src/
    ├── main.cpp            # 程序入口
    ├── mainwindow.h/cpp    # 主窗口
    ├── mainwindow.ui       # 界面文件
    ├── worker.h/cpp        # 工作线程
    └── encodedetector.h/cpp # 编码检测器
```

## 依赖

- **Qt 6** (6.5+)
- **uchardet** (1.0+)
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

# 运行
./CodeConvert
```

## 编码检测

使用 **uchardet** 进行编码检测：
- 来自 Mozilla 项目，与 Python chardet 同源
- 支持 UTF-8、GB2312、Big5、Shift_JIS 等常见编码

## 许可证

MIT License
