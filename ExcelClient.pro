QT += core gui widgets
CONFIG += c++11

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h \
           filterproxymodel.h

TARGET = ExcelClient
TEMPLATE = app

# QXlsx integration: if you have the QXlsx source in third_party/QXlsx/xlsx,
# qmake will add those sources/headers so the project builds out-of-the-box.
QTXLSX_DIR = $$PWD/third_party/QXlsx
exists($$QTXLSX_DIR/xlsx) {
    message("Including QXlsx from: $$QTXLSX_DIR/xlsx")
    INCLUDEPATH += $$QTXLSX_DIR/xlsx
    QXLSX_SOURCES = $$files($$QTXLSX_DIR/xlsx/*.cpp)
    QXLSX_HEADERS = $$files($$QTXLSX_DIR/xlsx/*.h)
    SOURCES += $$QXLSX_SOURCES
    HEADERS += $$QXLSX_HEADERS
}
