# ptrace Time Hook 工具

这是一个使用 ptrace 技术来拦截和修改程序中时间相关系统调用的工具。

## 功能

- ✅ **已验证有效** - 拦截 `time()` 系统调用并返回固定时间值
- ✅ 支持对静态链接和动态链接的二进制文件进行 hook
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
cc -o simple_hook src/main.c
cc -o tests/minimal_test tests/minimal_test.c
cc -o tests/multi_time_test tests/multi_time_test.c
```

## 使用方法

```bash
# 对任意二进制文件进行时间 hook
./simple_hook <target_binary> [args...]

# 示例：hook 测试程序
./simple_hook ./tests/multi_time_test
./simple_hook ./tests/minimal_test
```

## 验证结果

```bash
# 对比原始输出和 hook 后输出
echo "=== 原始 ===" && ./tests/minimal_test
echo "=== Hook ===" && ./simple_hook ./tests/minimal_test
```

输出示例：
```
=== 原始 ===
Time: 1754549070
=== Hook ===
Time: 1640995200
Found time() syscall!
Modified time() return to 1640995200
```

## 固定时间值

程序中设置的固定时间为：`1640995200` (2022-01-01 00:00:00 UTC)

你可以修改 `src/main.c` 中的 `FIXED_TIME` 宏来设置不同的时间值。

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