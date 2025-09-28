# 编译器原理与实践探索项目

本项目旨在通过实践操作，深入理解编译器的工作原理。通过使用 GCC 和 LLVM/Clang 工具链，我们将探索从高级语言（C 和 SysY）到不同目标架构（x86, ARM）机器码的完整编译流程。

项目核心位于 `lab1` 目录，其中包含源代码、库文件以及一个强大的 `Makefile` 来自动化所有编译和实验步骤。

## 特性与学习要点

- **多语言支持**: 支持 C 语言和类 C 的 SysY 语言。
- **双工具链**: 同时使用 GCC 和 LLVM/Clang，对比不同编译器的中间产物。
- **全流程探索**: 可生成从预处理、词法/语法分析、中间表示 (GIMPLE, LLVM IR) 到最终汇编代码的各个阶段产物。
- **LLVM 优化**: 集成了 `opt` 工具，可以方便地对 LLVM IR 应用各种优化遍 (Passes)，并提供多种方式来验证优化效果。
- **交叉编译**: 支持将代码交叉编译为 ARM aarch64 架构的汇编和可执行文件。
- **模拟执行**: 使用 QEMU 在 x86 主机上模拟运行 ARM 架构的可执行文件。
- **静态链接**: 演示了如何将代码与自定义的 SysY 静态库 (`.a` 文件) 进行链接。

## 环境要求

在开始之前，请确保您的系统（推荐使用 Ubuntu 或 WSL）已安装以下工具：

```bash
sudo apt-get update
sudo apt-get install -y build-essential gcc clang llvm make graphviz
# 用于交叉编译和模拟执行 ARM aarch64 程序
sudo apt-get install -y gcc-aarch64-linux-gnu qemu-user-static
```

.
├── lab1/
│   ├── main.c            # C 语言主源文件
│   ├── test.sy           # SysY 语言源文件
│   ├── test.c            # test.sy 的 C 语言等效实现
│   ├── test.ll           # test.c 生成的 LLVM IR
│   ├── test-arm.S        # test.c 生成的 ARM 汇编
│   ├── Makefile          # 自动化构建脚本
│   └── ...               # 其他编译过程中生成的中间文件
└── lib/
    ├── libsysy_aarch.a   # 预编译的 ARM 架构 SysY 库
    └── libsysy_x86.a     # 预编译的 x86 架构 SysY 库