 @echo off
 setlocal

 if not exist type.txt (
	 echo Build type file not found. Did you forget to run cfg.sh?
	 exit /b 1
 )

 set /p type=<type.txt

 echo Building config %type%

 cmake --build build --config %type%

 endlocal
