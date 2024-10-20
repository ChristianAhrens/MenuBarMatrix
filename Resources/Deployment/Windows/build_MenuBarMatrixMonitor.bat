@echo off

set VSDIR=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE
set WORKSPACE=..\..\..
set JUCEDIR=%WORKSPACE%\submodules\JUCE
set PROJUCERVSDIR=%JUCEDIR%\extras\Projucer\Builds\VisualStudio2022
echo.

echo Using variables:
echo JUCEDIR = %JUCEDIR%
echo VSDIR = %VSDIR%
echo PROJUCERVSDIR = %PROJUCERVSDIR%
echo WORKSPACE = %WORKSPACE%
echo.

echo Building Projucer binary
"%VSDIR%\devenv.com" %PROJUCERVSDIR%\Projucer.sln /build Release
echo.

echo Exporting Projucer project
"%PROJUCERVSDIR%\x64\Release\App\Projucer.exe" --resave %WORKSPACE%\MenuBarMatrixMonitor\MenuBarMatrixMonitor.jucer
echo.

echo Build release
"%VSDIR%\devenv.com" %WORKSPACE%\MenuBarMatrixMonitor\Builds\VisualStudio2022\MenuBarMatrixMonitor.sln /build Release
echo.
