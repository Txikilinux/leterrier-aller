#-------------------------------------------------
#
# Project created by QtCreator 2011-07-21T10:51:36
#
#-------------------------------------------------

QT       += core gui network xml

TARGET = leterrier-aller
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    abuleduoutilsv1.cpp \
    exerciceordrealpha1.cpp \
    exerciceparagraphesmelanges.cpp \
    exercicephrasesansespaces.cpp \
    exerciceclosure.cpp \
    allereditorv1.cpp \
    exercicerapido.cpp \
    exercicemotsmelanges.cpp \
    lib/abuledutexteditv1.cpp \
    exercicephrasesmelangees.cpp \
    abuleducorrecteurv1.cpp \
    abuledupiecebuttonv1.cpp \
    abuleduunitprinterv1.cpp

HEADERS  += mainwindow.h \
    exerciceordrealpha1.h \
    abuleduoutilsv1.h \
    exerciceparagraphesmelanges.h \
    exercicephrasesansespaces.h \
    exerciceclosure.h \
    allereditorv1.h \
    exercicerapido.h \
    exercicemotsmelanges.h \
    lib/abuledutexteditv1.h \
    exercicephrasesmelangees.h \
    abuleducorrecteurv1.h \
    version.h \
    abuledupiecebuttonv1.h \
    abuleduunitprinterv1.h

FORMS    += mainwindow.ui \
    allereditorv1.ui \
    abuleduunitprinterv1.ui

OTHER_FILES += data/mot_aller_cycle3.csv \
    android/AndroidManifest.xml \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/version.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-fa/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-el/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-ru/strings.xml

RESOURCES += \
    aller.qrc

win32{
  OTHER_FILES += ../windows/windows.rc
  RC_FILE = ../windows/windows.rc
}

macx {
 ICON = macos/icones/leterrier-aller.icns
}


include(lib/abuleduapplicationv1/abuleduapplicationv1.pri)
include(lib/abuleducommonstatesv1/abuleducommonstatesv1.pri)
include(lib/abuleduexercicev1/abuleduexercicev1.pri)
include(lib/abuleduflatboutonv1/abuleduflatboutonv1.pri)
include(lib/abuledustatemachinev1/abuledustatemachinev1.pri)
include(lib/abuledutetev1/abuledutetev1.pri)
include(lib/abuleduetiquettesv1/abuleduetiquettesv1.pri)
include(lib/abuleduconstantesv1/abuleduconstantesv1.pri)
include(lib/abuledulabelv1/abuledulabelv1.pri)
include(lib/abuledumessageboxv1/abuledumessageboxv1.pri)
include(lib/abuledupageaccueilv1/abuledupageaccueilv1.pri)
include(lib/abuledugraphicmenuv1/abuledugraphicmenuv1.pri)
include(lib/abuledufilev1/abuledufilev1.pri)
include(lib/abuledunetworkaccessmanagerv1/abuledunetworkaccessmanagerv1.pri)
include(lib/maia/maia.pri)
include(lib/abuleduhunspellv1/abuleduhunspellv1.pri)
include(lib/abuledusplashscreenv1/abuledusplashscreenv1.pri)
include(lib/abuleduidentitesv1/abuleduidentitesv1.pri)
include(lib/abuledulangv1/abuledulangv1.pri)

# traduction
include(lang/lang.pri)

include(lib/abuledusingletonv1/abuledusingletonv1.pri)
include(lib/abuledupluginloaderv1/abuledupluginloaderv1.pri)
include(lib/abuleduloggerv1/abuleduloggerv1.pri)
include(lib/abuleduaproposv1/abuleduaproposv1.pri)
include(lib/abuleduloadinganimationv1/abuleduloadinganimationv1.pri)
include(lib/abuledumultimediav1/abuledumultimediav1.pri)
include(lib/abuledupicottsv1/abuledupicottsv1.pri)
include(lib/abuleduaudiov1/abuleduaudiov1.pri)
include(lib/abuleduinstructionv1/abuleduinstructionv1.pri)
include(lib/abuledutextv1/abuledutextv1.pri)
DEFINES += __ABULEDUPRODUCTIONRELEASE__MODE__=1

#Qt5 sur trusty ...
exists(/usr/include/qt5/QtMultimedia/QMediaPlayer) {
  INCLUDEPATH += /usr/include/qt5/QtMultimedia
}

