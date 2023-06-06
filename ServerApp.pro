QT       += core
QT       -= gui

CONFIG += c++11

TARGET = ManageServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
        main.cpp \
        mytcpserver.cpp

HEADERS += \
        mytcpserver.h

LIBS += -L/usr/lib/x86_64-linux-gnu -lQt5Network

unix {
    target.path = /opt/$${TARGET}/bin
    INSTALLS += target
}
