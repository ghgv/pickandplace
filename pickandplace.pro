#
# Project created by QtCreator 2016-11-25T19:37:55
#
#-------------------------------------------------

QT       += core gui opengl serialport multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DEPRECATED_WARNINGS

TARGET = test
TEMPLATE = app

INCLUDEPATH += C:/opencv/build/install/include

LIBS += -LC:/Qt/5.6/mingw492_32/lib/libQt4OpenGL.a -lopengl32 -lglu32
LIBS += C:/opencv/build/install/x86/mingw/bin/libopencv_core320.dll
LIBS += C:/opencv/build/install/x86/mingw/bin/libopencv_highgui320.dll
LIBS += C:/opencv/build/install/x86/mingw/bin/libopencv_imgcodecs320.dll
LIBS += C:/opencv/build/install/x86/mingw/bin/libopencv_imgproc320.dll
LIBS += C:/opencv/build/install/x86/mingw/bin/libopencv_features2d320.dll
LIBS += C:/opencv/build/install/x86/mingw/bin/libopencv_calib3d320.dll

SOURCES += main.cpp\
        mainwindow.cpp \
    cameraframegrabber.cpp \
    filters.cpp \
    least_squares.cpp \
    rectangle.cpp \
    serial.cpp


HEADERS  += mainwindow.h \
    cameraframegrabber.h \
    least_squares.h \
    mainwindow.h \
    rectangle.h \
    serial.h

FORMS    += mainwindow.ui

DISTFILES += \
    position.pos
