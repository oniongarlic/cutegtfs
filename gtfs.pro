QT += core sql
QT -= gui

# CONFIG += c++11

TARGET = gtfs-test
CONFIG += console
CONFIG -= app_bundle

LIBS += -lquazip -lz

TEMPLATE = app

SOURCES += main.cpp \
    csv/readcsv.cpp \
    gtfs/gtfsimporter.cpp \
    gtfs/gtfsarchiveparser.cpp \
    gtfs/gtfsservice.cpp \
    gtfs/gtfsstop.cpp

HEADERS += \
    csv/readcsv.h \
    gtfs/gtfsimporter.h \
    gtfs/gtfsarchiveparser.h \
    gtfs/gtfsservice.h \
    gtfs/gtfsstop.h
