/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
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

#include "exercicephrasesansespaces.h"

ExercicePhraseSansEspaces::ExercicePhraseSansEspaces(QWidget *parent, QString repertoireBase) :
    AbulEduCommonStatesV1(parent), AbulEduOutilsV1(getAbeExerciceAireDeTravailV1())
{
    m_localDebug = true;
    float ratio = abeApp->getAbeApplicationDecorRatio();
    m_parent = parent;
    setHelpText("");
    m_messageBox = new AbulEduMessageBoxV1(QString(),QString(),false,getAbeExerciceAireDeTravailV1());
    m_messageBox->setVisible(false);
    // Attribution des images de fond de l'aire de travail et de la telecommande
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setAcceptDrops(true);
    setAbeTeteTelecommande("attente");

    textFormatErreur.setBackground(QBrush(QColor(234,33,0,179))); //Pour surligner les erreurs en rouge
    textFormatEspace.setBackground(QBrush(QColor(3,151,3,179)));// Pour afficher les espaces en vert
    textFormatEspace.setForeground(QBrush(QColor(0,0,0,0)));// pour éviter que "_" ne soit trop visible
    textFormatEspace.setFontWordSpacing(20*ratio);
    fontExo.setLetterSpacing(QFont::AbsoluteSpacing,7); //Définition de la font des exo
    fontExo.setFamily(abeApp->font().family());
    fontExo.setPointSize(abeApp->font().pointSize()*2*ratio);

    fontAvantExo.setFamily(abeApp->font().family());
    fontAvantExo.setPointSize(abeApp->font().pointSize()*2*ratio);
    fontAvantExo.setLetterSpacing(QFont::AbsoluteSpacing,2);

    // initialisations du QTextEdit
    m_AireDeJeu = new QTextEdit(getAbeExerciceAireDeTravailV1());
    m_AireDeJeu->setContextMenuPolicy(Qt::NoContextMenu);
    m_AireDeJeu->setReadOnly(true);
    m_AireDeJeu->setFont(fontAvantExo);
    m_AireDeJeu->setLineWrapMode(QTextEdit::FixedPixelWidth); //La taille est fixée dans le showEvent et resizeEvent
    m_AireDeJeu->setProperty("isAbulEduVirtualKeyboardDisabled",true);
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(m_AireDeJeu); // le proxy va permettre de modifier l'ordre d'affichage des objets
    proxy->setZValue(-1);
    m_AireDeJeu->setVisible(false);
    m_curseur=(m_AireDeJeu->textCursor());
    m_curseur.setPosition(0);
    connect(m_AireDeJeu,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPositionChanged()));
    /* Connecte le signal émis par la page d'accueil quand ses dimensions changent
       avec le slot setDimensionsWidgets() */
    if(m_parent->inherits("AbulEduPageAccueilV1")) {
            connect(m_parent, SIGNAL(dimensionsChangees()),this, SLOT(setDimensionsWidgets()),Qt::UniqueConnection);
    }

    // Entrée dans les fichiers décompressés du repertoire.abe
    if(!QDir(repertoireBase).exists()) {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Échec"),trUtf8("Impossible d'ouvrir le fichier spécifié !"));
    }
    else {
        m_config = new QSettings(repertoireBase + "/conf/module.conf",QSettings::IniFormat);
        //        m_config->setIniCodec("UTF-8");
        QString fic(QString("%1/data/%2").arg(repertoireBase).arg(m_config->value("fichierTexte").toString()));
        m_texte = new AbulEduTextV1(fic);
        // Préparation des exercices
        if(m_config->value("phraseSansEspace/afficherClicsMalPlaces").toBool()){
            JEVEUXCOLORIERETIQUETTESMALPLACEES=1;
        }
        else{
            JEVEUXCOLORIERETIQUETTESMALPLACEES=0;
        }

        if (m_config->value("phraseSansEspace/exerciceActive").toBool() == false)
        {
            displayErrorMissingExercise(QString::fromUtf8("Phrase sans espaces"),parent);
            this->deleteLater();
        }
    }
    m_nbEspacesBienPlaces = 0;
    m_nbEspacesMalPlaces = 0;
    m_nbEspacesAPlacer = 0;
    m_motsEnErreur.clear();
    m_AireDeJeu->installEventFilter(this); // Installe un filtre évènement sur la phrase sans espaces
    getAbeExerciceMessageV1()->setParent(m_parent);
    sequenceMachine->start();

    //! Icham 19.03.2013 --> Pour être sûr qu'au démarrage, le curseur soit visible =)
    m_AireDeJeu->ensureCursorVisible();
    //! Instantiation a NULL des btnTab

#ifdef __ABULEDUTABLETTEV1__MODE__
    btnBarreEspace = btnCurseurGauche = btnCurseurDroite = NULL;
    m_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    QGraphicsScene *item = new Q
