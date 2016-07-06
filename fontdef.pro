TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS +=  \
            programoptions.h \
            glyphinfo.h \
            rect.h \
            codepoint.h \
    fontdefgenerator.h

SOURCES +=  main.cpp \
    programoptions.cpp \
    codepoint.cpp \
    fontdefgenerator.cpp

DEFINES += BOOST_ALL_DYN_LINK

win32-msvc2015 {
    INCLUDEPATH+=../controlib/include

    LIBS += -L../controlib/lib

    CONFIG(release, debug|release) {
        LIBS += -lfreetype263MT
    }

    CONFIG(debug, debug|release) {
        LIBS += -lfreetype263MTd
    }

    LIBS += -lFreeImage -lfreetype
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += freetype2

    LIBS += -lpthread -lboost_program_options -lfreeimage
}

HEADERS += \
    programoptions.h
