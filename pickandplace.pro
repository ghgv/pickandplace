#
# Project created by QtCreator 2016-11-25T19:37:55
#
#-------------------------------------------------

QT       += core gui opengl serialport multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app


LIBS += -LC:/Qt/5.6/mingw492_32/lib/libQt4OpenGL.a -lopengl32 -lglu32

SOURCES += main.cpp\
        mainwindow.cpp \
    cameraframegrabber.cpp \
    filters.cpp \
    least_squares.cpp


HEADERS  += mainwindow.h \
    cameraframegrabber.h \
    least_squares.h \
    mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    position.pos
