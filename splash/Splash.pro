# -------------------------------------------------
# Project created by QtCreator 2009-07-26T15:12:22
# -------------------------------------------------
include(../Shared/jsoncpp/jsoncpp.pri)
include(../Shared/misc/misc.pri)
include(../Shared/irrKlang/irrKlang.pri)

include(ImagePaletDialog/ImagePaletDialog.pri)
include(DataModels/DataModels.pri)
include(AnimationPlayer/AnimationPlayer.pri)
include(AnimationViewer/AnimationViewer.pri)
include(Widgets/QTimelineWidget/QTimelineWidget.pri)
include(Widgets/ResourceTree/ResourceTree.pri)
INCLUDEPATH += Widgets ../Shared
QT += opengl
QT += xml
QT += multimedia
TARGET = Splash
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ResourceManager.cpp
HEADERS += mainwindow.h \
    Macros.h \
    ResourceManager.h
FORMS += mainwindow.ui
RESOURCES += ui.qrc
RC_FILE = Splash.rc