#endif


}
ExercicePhraseSansEspaces::~ExercicePhraseSansEspaces()
{
    delete m_texte;
    delete m_config;
    //    delete getAbeExerciceMessageV1();

    pushAbulEduLogs();  //On expédie les logs sur le serveur

    emit exerciceExited(); // Permet à la MainWindow de savoir que l'exercice est terminé
}

int ExercicePhraseSansEspaces::verifieReponse()
{
    /* On a une méthode dans cette classe verifieEspace(),
       et la vérification n'est pas déclenchée au clic sur le bouton Vérification */
    return 0;
}

void ExercicePhraseSansEspaces::slotSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspace::slotSequenceEntered()";
//    getAbeExerciceTelecommandeV1()->ui->lblNiveauEnCours->setPixmap(QPixmap(":/abuleduexercicev1/niveaux/clesusb/"+QString::number(m_numLevel)));

    setAbeNbExercices(m_config->value("phraseSansEspace/exercice/size").toInt());

    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());
    //Quelques customisations

    // Determine si on présente la séquence ou pas.
    onPeutPresenterSequence = m_config->value("phraseSansEspace/afficherConsigneSequence").toBool() || m_config->value("phraseSansEspace/afficherTexteSequence").toBool();

    // Determine si on affiche le bilan de l'exercice
    onPeutPresenterBilanExercice = true;

    AbulEduCommonStatesV1::slotSequenceEntered();

}
void ExercicePhraseSansEspaces::slotPresenteSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotPresenteSequenceEntered();";
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Phrases sans espaces"));
    if (m_config->value("phraseSansEspace/afficherConsigneSequence").toBool())
    {
        QString debutTableau = "<tr valign=middle>";
        QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
        QString consigne = "<td align = right> " +  m_config->value("phraseSansEspace/consigneSequence").toString() + "</td>";
        QString finTableau = "</tr>";
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);
        QFileInfo fi(m_config->fileName());
        QString instructionFilePath = fi.absolutePath().replace("conf","data/instructions/pseConsigneSequence.ogg");
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
    QString messageTexte = "";
    QStringList listeParagraphes = m_texte->abetextParagraphes().values();

    if(m_config->value("phraseSansEspace/afficherTexteSequence").toBool()){
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(false);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
        for(int i = 0; i<listeParagraphes.size(); i++)
        {
            messageTexte = messageTexte + listeParagraphes[i] +"<br />";
        }
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(messageTexte);

    }
    else
    {
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    }
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneConsigne();
    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();
}

void ExercicePhraseSansEspaces::slotExercicesEntered()
{
    if (m_localDebug) qDebug()<<"ExercicePhraseSansEspaces::slotExercicesEntered() : "<<m_numExercice+1;
    getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),QString()));
    m_config->beginGroup("phraseSansEspace");
    int exercisesNumber = m_config->beginReadArray("exercice");
    setAbeNbExercices(exercisesNumber);
    m_config->setArrayIndex(m_numExercice);
    onPeutPresenterExercice = m_config->value("afficherConsigneExercice").toBool() || m_config->value("afficherPhrase").toBool();
    m_config->endArray();
    m_config->endGroup();
    AbulEduStateMachineV1::slotExercicesEntered();
}

void ExercicePhraseSansEspaces::slotPresentationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotPresenteExoEntered();------";
    m_phrase = takeSentence(m_config->fileName(),m_numExercice,m_texte);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Exercice n°") + QString::number(m_numExercice + 1));
    m_config->beginGroup("phraseSansEspace");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    m_numLevel = m_config->value("niveauDifficulte").toInt();
    if (m_config->value("afficherConsigneExercice").toBool())
    {
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(m_config->value("consigneExercice").toString());
    }
    else
    {
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(QString());
    }
    bool afficherPhrase = m_config->value("afficherPhrase",false).toBool();
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(afficherPhrase);
    if(afficherPhrase){
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(m_phrase);
    }
    else{
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(QString());
    }
    m_config->endArray();
    m_config->endGroup();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    (getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2 - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif

    AbulEduCommonStatesV1::slotPresentationExerciceEntered();
}

void ExercicePhraseSansEspaces::slotRealisationExerciceEntered()
{
    m_nbEspacesBienPlaces = 0;
    m_nbEspacesMalPlaces = 0;
    m_nbEspacesAPlacer = 0;

    m_config->beginGroup("phraseSansEspace");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    m_numLevel = m_config->value("niveauDifficulte").toInt();
    m_config->endArray();
    m_config->endGroup();

    m_phrase = takeSentence(m_config->fileName(),m_numExercice,m_texte);

    getAbeExerciceTelecommandeV1()->setAbeLevel(QString::number(m_numLevel-1));

    //! Icham 26/03/13 --> On assure que le curseur est là, ce qui donne le focus à l'aire de jeu, et donc l'affichage du curseur sur tablette
    m_AireDeJeu->ensureCursorVisible();

#ifdef __ABULEDUTABLETTEV1__MODE__
    if(btnBarreEspace == NULL)
        createBtnForTab();
#endif

    //! Icham 19/03/2013 pour apparition du curseur (le Focus doit etre piqué par la création des boutons)
    m_AireDeJeu->setFocus();
    if (m_localDebug) qDebug()<<" ExercicePhraseSansEspaces::slotRealisationExerciceEntered() "<<QString::number(m_numExercice);
    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    setAbeExerciceEvaluation(abe::evalY);
}

