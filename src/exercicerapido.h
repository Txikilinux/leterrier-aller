/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2012 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
  * @see The GNU Public License (GPL)
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program; if not, write to the Free Software Foundation, Inc.,
  * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */

#ifndef EXERCICERAPIDO_H
#define EXERCICERAPIDO_H

#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QTimer>
#include <QScrollBar>
#include "abuleducommonstatesv1.h"
#include "abuleduoutilsv1.h"
#include "abuledutextv1.h"
#include "abuleduetiquettesv1.h"

class ExerciceRapido : public AbulEduCommonStatesV1, public AbulEduOutilsV1
{
    Q_OBJECT
public:
    ExerciceRapido(QWidget *parent = 0, QString repertoireBase = "");

    ~ExerciceRapido();

    /** Lit dans le fichier module.conf ou tire au sort les mots qui vont être utilisés dans l'exercice exerciseNumber */
    static QStringList takeExerciseWords(const QString &unitPath, int exerciseNumber, AbulEduTextV1 *text);

protected:
    /** Vérifie la réponse proposée
      *@return le nombre d'erreurs
      */
    int verifieReponse();

    /** Donne la bonne réponse à l'utilisateur
      * Renvoie une évaluation abe::evalZ à la question
      * @return par convention -1 correspond à "abandon" */
    int donneReponse();

private:
    /** Stocke le lien vers le fichier module.conf et permet son parcours pour lecture et écriture */
    QSettings* m_config;

    /** Une AbulEduMessageBoxV1 déclarée en attribut, et réutilisée plutôt que d'en créer une à chaque fois et la détruire à la fermeture */
    AbulEduMessageBoxV1* m_messageBox;

    /** Stocke le texte de l'exercice en cours dans un AbuleduTexte */
    AbulEduTextV1* m_texte;

//    /** Stocke les mots du fichier spécial "mot_aller_cycle3.csv" de Jean Louis
//      * Chaque QString de la liste aura le format mot->niveau->nature */
//    QStringList* m_motsOutils;

    /** Stocke les réponses fournies aux différentes questions */
    QStringList m_reponsesDonnees;

    /** Stocke les réponses attendues aux différentes questions */
    QStringList m_reponsesAttendues;

    /** La réussite à l'exercice
      * On stocke le numéro de l'exercice puis un nombre en fonction du pourcentage de réussite
      */
    QList<QPair<int, int> > m_resultats;

    /** Stocke le QWidget parent */
    QWidget* m_parent;

    /** Stocke la liste des mots qui va apparaître pour un exercice
      * @var J'utilise une QMap pour stocker les mots avec un index
      * index qui sera utilisé pour stocker les erreurs */
    QMap<int,QString>* m_listeMotsExercice;

    /** Liste de tous les mots du texte, pour un accès sans recalcul de la fonction abetextCompteMots() d'AbuleduTexte */
    QMap<int,QString> m_listeTousLesMots;

    /** Stocke le répertoire de travail (enregistrements, accès au fichier de configuration, au texte) */
    QDir* m_directoryWork;

    /** AbulEduEtiquettesV1 est dérivée de qGraphicsview */
   AbulEduEtiquettesV1* m_AireDeJeu;

   /** Pour stocker AbulEduLabelWidget */
   QGraphicsProxyWidget* m_proxy;

   /** Stocke le texte du mot affiché dans une question */
   QString m_currentWord;

   AbulEduPieceV1* m_currentPiece;

   /** Stocke l'évaluation de la dernière réponse à une question (clic sur une étiquette) */
   bool m_isTheRightOne;

   bool m_isVericationProcessing;

   /** Stocke la valeur de la clef du module.conf "afficherErreurs" */
   bool m_isErrorShown;

   /** Stocke le niveau de difficulté comme entier
     * Il sera la clef d'une QMap ouvrant la porte aux traductions */
   int m_numLevel;

   /** Vaudra Qt::CaseInsensitive ou Qt::CaseSensitive */
   Qt::CaseSensitivity m_caseSensitivity;

   /** Pointeur vers la QProgressBar dans laquelle le mot à trouver est affiché */
   QPointer<QProgressBar> m_avancement;

   QPointer<QLabel> m_motAtrouver;

   int m_longueurMaxMot;

   /** QTimeLine destinée à faire avancer la QProgressBar d'affichage du mot à trouver
     * Signal finished()) connecté au slotVerificationQuestionEntered()
     * Détruite dans slotVerificationQuestionEntered() */
   QPointer<QTimeLine> m_timeLine;

   /** Retourne la première occurence du mot recherché dans le texte
     * @return retourne un pointeur vers l'AbulEduPieceV1 de la scene qui le contient */
   AbulEduPieceV1* findCorrectWord(QString word);

   /** Lit dans le fichier module.conf ou tire au sort les mots qui vont être utilisés dans les exercices, éventuellement en fonction de contraintes */
   void takeWords(QMap<int,QString> indexedList);


   /** Affiche les mots du texte dans des AbulEduPieceV1
     * L'affichage est limité aux 3/4 supérieurs de l'aire de jeu */
   void displayTextAsWords();

   /** Affiche le mot qui doit être trouvé dans le texte dans une QProgressBar qui montre le temps qui reste pour le trouver
     * Mis à jour après slotVerificationQuestionEntered() */
   void displayWordToBeFound(QString wordToBeFound);

   /** Garde pour un exercice le nombre de réponses exactes
     * Remis à zéro dans slotRealisationExerciceEntered() */
   int m_nbReponsesExactes;

   void redimensionneConsigne();
   void redimensionneBilan();

   bool m_localDebug;

   /** Stocke le taux de réussite sous forme de pourcentage */
   qreal m_percent;

   float m_pointSize;

private slots:
    void slotSequenceEntered();
    void slotPresenteSequenceEntered();
    void slotExercicesEntered();
    void slotPresentationExerciceEntered();
    void slotRealisationExerciceEntered();
    void slotInitQuestionEntered();
    void slotQuestionEntered();
    void slotAfficheVerificationQuestionEntered();
    void slotFinVerificationQuestionEntered();
    void slotAfficheCorrectionQuestionEntered();
    void slotFinCorrectionQuestionEntered();
    void slotFinQuestionEntered();
    void slotBilanExerciceEntered();
    void slotBilanSequenceEntered();
    void slotBilanSequenceExited();
    void slotQuitter();

    void setDimensionsWidgets();
    void verifyClicEtiquette(AbulEduPieceV1*);
    void postQuestionVerifieEvent();

signals:
    void exerciceExited();

};

#endif // EXERCICERAPIDO_H
