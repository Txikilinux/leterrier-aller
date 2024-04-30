QT += core gui

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/abuledustatemachinev1.h $$PWD/stringtransition.h
SOURCES += $$PWD/abuledustatemachinev1.cpp

!exists($$PWD/../abuleduapplicationv1/abuleduapplicationv1.pri) {
  error("la lib abuledustatemachinev1 dépend de abuleduapplicationv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduApplicationV1")
}

!exists($$PWD/../abuleduexercicev1/abuleduexercicev1.h ) {
  error("la lib abuledustatemachinev1 dépend de abuleduexercicev1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduExerciceV1")
}

#!exists($$PWD/../abuledutetev1/abuledutetev1.h ) {
#  error("la lib abuledustatemachinev1 dépend de abuledutetev1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduTeteV1")
#}
!exists($$PWD/../abuledutetev1/abuledugraphicsteteswidgetv1.h ) {
  error("la lib abuledustatemachinev1 dépend de abuledutetev1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduTeteV1")
}
!exists($$PWD/../abuleduconstantesv1/abuleduconstantesv1.h ) {
  error("la lib abuledustatemachinev1 dépend de abuleduconstantesv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduConstantesV1")
}