void ExercicePhraseSansEspaces::slotInitQuestionEntered()
{
    /*if (m_localDebug)*/ qDebug()<<"        ***********************       ExercicePhraseSansEspaces::slotInitQuestionEntered() -> "<<getAbeNumQuestion();
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    setAbeExerciceName(trUtf8("Phrases sans espaces - Exercice %1").arg(m_numExercice+1));

    getAbeExerciceTelecommandeV1()->ui->btnVerifier->setEnabled(false);
    getAbeExerciceTelecommandeV1()->ui->btnVerifier->setIconeNormale("");
    getAbeExerciceTelecommandeV1()->ui->btnVerifier->setIconeDisabled(":/abuleduexercicev1/boutons/btnVerifierDisabled");

    m_nbClicSurVerifier = 0;

    AbulEduCommonStatesV1::slotInitQuestionEntered();
}

void ExercicePhraseSansEspaces::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"      ********************************        ExercicePhraseSansEspaces::slotQuestionEntered()";
    redimensionneAireDeJeu();
    m_AireDeJeu->setVisible(true);
    m_AireDeJeu->setFocus(); // Pour avoir le clavier actif dans le widget de la phrase
    AbulEduCommonStatesV1::slotQuestionEntered();
    affichePhraseSansEspace(m_phrase);
    setHelpText(trUtf8("Pour ajouter un espace, <b>clique</b> à l'endroit où tu veux le mettre pour positionner le curseur, puis <b>appuie</b> sur la barre d'espace.")+"<br />"
                +trUtf8("Le nombre d'espaces à replacer est indiqué dans un rond au coin de la tête d'Asma."));

    setAbeNbTotalQuestions(m_nbEspacesAPlacer);
    getAbeExerciceTelecommandeV1()->ui->btnVerifier->setEnabled(false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    boiteTetes->setEtatTete(m_numExercice, abe::evalY,false,m_nbEspacesAPlacer-m_nbEspacesBienPlaces);
}

void ExercicePhraseSansEspaces::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhraseSansEspaces::slotAfficheVerificationQuestionEntered() : ";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }

    m_percent = setAbeTeteForResult(m_nbEspacesMalPlaces,m_nbEspacesAPlacer);
    setDimensionsWidgets();
    setHelpText(trUtf8("La couleur <b>verte</b> indique les espaces <b>bien</b> placés.")+"<br />"
                + trUtf8("La couleur <b>rouge</b> indique un mot dans lequel on a mis <b>à tort</b> un espace."));
    //Comme le choix du développeur initial était de ne permettre de Vérifier que quand les espaces à placer étaient tous mis, je n'enregistre que les erreurs
    m_answers.append(QPair<QVariant,QVariant>(QString::number(m_nbEspacesMalPlaces)+trUtf8(" faux"),trUtf8("sur ")+QString::number(m_nbEspacesAPlacer)));
    abeStateMachineSetVerifieReponse(verifieReponse());
    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();
}

void ExercicePhraseSansEspaces::slotFinVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhraseSansEspaces::slotFinVerificationQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }

    int score;
    sequenceMachine->postEvent(new StringEvent("Questionsdone"));

    if ((qCeil(m_percent) - m_percent)>(m_percent - qFloor(m_percent)))
        score = qFloor(m_percent);
    else
        score = qCeil(m_percent);
    setAbeLineLog(trUtf8("Phrase présentée non segmentée"),
                  m_answers.last().first.toString(),
                  score,
                  m_nbClicSurVerifier,
                  getAbeExerciceEvaluation(),
                  m_answers.last().second.toString());

    //Choix pédagogique de logger toutes les étapes
    //On ajoute une ligne de log
    if (m_localDebug)
    {
        if (m_answers.count() > 0)
        {
            qDebug() <<"Envoi ligne de resultats ----------------------------";
            qDebug()<<trUtf8("Phrase présentée non segmentée");
            qDebug()<<m_answers.last().first.toString();
            qDebug()<<score;
            qDebug()<<m_nbClicSurVerifier;
            //            qDebug()<<getAbeExerciceEvaluation();
            qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
            qDebug()<<m_answers.last().second.toString();
        }
        else
            qDebug()<<"Nibe a envoyer...";
    }
}

void ExercicePhraseSansEspaces::slotAfficheCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhraseSansEspaces::slotAfficheCorrectionQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    setHelpText("Les espaces tels qu'ils doivent être positionnés sont colorés en bleu.");
    abeStateMachineSetVerifieReponse(donneReponse());
    AbulEduStateMachineV1::slotAfficheCorrectionQuestionEntered();
}

