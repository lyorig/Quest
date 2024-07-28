 @echo off
 setlocal

 if not exist build/CMakeCache.txt (
	 echo Build type file not found. Did you forget to run cfg.sh?
	 exit /b 1
 )

for /f "tokens=2 delims==" %%g in (
	'findstr /L CMAKE_BUILD_TYPE build\CMakeCache.txt'
) do (
	set type=%%g
)

 echo Building config %type%

 cmake --build build -j --config %type%

 endlocal
