REM
REM Configure OACR when running SDV
REM
if /I "%1"=="off"     goto CLEAR
if /I "%1"=="OFF"     goto CLEAR
if /I "%1"=="on"      goto RESTORE
if /I "%1"=="ON"      goto RESTORE

goto END

:CLEAR
@echo Saving OACR environment variables.
set SDV_OLD_SUBSTITUTE_386_CC=%SUBSTITUTE_386_CC%
set SDV_OLD_SUBSTITUTE_AMD64_CC=%SUBSTITUTE_AMD64_CC%
set SDV_OLD_SUBSTITUTE_ARM_CC=%SUBSTITUTE_ARM_CC%
set SDV_OLD_SUBSTITUTE_LINK=%SUBSTITUTE_LINK%
set SDV_OLD_SUBSTITUTE_CS_COMPILER_NAME=%SUBSTITUTE_CS_COMPILER_NAME%
set SDV_OLD_BUILD_OACR=%BUILD_OACR%

REM
REM Clearing OACR environment variables.
REM
@echo Clearing OACR environment variables.
set SUBSTITUTE_386_CC=
set SUBSTITUTE_AMD64_CC=
set SUBSTITUTE_ARM_CC=
set SUBSTITUTE_LINK=
set SUBSTITUTE_CS_COMPILER_NAME=
set BUILD_OACR=

goto END

:RESTORE
REM
REM Restoring OACR environment variables.
REM
@echo Restoring OACR environment variables.
set SUBSTITUTE_386_CC=%SDV_OLD_SUBSTITUTE_386_CC%
set SUBSTITUTE_AMD64_CC=%SDV_OLD_SUBSTITUTE_AMD64_CC%
set SUBSTITUTE_ARM_CC=%SDV_OLD_SUBSTITUTE_ARM_CC%
set SUBSTITUTE_LINK=%SDV_OLD_SUBSTITUTE_LINK%
set SUBSTITUTE_CS_COMPILER_NAME=%SDV_OLD_SUBSTITUTE_CS_COMPILER_NAME%
set BUILD_OACR=%SDV_OLD_BUILD_OACR%

REM
REM Finished
REM

:END
@echo Finished.
