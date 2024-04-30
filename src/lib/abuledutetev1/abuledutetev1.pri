HEADERS += $$PWD/abuledugraphicsteteswidgetv1.h \
    $$PWD/abuledugraphicsellipseitem.h \
    $$PWD/abuledugraphicstextitem.h


SOURCES += $$PWD/abuledugraphicsteteswidgetv1.cpp \
    $$PWD/abuledugraphicsellipseitem.cpp \
    $$PWD/abuledugraphicstextitem.cpp

#on inclus le fichier ressources du developpeur de l'application en priorite
#mais seulement s'il existe. S'il n'existe pas on fallback sur le fichier
#de ressources propose par la lib
!exists($$PWD/../../data/abuledutetev1/abuledutetev1.qrc) {
  RESOURCES += $$PWD/abuledutetev1.qrc
}
exists($$PWD/../../data/abuledutetev1/abuledutetev1.qrc) {
  RESOURCES += $$PWD/../../data/abuledutetev1/abuledutetev1.qrc
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!exists($$PWD/../abuleduconstantesv1/abuleduconstantesv1.h ) {
  error("la lib abuledutetev1 dépend de abuleduconstantesv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduConstantesV1")
}