void ExercicePhraseSansEspaces::slotFinCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhraseSansEspaces::slotFinCorrigerQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    //Le problème du log, c'est d'être sûr que si on a envoyé des choses avant (des clics sur Vérification avant le clic sur Corriger), ils vont bien être envoyés
    sequenceMachine->cancelDelayedEvent(id_temporisation); // On détruit la transition postee dans slotCorrigerQuestionEntered()
    int score;

    sequenceMachine->postEvent(new StringEvent("QuestionsDoneCorrect"));
    if ((qCeil(m_percent) - m_percent)>(m_percent - qFloor(m_percent)))
        score = qFloor(m_percent);
    else
        score = qCeil(m_percent);
    if (m_answers.count() > 0)
    {
        setAbeLineLog(trUtf8("Phrase présentée non segmentée"),
                      m_answers.last().first.toString(),
                      score,
                      m_nbClicSurVerifier,
                      getAbeExerciceEvaluation(),
                      m_answers.last().second.toString());
    }

    //Choix pédagogique de logger toutes les étapes
    //On ajoute une ligne de log
    if (m_localDebug)
    {
        if (m_answers.count() > 0)
        {
            qDebug() <<" --> Correction : Envoi ligne de resultats ----------------------------";
            qDebug()<<trUtf8("Phrase présentée non segmentée");
            qDebug()<<m_answers.last().first.toString();
            qDebug()<<score;
            qDebug()<<m_nbClicSurVerifier;
            qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
            qDebug()<<m_answers.last().second.toString();
        }
        else
            qDebug()<<"Nibe a envoyer..."<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
    }
}

void ExercicePhraseSansEspaces::slotVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotVerificationQuestionEntered()";
    sequenceMachine->postEvent(new StringEvent("Questionsdone")); // La vérification a été faite dans verifieEspace()
}

void ExercicePhraseSansEspaces:: slotBilanExerciceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotBilanExerciceEntered();"<<m_percent;

    m_AireDeJeu->clear();
    QString message;

    if(m_percent <= 10)
    {
        message = trUtf8("Bravo, tu as très bien réussi cet exercice");
    }
    else if (m_percent <= 30)
    {
        message = trUtf8("Bravo, tu as assez bien réussi cet exercice");
    }
    else if(m_percent <= 70)
    {
        message = trUtf8("Attention, tu as fait beaucoup d'erreurs dans cet exercice");
    }
    else
    {
        message = trUtf8("Attention, tu as fait beaucoup trop d'erreurs dans cet exercice");
    }
    if (abeStateMachineGetVerifieReponse() == -1)
    {
        setAbeExerciceEvaluation(abe::evalZ);
        if(m_nbEspacesBienPlaces==0 && m_nbEspacesMalPlaces==0)
        {
            message = trUtf8("Solution demandée sans avoir proposé de réponse");
        }
        else if (m_nbEspacesBienPlaces+m_nbEspacesMalPlaces==1)
        {
            message = trUtf8("Solution demandée après %1 proposition de réponse").arg(m_nbEspacesBienPlaces+m_nbEspacesMalPlaces);
        }
        else
        {
            message = trUtf8("Solution demandée après %1 propositions de réponse").arg(m_nbEspacesBienPlaces+m_nbEspacesMalPlaces);
        }
        m_percent = -1;
    }

    QString imagetete = ":/bilan/"+boiteTetes->suffixe->value(getAbeExerciceEvaluation());
    m_resultats.append(QPair<int, int>(m_numExercice, m_percent));
    qDebug()<<QPair<int, int>(m_numExercice, m_percent )<<" ajouté à la structure de données m_resultats";
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation());
    AbulEduCommonStatesV1::slotBilanExerciceEntered();
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Bilan de l'exercice n°")+QString::number(m_numExercice + 1));
    //    QString imagetete = QString(":/"+m_prefixeTetes+"/"+boiteTetes->suffixe->value(boiteTetes->getEtatTete(m_numExercice)));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(QString("<center><img src=")+imagetete+QString("/><br>")
                                                           + message + QString("</center>"));
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}

