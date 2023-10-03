QT += core network gui
#QT -= gui

CONFIG += c++11

TARGET = pSimpass
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    NetworkClient.cpp \
    NetworkManager.cpp \
    Console.cpp \
    SIMPASS.cpp \
    Singleton.inl \
    LatLong.cpp \
    LatLong.inl \
    MathUtils.cpp \
    moosconnect.cpp

HEADERS += \
    NetworkClient.h \
    NetworkManager.h \
    Console.h \
    SIMPASS.h \
    Singleton.h \
    LatLong.h \
    MathUtils.h \
    moosconnect.h

DISTFILES += \
    pSimpass.ini

INCLUDEPATH += $$(HOME)/moos-ivp/lib
