debug { MMP_RULES -= PAGED
        MMP_RULES += UNPAGED }

include(photo_wall.pri)
include(qrcode.pri)

CONFIG += debug mobility
symbian {
MOBILITY = systeminfo messaging publishsubscribe
} else {
MOBILITY = systeminfo
}

debug { MMP_RULES -= PAGED
        MMP_RULES += UNPAGED }

TARGET = cuteBox
TEMPLATE = app
VERSION = 1.1.0

# All generated files goes same directory
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
DESTDIR = build

QT += core \
    gui \
    svg \
    sql \
    xml \
    webkit \
    network
    
DEFINES += Q_LOGGING DRAW_BOUNDARIES

SOURCES += src/common/genresourcereader.cpp \
    src/common/genfiletransfer.cpp \
    src/common/genpublicstrings.cpp \
    src/common/geninterfaces.cpp \
    src/common/genappmodel.cpp \
    src/common/genfileserverutils.cpp \
    src/common/genprofileutils.cpp \
    src/common/genfeedbackutils.cpp \
    src/common/gendropboxrequests.cpp \
    src/engines/genappmanager.cpp \
    src/engines/genhttpengine.cpp \
    src/engines/genwsengine.cpp \
    src/engines/genoauthrequest.cpp \
    src/engines/genoauthutils.cpp \
    src/logger/gendebugoutput.cpp \
    src/logger/genlog.cpp \
    src/logger/genlogdest.cpp \
    src/logger/genprofiling.cpp \
    src/ui/genbaseview.cpp \
    src/ui/genloginview.cpp \
    src/ui/genwelcomeview.cpp \
    src/ui/genregisterview.cpp \
    src/ui/genmainview.cpp \
    src/ui/genfilebrowserview.cpp \
    src/ui/genfiledetailsview.cpp \
    src/ui/gendownloadsview.cpp \
    src/ui/gensettingsview.cpp \
    src/ui/genstatuswidget.cpp \
    src/json/genjson.cpp \
    src/json/genjsonparser.cpp \
    src/main.cpp \
    src/ui/genpushbutton.cpp \
    src/common/genutils.cpp \
    src/engines/genfiledownloader.cpp \
    src/ui/genindicatorswidget.cpp \
    src/common/genfilesystemmodel.cpp \
    src/common/genfileiconprovider.cpp \
    src/ui/genquerypopup.cpp \
    src/ui/genpopupmenu.cpp \
    src/engines/genmediagallery.cpp \
    src/ui/genfilenamepopup.cpp \
    src/ui/gennotificationwidget.cpp \
    src/engines/genthumbnailmanager.cpp \
    src/engines/genthumbnaildownloader.cpp \
    src/ui/genmenubar.cpp \
    src/ui/genmainwindow.cpp \
    src/ui/genkineticscrollwidget.cpp \
    src/ui/genkineticscrollengine.cpp \
    src/db/gendbtables.cpp \
    src/db/gendbengine.cpp \
    src/db/gendataengine.cpp \
    src/common/genfileinfo.cpp \
    src/common/genfileinfomodel.cpp \
    src/ui/genfileinfodelegate.cpp \
    src/ui/genrotatingimage.cpp \
    src/engines/genfileuploader.cpp \
    src/common/genupfile.cpp \
    src/ui/genfileprogress.cpp \
    src/ui/genhelpview.cpp \
    src/ui/genpageindicator.cpp \
    src/common/genfilesystemfilterproxymodel.cpp \
    src/ui/genlistview.cpp \
    src/ui/genalbumwidget.cpp \
    src/ui/genwebview.cpp

