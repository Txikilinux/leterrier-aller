QT += core gui network xml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/abuleduexercicev1.h \
    $$PWD/abuleduexercicewidgettelecommandev1.h \
    $$PWD/abuleduexercicewidgetairedetravailv1.h \
    $$PWD/abuleduexercicewidgetmessagev1.h \
    $$PWD/abuleduexercicecommonv1.h \
    $$PWD/abuleduexercicelogsv1.h
SOURCES += $$PWD/abuleduexercicev1.cpp \
    $$PWD/abuleduexercicewidgettelecommandev1.cpp \
    $$PWD/abuleduexercicewidgetairedetravailv1.cpp \
    $$PWD/abuleduexercicewidgetmessagev1.cpp \
    $$PWD/abuleduexercicecommonv1.cpp \
    $$PWD/abuleduexercicelogsv1.cpp
FORMS += $$PWD/abuleduexercicewidgettelecommandev1.ui \
    $$PWD/abuleduexercicewidgetairedetravailv1.ui \
    $$PWD/abuleduexercicewidgetmessagev1.ui

!exists($$PWD/../../data/abuleduexercicev1/abuleduexercicev1.conf) {
  OTHER_FILES += $$PWD/abuleduexercicev1.conf
}
exists($$PWD/../../data/abuleduexercicev1/abuleduexercicev1.conf) {
  OTHER_FILES += $$PWD/../../data/abuleduexercicev1/abuleduexercicev1.conf
}

!exists($$PWD/../../data/abuleduexercicev1/abuleduexercicev1.qrc) {
  RESOURCES += $$PWD/abuleduexercicev1.qrc
}
exists($$PWD/../../data/abuleduexercicev1/abuleduexercicev1.qrc) {
  RESOURCES += $$PWD/../../data/abuleduexercicev1/abuleduexercicev1.qrc
}

!exists($$PWD/../abuleduapplicationv1/abuleduapplicationv1.pri) {
  error("la lib abuleduexercicev1 dépend de abuleduapplicationv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduApplicationV1")
}

!exists($$PWD/../abuleduconstantesv1/abuleduconstantesv1.h ) {
  error("la lib abuleduexercicev1 dépend de abuleduconstantesv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduConstantesV1")
}

!exists($$PWD/../abuleduflatboutonv1/abuleduflatboutonv1.h ) {
  error("la lib abuleduexercicev1 dépend de abuleduflatboutonv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduFlatBoutonV1")
}

!exists($$PWD/../abuledumessageboxv1/abuledumessageboxv1.h ) {
  error("la lib abuleduexercicev1 dépend de abuledumessageboxv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduMessageBoxV1")
}

!exists($$PWD/../abuledumessageboxv1/abuledumessageboxv1.h ) {
  error("la lib abuleduexercicev1 dépend de abuledumessageboxv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduMessageBoxV1")
}

!exists($$PWD/../maia/maia.pri) {
  error("la lib abuleduexercicev1 dépend de maia: https://redmine.ryxeo.com/projects/ryxeo/wiki/XMLRPC_avec_Maia")
}

!exists($$PWD/../abuledumultimediav1/abuledumultimediav1.h ) {
  error("la lib abuleduexercicev1 dépend de abuledumultimediav1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduMediaplayerV1")
}
