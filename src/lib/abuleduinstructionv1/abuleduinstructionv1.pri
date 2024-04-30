 INCLUDEPATH += $$PWD
 DEPENDPATH += $$PWD

HEADERS += $$PWD/abuleduinstructionv1.h
 
 SOURCES += $$PWD/abuleduinstructionv1.cpp
 
 FORMS += $$PWD/abuleduinstructionv1.ui

 
# si fichier ressource, on inclut le fichier ressources du developpeur de
# l'application en prioritemais seulement s'il existe. S'il n'existe pas on
# fallback sur le fichier de ressources propose par la lib
 !exists($$PWD/../../data/abuleduinstructionv1/abuleduinstructionv1.qrc) {
  RESOURCES += $$PWD/abuleduinstructionv1.qrc
 }
 exists($$PWD/../../data/abuleduinstructionv1/abuleduinstructionv1.qrc) {
   RESOURCES += $$PWD/../../data/abuleduinstructionv1/abuleduinstructionv1.qrc
}
