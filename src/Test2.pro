#-------------------------------------------------
#
# Project created by QtCreator 2013-01-05T04:22:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Frame.cpp \
    CacheUI.cpp \
    Cache.cpp

HEADERS  += mainwindow.h \
    CacheUI.h \
    Frame.h \
    Cache.h \
    Globals.h

CONFIG += console

FORMS    += mainwindow.ui
