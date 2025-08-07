{ stdenv, lib }:

stdenv.mkDerivation rec {
  pname = "ptrace-time-hook";
  version = "1.0.0";
  
  src = ./.;

  # No external dependencies needed - only standard C library
  nativeBuildInputs = [ ];
  
  # Source files to include in build
  sourceFiles = [
    "src/main.c"
    "tests/multi_time_test.c"
    "tests/minimal_test.c"
    "README.md"
  ];

  buildPhase = ''
    runHook preBuild
    
    echo "Building ptrace time hook programs..."
    
    # Build main hook program
    $CC -Wall -Wextra -g -o simple_hook src/main.c
    
    # Build test programs  
    $CC -Wall -Wextra -g -o multi_time_test tests/multi_time_test.c
    $CC -Wall -Wextra -g -o minimal_test tests/minimal_test.c
    
    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall
    
    # Create output directories
    mkdir -p $out/bin
    mkdir -p $out/share/doc/ptrace-time-hook
    mkdir -p $out/share/ptrace-time-hook/tests
    
    # Install main program
    install -m755 simple_hook $out/bin/
    
    # Install test programs
    install -m755 multi_time_test $out/share/ptrace-time-hook/tests/
    install -m755 minimal_test $out/share/ptrace-time-hook/tests/
    
    # Install documentation
    install -m644 README.md $out/share/doc/ptrace-time-hook/
    
    # Install source files for reference
    install -m644 src/main.c $out/share/doc/ptrace-time-hook/
    
    runHook postInstall
  '';

  doCheck = true;
  
  checkPhase = ''
    runHook preCheck
    
    echo "Running ptrace time hook tests..."
    
    # Test 1: Basic functionality test
    echo "Test 1: Verify programs work individually"
    ./minimal_test > original_output.txt
    echo "✅ Test program runs successfully"
    
    # Test 2: Hook functionality test  
    echo "Test 2: Verify hook intercepts and modifies time"
    timeout 10s ./simple_hook ./minimal_test > hooked_output.txt 2>&1 || true
    
    if grep -q "Found time() syscall!" hooked_output.txt && grep -q "Time: 1640995200" hooked_output.txt; then
      echo "✅ Hook successfully intercepted and modified time() syscall"
    else
      echo "❌ Hook test failed"
      echo "Hook output:"
      cat hooked_output.txt
      exit 1
    fi
    
    # Test 3: Multiple time calls test
    echo "Test 3: Verify multiple time call handling"
    timeout 15s ./simple_hook ./multi_time_test > multi_output.txt 2>&1 || true
    
    if grep -q "Call 1: time = 1640995200" multi_output.txt && grep -q "Call 3: time = 1640995200" multi_output.txt; then
      echo "✅ Hook successfully handled multiple time calls with consistent fixed value"
    else
      echo "❌ Multiple time calls test failed"
      echo "Multi-test output:"
      cat multi_output.txt
      exit 1
    fi
    
    echo "All tests passed! ✅"
    
    runHook postCheck
  '';

  meta = with lib; {
    description = "A ptrace-based tool to hook and modify time() syscalls in Linux programs";
    longDescription = ''
      This tool uses Linux ptrace to intercept time() system calls in target programs 
      and replace the returned time with a fixed value. It's useful for program analysis,
      security testing, and malware analysis in sandboxed environments.
      
      Features:
      - Intercepts time() syscalls and returns fixed time value (2022-01-01 00:00:00 UTC)  
      - Works with both statically and dynamically linked binaries
      - Real-time syscall modification
      - Defensive security tool for analysis purposes
    '';
    homepage = "https://github.com/example/ptrace-time-hook";
    license = licenses.mit;
    platforms = [ "x86_64-linux" ];
    maintainers = [ ];
  };
}
