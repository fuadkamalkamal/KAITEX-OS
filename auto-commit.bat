@echo off
echo ==========================================
echo       AUTO COMMIT ^& PUSH TO GITHUB
echo ==========================================
echo.

set /p commit_msg="Masukkan pesan commit (atau tekan Enter untuk 'Auto commit: update files'): "
if "%commit_msg%"=="" set commit_msg=Auto commit: update files

echo.
echo Menambahkan file ke Git (git add .)...
git add .

echo.
echo Melakukan commit...
git commit -m "%commit_msg%"

echo.
echo Mengirim ke GitHub (git push)...
git push

echo.
echo ==========================================
echo ✅ Selesai!
echo ==========================================
pause
