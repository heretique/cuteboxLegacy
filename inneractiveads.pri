# Features defines
DEFINES += FEATURE_ADS

INCLUDEPATH += inneractiveads/includes

win32 {
    LIBS += -L$$PWD/inneractiveads/libs/win32 -lInnerActiveAdQtLib
}

symbian {
    LIBS += -L$$PWD/inneractiveads/libs/symbian -lInnerActiveAdQtLib.lib
}