void ExercicePhraseSansEspaces::slotBilanSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotBilanSequenceEntered();------";
    AbulEduCommonStatesV1::slotBilanSequenceEntered();
    QString message = "";
    int tailleImageTete = QPixmap(":/bilan/"+boiteTetes->suffixe->value(0)).width() * abeApp->getAbeApplicationDecorRatio();
    message = "<center>" + message;
    int numExercice = 0;
    int numExerciceAtraiter = 0;
    QPair<int, int> resultat;
    message = message + QString("<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\" align=\"center\" valign=\"middle\"><tr><td>") + trUtf8("Exercice N°")+QString::number(m_resultats[numExercice].first + 1)+ QString("</td>");
    foreach(resultat, m_resultats){
        qDebug()<<"Pour l'exercice "<<resultat.first<<", on a "<<resultat.second;
        numExerciceAtraiter = resultat.first; // On récupère le numéro de l'exercice
        if(numExercice != numExerciceAtraiter) // Si le numéro est différent on change de ligne
        {
            numExercice = numExerciceAtraiter;
            message = message +"</tr>"+QString("<tr><td>") + trUtf8("Exercice N°")+QString::number(resultat.first + 1)+ QString("</td>");
        }

        if (resultat.second == -1)
        {
            setAbeTeteTelecommande("rien");
            setAbeExerciceEvaluation(abe::evalZ);
        }
        else if (resultat.second <= 10)
        {
            setAbeTeteTelecommande("tresbien");
            setAbeExerciceEvaluation(abe::evalA);
        }
        else if (resultat.second <= 30)
        {
            setAbeTeteTelecommande("bien");
            setAbeExerciceEvaluation(abe::evalB);
        }
        else if (resultat.second <= 70)
        {
            setAbeTeteTelecommande("moyen");
            setAbeExerciceEvaluation(abe::evalC);
        }
        else
        {
            setAbeTeteTelecommande("mal");
            setAbeExerciceEvaluation(abe::evalD);
        }
        qDebug()<<"m_evaluation vaut "<<getAbeExerciceEvaluation() ;
        QString imagetete = "<td> " + QString(" <img src=\":/bilan/"+boiteTetes->suffixe->value(getAbeExerciceEvaluation())+"\" width=%1/></td>")
                .arg(tailleImageTete);
        message = message + imagetete;

        if (m_localDebug) qDebug()<<"code pour l'exercice "<<numExercice<<" : "<<message;
    }
    message = message+"</table></center>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Phrases sans espaces"));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(message);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    redimensionneBilan();
}

void ExercicePhraseSansEspaces::slotBilanSequenceExited()
{
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    AbulEduCommonStatesV1::slotBilanSequenceExited();
}

void ExercicePhraseSansEspaces::slotQuitter()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotQuitter";
    AbulEduCommonStatesV1::slotQuitter();
}

//-------------------------------------------------------------------------
//                      Interface de l'exercice
//-------------------------------------------------------------------------

void ExercicePhraseSansEspaces::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"                ExercicePhraseSansEspaces::setDimensionsWidgets()---start";
    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio);

    // Redimensionnement des Widgets
    if(m_AireDeJeu->isVisible()){
        redimensionneAireDeJeu();
    }

    QSet<QAbstractState *> etats = sequenceMachine->configuration(); // On ne bouge que si on est dans l'état aproprié
    // Redimensionne le widget de consignes et de bilan
    redimensionneConsigne();
    redimensionneBilan();

    //    if(etats.contains(presentationExercices)
    //            | etats.contains(presentationSequence)
    //            | etats.contains(bilanExercice)
    //            | etats.contains(bilanSequence)){
    //        if(getAbeExerciceMessageV1()->isVisible()){
    //            getAbeExerciceMessageV1()->abeWidgetMessageResize();
    //        }
    //    }

    AbulEduCommonStatesV1::setDimensionsWidgets();

    if (m_localDebug) qDebug()<<"                ExercicePhraseSansEspaces::setDimensionsWidgets() --- end";
}
void ExercicePhraseSansEspaces::redimensionneConsigne()
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

void ExercicePhraseSansEspaces::redimensionneBilan()
{
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}

void ExercicePhraseSansEspaces::redimensionneAireDeJeu()
{
    float ratio = abeApp->getAbeApplicationDecorRatio();
    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    m_AireDeJeu->setFixedWidth(large -250 * ratio);
    m_AireDeJeu->setFixedHeight(m_AireDeJeu->document()->size().height()+100);
    m_AireDeJeu->move((260 * ratio) / 2, (haut-m_AireDeJeu->height())/2);
    // Fixe la largeur du Texte dans le QTextEdit égale à celle du widget
    m_AireDeJeu->setLineWrapColumnOrWidth(large - 260 * ratio);
}

//-------------------------------------------------------------------------
//                      Fonctions de l'exercice
//-------------------------------------------------------------------------

