# Features defines
DEFINES += FEATURE_QRCODE

HEADERS += \
    libqrencode/bitstream.h \
    libqrencode/mask.h \
    libqrencode/qrencode.h \
    libqrencode/qrencode_inner.h \
    libqrencode/qrinput.h \
    libqrencode/qrspec.h \
    libqrencode/rscode.h \
    libqrencode/split.h \
    libqrencode/genqrencoder.h

SOURCES += \ 
    libqrencode/bitstream.c \
    libqrencode/mask.c \
    libqrencode/qrencode.c \
    libqrencode/qrinput.c \
    libqrencode/qrspec.c \
    libqrencode/rscode.c \
    libqrencode/split.c \
    libqrencode/genqrencoder.cpp

INCLUDEPATH += libqrencode
