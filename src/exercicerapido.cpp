/**
 * Aller (le terrier d'AbulEdu)
 *
 * @warning aucun traitement d'erreur n'est pour l'instant implémenté
 * @see https://redmine.ryxeo.com/projects/leterrier-aller
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

#include "exercicerapido.h"

ExerciceRapido::ExerciceRapido(QWidget *parent, QString repertoireBase)
    : AbulEduCommonStatesV1(parent), AbulEduOutilsV1(getAbeExerciceAireDeTravailV1())
{
    m_localDebug = true;

    m_parent = parent;
    setHelpText("");
    m_messageBox = new AbulEduMessageBoxV1(QString(),QString(),false,getAbeExerciceAireDeTravailV1());
    m_messageBox->setVisible(false);
    m_directoryWork = new QDir(repertoireBase);
    m_isErrorShown = true;
    m_isVericationProcessing = false;
    m_pointSize = 24;

    if(m_parent->inherits("AbulEduPageAccueilV1")) {
            connect(m_parent, SIGNAL(dimensionsChangees()),this, SLOT(setDimensionsWidgets()),Qt::UniqueConnection);
    }if(!QDir(repertoireBase).exists()) {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Échec"),trUtf8("Impossible d'ouvrir le fichier spécifié !"));
    }
    else
    {
        if (m_localDebug) qDebug()<<trUtf8("Constructeur Rapido -- fichier de configuration trouvé");
        m_config = new QSettings(repertoireBase + "/conf/module.conf",QSettings::IniFormat);
//        m_config->setIniCodec("UTF-8");
        m_config->beginGroup("rechercheRapide");
        if (m_config->value("exerciceActive").toBool() == false)
        {
            displayErrorMissingExercise(QString::fromUtf8("Recherche rapide"),parent);
            this->deleteLater();
        }
        m_config->endGroup();

        QString fic(QString("%1/data/%2").arg(repertoireBase).arg(m_config->value("fichierTexte").toString()));
        m_texte = new AbulEduTextV1(fic);
    }

    // Création de l'Aire de jeu : c'est une AbulEduEtiquetteV1
    m_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));

#ifdef __ABULEDUTABLETTEV1__MODE__
    m_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    connect(m_AireDeJeu,SIGNAL(wheelTurn(int)),m_AireDeJeu->verticalScrollBar(),SLOT(setValue(int)));
    m_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif

    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
    m_proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(m_AireDeJeu); // le proxy va permettre de modifier l'ordre d'affichage des objets
    m_proxy->setZValue(-1); // m_AireDeJeu sera en dessous des autres objets
    m_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    m_AireDeJeu->setFrameShape(QFrame::NoFrame);
    getAbeExerciceMessageV1()->setParent(m_parent);
    m_listeMotsExercice = new QMap<int,QString>();

    m_listeTousLesMots = m_texte->abetextMotsPonctues();
    sequenceMachine->start();
}

ExerciceRapido::~ExerciceRapido()
{
    pushAbulEduLogs(abeApp->property("moduleEnCours").toString());
    emit exerciceExited(); // Permet à la MainWindow de savoir que l'exercice est terminé
}

QStringList ExerciceRapido::takeExerciseWords(const QString &unitPath, int exerciseNumber, AbulEduTextV1 *text)
{
    QStringList exerciseWords;
    QSettings* config = new QSettings(unitPath, QSettings::IniFormat);
    config->beginGroup("rechercheRapide");
    config->beginReadArray("exercice");
    config->setArrayIndex(exerciseNumber);
    qDebug()<<"Je suis à l'exercice "<<exerciseNumber;
    if (config->value("modeManuel").toBool()){
        exerciseWords = config->value("listeMots").toString().split(";",QString::SkipEmptyParts);
        if(config->value("ordreTexte",false).toBool() == false)
        {
            QStringList shaked;
            while(!exerciseWords.isEmpty()){
                shaked << exerciseWords.takeAt(rand()%exerciseWords.size());
            }
            exerciseWords = shaked;
        }
    }
    else{
        QStringList allWords;
        QMapIterator<int,QString> mapIter(text->abetextMotsPonctues());
        while(mapIter.hasNext()){
            mapIter.next();
            if(mapIter.key() > config->value("numeroMotDebut").toInt() && mapIter.key() < config->value("numeroMotFin").toInt()){
                allWords << mapIter.value();
            }
            int i = 0;
            while (exerciseWords.count() < config->value("nbMotsATrouver").toInt() && !allWords.isEmpty())
            {
                int hasard = qrand()%allWords.size();
                QString mot = allWords.takeAt(hasard);
                if (!exerciseWords.contains(mot)
                        && mot.simplified() != ";"
                        && mot.simplified() != ":"
                        && mot.simplified() != "!"
                        && mot.simplified() != "?"
                        && mot.simplified() != QString::fromUtf8("…")
                        && mot.simplified() != QString::fromUtf8("―")
                        && mot.simplified() != "retour_a_la_ligne"
                        && mot.simplified() != QString::fromUtf8("«")
                        && mot.simplified() != QString::fromUtf8("»")
                        && mot.simplified() != "("
                        && mot.simplified() != ")"
                        && mot.simplified() != ","
                        && mot.simplified() != "."
                        && mot.simplified() != "'"
                        && !mot.simplified().isEmpty())
                {
                    exerciseWords << mot;
                }
                i++;
            }
        }
        config->endArray();
        config->endGroup();
        config->deleteLater();
        return exerciseWords;
    }
}

int ExerciceRapido::verifieReponse()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    /* On a une méthode dans cette classe verifyClicEtiquette(),
       et la vérification n'est pas déclenchée au clic sur le bouton Vérification */
    return 0;
}

