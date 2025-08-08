# time-hook

A tool that uses ptrace technology to intercept and modify time() system calls in programs.

## Features

- ✅ **Verified Working** - Intercepts `time()` system calls and returns specified time values
- ✅ Supports hooking both statically and dynamically linked binaries
- ✅ Configurable target time (Unix timestamp)
- ✅ Optional verbose output mode
- ✅ Real-time system call return value modification

## File Structure

```
├── src/
│   └── main.c              # Main ptrace hook program file
├── tests/
│   ├── minimal_test.c      # Minimal test program (no library dependencies)
│   └── multi_time_test.c   # Multiple time call test program
├── flake.nix               # Nix flake configuration
├── default.nix             # Nix package configuration
└── README.md               # Project documentation
```

## Building

```bash
make
```

## Usage

```bash
# Basic usage
./ptrace-time-hook [OPTIONS] <program> [args...]

# Options
#   -v, --verbose     Enable verbose output
#   -t, --time TIME   Set target time (Unix timestamp, default 0)
#   -h, --help        Show help information

# Examples
./ptrace-time-hook ./tests/minimal_test                    # Default time is 0 (1970-01-01)
./ptrace-time-hook --time 1640995200 ./tests/minimal_test  # Set to 2022-01-01 00:00:00 UTC
./ptrace-time-hook --verbose ./tests/multi_time_test       # Enable verbose output
./ptrace-time-hook -t 999999999 -v ./tests/minimal_test    # Short option format
```

## Verifying Results

```bash
# Compare original output with hooked output
echo "=== Original ===" && ./tests/minimal_test
echo "=== Hook (default) ===" && ./ptrace-time-hook ./tests/minimal_test
echo "=== Hook (verbose) ===" && ./ptrace-time-hook --verbose ./tests/minimal_test
```

Example output:
```
=== Original ===
Time: 1754549070

=== Hook (default) ===
Time: 0

=== Hook (verbose) ===
Time: 0
ptrace-time-hook: Target program: ./tests/minimal_test
ptrace-time-hook: Target time: 0
ptrace-time-hook: Found time() syscall
ptrace-time-hook: Modified time() return to 0
ptrace-time-hook: Process finished
```

## Time Configuration

- **Default time**: `0` (1970-01-01 00:00:00 UTC, Unix epoch)
- **Custom time**: Use `--time` option to specify any Unix timestamp

Common timestamps:

- `0` - 1970-01-01 00:00:00 UTC (Unix epoch)
- `1640995200` - 2022-01-01 00:00:00 UTC
- `1577836800` - 2020-01-01 00:00:00 UTC

## Technical Details

This tool uses Linux's ptrace system call to:

1. Trace target process system calls
2. Identify time() system calls
3. Modify return values when system calls return

## Limitations

- Only supports Linux x86_64 architecture
- Requires root privileges or appropriate ptrace permissions
- May not work with vDSO-optimized time calls
- Can only hook programs that actually execute system calls

## Defensive Use Cases

This tool is primarily intended for:
- Program analysis and debugging
- Security testing
- Testing time-sensitive applications
- Malware analysis (sandboxed environments)