void ExercicePhraseSansEspaces::verifieEspace()
{
    if (m_localDebug)
        qDebug()<<"::::::::::::::::::::::::::Verifie espace:::::::::::::::::::::::::::::::::::::::::::::";
    //On compare la position du curseur avec la longueur de la chaine à gauche du curseur débarrasée de ses espaces

    //On calcule la position du curseur dans la phrase sans espace
    int posCurs=m_AireDeJeu->toPlainText().mid(0,m_curseurPosition).remove(" ").size();
    if (posCurs==0 || posCurs>=longueurChaineSans)
    {
        // On ne fait rien, on peut en profiter pour envoyer un message
    }
    else
    {
        if(m_espacePos.contains(posCurs))
        {
            // Y a-t-il déjà un espace à droite ou à gauche du curseur ?
            if(m_AireDeJeu->toPlainText().mid(m_curseurPosition-1,2).contains(" "))
            {
                // On ne fait rien, on peut en profiter pour envoyer un message
            }
            else
            {
                // On insére un espace
                m_curseur.insertText(" ");
                //                m_curseur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,2); // On sélectionne l'espace
                m_curseur.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor); // On sélectionne l'espace
                m_curseur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor); // On sélectionne l'espace
                m_curseur.setCharFormat(textFormatEspace); // On le met en couleur

                //                m_curseur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor); //On se positionne après l'espace

                m_nbEspacesBienPlaces++; // On incrémente le score

                boiteTetes->setEtatTete(m_numExercice, abe::evalA,false,m_nbEspacesAPlacer-m_nbEspacesBienPlaces);

//                boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);

                if(m_nbEspacesBienPlaces == m_nbEspacesAPlacer){
                    // On passe à la suite
                    sequenceMachine->postDelayedEvent(new StringEvent("QuestionVerifie"),2000);
                }
                //                emit afficheTete(bien);
            }
        }
        else
        {
            // On traite l'erreur
            // Il faut retrouver les indexinf et sup des espaces bordants le mot pointé par le curseur
            // On cherche dans la liste des index
            int i = 0;
            while (m_espacePos[i]<posCurs)
            {
                i++;
            }
            // On sélectionne le mot
            m_curseur.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor,posCurs-m_espacePos[i-1]);
            m_curseur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_espacePos[i]-m_espacePos[i-1]);

            m_curseur.setCharFormat(textFormatErreur); // On le met en couleur
            m_motsEnErreur.append(m_curseur.selectedText());
            m_curseur.clearSelection(); //On annule la sélection
            m_curseur.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor,m_espacePos[i]-m_espacePos[i-1]-(posCurs-m_espacePos[i-1]));
            m_nbEspacesMalPlaces++; // On incrémente le nombre des réponses fausses
            setAbeTeteTelecommande("mal");            //            emit afficheTete(rate);
            boiteTetes->setEtatTete(m_numExercice, abe::evalD,false,m_nbEspacesAPlacer-m_nbEspacesBienPlaces);
        }
    }
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    m_curseurPosition = m_curseur.position();
    m_AireDeJeu->clearFocus();
    m_curseur.setPosition(m_curseurPosition);
    m_AireDeJeu->setFocus();
    m_AireDeJeu->setTextCursor(m_curseur); // Affiche le curseur au bon endroit
    setHelpText(trUtf8("Continue...")+"<br />"
                +trUtf8("Les espaces bien placés sont mis en vert")+"<br />"
                +trUtf8("Par contre si un espace a été mis à l'intérieur d'un mot, c'est tout le mot qui est coloré en rouge."));

    m_AireDeJeu->ensureCursorVisible();
    /* Je mets ça là en attendant mais c'est du provisoire, en attendant de gérer le cas des questions */
//    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
}

int ExercicePhraseSansEspaces::donneReponse()
{
    setAbeExerciceEvaluation(abe::evalZ);
    m_answers.append(QPair<QVariant,QVariant>(" --- ","Phrase correctement segmentée"));

    setAbeTeteTelecommande("rien");
    setDimensionsWidgets();
    affichePhraseCorrigee(m_phrase);
    return -1;
}

void ExercicePhraseSansEspaces::affichePhraseSansEspace(QString phraseExo)
{
    if (m_localDebug) qDebug()<<"              affichePhraseSansEspace";
    // Il faut enlever les espaces en stockant leur position
    QString laPhrase=phraseExo;
    if (m_localDebug) qDebug()<<"PhraseExo"<<laPhrase;
    if(!m_espacePos.isEmpty())
    {
        m_espacePos.clear();
    }
    m_espacePos.append(0);
    int i=laPhrase.indexOf(" ");
    while(i>-1)
    {
        m_espacePos.append(i); // ajoute la position de l'espace à la liste
        laPhrase.remove(i,1); // enlève l'espace à la position i
        i=laPhrase.indexOf(" "); //espace suivant, s'il n'y en a plus la fonctio indexOf renvoie -1
    }
    longueurChaineSans=laPhrase.size();
    m_espacePos.append(longueurChaineSans);
    if (m_localDebug) qDebug()<<"Position des espaces"<<m_espacePos;

    m_nbEspacesAPlacer = m_espacePos.size()-2; // Initialise le nombre de réponses à fournir

    m_AireDeJeu->setReadOnly(false); //! @note Modif Icham
    qDebug() << m_AireDeJeu->textInteractionFlags();
    m_AireDeJeu->setTextInteractionFlags(m_AireDeJeu->textInteractionFlags() /*&
                                         ~Qt::TextEditable*/ & ~Qt::TextSelectableByMouse & ~Qt::TextSelectableByKeyboard
                                        );
    qDebug() << "-----------------------------------------------------------------------";
    qDebug() << m_AireDeJeu->textInteractionFlags();


    m_AireDeJeu->setWordWrapMode(QTextOption::WrapAnywhere);
    m_AireDeJeu->setFont(fontExo);
    m_AireDeJeu->setFrameShape(QFrame::NoFrame);
    m_AireDeJeu->setStyleSheet("background-color: transparent;");
    m_AireDeJeu->setHtml(laPhrase); // Affiche la phrase
    m_curseur.atStart();
    m_AireDeJeu->setTextCursor(m_curseur); // Rend le curseur visible
#ifdef __ABULEDUTABLETTEV1__MODE__
    m_AireDeJeu->setCursorWidth(2);
    m_AireDeJeu->ensureCursorVisible();
    m_AireDeJeu->textCursor().setVisualNavigation(true);
#else
    m_AireDeJeu->grabKeyboard(); // Le widget a le focus pour le clavier (les touches agissent sur la phrase)
#endif
}