int ExerciceRapido::donneReponse()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    findCorrectWord(m_listeMotsExercice->value(getAbeNumQuestion()-1));
    return -1;
}

//----------------------------------------------------------------------------------------------------
//                      Réimplémentation des slots de l'AbulEduStateMachine
//----------------------------------------------------------------------------------------------------

void ExerciceRapido::slotSequenceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    m_config->beginGroup("rechercheRapide");
        setAbeNbExercices(m_config->value("exercice/size").toInt());
        getAbeExerciceMessageV1()->setParent(0);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());
        onPeutPresenterSequence = m_config->value("afficherConsigneSequence").toBool() || m_config->value("afficherTexteSequence").toBool();

        if (!m_config->value("afficherErreurs").isNull())
            m_isErrorShown = m_config->value("afficherErreurs").toBool();
    m_config->endGroup();
    AbulEduCommonStatesV1::slotSequenceEntered();
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", true);
    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    continueQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
}

void ExerciceRapido::slotPresenteSequenceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Recherche rapide"));

    m_config->beginGroup("rechercheRapide");
    if (m_config->value("afficherConsigneSequence").toBool())
    {
        QString debutTableau = "<tr valign=middle>";
        QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
        QString consigne = "<td align = right> " +  m_config->value("consigneSequence").toString() + "</td>";
        QString finTableau = "</tr>";
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);
        QFileInfo fi(m_config->fileName());
        QString instructionFilePath = fi.absolutePath().replace("conf","data/instructions/rrConsigneSequence.ogg");
        if(QFile(instructionFilePath).exists()){
            getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),instructionFilePath));
        }
        else {
            getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),QString()));
        }
    }
	else
    {
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne("");
    }
    if(m_config->value("afficherTexteSequence").toBool())
    {
        if (m_localDebug) qDebug()<<"Affichage du texte demandé";
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(false);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(m_texte->abeGetText());
    }
    else{
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    }
    m_config->endGroup();
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneConsigne();
}

void ExerciceRapido::slotExercicesEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<abeStateMachineGetNumExercice()+1;
    getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),QString()));
    m_config->beginGroup("rechercheRapide");
        m_config->beginReadArray("exercice");
            m_config->setArrayIndex(m_numExercice);
            onPeutPresenterExercice = m_config->value("afficherConsigneExercice").toBool();
        m_config->endArray();
    m_config->endGroup();
    AbulEduStateMachineV1::slotExercicesEntered();
}

void ExerciceRapido::slotPresentationExerciceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    AbulEduCommonStatesV1::slotPresentationExerciceEntered();
    m_config->beginGroup("rechercheRapide");
        m_config->beginReadArray("exercice");
            m_config->setArrayIndex(m_numExercice);
            getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(m_config->value("afficherConsigneExercice").toBool());
            getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(m_config->value("consigneExercice").toString());
        m_config->endArray();
    m_config->endGroup();
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Exercice n°%1").arg(m_numExercice+1));
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneConsigne();
}

void ExerciceRapido::slotRealisationExerciceEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<abeStateMachineGetNumExercice()+1;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    m_nbReponsesExactes = 0;
    m_reponsesDonnees.clear();
    m_reponsesAttendues.clear();
    m_config->beginGroup("rechercheRapide");
        m_config->beginReadArray("exercice");
        /* --------------------------- Configuration de l'exercice en cours --------------------------- */

        m_config->setArrayIndex(m_numExercice);
        m_numLevel = m_config->value("niveauDifficulte").toInt();
        getAbeExerciceTelecommandeV1()->setAbeLevel(QString::number(m_numLevel-1));

        if (m_config->value("insensibleCasse").toBool())
        {
            m_caseSensitivity = Qt::CaseInsensitive;
        }
        else
        {
            m_caseSensitivity = Qt::CaseSensitive;
        }

        setAbeExerciceName(trUtf8("Recherche rapide - Exercice %1").arg(m_numExercice +1)); // Sera utilisé par le système de log
        setAbeNbTotalQuestions(m_config->value("nombreMots").toInt());
        m_parent->parentWidget()->parentWidget()->parentWidget()->setWindowTitle(abeApp->getAbeApplicationLongName()+"--"+getAbeExerciceName()); // Titre de la fenêtre
        m_config->endArray();
    m_config->endGroup();

//    takeWords(m_texte->abetextMots());
    setAbeNbTotalQuestions(m_listeMotsExercice->size());

    displayTextAsWords();
    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    setAbeExerciceEvaluation(abe::evalY);
}

void ExerciceRapido::slotInitQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<" venant de question "<<getAbeNumQuestion()<<" sur "<<getAbeNbTotalQuestions();
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    //à voir si ces 2 lignes vont bien là <-- non, mais je n'ai pas encore réfléchi à "où"

    if (m_localDebug) qDebug()<<"ExerciceRapido::slotInitQuestionEntered(Debut);";
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    AbulEduCommonStatesV1::slotInitQuestionEntered();
    m_currentPiece = 0;
    boiteTetes->setEtatTete(m_numExercice, abe::evalY,false,getAbeNbTotalQuestions()-getAbeNumQuestion()+1);
}

