#-------------------------------------------------
#
# Project created by QtCreator 2017-11-19T23:48:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JPEG_Hero
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clusterdlg.cpp

HEADERS  += mainwindow.h \
    include_libs.h \
    clusterdlg.h

FORMS    += mainwindow.ui \
    clusterdlg.ui
