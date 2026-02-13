TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    familytree.cpp \
    cli.cpp \
    utils.cpp

HEADERS += \
    familytree.hh \
    cli.hh \
    utils.hh

DISTFILES += \
    data
