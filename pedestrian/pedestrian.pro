#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T18:39:24
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = pedestrian
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
	main.cpp \
    pedestrian.cpp

HEADERS += \
	debug.h \
    pedestrian.h

# OpenCV
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui

# LIBLINEAR
SOURCES += \
	linear/blas/daxpy.c \
	linear/blas/ddot.c \
	linear/blas/dnrm2.c \
	linear/blas/dscal.c \
	linear/linear.cpp \
	linear/tron.cpp
HEADERS += \
	linear/linear.h \
	linear/tron.h \
	linear/blas/blas.h \
	linear/blas/blasp.h \





