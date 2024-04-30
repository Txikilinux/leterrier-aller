/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)



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

#include "abuledustatemachinev1.h"

#ifndef DEBUG_ABULEDUSTATEMACHINEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduStateMachineV1::AbulEduStateMachineV1(QWidget *parent) :
    AbulEduExerciceV1(parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    m_isAdjourned = false;
    abeApp->installEventFilter(this);
    onPeutPresenterSequence = true;
    onPeutPresenterExercice = true;
    onPeutPresenterBilanExercice = true;
    m_nbExercices = 1;
    m_numExercice = 0;
    setAbeNumQuestion(0);
    m_numQuestionInSequence = 0;
    id_temporisation = 0;

    getAbeExerciceTelecommandeV1()->ui->framePopupQuitter->setVisible(false);
    getAbeExerciceTelecommandeV1()->ui->btnCorriger->setVisible(true);
    getAbeExerciceTelecommandeV1()->ui->btnCorriger->setEnabled(false);
    getAbeExerciceTelecommandeV1()->ui->btnRefaire->setVisible(true);
    connect(getAbeExerciceTelecommandeV1(), SIGNAL(btnTelecommandeQuitterClicked()),this, SLOT(slotQuitter()));
    connect(getAbeExerciceTelecommandeV1(), SIGNAL(btnTelecommandeQuitterFermerClicked()),this, SLOT(slotQuitterAccueil()));
    connect(getAbeExerciceTelecommandeV1(), SIGNAL(btnTelecommandeQuitterAnnulerClicked()),this, SLOT(slotQuitterAnnuler()));

    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Démarrage de la machine à états finis
    initSequenceStateMachine();
}

AbulEduStateMachineV1::~AbulEduStateMachineV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    getAbeExerciceMessageV1()->abeWidgetMessageClose();
    getAbeExerciceTelecommandeV1()->abeTelecommandeClose();
    getAbeExerciceAireDeTravailV1()->abeAireDeTravailClose();
    delete sequenceMachine;
    /* Pour mémoire, au cas où ça changerait, mais cet objet n'est pas (plus ?) instancié */
    //delete boiteTetes;
}

void AbulEduStateMachineV1::initSequenceStateMachine()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    sequenceMachine     = new QStateMachine(this);
    m_doWaitToContinue = false;

//----------------------------------------------------------------------------
// Définition des états de la "Machine Séquence"
//----------------------------------------------------------------------------
    sequence            = new QState(sequenceMachine); // Etat englobant toute la séquence d'execices
    sequence->setObjectName("sequence");
      testJouerPresentationSequence = new QState(sequence);
      testJouerPresentationSequence->setObjectName("testJouerPresentationSequence");
      initSequence      =new QState(sequence);// Ces états héritent de exercices et forment un "groupe"
      initSequence->setObjectName("initSequence");
      presentationSequence  = new QState(sequence);
      presentationSequence->setObjectName("presentationSequence");
      finInitSequence   = new QState(sequence);
      finInitSequence->setObjectName("finInitSequence");
      exercices         = new QState(sequence);
      exercices->setObjectName("exercices");
      testJouerPresentationExercice = new QState(exercices);
      testJouerPresentationExercice->setObjectName("testJouerPresentationExercice");
          initExo           = new QState(exercices);
          initExo->setObjectName("initExo");
          presentationExercices   = new QState(exercices);
          presentationExercices->setObjectName("presentationExercices");
          finInitExo       = new QState(exercices);
          finInitExo->setObjectName("finInitExo");
          realisationExercice       = new QState(exercices);// Ici on est dans l'exercice lui même
          realisationExercice->setObjectName("realisationExercice");
             initQuestion   = new QState(realisationExercice);
             initQuestion->setObjectName("initQuestion");
             question       = new QState(realisationExercice);
             question->setObjectName("Question");
             continueQuestion = new QState(realisationExercice);
             continueQuestion->setObjectName("continueQuestion");
             afficheVerificationQuestion = new QState(realisationExercice);
             afficheVerificationQuestion->setObjectName("afficheVerificationQuestion");
             finVerificationQuestion = new QState(realisationExercice);
             finVerificationQuestion->setObjectName("finVerificationQuestion");
             afficheCorrectionQuestion = new QState(realisationExercice);
             afficheCorrectionQuestion->setObjectName("afficheCorrectionQuestion");
             finCorrectionQuestion = new QState(realisationExercice);
             finCorrectionQuestion->setObjectName("finCorrectionQuestion");
             finQuestion    = new QFinalState(realisationExercice);
             finQuestion->setObjectName("finQuestion");
          testJouerBilanExercice = new QState (exercices);
          testJouerBilanExercice->setObjectName("testJouerBilanExercice");
          bilanExercice = new QState(exercices);
          bilanExercice->setObjectName("bilanExercice");
      finExercice       = new QState(exercices);
      finExercice->setObjectName("finExercice");
      exitExercice      = new QFinalState(exercices);
      exitExercice->setObjectName("exitExercice");

      bilanSequence       = new QState(sequence);
      bilanSequence->setObjectName("bilanSequence");
    termineSequence     = new QFinalState(sequenceMachine);
    termineSequence->setObjectName("termineSequence");

