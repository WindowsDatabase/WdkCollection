@echo off
setlocal EnableDelayedExpansion

REM Usage:
if "%1"=="" (
  echo espsmvprint: Translates C/C++ code to ESP CFG.
  echo.
  echo espsmvprint args-to-be-passed-to-cl
  echo.
  echo Example to translate foo.c to foo.c.txt: 
  echo   espsmvprint foo.c
  echo.
  exit /b 1
)

REM cfgpersist.dll:
set CFGPERSIST=%~DP0cfgpersist.dll
if not exist "%CFGPERSIST%" (
  echo espsmvprint: Error: Could not find %CFGPERSIST%
  exit /b 1
)

REM espsmvprintaux.exe:
set ESPSMVPRINTAUX=%~DP0espsmvprintaux.exe
if not exist "%ESPSMVPRINTAUX%" (
  echo espsmvprint: Error: Could not find %ESPSMVPRINTAUX%
  exit /b 1
)

REM cl:
set COMPILER=cl
where %COMPILER% >NUL 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  echo espsmvprint: Error: Could not find compiler %COMPILER%
  exit /b %ERRORLEVEL%
)

REM Cleanup stale local rawcfgf files:
del 2>NUL *.rawcfgf

REM Set output cfgs to local folder with same name as input:
set ESP.CfgPersist.PersistFile=%CD%\$SOURCEFILE.rawcfgf

REM Write cfgf files:
%COMPILER% /nologo /analyze:only /analyze:autolog- /astfe:d1Adisablecoreplugins /analyze:plugin %CFGPERSIST% %*

if NOT %ERRORLEVEL% EQU 0 (
  echo espsmvprint: Error: Call to cl failed with error %ERRORLEVEL%
  exit /b %ERRORLEVEL%
)

REM Write txt files:
for %%f in (*.rawcfgf) do (
  %ESPSMVPRINTAUX% %%f >%%~nf.txt
  if NOT !ERRORLEVEL! EQU 0 (
    echo espsmvprint: Error: Call to espsmvprintaux failed with error !ERRORLEVEL!
    exit /b !ERRORLEVEL!
  )
  echo espsmvprint: Module %%~nf translated to ESP CFG %%~nf.txt
  del %%f
)

REM Cleanup remaining local rawcfgf files:
del 2>NUL *.rawcfgf

endlocal
