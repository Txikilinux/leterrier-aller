HEADERS += $$PWD/abuledutextv1.h
SOURCES += $$PWD/abuledutextv1.cpp

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!exists($$PWD/../abuledumessageboxv1/abuledumessageboxv1.h ) {
  error("la lib abuledutextv1 dépend de abuledumessageboxv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduMessageBoxV1")
}