//----------------------------------------------------------------------------
//             Définitions de transitions
//----------------------------------------------------------------------------
    t_sequenceFinished = new QSignalTransition(this, SIGNAL(signalSequenceFinished()));
    t_sequenceFinished->setTargetState(bilanSequence);
    t_initQuestionFinished = new QSignalTransition(this, SIGNAL(signalinitQuestionFinished()));
    t_initQuestionFinished->setTargetState(question);

    t_toucheEntreeQuestionTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, question);
    t_toucheEntreeQuestionTransition->setTargetState(afficheVerificationQuestion);
    t_toucheEnterQuestionTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Enter, question);
    t_toucheEnterQuestionTransition->setTargetState(afficheVerificationQuestion);

    // Animations de la présentation des exercices : création des QSignalTransition
    t_InitSequencePresentationSequence =  initSequence->addTransition(initSequence, SIGNAL(entered()), presentationSequence);
    t_toucheEntreePresentationSequenceTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, sequence);
    t_toucheEntreePresentationSequenceTransition->setTargetState(finInitSequence);
    t_PresentationSequenceFinInitSequence = presentationSequence->addTransition(getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()),finInitSequence);
    t_InitExoPresentationExo = initExo->addTransition(initExo, SIGNAL(entered()),presentationExercices);
    t_toucheEntreePresentationExercicesTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, exercices);
    t_toucheEntreePresentationExercicesTransition->setTargetState(finInitExo);
    t_PresentationFinInitExo =  presentationExercices->addTransition(getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()),finInitExo);

    // Guarded Transitions
    t_QuestionsLoopCorrect = new StringTransition("QuestionsLoopCorrect");
    t_QuestionsLoopCorrect->setTargetState(initQuestion);
    t_QuestionsDoneCorrect = new StringTransition("QuestionsDoneCorrect");
    t_QuestionsDoneCorrect->setTargetState(finQuestion);
    t_QuestionRedoCorrect = new StringTransition("QuestionRedoCorrect");
    t_QuestionRedoCorrect->setTargetState(question);
    t_QuestionsLoop = new StringTransition("Questionsloop");
    t_QuestionsLoop->setTargetState(initQuestion);
    t_QuestionsDone = new StringTransition("Questionsdone");
    t_QuestionsDone->setTargetState(finQuestion);
    t_QuestionRedo = new StringTransition("Questionredo");
    t_QuestionRedo->setTargetState(question);
    t_QuestionContinue = new StringTransition("Questioncontinue");
    t_QuestionContinue->setTargetState(continueQuestion);
    t_exercicesLoop = new StringTransition("exercicesloop");
    t_QuestionVerifie = new StringTransition("QuestionVerifie");
    t_QuestionVerifie->setTargetState(afficheVerificationQuestion);
    t_QuestionVerifieEmpty = new StringTransition("QuestionVerifieEmpty");
    t_QuestionVerifieEmpty->setTargetState(afficheVerificationQuestion);
    t_QuestionVerifieContinue = new StringTransition("QuestionVerifieContinue");
    t_QuestionVerifieContinue->setTargetState(afficheVerificationQuestion);

    t_exercicesLoop->setTargetState(testJouerPresentationExercice);
    t_exercicesDone = new StringTransition("exercicesdone");
    t_exercicesDone->setTargetState(exitExercice);
    t_passerPresentationSequence = new StringTransition("passePresentationSequence");
    t_passerPresentationSequence->setTargetState(finInitSequence);
    t_jouerPresentationSequence = new StringTransition("jouePresentationSequence");
    t_jouerPresentationSequence->setTargetState(initSequence);
    t_jouerPresentationExercice = new StringTransition("jouePresentationExercice");
    t_jouerPresentationExercice->setTargetState(initExo);
    t_passerPresentationExercice = new StringTransition("passePresentationExercice");
    t_passerPresentationExercice->setTargetState(finInitExo);
    t_jouerBilanExercice = new StringTransition("joueBilanExercice");
    t_jouerBilanExercice->setTargetState(bilanExercice);
    t_passerBilanExercice = new StringTransition("passeBilanExercice");
    t_passerBilanExercice->setTargetState(finExercice);
    t_finCorrectionQuestion = new StringTransition("finAfficheCorrection");
    t_finCorrectionQuestion->setTargetState(finCorrectionQuestion);
    t_finVerificationQuestion = new StringTransition("finAfficheVerification");
    t_finVerificationQuestion->setTargetState(finVerificationQuestion);

