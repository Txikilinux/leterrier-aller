QT += core gui network xml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/abuledugraphicmenuv1.h

SOURCES += $$PWD/abuledugraphicmenuv1.cpp
FORMS += $$PWD/abuledugraphicmenuv1.ui

# on inclut le fichier ressources et le fichier conf du developpeur de l'application en priorite
# mais seulement s'il existe. S'il n'existe pas on fallback sur le fichier
# de ressources et le fichier conf proposes par la lib
!exists($$PWD/../../data/abuledugraphicmenuv1/abuledugraphicmenuv1.qrc) {
  RESOURCES += $$PWD/abuledugraphicmenuv1.qrc
}
exists($$PWD/../../data/abuledugraphicmenuv1/abuledugraphicmenuv1.qrc) {
  RESOURCES += $$PWD/../../data/abuledugraphicmenuv1/abuledugraphicmenuv1.qrc
}

!exists($$PWD/../../data/abuledugraphicmenuv1/abuledugraphicmenuv1.conf) {
  OTHER_FILES += $$PWD/abuledugraphicmenuv1.conf
}
exists($$PWD/../../data/abuledugraphicmenuv1/abuledugraphicmenuv1.conf) {
  OTHER_FILES += $$PWD/../../data/abuledugraphicmenuv1/abuledugraphicmenuv1.conf
}

!exists($$PWD/../abuledulabelv1/abuledulabelv1.h ) {
  error("la lib abuledugraphicmenuv1 dépend de abuledulabelv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduLabelV1")
}

!exists($$PWD/../abuleduapplicationv1/abuleduapplicationv1.pri) {
  error("la lib abuledugraphicmenuv1 dépend de abuleduapplicationv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduApplicationV1")
}

OTHER_FILES = $$PWD/abuledugraphicmenuv1.conf
