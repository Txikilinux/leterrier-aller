
# a faire -> fichier pri pour simplifier la vie des developpeurs comme d'hab
!android {
  unix:!macx {
    !exists(/usr/include/hunspell ) {
      error("Dependance manquante apt-get install libhunspell-dev https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduHunspellV1")
    }
    LIBS += -lhunspell
  }
}
android {
    #ajout de la lib SSL dans android ...
    exists(/opt/android/AndroidHunspellService/libs/armeabi) {
      LIBS += -L/opt/android/AndroidHunspellService/libs/armeabi
      LIBS += -lhunspell

      INCLUDEPATH += $$ANDROID_OPENSSL/include/
      ANDROID_EXTRA_LIBS += /opt/android/AndroidHunspellService/libs/armeabi/libhunspell.so
  }
  !exists(/opt/android/AndroidHunspellService/libs/armeabi) {
    warning("La lib Hunspell pour android n'est pas présente dans /opt/android/AndroidHunspellService/libs/armeabi")
  }

}

macx {
    LIBS += /Developer/SDKs/MacOSX10.6.sdk/usr/lib/libhunspell-1.2.dylib
}

#sous windows, on simplifie le boulot de packaging (hudson) en ayant les libs dans C:/code
win32 {
    #si C:/code/*hunspell.dll existe
    exists(C:/code/*hunspell*.dll) {
      LIBS += -LC:/code/ -lhunspell
    }
    #sinon
    !exists(C:/code/*hunspell*.dll) {
      !exists($$PWD/lib/*hunspell*.dll ) {
        error("Pensez a compiler la lib hunspell en ouvrant le .pro: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduHunspellV1")
      }
      LIBS += -L$$PWD/lib/ -lhunspell
    }
}

INCLUDEPATH += $$PWD/ \
    $$PWD/src/ \
    $$PWD/src/hunspell \
    $$PWD/src/parsers \
    $$PWD/src/tools \
    $$PWD/src/win_api
