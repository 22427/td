TEMPLATE = app
TARGET = td
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH +=
SOURCES += \
	td.cpp \
    	td_image.cpp


CONFIG += c++11


DESTDIR = bin
OBJECTS_DIR = obj


HEADERS += \
	td_image.h \
	td.h

