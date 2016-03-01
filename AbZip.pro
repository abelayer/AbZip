TEMPLATE = lib
CONFIG += qt warn_on
QT -= gui

TARGET = AbZip
CONFIG -= app_bundle

CONFIG(release, debug|release): DESTDIR = lib

INCLUDEPATH += $$PWD/Compressors
INCLUDEPATH += $$PWD/Encryption

# needed for QT4
lessThan(QT_MAJOR_VERSION, 5) {
INCLUDEPATH += $$PWD
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib
#win32:QMAKE_LFLAGS += -shared
}

# if you want Bzip2 support....
CONFIG+=bzip2

bzip2 {
DEFINES += USE_BZIP2
INCLUDEPATH += $$PWD/bzip2
LIBS += -L$$PWD/bzip2/release -lbzip2
}

# if you want WinZip AES support....
CONFIG+=aes

aes {
DEFINES += USE_AES
INCLUDEPATH += $$PWD/aes
LIBS += -L$$PWD/aes/release -laes
}

# if you want LZMA support....
#CONFIG+=lzma2

lzma2 {
DEFINES += USE_LZMA2
INCLUDEPATH += $$PWD/Lzma2
LIBS += -L$$PWD/Lzma2/release -lLzma2
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
    Encryption/AesEncryption.cpp

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
    Encryption/AesEncryption.h


VERSION = 0.1.0.0
QMAKE_TARGET_COMPANY = Andy Bray
QMAKE_TARGET_PRODUCT = "AbZip"
QMAKE_TARGET_COPYRIGHT = copyright (c) Andy Bray