//----------------------------------------------------------------------------
//             Paramétrage des états et des transitions
//----------------------------------------------------------------------------
    sequenceMachine->setInitialState(sequence); // Ne pas oublier le point d'entrée !
    sequence->setInitialState(testJouerPresentationSequence);
    sequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide, "enabled", false);
    sequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);

    connect(sequence, SIGNAL(entered()), this, SLOT(slotSequenceEntered()));
    connect(sequence, SIGNAL(exited()), this, SLOT(slotSequenceExited()));

    testJouerPresentationSequence->addTransition(t_passerPresentationSequence);
    testJouerPresentationSequence->addTransition(t_jouerPresentationSequence);
    connect(testJouerPresentationSequence, SIGNAL(entered()), this, SLOT(slotTestJouerSequenceEntered()));

    presentationSequence->addTransition(t_toucheEntreePresentationSequenceTransition);

    connect(presentationSequence, SIGNAL(entered()), this, SLOT(slotPresenteSequenceEntered()));
    connect(presentationSequence, SIGNAL(exited()), this, SLOT(slotPresenteSequenceExited()));
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "text", trUtf8("Exercices"));
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);

    finInitSequence->addTransition(finInitSequence, SIGNAL(entered()), exercices);
    exercices->setInitialState(testJouerPresentationExercice);
    exercices->addTransition(exercices, SIGNAL(finished()), bilanSequence);
    connect(exercices, SIGNAL(entered()), this, SLOT(slotExercicesEntered()));
    connect(exercices, SIGNAL(exited()), this, SLOT(slotExercicesExited()));

    testJouerPresentationExercice->addTransition(t_passerPresentationExercice);
    testJouerPresentationExercice->addTransition(t_jouerPresentationExercice);
    connect(testJouerPresentationExercice, SIGNAL(entered()), this, SLOT(slotTestJouerExerciceEntered()));

    presentationExercices->addTransition(t_toucheEntreePresentationExercicesTransition);
    connect(presentationExercices, SIGNAL(entered()), this, SLOT(slotPresentationExerciceEntered()));
    connect(presentationExercices, SIGNAL(exited()), this, SLOT(slotPresentationExerciceExited()));
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "text", trUtf8("Exercice"));
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide, "enabled", false);
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);

    finInitExo->addTransition(finInitExo, SIGNAL(entered()), realisationExercice);

    realisationExercice->setInitialState(initQuestion);
    realisationExercice->addTransition(realisationExercice, SIGNAL(finished()), testJouerBilanExercice);
    realisationExercice->addTransition(getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()), bilanExercice);
        connect(getAbeExerciceTelecommandeV1(), SIGNAL(btnTelecommandeAideClicked()), this, SLOT(slotAide()));
    connect(realisationExercice, SIGNAL(entered()), this, SLOT(slotRealisationExerciceEntered()));
    connect(realisationExercice, SIGNAL(exited()), this, SLOT(slotRealisationExerciceExited()));
    realisationExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    realisationExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "text", trUtf8("Suite"));
    realisationExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", true);
    realisationExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    realisationExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide, "enabled", true);
    realisationExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", true);

    initQuestion->addTransition(t_initQuestionFinished);
    connect(initQuestion, SIGNAL(entered()), this, SLOT(slotInitQuestionEntered()));
    connect(initQuestion, SIGNAL(exited()), this, SLOT(slotInitQuestionExited()));
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide, "enabled", true);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", true);

    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", true);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", true);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    question->addTransition(getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()), afficheVerificationQuestion);
    question->addTransition(getAbeExerciceTelecommandeV1()->ui->btnCorriger, SIGNAL(clicked()), afficheCorrectionQuestion);
    question->addTransition(t_toucheEntreeQuestionTransition);
    question->addTransition(t_toucheEnterQuestionTransition);
    question->addTransition(t_QuestionVerifie);
    question->addTransition(t_QuestionVerifieEmpty);
    connect(question, SIGNAL(entered()), this, SLOT(slotQuestionEntered()));
    connect(question, SIGNAL(exited()), this, SLOT(slotQuestionExited()));

    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", true);
    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", true);
    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    continueQuestion->addTransition(getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()), afficheVerificationQuestion);
    continueQuestion->addTransition(getAbeExerciceTelecommandeV1()->ui->btnCorriger, SIGNAL(clicked()), afficheCorrectionQuestion);
    continueQuestion->addTransition(t_QuestionVerifieContinue);
    connect(continueQuestion, SIGNAL(entered()), this, SLOT(slotContinueQuestionEntered()), Qt::UniqueConnection);

    afficheCorrectionQuestion->addTransition(t_finCorrectionQuestion);
    afficheCorrectionQuestion->addTransition(getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()), finCorrectionQuestion);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide, "enabled", false);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    connect(afficheCorrectionQuestion, SIGNAL(entered()), this, SLOT(slotAfficheCorrectionQuestionEntered()));

    finCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    finCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    finCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    finCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", true);
    finCorrectionQuestion->addTransition(t_QuestionsLoopCorrect);
    finCorrectionQuestion->addTransition(t_QuestionsDoneCorrect);
    finCorrectionQuestion->addTransition(t_QuestionRedoCorrect);
    connect(finCorrectionQuestion, SIGNAL(entered()), this, SLOT(slotFinCorrectionQuestionEntered()));

    afficheVerificationQuestion->addTransition(t_finVerificationQuestion);
    afficheVerificationQuestion->addTransition(getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()), finVerificationQuestion);
    afficheVerificationQuestion->addTransition(getAbeExerciceTelecommandeV1()->ui->btnCorriger, SIGNAL(clicked()), afficheCorrectionQuestion);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    connect(afficheVerificationQuestion, SIGNAL(entered()),this, SLOT(slotAfficheVerificationQuestionEntered()));

    finVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    finVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    finVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    finVerificationQuestion->addTransition(t_QuestionsLoop);
    finVerificationQuestion->addTransition(t_QuestionsDone);
    finVerificationQuestion->addTransition(t_QuestionRedo);
    finVerificationQuestion->addTransition(t_QuestionContinue);
    connect(finVerificationQuestion, SIGNAL(entered()), this, SLOT(slotFinVerificationQuestionEntered()));

    connect(finQuestion, SIGNAL(entered()), this, SLOT(slotFinQuestionEntered()));
    connect(finQuestion, SIGNAL(exited()), this, SLOT(slotFinQuestionExited()));

    testJouerBilanExercice->addTransition(t_jouerBilanExercice);
    testJouerBilanExercice->addTransition(t_passerBilanExercice);
    connect(testJouerBilanExercice, SIGNAL(entered()), this, SLOT(slotTestJouerBilanExerciceEntered()));

    bilanExercice->addTransition(t_sequenceFinished);
    bilanExercice->addTransition(getAbeExerciceTelecommandeV1()->ui->btnRefaire, SIGNAL(clicked()), testJouerPresentationExercice); // A voir a quel niveau on recommence
    bilanExercice->addTransition(getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()),finExercice); // On passe à la suite
    connect(bilanExercice, SIGNAL(entered()), this, SLOT(slotBilanExerciceEntered()));
    connect(bilanExercice, SIGNAL(exited()), this, SLOT(slotBilanExerciceExited()));
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant,"enabled", true);
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", true);
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", true);
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide, "enabled", false);
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);

    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier,"enabled", false);
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "text", trUtf8("Suite"));

    finExercice->addTransition(t_exercicesLoop);
    finExercice->addTransition(t_exercicesDone);
    connect(finExercice, SIGNAL(entered()), this, SLOT(slotFinExerciceEntered()));
    connect(finExercice, SIGNAL(exited()), this, SLOT(slotFinExerciceExited()));

    connect(bilanSequence, SIGNAL(entered()), this, SLOT(slotBilanSequenceEntered()));
    connect(bilanSequence, SIGNAL(exited()), this, SLOT(slotBilanSequenceExited()));
    bilanSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire, "enabled", false);
    bilanSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant,"enabled", false);

    connect(termineSequence, SIGNAL(entered()), this, SLOT(slotTermineSequenceEntered()));
    connect(termineSequence, SIGNAL(exited()), this, SLOT(slotTermineSequenceExited()));
}