void ExercicePhraseSansEspaces::affichePhraseCorrigee(QString phraseExo)
{
    if (m_localDebug) qDebug()<<"              affichePhraseCorrigee";
    QString laPhrase=phraseExo;
    if (m_localDebug) qDebug()<<"PhraseExo"<<laPhrase;
    QTextCharFormat couleurCorrection;
    couleurCorrection.setBackground(QBrush(QColor(255, 0, 0)));
    m_curseur.movePosition(QTextCursor::End);
    int indexFin = m_curseur.position();
    qDebug()<<" fin du texte en "<<indexFin;
    m_curseur.movePosition(QTextCursor::Start);
    /*    int i=laPhrase.indexOf(" ");
    while(i>-1 && i<indexFin)
    {
        qDebug()<<" ____________________________________ "<<i;
        m_curseur.setPosition(i,QTextCursor::MoveAnchor);
        m_curseur.setPosition(i+1,QTextCursor::KeepAnchor);
        m_curseur.setCharFormat(couleurCorrection);
        qDebug()<<m_curseur.selectedText().replace(" ","#");
        i=laPhrase.indexOf(" ",m_curseur.position());
    }
    int j=0;

    Les espaces n'ont pas l'air de compter...
    while (j<indexFin)
    {
        m_curseur.setPosition(j,QTextCursor::KeepAnchor);
        qDebug()<<j<<"::"<<m_curseur.selectedText();//.replace(" ","#");
        j++;
    }*/
    QString ph = m_phrase;
    ph.replace(" ","<span style=\"color:rgba(0, 108, 192,255);font-weight:bold\"> | </span>");
    qDebug()<<ph;
    m_AireDeJeu->setHtml(ph);
    m_AireDeJeu->setReadOnly(true);
    m_AireDeJeu->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_AireDeJeu->setFont(fontExo);
    m_AireDeJeu->setFrameShape(QFrame::NoFrame);
    m_AireDeJeu->setStyleSheet("background-color: transparent;");
}

bool ExercicePhraseSansEspaces::eventFilter(QObject *obj, QEvent *event)
{
    if(sequenceMachine->configuration().contains(question) || sequenceMachine->configuration().contains(continueQuestion))
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            switch(keyEvent->key())
            {
            case Qt::Key_Space:
                if (m_localDebug) qDebug()<<"Appui sur espace";
                ExercicePhraseSansEspaces::verifieEspace();
                return true;
                break;
            case Qt::Key_Left:
                if (m_localDebug) qDebug()<<"Appui sur Key_Left";
                m_curseur.movePosition(QTextCursor::Left);
                m_AireDeJeu->setTextCursor(m_curseur);
                return true;
                break;
            case Qt::Key_Right:
                if (m_localDebug) qDebug()<<"Appui sur Key_Right";
                m_curseur.movePosition(QTextCursor::Right);
                m_AireDeJeu->setTextCursor(m_curseur);
                return true;
                break;
            case Qt::Key_Up:
                if (m_localDebug) qDebug()<<"Appui sur Key_Up";
                m_curseur.movePosition(QTextCursor::Up);
                m_AireDeJeu->setTextCursor(m_curseur);
                return true;
                break;
            case Qt::Key_Down:
                if (m_localDebug) qDebug()<<"Appui sur Key_Down";
                m_curseur.movePosition(QTextCursor::Down);
                m_AireDeJeu->setTextCursor(m_curseur);
                return true;
                break;
            default:
                return true;
            }
        }
        else
        {
            // Traitement standard des autres évènements
            return AbulEduStateMachineV1::eventFilter(obj, event);
        }
    }
    else
    {
        return AbulEduStateMachineV1::eventFilter(obj, event);
    }
}

void ExercicePhraseSansEspaces::cursorPositionChanged()
{
    m_curseur=(m_AireDeJeu->textCursor());
    m_curseurPosition = m_curseur.position();
}