void ExerciceRapido::slotQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceRapido::slotQuestionEntered("<<getAbeNumQuestion()<<"/"<<getAbeNbTotalQuestions()<<")";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    m_nbClicSurVerifier = 0;
    m_isTheRightOne = false;
    AbulEduCommonStatesV1::slotQuestionEntered();
    /* J'ai depuis mon slotRealisationExerciceEntered() mon texte et ma liste de mots pour l'exercice, je peux maintenant lancer les questions */
    if (m_localDebug) qDebug()<<"ExerciceRapido::slotQuestionEntered("<<getAbeNumQuestion()<<"/"<<getAbeNbTotalQuestions()<<") :: "<<m_listeMotsExercice->value(getAbeNumQuestion());
    displayWordToBeFound(m_listeMotsExercice->value(getAbeNumQuestion()-1));

    setHelpText("Un mot s'affiche dans la barre de progression. Cherche dans le texte, puis clique dessus.");
}

void ExerciceRapido::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceRapido::slotAfficheVerificationQuestionEntered("<<getAbeNumQuestion()<<"/"<<getAbeNbTotalQuestions()<<")";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    if (m_currentPiece == 0)
    {
        m_reponsesDonnees << "abandon";
        qDebug()<<"Ajout de nibe aux réponses données, ajout au nombreCLicsQuestions de "<<m_nbClicSurVerifier;
        setAbeTeteForResult(1,1);
        m_isTheRightOne = false;
        m_timeLine.data()->setPaused(true);
    }
    else
    {
        m_reponsesDonnees << m_currentPiece->abePieceGetTexte();
        if (QString::compare(m_texte->abetextNettoieMot(m_currentPiece->abePieceGetTexte()),
                             m_texte->abetextNettoieMot(m_currentWord),
                             m_caseSensitivity)==0)
        {
            setAbeTeteForResult(0,1);
//            setAbeTeteTelecommande("tresbien");
            m_nbReponsesExactes++;
            m_currentPiece->abePieceSetBrush(QColor(3,151,3,179));
            m_currentPiece->update();
            m_isTheRightOne = true;
            m_avancement.data()->deleteLater();
            m_motAtrouver.data()->deleteLater();
            /* Le fait d'affecter une valeur différente de 0 à m_verifieReponse fait que le passage au mot suivant est instantanné si la réponse est juste */
            abeStateMachineSetVerifieReponse(-1);
            AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();
        }
        else
        {
            setAbeTeteForResult(1,1);
//            setAbeTeteTelecommande("mal");
            m_currentPiece->abePieceSetBrush(QColor(234,33,0,179));
            m_currentPiece->update();
            m_isTheRightOne = false;
            m_timeLine.data()->setPaused(true);
        }
    }
    /* Je mets ça là en attendant mais c'est du provisoire, en attendant de gérer le cas des questions */
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);

    m_reponsesAttendues << m_currentWord;
//    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
//    setDimensionsWidgets();
}

void ExerciceRapido::slotFinVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceRapido::slotFinVerificationQuestionEntered("<<getAbeNumQuestion()<<"/"<<getAbeNbTotalQuestions()<<")";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    m_timeLine.data()->stop();
    m_timeLine.data()->deleteLater();
    if (m_avancement) m_avancement.data()->deleteLater();
    if (m_motAtrouver) m_motAtrouver.data()->deleteLater();
    m_isVericationProcessing = false;

    if (getAbeNumQuestion() == getAbeNbTotalQuestions())
    {
        sequenceMachine->postDelayedEvent(new StringEvent("Questionsdone"),2000);
    }
    else
    {
        sequenceMachine->postDelayedEvent(new StringEvent("Questionsloop"),2000);
    }
}

void ExerciceRapido::slotAfficheCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceRapido::slotAfficheCorrectionQuestionEntered("<<getAbeNumQuestion()<<"/"<<getAbeNbTotalQuestions()<<")";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    abeStateMachineSetVerifieReponse(donneReponse());
    AbulEduStateMachineV1::slotAfficheCorrectionQuestionEntered();
}

void ExerciceRapido::slotFinCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceRapido::slotFinCorrectionQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    if (m_avancement) m_avancement.data()->deleteLater();
    if (m_motAtrouver) m_motAtrouver.data()->deleteLater();
    if (getAbeNumQuestion() == getAbeNbTotalQuestions())
    {
        sequenceMachine->postEvent(new StringEvent("QuestionsDoneCorrect"));
    }
    else
    {
        sequenceMachine->postEvent(new StringEvent("QuestionsLoopCorrect"));
    }
}

void ExerciceRapido::slotFinQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceRapido::slotFinQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    m_answers.append(QPair<QVariant,QVariant>(m_reponsesDonnees.join(","),m_reponsesAttendues.join(",")));
    qDebug()<<m_answers.last().first;
    qDebug()<<m_answers.last().second;
}