//---------------------------------------------------------------------
//             Slots
//---------------------------------------------------------------------
void AbulEduStateMachineV1::slotTestJouerSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    // Condition de Garde
    if(onPeutPresenterSequence)
    {
        ABULEDU_LOG_DEBUG() << "jouePresentationSequence";
        sequenceMachine->postEvent(new StringEvent("jouePresentationSequence"));
    }
    else
    {
        ABULEDU_LOG_DEBUG() << "passePresentationSequence";
        sequenceMachine->postEvent(new StringEvent("passePresentationSequence"));
    }
}

void AbulEduStateMachineV1::slotSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotSequenceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotPresenteSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotPresenteSequenceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotExercicesEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotExercicesExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotTestJouerExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onPeutPresenterSequence;
    // Condition de Garde
    if(onPeutPresenterExercice)
    {
        ABULEDU_LOG_DEBUG() << "jouePresentationExercice";
        sequenceMachine->postEvent(new StringEvent("jouePresentationExercice"));
    }
    else
    {
        ABULEDU_LOG_DEBUG() << "passePresentationExercice";
        sequenceMachine->postEvent(new StringEvent("passePresentationExercice"));
    }
}

void AbulEduStateMachineV1::slotPresentationExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotPresentationExerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotRealisationExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setAbeNumQuestion(0);
}

