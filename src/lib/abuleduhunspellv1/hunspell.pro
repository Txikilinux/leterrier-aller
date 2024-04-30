#-------------------------------------------------
#
# Project created by QtCreator 2011-07-04T16:26:46
#
#-------------------------------------------------

QT -= core gui

TARGET = hunspell
#TARGETTYPE=DLL
DEFINES += HUNSPELL_LIBRARY

TEMPLATE = lib
#CONFIG += static
#CONFIG += staticlib
#CONFIG += debug_and_release
CONFIG += release
CONFIG += precompile_header

OBJECTS_DIR += temp
DESTDIR = ./lib
INCLUDEPATH += ./ \
    src/hunspell \
    src/parsers \
    src/tools \
    src/win_api


SOURCES += \
# hunspell
    src/hunspell/affentry.cxx \
    src/hunspell/affixmgr.cxx \
    src/hunspell/csutil.cxx \
    src/hunspell/dictmgr.cxx \
    src/hunspell/filemgr.cxx \
    src/hunspell/hashmgr.cxx \
    src/hunspell/hunspell.cxx \
    src/hunspell/hunzip.cxx \
    src/hunspell/phonet.cxx \
    src/hunspell/replist.cxx \
    src/hunspell/suggestmgr.cxx \
    src/hunspell/utf_info.cxx \
# parsers
    src/parsers/firstparser.cxx \
    src/parsers/htmlparser.cxx \
    src/parsers/latexparser.cxx \
    src/parsers/manparser.cxx \
    src/parsers/odfparser.cxx \
    src/parsers/textparser.cxx \
    src/parsers/xmlparser.cxx

HEADERS +=\
# hunspell
    src/hunspell/affentry.hxx \
    src/hunspell/affixmgr.hxx \
    src/hunspell/atypes.hxx \
    src/hunspell/baseaffix.hxx \
    src/hunspell/csutil.hxx \
    src/hunspell/dictmgr.hxx \
    src/hunspell/filemgr.hxx \
    src/hunspell/hashmgr.hxx \
    src/hunspell/htypes.hxx \
    src/hunspell/hunspell.h \
    src/hunspell/hunspell.hxx \
    src/hunspell/hunvisapi.h \
    src/hunspell/hunzip.hxx \
    src/hunspell/langnum.hxx \
    src/hunspell/phonet.hxx \
    src/hunspell/replist.hxx \
    src/hunspell/suggestmgr.hxx \
    src/hunspell/w_char.hxx \
# parsers
    src/parsers/firstparser.hxx \
    src/parsers/htmlparser.hxx \
    src/parsers/latexparser.hxx \
    src/parsers/manparser.hxx \
    src/parsers/odfparser.hxx \
    src/parsers/textparser.hxx \
    src/parsers/xmlparser.hxx \
    src/win_api/config.h

!static: {
  win32:RC_FILE = src/win_api/Hunspell.rc
}

OTHER_FILES +=\
    src/hunspell/license.myspell \
    src/hunspell/license.hunspell \
    src/hunspell/utf_info.cxx

unix {
#libhunspell-dev
}
