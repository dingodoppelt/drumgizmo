TEMPLATE = app
TARGET = FontGen
DEPENDPATH += .
INCLUDEPATH += .
#CONFIG += console debug
QT += widgets

# Input
HEADERS += generator.h

SOURCES += generator.cc \
           main.cc
