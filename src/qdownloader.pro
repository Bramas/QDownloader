#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T12:59:05
#
#-------------------------------------------------

QT       += network widgets

CONFIG += c++11


TARGET = qdownloader

TEMPLATE = app

SOURCES += main.cpp \
    downloadmanager.cpp \
    item.cpp \
    additemdialog.cpp \
    listmodel.cpp \
    mainwindow.cpp \
    itemdelegate.cpp \
    authenticatedialog.cpp

HEADERS += \
    downloadmanager.h \
    item.h \
    additemdialog.h \
    listmodel.h \
    mainwindow.h \
    itemdelegate.h \
    authenticatedialog.h

FORMS += \
    additemdialog.ui \
    mainwindow.ui \
    authenticatedialog.ui
