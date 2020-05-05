DEPENDPATH += . \
    Widgets/QTimelineWidget
TEMPLATE = app
SOURCES += $$PWD/QTimelineWidget.cpp \
    $$PWD/QTimelinePanel.cpp
HEADERS += $$PWD/QTimelineWidget.h \
    $$PWD/QTimelinePanel.h
RESOURCES += $$PWD/icons.qrc
FORMS += $$PWD/QTimelinePanel.ui
