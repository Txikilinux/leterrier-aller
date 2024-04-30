/**
   *Aller (le terrier d'AbulEdu)
  *
   *@warning aucun traitement d'erreur n'est pour l'instant implémenté
   *@see https://redmine.ryxeo.com/projects/leterrier-aller
   *@author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
   *@see The GNU Public License (GPL)



  *This program is free software; you can redistribute it and/or modify
  *it under the terms of the GNU General Public License as published by
  *the Free Software Foundation; either version 2 of the License, or
  *(at your option) any later version.
 *
  *This program is distributed in the hope that it will be useful, but
  *WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  *or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  *for more details.
 *
  *You should have received a copy of the GNU General Public License along
  *with this program; if not, write to the Free Software Foundation, Inc.,
  *59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef ABULEDUSTATEMACHINEV1_H
#define ABULEDUSTATEMACHINEV1_H
#include <QMainWindow>
#include <QTextEdit>
#include <QDesktopWidget>
#include <QStateMachine>
#include <QSettings>
#include <QKeyEventTransition>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QCloseEvent>
#include "stringtransition.h"
#include "abuleduexercicev1.h"
#include "abuleduapplicationv1.h"
#include "abuledugraphicsteteswidgetv1.h"
#include "abuledumessageboxv1.h"
#include "abuleduloggerv1.h"

/**
   *Attention, le #include ci dessous est necessaire pour que le type de l'objet ui soit "connu" et donc eviter le message de compilation
   *error: invalid use of incomplete type ‘struct Ui::Sequence’
  */
//#include "ui_abuledustatemachinev1.h"

/**
  * @file abuledustatemachinev1.h
  * @version 1
  * @brief Cette classe héritant de AbulEduExercicesV1, instancie une Machine à états finis. Cette QStateMachine gère le déroulement d'une séquence d'exercices
  * comportant un certain nombre d'items chacun.
  * Pour faire vite, une QStateMachine s'organise en un certain nombre d'états, le passage d'un état à un autre se fait grâce à des transitions déclenchées par des
  * événements (clic sur un bouton, appui sur une touche, test d'une variable...)
  * Le schéma de la page BoiteAOutilsV1 présente l'organisation simplifiée de la AbulEduStateMachine
  * (!) Cette classe ne devrait pas être utilisée directement, il faut passer par la AbulEduCommonStateV1
  * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduCommonStatesV1
  * @link http://doc.qt.nokia.com/4.7-snapshot/statemachine-api.html
  */

class AbulEduStateMachineV1 :public AbulEduExerciceV1
{
    Q_OBJECT

public:
    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le composant graphique parent de cet objet, initialise par defaut a 0
      */
    explicit AbulEduStateMachineV1(QWidget *parent = 0);

    /** Destructeur par defaut de la classe
      */
    virtual ~AbulEduStateMachineV1();

    /** Permet de définir le nombre d'exercices qui composent la séquence
      * @obsolete
      * @param int, entier, le nombre d'exercices que l'on souhaite
      * @return void
      */
    void setAbeNbExercices(int);

    inline void abeStateMachineSetDoWaitToContinue(bool doWaitToContinue){m_doWaitToContinue = doWaitToContinue;}

protected:
    /** @todo
      */
    void changeEvent(QEvent *event);

    /** Creer la state machine qui prendra en charge le déroulement de l'exercice
      * @return void
      */
    void initSequenceStateMachine();

    QStateMachine *sequenceMachine;
    QState *sequence;
    QState *testJouerPresentationSequence;
    QState *initSequence;
    QState *presentationSequence;
    QState *finInitSequence;
    QState *exercices;
    QState *testJouerPresentationExercice;
        QState *initExo;
        QState *presentationExercices;
        QState *finInitExo;
        QState *realisationExercice;
            QState *initQuestion;
            QState *question;
            QState *continueQuestion;
            QState *afficheVerificationQuestion;
            QState *finVerificationQuestion;
            QFinalState *finQuestion;
        QState *testJouerBilanExercice;
        QState *bilanExercice;
        QState *finExercice;
        QFinalState *exitExercice;
    QState *bilanSequence;
    QFinalState *termineSequence;

    QState *afficheCorrectionQuestion; //jlf 20120610
    QState *finCorrectionQuestion; //jlf 20120610

