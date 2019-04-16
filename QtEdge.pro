#-------------------------------------------------
#
# Project created by QtCreator 2019-03-21T15:35:49
#
#-------------------------------------------------

QT       += core gui multimedia

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
SOURCES += src/main.cpp \
    src/core/QEdgeDecoderBase.cpp
#core

HEADERS += src/core/IPlayer.h \
    src/core/QEdgeDecoderBase.h

HEADERS += src/core/QEdgeUtils.h
SOURCES += src/core/QEdgeUtils.cpp

HEADERS += src/core/QEdgePlayer.h
SOURCES += src/core/QEdgePlayer.cpp

HEADERS += src/core/IDemuxer.h

HEADERS += src/core/QEdgeDemuxer.h
SOURCES += src/core/QEdgeDemuxer.cpp

HEADERS += src/core/IDecoder.h

HEADERS += src/core/QEdgeVideoDecoder.h
SOURCES += src/core/QEdgeVideoDecoder.cpp

HEADERS += src/core/QEdgeAudioDecoder.h
SOURCES += src/core/QEdgeAudioDecoder.cpp

HEADERS += src/core/IMediaController.h

HEADERS += src/core/QEdgeMediaController.h
SOURCES += src/core/QEdgeMediaController.cpp

#ui
HEADERS += src/core/QEdgeAudioReproductor.h
SOURCES += src/core/QEdgeAudioReproductor.cpp

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
