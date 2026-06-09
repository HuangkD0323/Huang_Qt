#!/bin/sh
# Fetch QXlsx into third_party/QXlsx (Linux / macOS / WSL / Git Bash)
set -e
QTXLSX_DIR="third_party/QXlsx"
if [ -d "$QTXLSX_DIR" ]; then
  echo "QXlsx already exists at $QTXLSX_DIR"
  exit 0
fi

echo "Cloning QXlsx into $QTXLSX_DIR ..."
mkdir -p third_party
git clone --depth 1 https://github.com/QtExcel/QXlsx.git "$QTXLSX_DIR"

echo "Done. You can now open ExcelClient.pro in Qt Creator and build."
