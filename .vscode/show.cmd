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
echo * Showing %PRJNAME%
echo ************************************************************************

%GSEVISUEXE% "%PRJDIR%\%PRJNAME%.gds"
