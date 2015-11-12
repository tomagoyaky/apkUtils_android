@echo off
color 0f
cd /d %~dp0
call jni\\build.cmd
cd /d %~dp0
echo==========================================================================================================
echo Push ...
adb push %~dp0libs\armeabi\search /data/local/tmp

echo Chmod ...
adb shell su -c "chmod 777 /data/local/tmp/search"

echo Execute ...
echo.
echo==========================================================================================================
echo.
rem adb shell "ps | grep com.tomagoyaky"
adb shell "ps | grep com.tomagoyaky"
set /p pid=请输入pid:
rem adb shell su -c "cat /proc/%pid%/maps"
adb shell su -c "/data/local/tmp/search" %pid% "17"
echo.
echo==========================================================================================================
echo 脚本运行完毕
echo==========================================================================================================
echo.
pause