    QSignalTransition *t_InitSequencePresentationSequence;
    QSignalTransition *t_PresentationSequenceFinInitSequence;
    QSignalTransition *t_PresentationSequenceFinInitSequence2;
    QSignalTransition *t_InitExoPresentationExo;
    QSignalTransition *t_PresentationFinInitExo;
    StringTransition *t_QuestionsLoopCorrect;
    StringTransition *t_QuestionsDoneCorrect;
    StringTransition *t_QuestionRedoCorrect;
    StringTransition *t_QuestionsLoop;
    StringTransition *t_QuestionsDone;
    StringTransition *t_QuestionRedo;
    StringTransition *t_QuestionContinue;
    StringTransition *t_QuestionVerifie;
    StringTransition *t_QuestionVerifieContinue;
    StringTransition *t_QuestionVerifieEmpty;
    StringTransition *t_exercicesLoop;
    StringTransition *t_exercicesDone;
    StringTransition *t_passerPresentationSequence;
    StringTransition *t_jouerPresentationSequence;
    StringTransition *t_passerPresentationExercice;
    StringTransition *t_jouerPresentationExercice;
    StringTransition *t_passerBilanExercice;
    StringTransition *t_jouerBilanExercice;
    StringTransition *t_finCorrectionQuestion;
    StringTransition *t_finVerificationQuestion;
    QKeyEventTransition *t_toucheEntreeQuestionTransition;
    QKeyEventTransition *t_toucheEnterQuestionTransition;
    QKeyEventTransition *t_toucheEntreePresentationSequenceTransition;
    QKeyEventTransition *t_toucheEntreePresentationExercicesTransition;
    QSignalTransition *t_initQuestionFinished;
    QSignalTransition *t_sequenceFinished;

//    /** Le fichier de configuration de l'exercice */
    /** Le nombre d'exercices de la séquence */
    int m_nbExercices;

    /** Lecture du nombre d'exercices d'un type présent dans un module */
    inline int abeStateMachineGetNbExercices() {return m_nbExercices;}

    /** Écriture du nombre d'exercices présents dans un séquence */
    inline void abeStateMachineSetNbExercices(int nb) {m_nbExercices = nb;}

    /** Le numéro de l'exercice en cours */
    int m_numExercice;

    /** Lecture du numéro d'exercice (pas de setteur, il est incrémenté par l'AbulEduStateMachineV1 */
    inline int abeStateMachineGetNumExercice() {return m_numExercice;}

    /** Nombre de fois où on a proposé une réponse pour le bouton Vérifier */
    int m_nbClicSurVerifier;

    /** Lecture du nombre de fois où on a proposé une réponse pour le bouton Vérifier */
    inline int abeStateMachineGetNbClicSurVerifier() {return m_nbClicSurVerifier;}

    /** Écriture du nombre de fois où on a proposé une réponse pour le bouton Vérifier */
    inline void abeStateMachineSetNbClicSurVerifier(int nombreClics) {m_nbClicSurVerifier = nombreClics;}

    /** Teste le résultat de l'item
      * @return resultat
      */

    /** QHBoxLayout avec les tête d'évaluation des exercices */
//    AbulEduTeteV1 *boiteTetes;
    AbulEduGraphicsTetesWidgetV1 *boiteTetes;

    /** Accès en lecture à boiteTetes */
    inline AbulEduGraphicsTetesWidgetV1* abeStateMachineGetBoiteTetes() {return boiteTetes;}

    /** Permet de récupérer la variable m_verifieReponse depuis une classe fille */
    inline int abeStateMachineGetVerifieReponse() {return m_verifieReponse;}

    /** Permet d'écrire une valeur pour la variable m_verifieReponse depuis une classe fille */
    inline void abeStateMachineSetVerifieReponse(int nombreErreurs) {m_verifieReponse = nombreErreurs;}

    /** Fonction à dériver dans les classes qui héritent d'AbulEduStateMachineV1
      * Cette fonction est virtuelle pure puisque l'évaluation de la réponse dépend de la question posée
      * @return le nombre d'erreurs dans la réponse */
    virtual int verifieReponse() = 0;

    /** Fonction à dériver dans les classes qui héritent d'AbulEduStateMachineV1
      * Cette fonction est virtuelle pure puisque la réponse dépend de la question posée
      * @return -1 qui sera interprété comme "abandon" */
    virtual int donneReponse() = 0;

    /** Identifiant des delayedPostEvent */
    int id_temporisation;

    bool onPeutPresenterSequence;
    bool onPeutPresenterExercice;
    bool onPeutPresenterBilanExercice;

    /** Écriture du booléen autorisant l'affichage des consignes de séquence */
    inline void abeStateMachineSetOnPeutPresenterSequence(bool trueFalse) {onPeutPresenterSequence = trueFalse;}

    /** Écriture du booléen autorisant l'affichage des consignes de l'exercice en cours */
    inline void abeStateMachineSetOnPeutPresenterExercice(bool trueFalse) {onPeutPresenterExercice = trueFalse;}

    /** Écriture du booléen autorisant l'affichage du bilan de l'exercice en cours */
    inline void abeStateMachineSetOnPeutPresenterBilanExercice(bool trueFalse) {onPeutPresenterBilanExercice = trueFalse;}

    /** Permet de savoir si on est dans une vérification ou une correction
      * @deprecated */
    bool m_isCorrection;

    /** Definit la dimension de widget
    * @todo methode a implementer
    */
    void setDimensionWidgets();

    /** Met à jour la variable m_helpText */
    void setHelpText(const QString&);

    /** Permet de mettre les événements clavier "en pause", notamment si on affiche une AbulEduMessageBoxV1 */
    bool m_isAdjourned;

private:

    QString m_helpText;

