#-------------------------------------------------
#
# Project created by QtCreator 2016-02-20T11:49:52
#
#-------------------------------------------------

QT       -= core gui

TARGET = bzip2
TEMPLATE = lib
CONFIG += staticlib

SOURCES += blocksort.c \
    bzlib.c \
    compress.c \
    crctable.c \
    decompress.c \
    huffman.c \
    randtable.c

HEADERS += \
    bzlib.h \
    bzlib_private.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
