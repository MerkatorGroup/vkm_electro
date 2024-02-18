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
IF EXIST "%PRJDIR%\%PRJNAME%.gdz" (
    del "%PRJDIR%\%PRJNAME%.gdz"
)
IF "%1" == "withos" (
    echo.
    echo ************************************************************************
    echo * Exporting %PRJNAME% to %PRJNAME%.gdz with OS
    echo ************************************************************************
    %GSEVISUEXE% "%PRJDIR%\%PRJNAME%.gds" "-withos -e" "%PRJDIR%\%PRJNAME%.gdz"
) ELSE IF "%1" == "genreset" (
    echo.
    echo ************************************************************************
    echo * Exporting %PRJNAME% to %PRJNAME%.gdz with general reset and OS
    echo ************************************************************************
    %GSEVISUEXE% "%PRJDIR%\%PRJNAME%.gds" "-genreset -withos -e" "%PRJDIR%\%PRJNAME%.gdz"
) ELSE (
    echo.
    echo ************************************************************************
    echo * Exporting %PRJNAME% to %PRJNAME%.gdz
    echo ************************************************************************
    %GSEVISUEXE% "%PRJDIR%\%PRJNAME%.gds" "-e" "%PRJDIR%\%PRJNAME%.gdz"
)

echo.
IF EXIST "%PRJDIR%\%PRJNAME%.gdz" (
    echo ************************************************************************
    echo * Successfully done
    echo ************************************************************************
) ELSE (
    echo ************************************************************************
    echo * !!!!! ERROR !!!!!
    echo ************************************************************************
)
