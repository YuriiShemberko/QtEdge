#-------------------------------------------------
#
# Project created by QtCreator 2019-03-21T15:35:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtEdge
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#entrypoint
SOURCES += src/main.cpp

#core
HEADERS += src/core/IFrameProvider.h

HEADERS += src/core/CNullFrameProvider.h

HEADERS += src/core/QEdgeFrameProvider.h
SOURCES += src/core/QEdgeFrameProvider.cpp

#ui
HEADERS += src/ui/QEdgeFrameView.h
SOURCES += src/ui/QEdgeFrameView.cpp
FORMS += src/ui/QEdgeFrameView.ui

HEADERS += src/ui/QEdgeMainWindow.h
SOURCES += src/ui/QEdgeMainWindow.cpp
FORMS += src/ui/QEdgeMainWindow.ui

#-------------------------------------

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/lib/ffmpeg/include
LIBS += -L$$PWD/lib/ffmpeg/lib -lavutil -lavcodec -lswscale -lavformat
