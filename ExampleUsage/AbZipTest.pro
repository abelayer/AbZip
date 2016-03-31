#-------------------------------------------------
#
# Project created by QtCreator 2016-02-19T10:01:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = AbZipTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/..

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib -lAbZip1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../debug -lAbZip1
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../debug/ -lAbZip
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lAbZip


SOURCES += main.cpp