HEADERS += inc/common/genresourcereader.h \
    inc/common/genfiletransfer.h \
    inc/common/genpublicstrings.h \
    inc/common/geninterfaces.h \
    inc/common/genappmodel.h \
    inc/common/genliterals.h \
    inc/common/genconstants.h \
    inc/common/genenums.h \
    inc/common/genstateevent.h \
    inc/engines/genappmanager.h \
    inc/engines/genhttpengine.h \
    inc/engines/genwsengine.h \
    inc/engines/genoauthcommon.h \
    inc/engines/genoauthrequest.h \
    inc/engines/genoauthutils.h \
    inc/logger/gendebugoutput.h \
    inc/logger/genlog.h \
    inc/logger/genlogdest.h \
    inc/logger/genprofiling.h \
    inc/common/genfileserverutils.h \
    inc/common/genprofileutils.h \
    inc/common/genfeedbackutils.h \
    inc/ui/genbaseview.h \
    inc/ui/genloginview.h \
    inc/json/genjson.h \
    inc/ui/genwelcomeview.h \
    inc/ui/genmainview.h \
    inc/ui/genregisterview.h \
    inc/ui/genfilebrowserview.h \
    inc/ui/genfiledetailsview.h \
    inc/ui/gendownloadsview.h \
    inc/ui/gensettingsview.h  \
    inc/ui/genstatuswidget.h \
    inc/ui/genpushbutton.h \
    inc/common/genutils.h \
    inc/engines/genfiledownloader.h \
    inc/ui/genindicatorswidget.h \
    inc/common/genfilesystemmodel.h \
    inc/common/genfileiconprovider.h \
    inc/ui/genquerypopup.h \
    inc/ui/genpopupmenu.h  \
    inc/engines/genmediagallery.h \
    inc/ui/genfilenamepopup.h \
    inc/ui/gennotificationwidget.h \
    inc/engines/genthumbnailmanager.h \
    inc/engines/genthumbnaildownloader.h \
    inc/ui/genmenubar.h \
    inc/ui/genmainwindow.h \
    inc/ui/genkineticscrollwidget.h \
    inc/ui/genkineticscrollengine.h \
    inc/db/gendbtables.h \
    inc/db/gendbengine.h \
    inc/db/gendataengine.h \
    inc/common/genfileinfo.h \
    inc/common/gentypes.h \
    inc/common/genfileinfomodel.h \
    inc/common/gendropboxrequests.h \
    inc/ui/genfileinfodelegate.h \
    inc/ui/genrotatingimage.h \
    inc/engines/genfileuploader.h \
    inc/common/genupfile.h \
    inc/ui/genfileprogress.h \ 
    inc/ui/genhelpview.h \
    inc/ui/genpageindicator.h \
    inc/common/genfilesystemfilterproxymodel.h \
    inc/ui/genlistview.h \
    inc/ui/genalbumwidget.h \
    inc/ui/genwebview.h
    
INCLUDEPATH += inc \
    inc/common \
    inc/db \
    inc/engines \
    inc/logger \
    inc/json \
    inc/ui
    
FORMS += \
    ui/genmainview.ui \
    ui/genloginview.ui \
    ui/genwelcomeview.ui \
    ui/gendownloadsview.ui \
    ui/gensettingsview.ui \
    ui/genfiledetailsview.ui \
    ui/genfilebrowserview.ui \
    ui/genregisterview.ui \
	ui/genstatuswidget.ui \
    ui/genindicatorswidget.ui \
    ui/genquerypopup.ui \
    ui/genpopupmenu.ui \
    ui/genfilenamepopup.ui \
    ui/gennotificationwidget.ui \
	ui/genwaitpopup.ui \
    ui/genmenubar.ui \
    ui/genmainwindow.ui \
    ui/genfileprogress.ui \
    ui/genhelpview.ui \
    ui/genpageindicator.ui \
    ui/genwebview.ui
    
 
    
RESOURCES = resources/cutebox.qrc
    
win32 {
    SOURCES += src/common/genprofileutilsimpl.cpp \
               src/common/genfeedbackutilsimpl.cpp \
               src/common/genfileserverimpl.cpp  \
               src/engines/genmediagalleryimpl.cpp      
    HEADERS += inc/common/genfeedbackutilsimpl.h \
               inc/common/genprofileutilsimpl.h \
               inc/common/genfileserverimpl.h  \
               inc/engines/genmediagalleryimpl.h
}    
    
# S60    
symbian { 
    TARGET.UID3 = 0x2004111A
    TARGET.CAPABILITY += NetworkServices \
    LocalServices \
    ReadUserData \
    WriteUserData \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData \
    SwEvent
    
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 \
        0x1000000
    ICON = resources/cutebox.svg
    crml.sources = src/cuteboxkeydefs.qcrml
    crml.path = c:/resource/qt/crml
    DEPLOYMENT += crml
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    # Vendor info
    vendorinfo = \
         "%{\"Genera Software\"}" \
         ":\"Genera Software\""

    vendor.pkg_prerules = vendorinfo
    DEPLOYMENT += vendor

    # Supported platform: Symbian^3
    supported_platforms = \
    "; Supports S^3 only" \
    "[0x20022E6D],0,0,0,{\"S60ProductID\"}"
    default_deployment.pkg_prerules -= pkg_platform_dependencies
    platformDeps.pkg_prerules += supported_platforms
    DEPLOYMENT += platformDeps

    OTHER_FILES += src/cuteboxkeydefs.qcrml
                         
    SOURCES += src/common/genprofileutilssymimpl.cpp \
               src/common/genfeedbackutilssymimpl.cpp \
               src/common/genfileserverimpl.cpp \
               src/common/genfileserversymimpl.cpp \
               src/engines/genmediagallerysymimpl.cpp
    HEADERS += inc/common/genfeedbackutilssymimpl.h \
               inc/common/genprofileutilssymimpl.h \
               inc/common/genfileserverimpl.h \
               inc/common/genfileserversymimpl.h \
               inc/engines/genmediagallerysymimpl.h

    LIBS += -leuser \
            -lavkon \
            -leikcore \
            -lws32 \
            -leiksrv \
            -lefsrv \
            -lcentralrepository \
            -ltouchfeedback \
            -lbafl \
            -lmgfetch \
            -lcone \
            -letext \
            -lestor \
            -lInnerActiveAdQtLib
}
