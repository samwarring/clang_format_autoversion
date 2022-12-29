@echo off

:: Check command line usage
if "%1" == "" (
    echo Usage: test_msi.bat ^<path to .msi file^>
    exit /b 1
)

:: Check if provided MSI file exists
set MSI_PATH=%~f1
if NOT EXIST "%MSI_PATH%" (
    echo ----------------------------
    echo The MSI file does not exist:
    echo "%MSI_PATH%"
    echo ----------------------------
    exit /b 1
)


:: Install the MSI
set COMMAND=start /wait msiexec /i "%MSI_PATH%" /quiet /norestart /log msi_install.log
echo + %COMMAND%
%COMMAND%
if errorlevel 1 (
    echo ---------------------------------------------------------
    echo The above command exited with error %errorlevel%
    echo Below are lines from 'msi_install.log' containing 'error'
    echo ---------------------------------------------------------
    type msi_install.log | findstr /I error
    exit /b 1
)

:: Check that the PATH was updated by the installer
:: https://stackoverflow.com/a/39962543/1123681
for /f "tokens=3*" %%A in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path') do set syspath=%%A%%B
echo %syspath% | findstr "C:\Program Files\Clang Format Autoversion\bin" >NUL
if errorlevel 1 (
    echo --------------------------------------------------
    echo The installer did not not update the PATH variable
    echo --------------------------------------------------
    exit /b 1
)
set "PATH=%PATH%;C:\Program Files\Clang Format Autoversion\bin"

:: Attempt to run the applicaiton
set COMMAND=clang-format-av --version
echo + %COMMAND%
%COMMAND%
if errorlevel 1 (
    echo ------------------------------------------------
    echo The above command exited with error %errorlevel%
    echo ------------------------------------------------
    exit /b 1
)

pushd %~dp0\example_13
echo + cd example_13 ^&^& clang-format-av --version
clang-format-av --version | findstr "clang-format version 13"
if errorlevel 1 (
    echo --------------------------------------------------------
    echo Inside the example_13 directory, clang-format-av did not
    echo redirect to clang-format-av-13
    echo --------------------------------------------------------
    popd
    exit /b 1
)
popd

pushd %~dp0\example_14
echo + cd example_14 ^&^& clang-format-av --version
clang-format-av --version | findstr "clang-format version 14"
if errorlevel 1 (
    echo --------------------------------------------------------
    echo Inside the example_14 directory, clang-format-av did not
    echo redirect to clang-format-av-14
    echo --------------------------------------------------------
    popd
    exit /b 1
)
popd

pushd %~dp0\example_15
echo + cd example_15 ^&^& clang-format-av --version
clang-format-av --version | findstr "clang-format version 15"
if errorlevel 1 (
    echo --------------------------------------------------------
    echo Inside the example_15 directory, clang-format-av did not
    echo redirect to clang-format-av-15
    echo --------------------------------------------------------
    popd
    exit /b 1
)
popd

echo -----------------
echo All tests passed!
echo -----------------
exit /b 0