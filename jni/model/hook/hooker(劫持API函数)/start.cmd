@echo off
color 0f
cd /d %~dp0
call jni\\build.cmd
cd /d %~dp0
echo ==================================
echo Push ...
adb push %~dp0libs\armeabi\hookclient /data/local/tmp

echo Chmod ...
adb shell su -c "chmod 777 /data/local/tmp/hookclient"

echo Execute ...
echo.
adb shell "ps | grep com.tomagoyaky"
set /p pid=«Î ‰»Îpid:
echo==================================
echo.
adb shell su -c "/data/local/tmp/hookclient %pid% /data/local/tmp/hello.so hook_entry aaaaaaaaaaaaa"
echo.
echo==================================
echo.
pause