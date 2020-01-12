#-------------------------------------------------
#
# Project created by QtCreator 2020-01-12T16:09:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Orion_Overlay
TEMPLATE = app

INCLUDEPATH += ../openvr/headers

SOURCES += main.cpp \
    orionoverlaywidget.cpp \
    openvroverlaycontroller.cpp

HEADERS  += \
    openvroverlaycontroller.h \
    orionoverlaywidget.h

FORMS    += \
    orionoverlaywidget.ui

win32:contains(QMAKE_HOST.arch, x86_64) {
    LIBS += -L../openvr/lib/win64 -lopenvr_api
    DESTDIR = ../bin/win64
} else {
    LIBS += -L../openvr/lib/win32 -lopenvr_api
    DESTDIR = ../bin/win32
}