void ExerciceRapido::slotBilanExerciceEntered()
{
    if (m_avancement)
    {
        m_avancement.data()->hide();
    }
    if(m_motAtrouver)
    {
        m_motAtrouver.data()->hide();
    }
    if (m_localDebug)
    {
        qDebug()<<"ExerciceRapido::slotBilanExerciceEntered()";
        qDebug()<<"Avant, tetes : "<<boiteTetes->getEtatTetes();
    }

    AbulEduCommonStatesV1::slotBilanExerciceEntered();

    QString message;
    if (m_nbReponsesExactes > 1)
    {
        message = trUtf8("Tu as trouvé ")+QString::number(m_nbReponsesExactes)+trUtf8(" mots sur ")+QString::number(getAbeNbTotalQuestions())+trUtf8(" proposés.");
    }
    else
    {
        message = trUtf8("Tu as trouvé ")+QString::number(m_nbReponsesExactes)+trUtf8(" mot sur ")+QString::number(getAbeNbTotalQuestions())+trUtf8(" proposés.");
    }
    m_AireDeJeu->scene()->clear(); // On efface les cellules et pieces

    qreal exactAsReal = static_cast<qreal> (m_nbReponsesExactes);
    qreal percentage = (exactAsReal/getAbeNbTotalQuestions()) * 100;

    QString imagetete = ":/bilan/";
    if (percentage >= 90)
    {
        m_resultats.append(QPair<int, int>(m_numExercice, 0));
        setAbeExerciceEvaluation(abe::evalA);
    }
    else if (percentage >= 70)
    {
        m_resultats.append(QPair<int, int>(m_numExercice, 1));
        setAbeExerciceEvaluation(abe::evalB);
    }
    else if (percentage >= 40)
    {
        m_resultats.append(QPair<int, int>(m_numExercice, 3));
        setAbeExerciceEvaluation(abe::evalC);
    }
    else
    {
        m_resultats.append(QPair<int, int>(m_numExercice, 7));
        setAbeExerciceEvaluation(abe::evalD);
    }
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation());
    imagetete.append(boiteTetes->suffixe->value(getAbeExerciceEvaluation()));
    m_percent = 100-percentage;
    int score;
    if ((qCeil(m_percent) - m_percent)>(m_percent - qFloor(m_percent)))
        score = qFloor(m_percent);
    else
        score = qCeil(m_percent);
    if (m_answers.count() > 0)
    {
        setAbeLineLog(m_answers.last().second.toString(),
                      m_answers.last().first.toString(),
                      score,
                      m_nbClicSurVerifier,
                      getAbeExerciceEvaluation(),
                      m_answers.last().second.toString());
    }
    if (m_localDebug)
    {
        if (m_answers.count() > 0)
        {
            qDebug() <<" --> Correction : Envoi ligne de resultats ----------------------------";
            qDebug()<<m_answers.last().second.toString();
            qDebug()<<m_answers.last().first.toString();
            qDebug()<<score;
            qDebug()<<m_nbClicSurVerifier;
            qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
            qDebug()<<m_answers.last().second.toString();
        }
        else
            qDebug()<<"Nibe a envoyer..."<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
    }

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Bilan de l'exercice n°")+QString::number(m_numExercice + 1));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(QString("<center><img src=")+imagetete+QString("/><br>")
                                           + message + QString("</center>"));
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);

    redimensionneBilan();
    m_listeTousLesMots = m_texte->abetextMotsPonctues();
}

void ExerciceRapido::slotBilanSequenceEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"ExerciceRapido::slotBilanSequenceEntered()";
        qDebug()<<boiteTetes->getEtatTetes();
    }

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Bilan des exercices")+"\n");

    AbulEduCommonStatesV1::slotBilanSequenceEntered();

    QString message = "";
    int tailleImageTete = QPixmap(":/bilan/"+boiteTetes->suffixe->value(0)).width() * abeApp->getAbeApplicationDecorRatio();
    message = "<center>" + message;
    int numExercice = 0;
    int numExerciceAtraiter = 0;
    int numtete = 0;
    QPair<int, int> resultat;
    message = message + QString("<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\" align=\"center\" valign=\"middle\"><tr><td>") + trUtf8("Exercice N°")+QString::number(m_resultats[numExercice].first + 1)+ QString("</td>");
    foreach(resultat, m_resultats){
        numExerciceAtraiter = resultat.first; // On récupère le numéro de l'exercice
        if(numExercice != numExerciceAtraiter) // Si le numéro est différent on change de ligne
        {
            numExercice = numExerciceAtraiter;
            message = message +"</tr>"+QString("<tr><td>") + trUtf8("Exercice N°")+QString::number(resultat.first + 1)+ QString("</td>");
        }

        switch(resultat.second)
        {
        case -1:
        case 0:
            numtete = abe::evalA;
            break;
        case 1:
            numtete = abe::evalB;
            break;
        case 3:
            numtete = abe::evalC;
            break;
        case 7:
            numtete = abe::evalD;
            break;
        default:
            numtete = abe::evalD;
        }
        QString imagetete = "<td> " + QString(" <img src=\":/bilan/"+boiteTetes->suffixe->value(numtete)+"\" width=%1/></td>")
                .arg(tailleImageTete);
        message = message + imagetete;

        if (m_localDebug) qDebug()<<"code pour l'exercice "<<numExercice<<" : "<<message;
    }
    message = message+"</table></center>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Recherche rapide"));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(message);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneBilan();
}

