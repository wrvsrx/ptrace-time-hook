# Nix 打包使用说明

## 构建

使用 Nix flake 构建项目：

```bash
# 构建项目
nix build .#default

# 或使用开发环境
nix develop
```

## 安装

构建完成后，程序会被安装到以下位置：

- **主程序**: `result/bin/simple_hook` 和 `result/bin/time_hook`
- **测试程序**: `result/share/ptrace-time-hook/tests/`
- **文档**: `result/share/doc/ptrace-time-hook/`

## 使用

```bash
# 基本用法
result/bin/simple_hook <target_program>

# 测试示例
result/bin/simple_hook result/share/ptrace-time-hook/tests/minimal_test
```

## 测试

项目包含自动化测试，在构建过程中会自动运行：

1. **基本功能测试** - 验证程序正常编译和运行
2. **Hook 功能测试** - 验证 ptrace hook 能成功拦截和修改 time() 系统调用
3. **多次调用测试** - 验证对多个时间调用的一致性处理

测试输出示例：
```
Test 1: Verify programs work individually
✅ Test program runs successfully
Test 2: Verify hook intercepts and modifies time
✅ Hook successfully intercepted and modified time() syscall
Test 3: Verify multiple time call handling
✅ Hook successfully handled multiple time calls with consistent fixed value
All tests passed! ✅
```

## 开发环境

```bash
# 进入开发环境
nix develop

# 手动构建（在开发环境中）
cc -o simple_hook simple_hook.c
cc -o minimal_test minimal_test.c

# 手动测试
./simple_hook ./minimal_test
```

## 包信息

- **包名**: ptrace-time-hook
- **版本**: 1.0.0
- **平台**: x86_64-linux
- **依赖**: 无外部依赖，仅使用标准 C 库
- **许可证**: MIT

## 打包特性

- ✅ 完整的 Nix flake 支持
- ✅ 自动化测试 phase
- ✅ 文档和源码安装
- ✅ 开发环境支持
- ✅ 规范的目录结构