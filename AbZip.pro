TEMPLATE = lib
CONFIG += qt warn_on
QT -= gui

TARGET = AbZip
CONFIG -= app_bundle

VERSION = 1.5.0

CONFIG(debug, debug|release) {
DESTDIR = debug
OBJECTS_DIR = debug
MOC_DIR = debug
} else {
DESTDIR = lib
OBJECTS_DIR = objs
MOC_DIR = objs
}


unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$PWD/Compressors
INCLUDEPATH += $$PWD/Encryption

# needed for QT4
lessThan(QT_MAJOR_VERSION, 5) {
INCLUDEPATH += $$PWD
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib
}

# if you want Bzip2 support....
CONFIG+=bzip2

bzip2 {
DEFINES += USE_BZIP2
INCLUDEPATH += $$PWD/bzip2
win32: LIBS += -L$$PWD/bzip2/release -lbzip2
}

# if you want WinZip AES support....
CONFIG+=aes

aes {
DEFINES += USE_AES
INCLUDEPATH += $$PWD/aes
win32: LIBS += -L$$PWD/aes/release -laes
}

# if you want LZMA support....
#CONFIG+=lzma

lzma {
DEFINES += USE_LZMA
INCLUDEPATH += $$PWD/Lzma/include
win32: LIBS += -L$$PWD/Lzma/lib -lLzma
}

#CONFIG += staticlib

staticlib{
DEFINES += ZIP_STATIC_LIBRARY
}

SOURCES += \
    AbZip.cpp \
    iobuffer.cpp \
    CentralDirFileHeader.cpp \
    EndOfCentralDir.cpp \
    LocalFileHeader.cpp \
    ZipCentralDir.cpp \
    Utils.cpp \
    Compressors/Compressor.cpp \
    Compressors/DeflateCompressor.cpp \
    Compressors/InflateCompressor.cpp \
    Encryption/encryption.cpp \
    Encryption/CrcEncryption.cpp \
    Compressors/StoreCompressor.cpp \
    ExtraFields.cpp \
    ZipFileInfo.cpp \
    ZipDirIterator.cpp \
    Compressors/Bzip2Compressor.cpp \
    Compressors/Bzip2Decompressor.cpp \
    Encryption/AesEncryption.cpp \
    Compressors/LzmaCompressor.cpp \
    Compressors/LzmaDecompressor.cpp

HEADERS += \
    AbZip.h \
    iobuffer.h \
    CentralDirFileHeader.h \
    EndOfCentralDir.h \
    LocalFileHeader.h \
    ZipCentralDir.h \
    Utils.h \
    Compressors/Compressor.h \
    Compressors/DeflateCompressor.h \
    Compressors/InflateCompressor.h \
    Encryption/encryption.h \
    Encryption/CrcEncryption.h \
    zipglobal.h \
    Compressors/StoreCompressor.h \
    ExtraFields.h \
    abzip_p.h \
    ZipFileInfo.h \
    ZipDirIterator.h \
    Compressors/Bzip2Compressor.h \
    Compressors/Bzip2Decompressor.h \
    Encryption/AesEncryption.h \
    Compressors/LzmaCompressor.h \
    Compressors/LzmaDecompressor.h



QMAKE_TARGET_COMPANY = Andy Bray
QMAKE_TARGET_PRODUCT = "AbZip"
QMAKE_TARGET_COPYRIGHT = copyright (c) 2016 Andy Bray
