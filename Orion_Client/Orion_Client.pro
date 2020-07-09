QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    blehandler.cpp \
    main.cpp \
    mainmenu.cpp

HEADERS += \
    blehandler.h \
    mainmenu.h

FORMS += \
    mainmenu.ui

INCLUDEPATH += \
    $$PWD/../

OBJECTS_DIR = $$PWD/../bin/client_win64_build/obj
MOC_DIR = $$PWD/../bin/client_win64_build/moc
RCC_DIR = $$PWD/../bin/client_win64_build/rcc
UI_DIR = $$PWD/../bin/client_win64_build
DESTDIR = $$PWD/../bin/client_win64