    /** Stocke le nombre d'erreurs renvoyé par les fonctions d'analyse ou d'affichage de la réponse
      * @value -1 signifie que la correction a été demandée */
    int m_verifieReponse;

    /** Contrôle si on continue après vérification évaluée fausse immédiatement ou après 3 secondes */
    bool m_doWaitToContinue;

protected slots:
    /** Donne la valeur true ou false à m_isAdjourned, désactivant ou réactivant ainsi les événements clavier */
    void slotAbeStateMachineSetAdjourned(bool trueFalse = false) {m_isAdjourned = trueFalse;}


    /** Slot de commencement de la sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotSequenceEntered();

    /** Slot de fin de la sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotSequenceExited();

    /** Slot de commencement de test jouer sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de test de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotTestJouerSequenceEntered();

    /** Slot de commencement de la presentation sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la presentation de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresenteSequenceEntered();

    /** Slot de fin de presentation sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de presentation de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresenteSequenceExited();

    /** Slot de commencement exercices
      * @brief Ce slot contient le code a executer a l'etat de commencement de(s) exercice(s)
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotExercicesEntered();

    /** Slot de fin exercices
      * @brief Ce slot contient le code a executer a l'etat de fin de(s) exercice(s)
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotExercicesExited();

    /** Slot de commencement test jouer exercices
      * @brief Ce slot contient le code a executer a l'etat de commencement de test de(s) exercice(s)
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotTestJouerExerciceEntered();

    /** Slot de commencement de presentation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la presentation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresentationExerciceEntered();

    /** Slot de fin de presentation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de presentation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotPresentationExerciceExited();

    /** Slot de commencement de realisation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la realisation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotRealisationExerciceEntered();

    /** Slot de fin de realisation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de la realisation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotRealisationExerciceExited();

    /** Slot de commencement bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement du bilan de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanExerciceEntered();

    /** Slot de fin bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de fin du bilan de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanExerciceExited();

    /** Slot de commencement bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement du bilan de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotFinExerciceEntered();

    /** Slot de fin bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de fin du bilan de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotFinExerciceExited();

    /** Slot de commencement bilan sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement du bilan de la sequence
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanSequenceEntered();

    /** Slot de fin bilan sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de fin du bilan de la sequence
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanSequenceExited();

    /** Slot de commencement terminer sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de terminaison de la sequence
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotTermineSequenceEntered();

    /** Slot de fin terminer sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de terminaison de la sequence
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotTermineSequenceExited();

    /** Slot de commencement initialisation question
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de l'initialisation de la question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotInitQuestionEntered();

    /** Slot de fin initialisation question
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de l'initialisation de la question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotInitQuestionExited();

    /** Slot de commencement question
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotQuestionEntered();

    /** Slot de fin question
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de la question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotQuestionExited();
    virtual void slotAfficheVerificationQuestionEntered();
    virtual void slotFinVerificationQuestionEntered();

    /** Slot de continuation d'exercice
      * @brief Slot de commencement continuer dans l'exercice après une vérification évaluée fausse
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotContinueQuestionEntered();

    /** Slot de commencement test jouer bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement du test de jouer le bilan exercice
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotTestJouerBilanExerciceEntered();

    /** Slot de commencement fin question
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de fin de la question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotFinQuestionEntered();

    /** Slot de fin fin question
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de fin de la question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotFinQuestionExited();

    /** Slotquitter
      * @brief Ce slot contient le code qui s'executera a l'etat quitter
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotQuitter();

    /** Slot annuler quitter
      * @brief Ce slot contient le code qui s'executera a l'etat d'annulation de quitter
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotQuitterAnnuler();

    /** Slot quitter accueil
      * @brief Ce slot contient le code qui s'executera a l'etat de quitter l'accueil
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @todo methode a implementer
      * @return void
      */
    void slotQuitterAccueil();

    /** Slot aide
      * @brief Ce slot contient le code qui s'executera a l'etat d'aide
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotAide();

    /*** erics*/
    virtual void slotAfficheCorrectionQuestionEntered();

    /** Slot de commencement de fin corriger question
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la fin corriger question
      * @linK https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotFinCorrectionQuestionEntered();

    /** @todo methode a implementer
      */
    void closeEvent(QCloseEvent *);

    /** @todo methode a implementer
      */
    void showEvent(QShowEvent *);

    /** @todo methode a implementer
      */
    void resizeEvent(QResizeEvent *);

    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    /** Signal de fin init question
      * @brief Ce signal emet la fin de l'initialisation de la question
      */
    void signalinitQuestionFinished();

    /** Signal de fin init exercice
      * @brief Ce signal emet la fin de l'initialisation de l'exercice
      */
    void signalInitExoFinished();

    /** Signal de fin sequence
      * @brief Ce signal emet la fin de la sequence
      */
    void signalSequenceFinished();

    /** Signal de fin affiche correction
      * @brief Ce signal emet la fin de l'affichage de la correction
      */
    void signalFinAfficheCorrection();

private slots:


};


#endif // ABULEDUSTATEMACHINEV1_H
