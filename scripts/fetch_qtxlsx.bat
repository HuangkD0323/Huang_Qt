@echo off
REM Fetch QXlsx into third_party\QXlsx (Windows CMD)
set QTXLSX_DIR=third_party\QXlsx
if exist "%QTXLSX_DIR%" (
  echo QXlsx already exists at %QTXLSX_DIR%
  exit /b 0
)
echo Cloning QXlsx into %QTXLSX_DIR% ...
mkdir third_party
git clone --depth 1 https://github.com/QtExcel/QXlsx.git "%QTXLSX_DIR%"
echo Done. You can now open ExcelClient.pro in Qt Creator and build.