void ExerciceRapido::slotBilanSequenceExited()
{
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    AbulEduCommonStatesV1::slotBilanSequenceExited();
}

void ExerciceRapido::slotQuitter()
{
    AbulEduCommonStatesV1::slotQuitter();
    if (m_localDebug) qDebug()<<"ExerciceRapido::slotQuitter()";
}

//----------------------------------------------------------------------------------------------------
//                                     Fonctions de l'exercice
//----------------------------------------------------------------------------------------------------

void ExerciceRapido::displayTextAsWords()
{
    float ratio = abeApp->getAbeApplicationDecorRatio();
    m_config->beginGroup("rechercheRapide");
        m_config->beginReadArray("exercice");
            m_config->setArrayIndex(abeStateMachineGetNumExercice());
            m_pointSize = qMax(8.0,m_config->value("fontSize",24).toInt()*1.1*ratio);
            int numDebut = m_config->value("numeroMotDebut").toInt();
            int numFin = m_config->value("numeroMotFin").toInt();
        m_config->endArray();
    m_config->endGroup();
    if (m_localDebug)
    {
        qDebug()<<"ExerciceRapido::displayTextAsWords() :: exercice "<<abeStateMachineGetNumExercice()+1;
        qDebug()<<" Texte affiche des mots "<<numDebut<<" à "<<numFin;
    }
    int z=m_listeTousLesMots.count();
    while (z > numFin-1)
    {
        m_listeTousLesMots.remove(z);
        z--;
    }
    z = 0;
    while (z < numDebut-1)
    {
        m_listeTousLesMots.remove(z);
        z++;
    }

    qDebug()<<m_listeTousLesMots;

    if (m_localDebug) qDebug()<<"ExerciceRapido::displayTextAsWords(debut) -- "<<ratio;

    qDebug()<<" &&&&&& "<<m_pointSize;
    QFont fonteUtilisee(abeApp->font().family(),m_pointSize,abeApp->font().weight(),abeApp->font().italic());
    QFontMetrics mesureurDeMot(fonteUtilisee);
    int largeurMinimalePiece = mesureurDeMot.width("il");
    int longueurMot = 0;
    QRect positionPiece(0,0,0,0); //la position de la piece dans la zone de texte, comme il faut aussi la largeur et la hauteur occupee on utilise un QRect
    int positionXmaximale = m_AireDeJeu->width() - 15*ratio;       //La limite a ne pas depasser a droite
    int margeInterneEtiquette = 15;//*ratio;
    int espaceEntreEtiquettes = 0*ratio;
    int espaceEntreLignes     = m_pointSize;

    QRectF nouvelleScene(m_AireDeJeu->sceneRect().x(),m_AireDeJeu->sceneRect().y(),m_AireDeJeu->sceneRect().width(),10);
    int hauteur = 10;

    int i=numDebut-1;

    while (i<numFin)
    {
        if (m_listeTousLesMots.value(i).trimmed() != "")
        {
            if (mesureurDeMot.width(m_listeTousLesMots.value(i)) < largeurMinimalePiece)
            {
                longueurMot = largeurMinimalePiece;
            }
            else
            {
                longueurMot = mesureurDeMot.width(m_listeTousLesMots.value(i));
            }
            if(m_listeTousLesMots.value(i) == "retour_a_la_ligne")
            {
                longueurMot = -1*margeInterneEtiquette;
            }
            int hauteurMot = mesureurDeMot.height();
            AbulEduPieceV1* cellule = new AbulEduPieceV1(QSize(margeInterneEtiquette+longueurMot,espaceEntreLignes+hauteurMot));
            cellule->abePieceSetAcceptWordWrap(false);
            connect(cellule, SIGNAL(signalAbePieceCliquee(AbulEduPieceV1*)),this,SLOT(verifyClicEtiquette(AbulEduPieceV1*)));
            connect(cellule, SIGNAL(signalAbePieceCliquee(AbulEduPieceV1*)),getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()));
            if((positionPiece.x()+cellule->abePieceGetSize().width()) > positionXmaximale - 15*ratio || m_listeTousLesMots.value(i) == "retour_a_la_ligne")
            {
                positionPiece.setX(0);
                positionPiece.setY(positionPiece.y()+hauteurMot+espaceEntreLignes);
                hauteur = positionPiece.y()+cellule->abePieceGetSize().height()+espaceEntreLignes;

            }
            cellule->setObjectName(QString::number(i)+"_"+"piece");
            cellule->setFlag(QGraphicsItem::ItemIsMovable,false);
            cellule->abePiecePlaceFixe(true);
            cellule->abePieceSetRadius(0,0);
            if(m_listeTousLesMots.value(i) == "retour_a_la_ligne")
            {
                cellule->abePieceSetTexte("");
            }
            else
            {
                cellule->abePieceSetTexte(m_listeTousLesMots.value(i));
            }
            cellule->abePieceSetFont(fonteUtilisee);
            cellule->setParent(m_AireDeJeu->scene());
            m_AireDeJeu->scene()->addItem(cellule);
            m_AireDeJeu->updateSceneRect(nouvelleScene);
            cellule->setPos(positionPiece.x(),positionPiece.y());
            cellule->abePieceSetPositionOriginale(cellule->pos());
            positionPiece.setX(positionPiece.x()+cellule->abePieceGetSize().width()+espaceEntreEtiquettes);
//            if (m_localDebug) qDebug() << "Affiche a la position ::: " << positionPiece.x() << " x " << positionPiece.y() << " le mot "<<m_listeTousLesMots.value(i)<<" (position max : " << positionXmaximale;
//            if (m_localDebug) qDebug() << "Affiche le mot numero "<<i<<" :: "<<m_listeTousLesMots.value(i);
            //pratique pour developper, on affiche une bordure noire autour des cellules
//            cellule->abePieceSetPen(QPen(Qt::black));
            cellule->abePieceSetPen(QPen(QColor(Qt::black), 0, Qt::NoPen));
        }
        i++;
    }
    nouvelleScene = QRectF(m_AireDeJeu->sceneRect().x(),m_AireDeJeu->sceneRect().y(),m_AireDeJeu->sceneRect().width(),hauteur+10);
    m_AireDeJeu->setSceneRect(nouvelleScene);
    //On va enlever les deux derniers : le dernier parce qu'il est mis en retour à la ligne mais ajouté à la liste, l'avant dernier parce qu'il est remplacé par "(...)"
