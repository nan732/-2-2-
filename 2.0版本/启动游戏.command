#!/bin/zsh
set -e

cd "$(dirname "$0")"

if [ -f build/CMakeCache.txt ] && ! grep -q "CMAKE_HOME_DIRECTORY:INTERNAL=$(pwd)" build/CMakeCache.txt; then
    echo "检测到 build 缓存来自旧文件夹，正在重新生成..."
    rm -rf build
fi

cmake -S . -B build
cmake --build build

if [ -x "./build/TrapMindBall" ]; then
    ./build/TrapMindBall
else
    game=""
    while IFS= read -r candidate; do
        if [ -x "$candidate" ]; then
            game="$candidate"
            break
        fi
    done < <(find ./build -maxdepth 3 -type f -name 'TrapMindBall*')
    if [ -n "$game" ]; then
        "$game"
    else
        echo "没有找到游戏可执行文件。"
        read "?按回车退出..."
        exit 1
    fi
fi
