@echo off
echo Building QuizOwn installer...

REM Build the application
cmake --build build --config Release

REM Create installer
makensis installer.nsi

echo Installer created successfully!
pause 