//    if (m_localDebug) qDebug()<<"Mots presents : "<<actuallyDisplayedWords;
    takeWords(m_listeTousLesMots);
    m_longueurMaxMot = 0;
    for(int i = 0; i < m_listeMotsExercice->size(); i++)
    {
        int tailleMot = m_listeMotsExercice->value(i).length();
        if(tailleMot > m_longueurMaxMot)
            m_longueurMaxMot = tailleMot;
        if (m_localDebug) qDebug()<<"Mot "<<m_listeMotsExercice->value(i)<<" :: "<<tailleMot<<" :: "<<m_longueurMaxMot;
    }
}

void ExerciceRapido::displayWordToBeFound(QString wordToBeFound)
{
    if (m_localDebug) qDebug()<<"displayWordToBeFound("<<wordToBeFound<<") :: "<<m_longueurMaxMot;

    foreach(AbulEduPieceV1* piece, m_AireDeJeu->scene()->findChildren<AbulEduPieceV1*>())
    {
        piece->abePieceSetAcceptMouse(true);
    }
    float ratio = abeApp->getAbeApplicationDecorRatio();
    m_currentWord = m_texte->abetextNettoieMot(wordToBeFound);
    QFont fonteUtilisee(abeApp->font().family(),abeApp->font().pointSize()*3*ratio,abeApp->font().weight(),abeApp->font().italic());
    QFontMetrics mesureurDeMot(fonteUtilisee);
    QString m("w");
    QString lemot(m.repeated(m_longueurMaxMot));
    int pixelsWideMotMax = mesureurDeMot.boundingRect(lemot).width();
    m_motAtrouver = new QLabel(m_currentWord);
    m_motAtrouver->setAlignment(Qt::AlignCenter);
    m_motAtrouver->setStyleSheet("background-color:transparent");
    m_motAtrouver->setFont(fonteUtilisee);
    m_avancement = new QProgressBar();
    m_avancement.data()->setRange(0,100);
    m_avancement.data()->setFormat("");
    m_avancement.data()->setStyleSheet("background-color:transparent;border: 2px solid #0058A3;border-radius: 5px;text-align: center} QProgressBar::chunk {background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 #39A8EB, stop: 1 #0065AD);");
    m_motAtrouver.data()->setGeometry((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - pixelsWideMotMax)/2,
                                      100*ratio,
                                      pixelsWideMotMax + 5*ratio,
                                      mesureurDeMot.height()+ 15*ratio);
    m_avancement.data()->setGeometry(m_motAtrouver.data()->x() + (m_motAtrouver.data()->width() - 200*ratio)/2,
                                      100*ratio + m_motAtrouver.data()->height(),
                                      200*ratio,
                                      15*ratio);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(m_avancement.data());
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(m_motAtrouver.data());

    m_config->beginGroup("rechercheRapide");
        m_config->beginReadArray("exercice");
            m_config->setArrayIndex(m_numExercice);
            int tempsAccorde = m_config->value("tempsAffichageMot").toInt();
        m_config->endArray();
    m_config->endGroup();
    m_timeLine = new QTimeLine(tempsAccorde*1000,this);
    m_timeLine.data()->setFrameRange(0,100);
    connect(m_timeLine.data(), SIGNAL(frameChanged(int)),m_avancement.data(), SLOT(setValue(int)));
    connect(m_timeLine.data(), SIGNAL(finished()),this, SLOT(postQuestionVerifieEvent()));
    m_timeLine.data()->start();
}

void ExerciceRapido::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"                ExerciceRapido::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(25*ratio, 10*ratio);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio,0);

    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    m_AireDeJeu->setFixedSize(large - 250 * ratio, haut - 200 * ratio);
    m_AireDeJeu->move((250 * ratio) / 2, 200 * ratio);
