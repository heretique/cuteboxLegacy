# Features defines

QT += opengl

DEFINES += FEATURE_PHOTO_WALL

HEADERS += \
    inc/ui/genphotoview.h \
    inc/ui/genphotobrowser.h \
    inc/ui/genpannablewall.h \
    inc/ui/genpannablewidget.h

SOURCES += \ 
    src/ui/genphotoview.cpp \
    src/ui/genphotobrowser.cpp \
    src/ui/genpannablewall.cpp \
    src/ui/genpannablewidget.cpp


INCLUDEPATH += inc \
    inc/ui

FORMS += ui/genphotoview.ui
