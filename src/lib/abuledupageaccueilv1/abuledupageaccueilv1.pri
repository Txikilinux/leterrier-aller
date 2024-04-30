QT += core gui

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/abuleduaccueilbullev1.h \
           $$PWD/abuleduaccueilzonev1.h \
           $$PWD/abuledupageaccueilv1.h \
           $$PWD/abuleduflechev1.h

SOURCES += $$PWD/abuleduaccueilbullev1.cpp \
           $$PWD/abuleduaccueilzonev1.cpp \
           $$PWD/abuledupageaccueilv1.cpp \
           $$PWD/abuleduflechev1.cpp

#on inclut le fichier ressources du developpeur de l'application en priorite
#mais seulement s'il existe. S'il n'existe pas on fallback sur le fichier
#de ressources propose par la lib
!exists($$PWD/../../data/abuledupageaccueilv1/abuledupageaccueilv1.qrc) {
  RESOURCES += $$PWD/abuledupageaccueilv1.qrc
}
exists($$PWD/../../data/abuledupageaccueilv1/abuledupageaccueilv1.qrc) {
  RESOURCES += $$PWD/../../data/abuledupageaccueilv1/abuledupageaccueilv1.qrc
}

#utilisation du fichier cpp de "traduction"
!exists($$PWD/../../data/abuledupageaccueilv1/abuledupageaccueilv1translation.h) {
  HEADERS += $$PWD/abuledupageaccueilv1translation.h
}
exists($$PWD/../../data/abuledupageaccueilv1/abuledupageaccueilv1translation.h) {
  HEADERS += $$PWD/../../data/abuledupageaccueilv1/abuledupageaccueilv1translation.h
}


!exists($$PWD/../abuledugraphicmenuv1/abuledugraphicmenuv1.h) {
  error("la lib abuledupageaccueilv1 dépend de abuledugraphicmenuv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduGraphicMenuV1")
}

!exists($$PWD/../abuleduapplicationv1/abuleduapplicationv1.pri) {
  error("la lib abuledupageaccueilv1 dépend de abuleduapplicationv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduApplicationV1")
}

