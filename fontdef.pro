TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += codepage.h \
           programoptions.h

SOURCES += main.cpp

win32-msvc2015 {
    DEFINES += BOOST_ALL_DYN_LINK
    INCLUDEPATH+=../controlib/include

    LIBS += -L../controlib/lib

    CONFIG(release, debug|release) {
        LIBS += -lfreetype263MT
    }

    CONFIG(debug, debug|release) {
        LIBS += -lfreetype263MTd
    }

    LIBS += -lsfml-graphics -lsfml-window -lsfml-system -lFreeImage
}

HEADERS += \
    programoptions.h \
    codepage.h
