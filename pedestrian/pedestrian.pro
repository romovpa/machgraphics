#-------------------------------------------------
#
# Project created by QtCreator 2011-10-13T02:06:55
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = pedestrian
CONFIG   += console
CONFIG   -= app_bundle

# OpenCV
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui

# LibLinear
SOURCES += \
	linear/linear.cpp \
	linear/tron.cpp \
	linear/blas/daxpy.c \
	linear/blas/ddot.c \
	linear/blas/dnrm2.c \
	linear/blas/dscal.c
HEADERS += \
	linear/linear.h \
	linear/tron.h \
	linear/blas/blas.h \
	linear/blas/blasp.h

TEMPLATE = app

SOURCES += main.cpp
