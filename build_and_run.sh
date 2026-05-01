#!/bin/bash

# 时间旅行调试器构建和运行脚本
# 适用于Linux/macOS系统

set -e  # 遇到错误时退出脚本

echo "=== 时间旅行调试器构建脚本 ==="
echo "当前目录: $(pwd)"

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "错误: CMake未安装"
    echo "请安装CMake:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  macOS: brew install cmake"
    echo "  Windows: 从 https://cmake.org/download/ 下载安装"
    exit 1
fi

# 检查Qt是否安装
if ! command -v qmake &> /dev/null; then
    echo "警告: qmake未找到，可能Qt未安装或未在PATH中"
    echo "请确保Qt6已安装并正确配置"
fi

# 创建构建目录
if [ ! -d "build" ]; then
    echo "创建构建目录..."
    mkdir build
fi

cd build

# 清理之前的构建
echo "清理构建目录..."
rm -rf *

# 配置项目
echo "配置CMake项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "编译项目..."
cmake --build . --config Release

# 检查可执行文件
if [ -f "TimeTravelDebugger" ]; then
    echo "构建成功！可执行文件: $(pwd)/TimeTravelDebugger"

    # 询问是否运行程序
    read -p "是否运行程序？(y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "运行时间旅行调试器..."
        ./TimeTravelDebugger
    fi
elif [ -f "TimeTravelDebugger.exe" ]; then
    echo "构建成功！可执行文件: $(pwd)/TimeTravelDebugger.exe"
    echo "请在Windows资源管理器中双击运行"
else
    echo "错误: 可执行文件未找到"
    echo "构建可能失败，请检查错误信息"
    exit 1
fi

cd ..