void AbulEduStateMachineV1::slotRealisationExerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

/**
  * etat commun à toutes les questions
  */
void AbulEduStateMachineV1::slotInitQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    increaseAbeNumQuestion();
    m_numQuestionInSequence++;
    emit signalinitQuestionFinished();
}

void AbulEduStateMachineV1::slotInitQuestionExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

/**
  * Cette fonction représente une question précise de l'exercice
  * Ce slot est appelé à l'entrée dans l'état question. On sortira de cet état par une des 4 transitions suivantes:
  * getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()), verificationQuestion
  * t_toucheEntreeQuestionTransition
  * t_toucheEnterQuestionTransition
  * t_QuestionVerifie
  */
void AbulEduStateMachineV1::slotQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << " Question "<<getAbeNumQuestion();
    /* Actualise la progressbarr du nombre de questions posées (attention on affiche question 1 alors qu'on est en question zéro)
       20130906 Philippe on pourrait sans doute faire un if(getAbeNbTotalQuestions() <= 1) getAbeExerciceTelecommandeV1()->ui->pbarQuestion->setVisible(false)
       qui éviterait d'avoir à le faire dans les exercices. Alors ligne suivante serait dans le else, par ne pas exécuter du code inutilement... */
}

void AbulEduStateMachineV1::slotQuestionExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotFinQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

/** Note: le déroulé par défaut d'un logiciel utilisant la machine a etat est de passer
  * automatiquement à la question suivante même en cas d'erreur (mauvaise réponse).
  * Si dans votre logiciel vous ne voulez pas de de comportement et "boucler" par exemple
  * sur la question tant que la bonne réponse n'est pas donnée (ou qu'un nombre max de
  * tentatives n'est pas atteint) vous devez surcharger cette fonction et ne pas faire appel
  * a la fonction de la classe dont vous héritez
  */
void AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //    vérifier dans AbulEduCommonstates V1
        m_nbClicSurVerifier++;
        if (m_verifieReponse == 0)
            id_temporisation = sequenceMachine->postDelayedEvent(new StringEvent("finAfficheVerification"), 7000);
        else
            sequenceMachine->postEvent(new StringEvent("finAfficheVerification"));
        ABULEDU_LOG_DEBUG() << " Question "<<getAbeNumQuestion();
}

void AbulEduStateMachineV1::slotFinVerificationQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ <<" Question "<<getAbeNumQuestion();
    sequenceMachine->cancelDelayedEvent(id_temporisation);

    if(m_verifieReponse == 0)
    {
        if(getAbeNumQuestion() == getAbeNbTotalQuestions()) {
            sequenceMachine->postEvent(new StringEvent("Questionsdone"));
            ABULEDU_LOG_DEBUG() << " -- AbulEduStateMachineV1::slotVerificationQuestionEntered() emet \"Questionsdone\"";
        }
        else {
            sequenceMachine->postEvent(new StringEvent("Questionsloop"));
            ABULEDU_LOG_DEBUG() << " -- AbulEduStateMachineV1::slotVerificationQuestionEntered() emet \"Questionsloop\"";
        }
    }
    else
    {
        if(m_doWaitToContinue){
            sequenceMachine->postDelayedEvent(new StringEvent("Questioncontinue"),3000);
            ABULEDU_LOG_DEBUG() << " -- AbulEduStateMachineV1::slotVerificationQuestionEntered() emet \"Questioncontinue\" après 3 secondes ";
        }
        else{
            sequenceMachine->postEvent(new StringEvent("Questioncontinue"));
            ABULEDU_LOG_DEBUG() << " -- AbulEduStateMachineV1::slotVerificationQuestionEntered() emet \"Questioncontinue\"";
        }
    }
}

void AbulEduStateMachineV1::slotContinueQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotFinQuestionExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotBilanExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotBilanExerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotTestJouerBilanExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    // Condition de Garde
    if(onPeutPresenterBilanExercice)
    {
        ABULEDU_LOG_DEBUG() << "joueBilanExercice";
        sequenceMachine->postEvent(new StringEvent("joueBilanExercice"));
    }
    else
    {
        ABULEDU_LOG_DEBUG() << "passeBilanExercice";
        sequenceMachine->postEvent(new StringEvent("passeBilanExercice"));
    }
}

void AbulEduStateMachineV1::slotFinExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ABULEDU_LOG_DEBUG() << " ***************************************************** "
                        <<" Exercice "<< m_numExercice;
    m_numExercice++;
    ABULEDU_LOG_DEBUG() << " Numero Exercice mis à "<<m_numExercice;
    // Condition de Garde
    if(m_numExercice == m_nbExercices)// || abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() != 1)
    {
        m_numExercice = 0;
        sequenceMachine->postEvent(new StringEvent("exercicesdone"));
        /* Désactivation de la restriction d'utilisation si on n'est pas authentifié (et la condition au-dessus)
        if (abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() != 1)
        {
            AbulEduMessageBoxV1* messageAide = new AbulEduMessageBoxV1(trUtf8("Restriction"),trUtf8("En mode anonyme, un seul exercice d'un type donné est possible..."));
            messageAide->show();
        }*/
    }
    else
    {
        sequenceMachine->postEvent(new StringEvent("exercicesloop"));
    }
}

void AbulEduStateMachineV1::slotFinExerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotBilanSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotBilanSequenceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotTermineSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
//    QMessageBox *alertBox=new QMessageBox(QMessageBox::Warning,trUtf8("Fin de l'activité !!! "),trUtf8("C'est fini, on ajoute un traitement de fin"));
//    alertBox->exec();
//    this->close();
    this->deleteLater();
}

void AbulEduStateMachineV1::slotTermineSequenceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduStateMachineV1::slotAide()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduMessageBoxV1 *messageAide = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"),m_helpText,getAbeExerciceAireDeTravailV1());
    messageAide->setWink();
    messageAide->show();
}