//    m_AireDeJeu->setSceneRect(0,0,m_AireDeJeu->width()-50,m_AireDeJeu->height()-50);

    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio);

    //La largeur de la zone des etiquettes = largeur max de la zone moins 100
    //    m_aireDeJeu->abelabelSetLargeurEtiquettes(labelZone->width() - 150);
    //    m_aireDeJeu->abelabelResizeLabels();
    //    m_aireDeJeu->abelabelResizeFrame();
    //    m_aireDeJeu->abelabelPlaceAbulEduLabelsByNumPlace(); // Car certaines étiquettes peuvent changer de hauteur

    QSet<QAbstractState *> etats = sequenceMachine->configuration(); // On récupère les états de la machine
    if(etats.contains(question)){ //On ne déplace l'exercice que lorsqu'il est visible
        displayTextAsWords();
    }
    // Redimmensionne le widget de consignes et de bilan
    redimensionneConsigne();
    redimensionneBilan();

    AbulEduCommonStatesV1::setDimensionsWidgets();

    if (m_localDebug) qDebug()<<"                ExerciceRapido::setDimensionsWidgets()---end";
}

void ExerciceRapido::redimensionneConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    if (getAbeExerciceMessageV1()->abeWidgetMessageHasImageFond()) {
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    }
    else
    {
        getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    (getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2);
    }
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}

void ExerciceRapido::redimensionneBilan()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}

void ExerciceRapido::verifyClicEtiquette(AbulEduPieceV1* piece)
{
    qDebug()<<"        ExerciceRapido::verifyClicEtiquette("<<piece<<")";
        m_currentPiece = piece;
        foreach(AbulEduPieceV1* piece, m_AireDeJeu->scene()->findChildren<AbulEduPieceV1*>())
        {
            piece->abePieceSetAcceptMouse(false);
        }
}

void ExerciceRapido::postQuestionVerifieEvent()
{
    if (m_localDebug) qDebug()<<"                  ExerciceRapido::postQuestionVerifieEvent";
//    m_motATrouver.data()->deleteLater();
//    if (m_localDebug) qDebug()<<"Je poste la transition QuestionVerifie";
    if (m_currentPiece == 0)
    {
        sequenceMachine->postEvent(new StringEvent("QuestionVerifieEmpty"));
        foreach(AbulEduPieceV1* piece, m_AireDeJeu->scene()->findChildren<AbulEduPieceV1*>())
        {
            piece->abePieceSetAcceptMouse(false);
        }
    }
    else
    {
        sequenceMachine->postEvent(new StringEvent("QuestionVerifie"));
    }
}