QString ExercicePhraseSansEspaces::takeSentence(const QString &unitPath, int exerciseNumber, AbulEduTextV1 *text)
{
    QString sentence;
    QSettings* config = new QSettings(unitPath, QSettings::IniFormat);
    config->beginGroup("phraseSansEspace");
    config->beginReadArray("exercice");
    config->setArrayIndex(exerciseNumber);
    qDebug()<<"Je suis à l'exercice "<<exerciseNumber;
    if (config->value("modeManuel").toBool())
    {

        /* ---------------------------- Cas n°1 : mode manuel + numero de phrase à -1 ------------------------
    ---------- C'est le cas où la phrase peut avoir été saisie et ne pas appartenir au texte ----------*/

        if(config->value("phraseNumero").toInt()==-1)
        {
            sentence = config->value("phrase").toString();
        }
        else
            /* ------------------- Cas n°2 : mode manuel + numero de phrase différent de -1 -------------------
    ---- La phrase a été choisie dans l'éditeur, on prend le numéro plutôt que le texte (+robuste) ----*/

        {
            sentence = text->abetextPhrase(config->value("phraseNumero").toInt()-1);
        }

    }
    else
        /* ----------------------------------- Cas n°3 : mode automatique  ----------------------------------- */
    {
        sentence = text->abetextPhrase(rand() % text->abetextPhrases().count());
    }
    qDebug()<<"Fin de takeSentence, m_phrase vaut "<<sentence<<" et m_config->phraseNumero vaut "<<config->value("phraseNumero").toInt();
    config->endArray();
    config->endGroup();
    config->deleteLater();
    return sentence.simplified();
}

/** ----------------------------------------------------------------------------------------------------------------------------------
                                        SPECIFICITE TABLETTE
    --------------------------------------------------------------------------------------------------------------------------------**/
#ifdef __ABULEDUTABLETTEV1__MODE__
void ExercicePhraseSansEspaces::createBtnForTab()
{
    /* Création Barre Espace */
    btnBarreEspace = new AbulEduFlatBoutonV1();
    btnBarreEspace->setText("Espace");
    btnBarreEspace->setCouleursTexte(Qt::white);
    btnBarreEspace->setIconeNormale(":/exercices/data/picto/barre-espace.png");
    btnBarreEspace->setIconePressed(":/exercices/data/picto/barre-espace-petit.png");
    btnBarreEspace->setFixedSize(QSize(300, 80));
    connect(btnBarreEspace, SIGNAL(clicked()),this, SLOT(on_btnBarreEspace_clicked()), Qt::UniqueConnection);
    QGraphicsProxyWidget* prox_barreEspace = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(btnBarreEspace); // le proxy va permettre de modifier l'ordre d'affichage des objets
    prox_barreEspace->setPos(boiteTetes->x()+boiteTetes->geometry().width()/2- btnBarreEspace->width()/2, boiteTetes->y()-btnBarreEspace->height() - 10);

    /* Création Bouton Gauche */
    btnCurseurGauche = new AbulEduFlatBoutonV1();
    btnCurseurGauche->setIconeNormale(":/exercices/data/picto/fleche-gauche.png");
    btnCurseurGauche->setIconePressed(":/exercices/data/picto/fleche-gauche-petit.png");
    btnCurseurGauche->setFixedSize(QSize(80,80));
    connect(btnCurseurGauche, SIGNAL(clicked()), this, SLOT(on_btnCurseurGauche_clicked()), Qt::UniqueConnection);
    QGraphicsProxyWidget *prox_curseurGauche = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(btnCurseurGauche);
    prox_curseurGauche->setPos(btnBarreEspace->x()-btnCurseurGauche->width(), btnBarreEspace->y());

    /* Création Bouton Droite */
    btnCurseurDroite = new AbulEduFlatBoutonV1();
    btnCurseurDroite->setIconeNormale(":/exercices/data/picto/fleche-droite.png");
    btnCurseurDroite->setIconePressed(":/exercices/data/picto/fleche-droite-petit.png");
    btnCurseurDroite->setFixedSize(QSize(80,80));
    connect(btnCurseurDroite, SIGNAL(clicked()), this, SLOT(on_btnCurseurDroite_clicked()), Qt::UniqueConnection);
    QGraphicsProxyWidget *prox_curseurDroite = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(btnCurseurDroite);
    prox_curseurDroite->setPos(btnBarreEspace->x()+btnBarreEspace->width() , btnBarreEspace->y());
}

void ExercicePhraseSansEspaces::on_btnBarreEspace_clicked()
{
    verifieEspace();
}

void ExercicePhraseSansEspaces::on_btnCurseurGauche_clicked()
{
    QCoreApplication::sendEvent(m_AireDeJeu, new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
    m_AireDeJeu->setFocus();
}

void ExercicePhraseSansEspaces::on_btnCurseurDroite_clicked()
{
    QCoreApplication::sendEvent(m_AireDeJeu, new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
    m_AireDeJeu->setFocus();
}
#endif
