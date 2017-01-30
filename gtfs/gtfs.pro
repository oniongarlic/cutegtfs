QT += core sql
QT -= gui

# CONFIG += c++11

TARGET = cutegtfs

LIBS += -lquazip -lz

TEMPLATE = lib

SOURCES += \
    csv/readcsv.cpp \
    gtfsimporter.cpp \
    gtfsarchiveparser.cpp \
    gtfsservice.cpp \
    gtfsstop.cpp

HEADERS += \
    csv/readcsv.h \
    gtfsimporter.h \
    gtfsarchiveparser.h \
    gtfsservice.h \
    gtfsstop.h
