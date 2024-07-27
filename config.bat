@echo off
setlocal

set "configs=d ^(Debug^), r ^(Release^), rd ^(RelWithDebInfo^), rs ^(MinSizeRel^)"

set "build_type="
set "tests=OFF"

if "%~1"=="" (
    echo A single parameter is required: %configs%.
    exit /b 1
)

set "cmd=cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DHalcyon_BUILD_EXAMPLES=ON"

if "%~1"=="d" (
    set "build_type=Debug"
    set "tests=ON"
) else if "%~1"=="r" (
    set "build_type=Release"
) else if "%~1"=="rd" (
    set "build_type=RelWithDebInfo"
) else if "%~1"=="rs" (
    set "build_type=MinSizeRel"
) else (
    echo Invalid configuration "%~1". Valid ones are %configs%.
    exit /b 1
)

echo %build_type% > type.txt

%cmd% -DCMAKE_BUILD_TYPE=%build_type% -DHalcyon_BUILD_TESTS=%tests%

endlocal
