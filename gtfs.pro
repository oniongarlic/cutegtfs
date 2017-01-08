QT += core sql
QT -= gui

# CONFIG += c++11

TARGET = gtfs-test
CONFIG += console
CONFIG -= app_bundle

LIBS += -lz -lquazip

TEMPLATE = app

SOURCES += main.cpp \
    cvs/readcsv.cpp \
    gtfs/gtfsimporter.cpp \
    gtfs/gtfsarchiveparser.cpp \
    gtfs/gtfsservice.cpp

HEADERS += \
    csv/readcsv.h \
    gtfs/gtfsimporter.h \
    gtfs/gtfsarchiveparser.h \
    gtfs/gtfsservice.h