void ExerciceRapido::takeWords(QMap<int, QString> indexedList)
{
    /* ---------- Construction de la liste des mots qui seront à trouver, 1 par question ---------- */

    m_config->beginGroup("rechercheRapide");
        m_config->beginReadArray("exercice");
        m_config->setArrayIndex(m_numExercice);

        /* Premier Cas : des mots sont indiqués dans le fichier module.conf */
        if (m_config->value("modeManuel").toBool())
            {
                if (m_localDebug) qDebug()<<" ----> Premier cas de figure : mots lus dans module.conf";
                m_listeMotsExercice->clear();
                QStringList listeMots(m_config->value("listeMots").toString().split(";",QString::SkipEmptyParts));

                int indexMap=0;
                if(m_config->value("ordreTexte",false).toBool() == true)
                {
                    foreach (QString mot,listeMots)
                    {
                        m_listeMotsExercice->insert(indexMap,mot);
                        indexMap++;
                        setAbeNbTotalQuestions(listeMots.count());
                    }
                    qDebug()<<m_listeMotsExercice->values()<<":true:"<<getAbeNbTotalQuestions();
                }
                else
                {
                    QStringList listeMotsMelanges;
                    while(listeMots.size() > 0) {
                        listeMotsMelanges.append(listeMots.takeAt(rand()%listeMots.size()));
                    }
                    foreach (QString mot,listeMotsMelanges)
                    {
                        m_listeMotsExercice->insert(indexMap,mot);
                        indexMap++;
                        setAbeNbTotalQuestions(listeMotsMelanges.count());
                    }
                    qDebug()<<m_listeMotsExercice->values()<<":false:"<<getAbeNbTotalQuestions();
                }
                /*A voir, est-ce que je décide d'écrire de toutes les façons automatiquement dans l'éditeur le nombre de mots choisis pour
                aller dans la liste des mots ? Sinon, il faudra la ligne au-dessous*/
            }
        else
        /* Deuxième Cas : pas de liste de mots dans le fichier module.conf mais des indications niveau et/ou nature */

            if (m_config->value("modeIntelligent").toBool())
            {
                if (m_localDebug) qDebug()<<" ----> Deuxième cas de figure : mots pioches dans le texte en fonction de nature "<<m_config->value("nature").toString()<<" et niveau "<<m_config->value("niveau").toString();
                m_listeMotsExercice->clear();
                int nat = m_config->value("indexNature").toInt();//qstring
                int niv = m_config->value("indexNiveau").toInt();//abelevel
                QStringList listeOutils = abeoutilsCreerListeMotsOutils(m_directoryWork->absolutePath()+"/data/dictionnaires", abe::ABE_LEVEL(niv),abe::ABE_NATURE(nat));
                //QMap<int,QString> listeTamponMotsTexte = m_texte->abetextMots();
                if (!m_config->value("nbMotsATrouver").toString().isEmpty())//  <-------------------------------------
                    setAbeNbTotalQuestions(m_config->value("nbMotsATrouver").toString().toInt());
                else setAbeNbTotalQuestions(10);
                QList<int> indexMotsExistants = indexedList.keys();
                QList<int> correspondance;
                for (int i = 0; i < indexMotsExistants.count(); i++)
                {
                    correspondance << i;
                }
                QList<int> indexMotsUtilises;

                int nombreMotsTrouves = 0;
                bool finListeAtteint = false;
                srand(2000);
                while (nombreMotsTrouves < getAbeNbTotalQuestions() && !finListeAtteint)
                {
                    int indexTirage = rand() % (correspondance.count());
                    int indexSorti = correspondance.takeAt(indexTirage);

                    if (!indexMotsUtilises.contains(indexSorti))
                    {
                        indexMotsUtilises << indexSorti;
                        if (listeOutils.contains(indexedList[indexSorti]) && !m_listeMotsExercice->values().contains(indexedList[indexSorti]))
                        {
                            m_listeMotsExercice->insertMulti(m_listeMotsExercice->count(),indexedList[indexSorti]);
                            nombreMotsTrouves++;
                            if (m_localDebug) qDebug()<<"mot "<<indexedList[indexSorti]<<" ajoute -> "<<nombreMotsTrouves;
                        }
                    }
                    if(indexMotsUtilises.count()>=indexMotsExistants.count()||correspondance.count() <= 0)
                    {
                        finListeAtteint = true;
                        QString msg = trUtf8("Le texte ne contient pas ")
                                        +QString::number(getAbeNbTotalQuestions())
                                        +trUtf8(" mots de type ")
                                        +abe::ABE_DONNENATURE().value(abe::ABE_NATURE(nat))
                                        +trUtf8(" devant être sûs à la fin du ")
                                        + abe::ABE_DONNENIVEAU().value(abe::ABE_LEVEL(niv));
                        m_messageBox->abeMessageBoxUpdate(trUtf8("Problème"),msg);
                    }
                }
            }
            else
                /* Troisième Cas : rien dans le fichier module.conf (ni liste, ni indication niveau et/ou nature) */
                {
                    setAbeNbTotalQuestions(m_config->value("nbMotsATrouver").toInt());
                    if (m_localDebug) qDebug()<<" ----> Troisieme cas de figure : aleatoire complet";
                    m_listeMotsExercice->clear();
                    if (!m_config->value("nbMotsATrouver").toString().isEmpty())//  <-------------------------------------
                        setAbeNbTotalQuestions(m_config->value("nbMotsATrouver").toString().toInt());
                    else setAbeNbTotalQuestions(10);
                    int i = 0;
                    while (m_listeMotsExercice->count() < getAbeNbTotalQuestions() && !indexedList.isEmpty())
                    {
                        int hasard = qrand()%indexedList.size();
                        QString mot=indexedList.values().takeAt(hasard);
                        if (!m_listeMotsExercice->values().contains(mot)
                                && mot.simplified() != ";"
                                && mot.simplified() != ":"
                                && mot.simplified() != "!"
                                && mot.simplified() != "?"
                                && mot.simplified() != QString::fromUtf8("…")
                                && mot.simplified() != QString::fromUtf8("―")
                                && mot.simplified() != "retour_a_la_ligne"
                                && mot.simplified() != QString::fromUtf8("«")
                                && mot.simplified() != QString::fromUtf8("»")
                                && mot.simplified() != "("
                                && mot.simplified() != ")"
                                && mot.simplified() != ","
                                && mot.simplified() != "."
                                && mot.simplified() != "'"
                                && !mot.simplified().isEmpty())
                        {
                            m_listeMotsExercice->insert(m_listeMotsExercice->count(),mot);
                        }
                        i++;
                    }
                }
        m_config->endArray();
    m_config->endGroup();

    if (m_localDebug) qDebug()<<"Resultat de recherche : "<<*m_listeMotsExercice;
}

AbulEduPieceV1 *ExerciceRapido::findCorrectWord(QString word)
{
    if (m_localDebug) qDebug()<<"                  ExerciceRapido::findCorrectWord("<<word<<")";
    AbulEduPieceV1* goodOne = new AbulEduPieceV1(QSize(0,0));
    goodOne->abePieceSetAcceptWordWrap(false);
    int i = 0;
    bool trouve = false;
    while (i<m_AireDeJeu->scene()->findChildren<AbulEduPieceV1*>().count() && trouve==false)
    {
        AbulEduPieceV1* piece = static_cast<AbulEduPieceV1*>(m_AireDeJeu->scene()->findChildren<AbulEduPieceV1*>()[i]);
        qDebug()<<"Tour "<<i<<", je lis "<<piece->abePieceGetTexte()<<" que je veux comparer avec "<<word;
        qDebug()<<QString::compare(piece->abePieceGetTexte(), word, m_caseSensitivity);
        if (QString::compare(piece->abePieceGetTexte().simplified(), word.simplified(), m_caseSensitivity)==0)
        {
            trouve = true;
            piece->abePieceSetBrush(QColor(0,108,192,179));
            piece->update();
            if(m_localDebug) qDebug()<<"Mot trouve ";
            setHelpText("La couleur bleue indique le mot qu'il fallait trouver");
            return piece;
        }
        i++;
    }
    return goodOne;
}
