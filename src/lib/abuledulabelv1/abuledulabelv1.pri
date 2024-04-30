HEADERS += $$PWD/abuledulabelframev1.h\
           $$PWD/abuledulabelv1.h\
           $$PWD/abuledulabelwidgetv1.h


SOURCES += $$PWD/abuledulabelframev1.cpp\
           $$PWD/abuledulabelv1.cpp\
           $$PWD/abuledulabelwidgetv1.cpp

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!exists($$PWD/../abuleduflatboutonv1/abuleduflatboutonv1.h ) {
  error("la lib abuledulabelv1 dépend de abuleduflatboutonv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduFlatBoutonV1")
}

RESOURCES += $$PWD/abuledulabelv1.qrc
