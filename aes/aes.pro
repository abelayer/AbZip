#-------------------------------------------------
#
# Project created by QtCreator 2016-02-20T11:49:52
#
#-------------------------------------------------

QT       -= core gui

TARGET = aes
TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = objs
MOC_DIR = objs

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    aes.h \
    aes_via_ace.h \
    aesopt.h \
    aestab.h \
    brg_endian.h \
    brg_types.h \
    entropy.h \
    fileenc.h \
    hmac.h \
    prng.h \
    pwd2key.h \
    sha1.h

SOURCES += \
    aescrypt.c \
    aeskey.c \
    aestab.c \
    entropy.c \
    fileenc.c \
    hmac.c \
    prng.c \
    pwd2key.c \
    sha1.c
