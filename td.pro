TEMPLATE = app
TARGET = td
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += submodules/stb include
SOURCES += \
	src/main.cpp \
    src/td_image.cpp


CONFIG += c++11


DESTDIR = bin
OBJECTS_DIR = obj


HEADERS += \
	src/td_image.h \
	src/td.h

