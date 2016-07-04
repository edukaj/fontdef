TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += codepage.h \
           programoptions.h \
    font.h \
    glyphinfo.h \
    rect.h

SOURCES += main.cpp \
    font.cpp

DEFINES += USE_OPENCV

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

    LIBS += -lFreeImage -lfreetype
}

unix {

    CONFIG += link_pkgconfig
    PKGCONFIG += freetype2 opencv

#    INCLUDEPATH += /usr/include/freetype2
    LIBS += -lboost_program_options -lfreeimage

}

HEADERS += \
    programoptions.h \
    codepage.h
