TEMPLATE = app

QT += core sql
QT -= gui
 
# CONFIG += c++11
 
TARGET = gtfs-test
CONFIG += console

unix:!macx: LIBS += -L$$OUT_PWD/../gtfs/ -lcutegtfs
win32: LIBS += -L$$OUT_PWD/../gtfs/release/ -lcutegtfs

INCLUDEPATH += $$PWD/../gtfs
DEPENDPATH += $$PWD/../gtfs

SOURCES += main.cpp
HEADERS += 
