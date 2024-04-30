QT += core gui

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/abuleducommonstatesv1.h
SOURCES += $$PWD/abuleducommonstatesv1.cpp

!exists($$PWD/../abuledustatemachinev1/abuledustatemachinev1.h ) {
  error("la lib abuleducommonstatesv1 dépend de abuledustatemachinev1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduStateMachineV1")
}

!exists($$PWD/../abuleduapplicationv1/abuleduapplicationv1.pri) {
  error("la lib abuleducommonstatesv1 dépend de abuleduapplicationv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduApplicationV1")
}
