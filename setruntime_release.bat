echo off

SET var=%~dp0
echo "SRHydra_win64.json"
reg add "HKLM\SOFTWARE\Khronos\OpenXR\1" /f /v "ActiveRuntime" /t "REG_EXPAND_SZ" /d "%var%XRGameBridge_RELEASE.json"

pause
