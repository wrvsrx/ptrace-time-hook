# time-hook

一个使用 ptrace 技术来拦截和修改程序中 time() 系统调用的工具。

## 功能

- ✅ **已验证有效** - 拦截 `time()` 系统调用并返回指定时间值
- ✅ 支持对静态链接和动态链接的二进制文件进行 hook
- ✅ 可配置的目标时间（Unix 时间戳）
- ✅ 可选的详细输出模式
- ✅ 实时修改系统调用返回值

## 文件结构

```
├── src/
│   └── main.c              # ptrace hook 程序主文件
├── tests/
│   ├── minimal_test.c      # 最小化测试程序（无库依赖）
│   └── multi_time_test.c   # 多次时间调用测试程序
├── flake.nix               # Nix flake 配置
├── default.nix             # Nix 打包配置
└── README.md               # 项目文档
```

## 编译

```bash
# 使用 Makefile
make all

# 或手动编译
cc -o time-hook src/main.c
cc -o tests/minimal_test tests/minimal_test.c
cc -o tests/multi_time_test tests/multi_time_test.c
```

## 使用方法

```bash
# 基本用法
./time-hook [OPTIONS] <program> [args...]

# 选项
#   -v, --verbose     启用详细输出
#   -t, --time TIME   设置目标时间（Unix 时间戳，默认为 0）
#   -h, --help        显示帮助信息

# 示例
./time-hook ./tests/minimal_test                    # 默认时间为 0 (1970-01-01)
./time-hook --time 1640995200 ./tests/minimal_test  # 设置为 2022-01-01 00:00:00 UTC
./time-hook --verbose ./tests/multi_time_test       # 启用详细输出
./time-hook -t 999999999 -v ./tests/minimal_test    # 短选项形式
```

## 验证结果

```bash
# 对比原始输出和 hook 后输出
echo "=== 原始 ===" && ./tests/minimal_test
echo "=== Hook (默认) ===" && ./time-hook ./tests/minimal_test
echo "=== Hook (详细) ===" && ./time-hook --verbose ./tests/minimal_test
```

输出示例：
```
=== 原始 ===
Time: 1754549070

=== Hook (默认) ===
Time: 0

=== Hook (详细) ===
Time: 0
time-hook: Target program: ./tests/minimal_test
time-hook: Target time: 0
time-hook: Found time() syscall
time-hook: Modified time() return to 0
time-hook: Process finished
```

## 时间配置

- **默认时间**: `0` (1970-01-01 00:00:00 UTC, Unix 纪元)
- **自定义时间**: 使用 `--time` 选项指定任意 Unix 时间戳

常用时间戳：
- `0` - 1970-01-01 00:00:00 UTC (Unix 纪元)
- `1640995200` - 2022-01-01 00:00:00 UTC
- `1577836800` - 2020-01-01 00:00:00 UTC

## 技术说明

该工具使用 Linux 的 ptrace 系统调用来：
1. 跟踪目标进程的系统调用
2. 识别 time() 系统调用
3. 在系统调用返回时修改返回值

## 限制

- 仅支持 Linux x86_64 架构
- 需要 root 权限或适当的 ptrace 权限
- 对于使用 vDSO 优化的时间调用可能无效
- 只能 hook 实际执行系统调用的程序

## 防御性用途

这个工具主要用于：
- 程序分析和调试
- 安全测试
- 时间敏感应用的测试
- 恶意软件分析（沙箱环境）