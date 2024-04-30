#include <QObject>
#include "abuledupageaccueilv1.h"

class AbulEduPageAccueilV1Translation : public QObject
{
    /** Special pour traductions : qt translator pourra extraire ces chaines a traduire
      * attention, le contexte AbulEduPageAccueilV1 est important pour que la localisation
      * s'applique bien à l'objet AbulEduPageAccueilV1
      */
    const QString a001 = AbulEduPageAccueilV1::trUtf8("Exercice 1 ...");
    const QString a002 = AbulEduPageAccueilV1::trUtf8("Exercice 2 ...");
    const QString a003 = AbulEduPageAccueilV1::trUtf8("Exercice 3 ...");

    // etc ...
}
