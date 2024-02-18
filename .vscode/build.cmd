@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Set environment from config.cmd
for /f "tokens=1,2 delims==" %%x in (%~dp0\config.cmd) do (
    IF NOT "%%y" == "" (
        set %%x=%%y
    )
)

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo.
echo ************************************************************************
echo * Building %PRJNAME%
echo ************************************************************************

call :_sub_Short_Path "%GSEVISU%"
set "GDSPATH=%_subvar_Short_Path%"

call :_sub_Short_Path "%PRJDIR%"
set "PRJDIR=%_subvar_Short_Path%"

cd /D %PRJDIR%

IF EXIST "%PRJDIR%\sys\make\Makefile" (
    set MAKEFILE=%PRJDIR%\sys\make\Makefile
) ELSE (
    set MAKEFILE=%PRJDIR%\Makefile
)

IF "%1" == "clean" (
    %GDSPATH%\bin\gnu\make.exe -f %PRJDIR%\sys\make\Makefile clean
) ELSE (
    IF "%1" == "rebuild" (
        %GDSPATH%\bin\gnu\make.exe -f %PRJDIR%\sys\make\Makefile clean
    )
    %GDSPATH%\bin\gnu\make.exe -f %PRJDIR%\sys\make\Makefile
)
goto EOF

:: Sub function which creates the short path (without spaces) which is
:: requiered by make)
:_sub_Short_Path
set _subvar_Short_Path=%~s1
EXIT /b

:EOF
