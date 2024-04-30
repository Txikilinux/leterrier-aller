HEADERS += $$PWD/abuleducellulev1.h \
        $$PWD/abuleduetiquettesv1.h \
	$$PWD/abuledupiecev1.h \
        $$PWD/abuledusceneetiquettesv1.h \
        $$PWD/abuledutableauv1.h


SOURCES += $$PWD/abuleducellulev1.cpp \
        $$PWD/abuleduetiquettesv1.cpp \
	$$PWD/abuledupiecev1.cpp \
        $$PWD/abuledusceneetiquettesv1.cpp \
        $$PWD/abuledutableauv1.cpp

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


!exists($$PWD/../abuleduconstantesv1/abuleduconstantesv1.h ) {
  error("la lib abuleduetiquettesv1 dépend de abuleduconstantesv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduConstantesV1")
}

RESOURCES += \
        $$PWD/abuleduetiquettesv1.qrc
