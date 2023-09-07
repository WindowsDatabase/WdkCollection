@echo off
setlocal EnableDelayedExpansion

REM Usage:
if "%1"=="" (
  echo espocamlprint: Translates C/C++ code to ESP OCaml XML.
  echo.
  echo espocamlprint args-to-be-passed-to-cl
  echo.
  echo Example to translate foo.c to foo.c.xml: 
  echo   espocamlprint foo.c
  echo.
  exit /b 1
)

REM espocamlprintaux.exe:
set ESPOCAMLPRINTAUX=%~DP0espocamlprintaux.exe
if not exist "%ESPOCAMLPRINTAUX%" (
  echo espocamlprint: Error: Could not find %ESPOCAMLPRINTAUX%
  exit /b 1
)

REM cl:
set COMPILER=cl
where %COMPILER% >NUL 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  echo espocamlprint: Error: Could not find compiler %COMPILER%
  exit /b %ERRORLEVEL%
)

REM Cleanup stale local rawcfgf files:
del 2>NUL *.rawcfgf

REM Set output cfgs to local folder with same name as input:
set ESP.CfgPersist.PersistFile=%CD%\$SOURCEFILE.rawcfgf

REM Write cfgf files:
%COMPILER% /nologo /analyze:only /analyze:autolog- /astfe:d1Adisablecoreplugins /analyze:plugin cfgpersist.dll %*

if NOT %ERRORLEVEL% EQU 0 (
  echo espocamlprint: Error: Call to cl failed with error %ERRORLEVEL%
  exit /b %ERRORLEVEL%
)

REM Write xml files:
for %%f in (*.rawcfgf) do (
  %ESPOCAMLPRINTAUX% %%f >%%~nf.xml
  if NOT !ERRORLEVEL! EQU 0 (
    echo espocamlprint: Error: Call to espocamlprintaux failed with error !ERRORLEVEL!
    exit /b !ERRORLEVEL!
  )
  echo espocamlprint: Module %%f translated to ESP OCaml XML %%~nf.xml
  del %%f
)

REM Cleanup remaining local rawcfgf files:
del 2>NUL *.rawcfgf

endlocal
