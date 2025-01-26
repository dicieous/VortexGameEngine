pushd %~dp0\..\
call vendor\bin\premake\premake5.exe vs2022
popd
PAUSE

if %errorlevel% neq 0 (
    echo Build failed. Exiting...
    PAUSE
    exit /b %errorlevel%
)