void AbulEduStateMachineV1::slotQuitter()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    getAbeExerciceTelecommandeV1()->ui->framePopupQuitter->setVisible(true);
    getAbeExerciceTelecommandeV1()->ui->btnQuitter->setEnabled(false);
}

void AbulEduStateMachineV1::slotQuitterAnnuler()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    getAbeExerciceTelecommandeV1()->ui->framePopupQuitter->setVisible(false);
    getAbeExerciceTelecommandeV1()->ui->btnQuitter->setEnabled(true);
}

void AbulEduStateMachineV1::slotQuitterAccueil()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    this->deleteLater();
}

void AbulEduStateMachineV1::closeEvent(QCloseEvent *e)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    e->accept();
}

// Redimensionnement des Widgets
void AbulEduStateMachineV1::resizeEvent(QResizeEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(event);
}

void AbulEduStateMachineV1::showEvent(QShowEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(event);
}

bool AbulEduStateMachineV1::eventFilter(QObject *obj, QEvent *event)
{
    /* Pas localDebug car il y a trop de message */
    if (event->type() == QEvent::KeyRelease && !m_isAdjourned)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        /* Protection contre les clics frénétiques */
        if(!keyEvent->isAutoRepeat())
        {
            /* Navigation avec la touche Entrée : l'appui sur la touche Entrée provoque : */
            switch(keyEvent->key())
            {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                /* Dans les états question et continueQuestion la Vérification ou la Solution si Ctrl est appuyé conjointement */
                if(sequenceMachine->configuration().contains(question) || sequenceMachine->configuration().contains(continueQuestion))
                {
                    if(keyEvent->modifiers() & Qt::ControlModifier )
                        getAbeExerciceTelecommandeV1()->ui->btnCorriger->click();
                    else
                        getAbeExerciceTelecommandeV1()->ui->btnVerifier->click();
                }
                /* La sortie en deux temps dans l'état bilanSequence */
                else if(sequenceMachine->configuration().contains(bilanSequence))
                        {
                            if(getAbeExerciceTelecommandeV1()->ui->framePopupQuitter->isVisible())
                                getAbeExerciceTelecommandeV1()->ui->btnQuitterRetourMenuPrincipal->click();
                            else
                                getAbeExerciceTelecommandeV1()->ui->btnQuitter->click();
                        }
                else
                /* Le passage à la Suite dans les autres états */
                {
                    getAbeExerciceTelecommandeV1()->ui->btnSuivant->click();
                }
                break;
            default:
                break;
            }
        }
        return true;
    }
    else
    {
        /* On fait suivre l'évènement, sinon tout est bloqué */
        return QObject::eventFilter(obj, event);
    }
}
void AbulEduStateMachineV1::setDimensionWidgets()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
//    if (m_localDebug) qDebug()<<" -- AbulEduStateMachineV1 :: setDimensionWidgets();;";
//    gsPrincipale->setSceneRect(0,0,m_uiAbulEduAireDeTravail->ui->gvPrincipale->width(),m_uiAbulEduAireDeTravail->ui->gvPrincipale->height());
}

void AbulEduStateMachineV1::setHelpText(const QString& text)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_helpText = text;
}

void AbulEduStateMachineV1::changeEvent(QEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(event);
    //    QMainWindow::changeEvent(e);
    //    switch (e->type()) {
    //    case QEvent::LanguageChange:
    //        ui->retranslateUi(this);
//        break;
//    default:
//        break;
//    }
}

void AbulEduStateMachineV1::setAbeNbExercices(int i)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_nbExercices = i;
}

void AbulEduStateMachineV1::slotAfficheCorrectionQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setAbeTeteTelecommande("rien");
    setAbeExerciceEvaluation(abe::evalZ);
    id_temporisation = sequenceMachine->postDelayedEvent(new StringEvent("finAfficheCorrection"), 7000);
}

void AbulEduStateMachineV1::slotFinCorrectionQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    sequenceMachine->cancelDelayedEvent(id_temporisation);

    if (getAbeNumQuestion() == getAbeNbTotalQuestions())
    {
        sequenceMachine->postEvent(new StringEvent("QuestionsDoneCorrect"));
    }
    else
    {
        sequenceMachine->postEvent(new StringEvent("QuestionsLoopCorrect"));
    }
}
