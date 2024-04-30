 /**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011-2014 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
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

#include "exercicemotsmelanges.h"

ExerciceMotsMelanges::ExerciceMotsMelanges(QWidget *parent, QString repertoireBase) :
    AbulEduCommonStatesV1(parent), AbulEduOutilsV1(getAbeExerciceAireDeTravailV1())
{
    m_localDebug = true;
    m_showSentenceAsHelpingHand = false;
    m_parent = parent;
    setHelpText("");
    m_messageBox = new AbulEduMessageBoxV1(QString(),QString(),false,getAbeExerciceAireDeTravailV1());
    m_messageBox->setVisible(false);

    if(m_parent->inherits("AbulEduPageAccueilV1")) {
            connect(m_parent, SIGNAL(dimensionsChangees()),this, SLOT(setDimensionsWidgets()),Qt::UniqueConnection);
    }
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setAcceptDrops(true);
    setAbeTeteTelecommande("attente");

    // Création de l'Aire de jeu : c'est une AbulEduEtiquetteV1
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));
    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu); // le proxy va permettre de modifier l'ordre d'affichage des objets
    proxy->setZValue(-1); // gv_AireDeJeu sera en dessous des autres objets
    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);


    if(!QDir(repertoireBase).exists()) {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Échec"),trUtf8("Impossible d'ouvrir le fichier spécifié !"));
    }
    else {
        m_config = new QSettings(repertoireBase + "/conf/module.conf",QSettings::IniFormat);
//        m_config->setIniCodec("UTF-8");
        QString fic(QString("%1/data/%2").arg(repertoireBase).arg(m_config->value("fichierTexte").toString()));
        m_texte = new AbulEduTextV1(fic);
        // Préparation des exercices
        if(m_config->value("motsMelanges/afficherMotsBienPlaces").toBool()){
            JEVEUXCOLORIERETIQUETTESMALPLACEES=1;
        }
        else{
            JEVEUXCOLORIERETIQUETTESMALPLACEES=0;
        }
        if (m_config->value("motsMelanges/exerciceActive").toBool() == false)
        {
            displayErrorMissingExercise(QString::fromUtf8("Mots mélangés"),parent);
            this->deleteLater();
        }
    }
    getAbeExerciceMessageV1()->setParent(m_parent);
    sequenceMachine->start();
}

ExerciceMotsMelanges::~ExerciceMotsMelanges()
{
    //On expédie les logs sur le serveur
    pushAbulEduLogs();
    delete m_texte;
    delete m_config;
    emit exerciceExited();

}

void ExerciceMotsMelanges::slotSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::slotSequenceEntered()";

    AbulEduCommonStatesV1::slotSequenceEntered();

    setAbeNbExercices(m_config->value("motsMelanges/exercice/size").toInt());
//    boiteTetes->resetTetes(m_nbExercices);

    setAbeNbTotalQuestions(1);
    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

    // Determine si on présente la séquence ou pas.
    onPeutPresenterSequence = m_config->value("motsMelanges/afficherConsigneSequence").toBool() || m_config->value("motsMelanges/afficherTexteSequence").toBool();
    // Determine si on affiche le bilan de l'exercice
    onPeutPresenterBilanExercice = true;
    boiteTetes->resetTetes(m_nbExercices);
}

void ExerciceMotsMelanges::slotPresenteSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::slotPresenteSequenceEntered();";
//    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true); // Pour activer le bouton suivant

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Mots mélangés"));
    if (m_config->value("motsMelanges/afficherConsigneSequence").toBool())
    {
        QString debutTableau = "<tr valign=middle>";
        QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
        QString consigne = "<td align = right> " +  m_config->value("motsMelanges/consigneSequence").toString() + "</td>";
        QString finTableau = "</tr>";
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);
        QFileInfo fi(m_config->fileName());
        QString instructionFilePath = fi.absolutePath().replace("conf","data/instructions/momConsigneSequence.ogg");
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
    if(m_config->value("motsMelanges/afficherTexteSequence").toBool()){
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(false);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
        QStringList m_listeParagraphes = m_texte->abetextParagraphes().values();
        for(int i = 0; i<m_listeParagraphes.size(); i++)
        {
            messageTexte = messageTexte + m_listeParagraphes[i] +"<br />";
        }
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(messageTexte);

    }
    else{
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    }
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneConsigne();
}

void ExerciceMotsMelanges::slotExercicesEntered()
{
    if (m_localDebug) qDebug()<<"ExerciceMotsMelanges::slotExercicesEntered() : "<<m_numExercice+1;
    getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),QString()));
    m_config->beginGroup("motsMelanges");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    m_numLevel = m_config->value("niveauDifficulte").toInt();
    onPeutPresenterExercice = m_config->value("afficherConsigneExercice").toBool() || m_config->value("afficherPhrase").toBool();
    qDebug()<<m_config->value("afficherConsigneExercice").toBool();
    qDebug()<<m_config->value("afficherPhrase").toBool();
    qDebug()<<onPeutPresenterExercice;
    m_config->endArray();
    m_config->endGroup();
    AbulEduStateMachineV1::slotExercicesEntered();
}

void ExerciceMotsMelanges::slotPresentationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhraseSansEspaces::slotPresenteExoEntered();------";
    m_phrase = takeSentence(m_config->fileName(),m_numExercice,m_texte);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Exercice n°") + QString::number(m_numExercice + 1));
    m_config->beginGroup("motsMelanges");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
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

void ExerciceMotsMelanges::slotRealisationExerciceEntered()
{
    if (onPeutPresenterExercice == false)
    {
        m_phrase = takeSentence(m_config->fileName(),m_numExercice,m_texte);
    }
    m_config->beginGroup("motsMelanges");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    m_numLevel = m_config->value("niveauDifficulte",1).toInt();
    setAbeExerciceName(trUtf8("Mots mélangés - Exercice %1").arg(m_numExercice+1)); // Sera utilisé par le système de log
    m_config->endArray();
    m_config->endGroup();
    getAbeExerciceTelecommandeV1()->setAbeLevel(QString::number(m_numLevel-1));
    setAbeLevel(QString::number(m_numLevel-1));
    if (m_localDebug) qDebug()<<" ExerciceMotsMelanges::slotRealisationExerciceEntered() "<<QString::number(m_numLevel);
    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    setAbeExerciceEvaluation(abe::evalY);
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
}

void ExerciceMotsMelanges::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug()<<"               AbulEduOrdreAlpha1::slotInitQuestionEntered();";
    getAbeExerciceMessageV1()->setVisible(false);
    gv_AireDeJeu->show();
//    getAbeExerciceTelecommandeV1()->ui->lblNiveauEnCours->setPixmap(QPixmap(":/abuleduexercicev1/niveaux/clesusb/"+QString::number(m_numLevel)));
    AbulEduTextV1 phraseChoisie;
    phraseChoisie.abeSetText(m_phrase);
    m_listeMots = phraseChoisie.abetextMotsPonctues().values();

    m_config->beginGroup("motsMelanges");
        m_config->beginReadArray("exercice");
            m_config->setArrayIndex(abeStateMachineGetNumExercice());
            m_showSentenceAsHelpingHand = m_config->value("afficherPhraseDansAide",false).toBool();
        m_config->endArray();
    m_config->endGroup();

    if (m_localDebug) qDebug() << "###############################################################";
    if (m_localDebug) qDebug() << m_config->group();
    if (m_localDebug) qDebug() << m_numExercice;
    if (m_localDebug) qDebug() << m_phrase;
    if (m_localDebug) qDebug() << m_listeMots;
    if (m_localDebug) qDebug() << "###############################################################";

    m_nbClicSurVerifier = 0;

    AbulEduCommonStatesV1::slotInitQuestionEntered();

    afficheMots();
}

void ExerciceMotsMelanges::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"              ExerciceMotsMelanges::slotQuestionEntered()";

//    setDimensionsWidgets(); // Place les Widgets à leur place

    AbulEduCommonStatesV1::slotQuestionEntered();
    if(m_showSentenceAsHelpingHand)
    {
        setHelpText(m_phrase);
    }
    else
    {
        setHelpText(trUtf8("Chaque étiquette contient un mot du texte. Fais un glisser-déposer vers l'endroit où tu penses qu'elle doit se trouver.")+"<br />"
                    + trUtf8("Clique sur le bouton <b>Vérification</b> quand tu as fini ou sur le bouton <b>Solution</b> si tu n'y arrives pas..."));
    }
}

void ExerciceMotsMelanges::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceMotsMelanges::slotAfficheVerificationQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    if (JEVEUXCOLORIERETIQUETTESMALPLACEES == 1)
    {
        setHelpText(trUtf8("La couleur <b>verte</b> indique les étiquettes <b>bien</b> placées.")+"<br />"
                    + trUtf8("Tu peux bouger des étiquettes puis cliquer sur le bouton <b>Vérification</b> à nouveau."));
    }
    else
    {
        setHelpText(trUtf8("Certaines étiquettes sont mal placées.")+"<br />"
                    + trUtf8("Tu peux bouger des étiquettes puis cliquer sur le bouton <b>Vérification</b> à nouveau."));
    }
    abeStateMachineSetVerifieReponse(verifieReponse());
    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();
}

void ExerciceMotsMelanges::slotFinVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceMotsMelanges::slotFinVerificationQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    int score;
    if(abeStateMachineGetVerifieReponse()==0)
    {
        sequenceMachine->postEvent(new StringEvent("Questionsdone"));
        if ((qCeil(m_percent) - m_percent)>(m_percent - qFloor(m_percent)))
                score = qFloor(m_percent);
        else
                score = qCeil(m_percent);
        setAbeLineLog(m_phrase,
                      m_answers.last().first.toString(),
                      score,
                      m_nbClicSurVerifier,
                      getAbeExerciceEvaluation(),
                      m_answers.last().second.toString());
    }
    else
    {
        sequenceMachine->postEvent(new StringEvent("Questioncontinue"));
        if ((qCeil(m_percent) - m_percent)>(m_percent - qFloor(m_percent)))
                score = qFloor(m_percent);
        else
                score = qCeil(m_percent);

        if (m_answers.count() > 0)
            setAbeLineLog(m_phrase,
                      m_answers.last().first.toString(),
                      score,
                      -1,
                      getAbeExerciceEvaluation(),
                      m_answers.last().second.toString());
    }

    //Choix pédagogique de logger toutes les étapes
    //On ajoute une ligne de log
    if (m_localDebug)
    {
        if (m_answers.count() > 0)
        {
            qDebug() <<"Envoi ligne de resultats ----------------------------";
            qDebug()<<m_phrase;
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

void ExerciceMotsMelanges::slotAfficheCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExerciceMotsMelanges::slotAfficheCorrectionQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    setHelpText(trUtf8("Les étiquettes ont été replacées au bon endroit.")+"<br />"
                + trUtf8("La couleur verte indique qu'une étiquette était bien placée.")+"<br />"
                + trUtf8("La couleur violette indique qu'une étiquette était mal placée.")+"<br />"
                + trUtf8("La couleur bleue indique qu'une étiquette n'était pas placée."));
    abeStateMachineSetVerifieReponse(donneReponse());
    AbulEduStateMachineV1::slotAfficheCorrectionQuestionEntered();
}

void ExerciceMotsMelanges::slotFinCorrectionQuestionEntered()
{
    {
        if (m_localDebug)
        {
            qDebug()<<"------------------------------------------------------------------------------------";
            qDebug()<<" ++++++++ ExerciceMotsMelanges::slotFinCorrigerQuestionEntered()";
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
            setAbeLineLog(m_phrase,
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
                qDebug()<<m_phrase;
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
}

void ExerciceMotsMelanges::slotBilanExerciceEntered()
{
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::slotBilanExerciceEntered();";

    gv_AireDeJeu->setVisible(false);
    // On affiche la bonne tete en fonction du nb de clics sur btnVerifier
    QString imagetete = ":/bilan/";

    switch(m_nbClicSurVerifier)
    {
    case 1:
        boiteTetes->setEtatTete(m_numExercice, abe::evalA);
        imagetete.append(boiteTetes->suffixe->value(abe::evalA));
        break;
    case 2:
        boiteTetes->setEtatTete(m_numExercice, abe::evalB);
        imagetete.append(boiteTetes->suffixe->value(abe::evalB));
        break;
    case 3:
    case 4:
        boiteTetes->setEtatTete(m_numExercice, abe::evalC);
        imagetete.append(boiteTetes->suffixe->value(abe::evalC));
        break;
    default:
        boiteTetes->setEtatTete(m_numExercice, abe::evalD);
        imagetete.append(boiteTetes->suffixe->value(abe::evalD));
        break;
    }

    AbulEduCommonStatesV1::slotBilanExerciceEntered();
    QString message;
    if (abeStateMachineGetVerifieReponse() == -1)
    {
        boiteTetes->setEtatTete(m_numExercice, abe::evalZ);
        imagetete = ":/bilan/"+boiteTetes->suffixe->value(abe::evalZ);
        if(m_nbClicSurVerifier==0)
        {
            message = trUtf8("Solution demandée sans avoir proposé de réponse");
        }
        else if (m_nbClicSurVerifier==1)
        {
            message = trUtf8("Solution demandée après %1 proposition de réponse").arg(m_nbClicSurVerifier);
        }
        else
        {
            message = trUtf8("Solution demandée après %1 propositions de réponse").arg(m_nbClicSurVerifier);
        }
        m_nbClicSurVerifier = -1;
    }
    else
    {
        if(m_nbClicSurVerifier==1)
        {
            message = trUtf8("Bravo, tu as réussi l'exercice du premier coup !");
        }
        else
        {
            message = trUtf8("Tu as réussi l'exercice en %1 essais","%1 est supérieur à 1").arg(m_nbClicSurVerifier); // Syntaxe utile pour la traduction
        }
    }
    m_resultats.append(QPair<int, int>(m_numExercice, m_nbClicSurVerifier));

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Bilan de l'exercice n°")+QString::number(m_numExercice + 1));
//    En attendant d'avoir trouvé le souci sur get ou setEtatTete, je construis imagetete plus haut
//    QString imagetete = QString(":/"+m_prefixeTetes+"/"+boiteTetes->suffixe->value(boiteTetes->getEtatTete(m_numExercice)));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(QString("<center><img src=")+imagetete+QString("/><br>")
                                           + message + QString("</center>"));
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();

    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
    //question meld 20120221
    redimensionneBilan();

}

void ExerciceMotsMelanges::slotBilanSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::slotBilanSequenceEntered();------";
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
            numtete = abe::evalZ;
            break;
        case 1:
            numtete = abe::evalA;
            break;
        case 2:
            numtete = abe::evalB;
            break;
        case 3:
        case 4:
            numtete = abe::evalC;
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
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Mots mélangés"));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(message);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneBilan();

}

void ExerciceMotsMelanges::slotBilanSequenceExited()
{
    getAbeExerciceMessageV1()->setVisible(false);
    AbulEduCommonStatesV1::slotBilanSequenceExited();
}


void ExerciceMotsMelanges::slotQuitter()
{
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::slotQuitter";
    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceMotsMelanges::slotAide()
{
    AbulEduStateMachineV1::slotAide();
}

//-------------------------------------------------------------------------
//                      Interface de l'exercice
//-------------------------------------------------------------------------

void ExerciceMotsMelanges::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::resizeEvent()";
    setDimensionsWidgets();
}
void ExerciceMotsMelanges::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    if (m_localDebug) qDebug()<<"               ExerciceMotsMelanges::showEvent()";
    setDimensionsWidgets();
}
void ExerciceMotsMelanges::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"                ExerciceMotsMelanges::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0, 0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();

    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    gv_AireDeJeu->move((150 * ratio) / 2, 50 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    QSet<QAbstractState *> etats = sequenceMachine->configuration(); // On ne bouge que si on est dans l'état aproprié
    // Redimensionne le widget de consignes et de bilan
    redimensionneConsigne();
    redimensionneBilan();
    placeMots();
    placeCellules();
//    if(etats.contains(presentationExercices)
//            | etats.contains(presentationSequence)
//            | etats.contains(bilanExercice)
//            | etats.contains(bilanSequence)){
//        if(getAbeExerciceMessageV1()->isVisible()){
//            getAbeExerciceMessageV1()->abeWidgetMessageResize();
//        }
//    }
    AbulEduCommonStatesV1::setDimensionsWidgets();

    if (m_localDebug) qDebug()<<"                ExerciceMotsMelanges::setDimensionsWidgets()---end";
}



//-------------------------------------------------------------------------
//                      Fonctions de l'exercice
//-------------------------------------------------------------------------

void ExerciceMotsMelanges::updateDropResultReceived(abe::ABE_EVAL eval)
{
    setHelpText(trUtf8("Oui, on fait comme ça.")+"<br />"
                +trUtf8("Continue en vérifiant bien le sens de la phrase que tu reconstruis."));
    m_receivedDropResults << eval;
    if (m_localDebug)
    {
        qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<m_receivedDropResults;
    }
    if (m_receivedDropResults.contains(abe::evalD))
    {
        setHelpText(trUtf8("Oui, on fait comme ça, mais...")+"<br />"
                    +trUtf8("Relis dans l'ordre les mots déjà placés.")+"<br />"+ trUtf8("Le sens te semble-t-il correct ?"));
    }
    if(m_showSentenceAsHelpingHand)
    {
        setHelpText(m_phrase);
    }
}

void ExerciceMotsMelanges::slotLimitePieceX()
{
    AbulEduPieceV1* p = (AbulEduPieceV1*) sender();
    if(p->x() > gv_AireDeJeu->width()-p->abePieceGetSize().width()){
        p->setX(gv_AireDeJeu->width()-p->abePieceGetSize().width());
    }
    if(p->x() < 0){
        p->setX(0);
    }
}

void ExerciceMotsMelanges::slotLimitePieceY()
{
    AbulEduPieceV1* p = (AbulEduPieceV1*) sender();
    if(p->y() > gv_AireDeJeu->height()-p->abePieceGetSize().height()){
        p->setY(gv_AireDeJeu->height()-p->abePieceGetSize().height());
    }
    if(p->y() < 0){
        p->setY(0);
    }
}

int ExerciceMotsMelanges::verifieReponse()
{
    if (m_localDebug)
        qDebug()<<"ExerciceMotsMelanges::verifieReponse()";
    QStringList reponseDonnee;
    QStringList reponseAttendue = m_listeMots;
    int nbreponsesFausses = 0;
    // C'est ici que l'on teste la validité des réponses et l'on renvoie le nombre de réponses fausses
        // 0 = tout bon, on peut passer à la suite
        // le nombre retourné sera testé dans le slotVerifieReponse()

    // On regarde si toutes les étiquettes sont placées
    bool termine = true;
    for(int i=0;i<m_listeMots.size();i++)
    {
        if(!m_listeCellules[i]->abeCelluleHasPiece()){
            termine = false;
        }
    }

    if(!termine)
    {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Réponse incomplète"),trUtf8("Place tous les mots avant de cliquer sur <b>Vérification</b>"));
        setAbeTeteTelecommande("rien");
//        setDimensionsWidgets();
        return 1;
    }
    else
    {
        // Maintenant on peut évaluer
        foreach(AbulEduCelluleV1 *cellule, m_listeCellules)
        {
            reponseDonnee.append(cellule->abeCelluleHasPiece()->abePieceGetTexte());
            if(cellule->objectName()==cellule->abeCelluleHasPiece()->objectName()){
                cellule->abeCelluleHasPiece()->abePieceSetMarge(0);
                cellule->abeCelluleHasPiece()->abePieceSetBrush(QColor(3,151,3,179));
                cellule->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                cellule->abeCelluleHasPiece()->update();
            }
            else{
                nbreponsesFausses++;
                cellule->abeCelluleHasPiece()->abePieceSetBrush(QColor(234,33,0,179));
                cellule->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                cellule->abeCelluleHasPiece()->update();
                cellule->abeCelluleHasPiece()->update();
            }
        }
    }
    if (m_localDebug)
        qDebug()<<"nbreponsesFausses"<<nbreponsesFausses;

    m_answers.append(QPair<QVariant,QVariant>(reponseDonnee.join(","),reponseAttendue.join(",")));
    m_percent = setAbeTeteForResult(nbreponsesFausses,m_listeCellules.count());

    if (m_localDebug)
    {
        qDebug()<<"LOG ajoute :";
        qDebug()<<m_answers.last().first.toString();
        qDebug()<<m_answers.last().second.toString();
        qDebug()<<m_percent;
        qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
    }
//    setDimensionsWidgets();
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);

    return nbreponsesFausses;
}

int ExerciceMotsMelanges::donneReponse()
{
    if (m_localDebug) qDebug()<<"          ExerciceMotsMelanges::donneReponse()";
    setAbeTeteTelecommande("rien");
    setDimensionsWidgets();
    setAbeExerciceEvaluation(abe::evalZ);

    //mettre toutes les pieces en bleu
    for (int i=0; i<m_listeEtiquettes.size();i++)
    {
        m_listeEtiquettes[i]->abePieceSetBrush(QColor(0,108,192,179));
        m_listeEtiquettes[i]->abePieceSetRadius(0,0);
        m_listeEtiquettes[i]->update();
    }
    foreach (AbulEduCelluleV1* cellule,m_listeCellules)
    {
        if (cellule->abeCelluleHasPiece() != 0)
        {
            if (cellule->abeCelluleHasPiece()->objectName() == cellule->objectName())
            {
                m_listeEtiquettes.removeOne(cellule->abeCelluleHasPiece());
                cellule->abeCelluleHasPiece()->abePieceSetBrush(QColor(1,125,116,179));
                cellule->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                cellule->abeCelluleHasPiece()->update();
            }
            else
            {
                cellule->abeCelluleHasPiece()->abePieceSetBrush(QColor(134,45,176,179));
                cellule->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                cellule->abeCelluleHasPiece()->update();
                foreach(AbulEduPieceV1* piece, m_listeEtiquettes)
                {
                    piece->setFlag(QGraphicsItem::ItemIsMovable,false);
                    if (piece->objectName() == cellule->objectName() )
                    {
                        piece->setPos(cellule->pos()+QPoint(5,5));
                        cellule->abeCelluleSetPiece(piece);
                        m_listeEtiquettes.removeOne(piece);
                    }
                }
            }
        }
        else
        {
            foreach(AbulEduPieceV1* piece, m_listeEtiquettes)
            {
                piece->setFlag(QGraphicsItem::ItemIsMovable,false);
                if (cellule->abeCelluleHasPiece() == 0 && piece->objectName() == cellule->objectName() )
                {
                    piece->setPos(cellule->pos()+QPoint(5,5));
                    cellule->abeCelluleSetPiece(piece);
                    m_listeEtiquettes.removeOne(piece);
                }
            }
        }
    }
    return -1;
}

void ExerciceMotsMelanges::redimensionneConsigne()
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
}

void ExerciceMotsMelanges::redimensionneBilan()
{
    //question meld 20120221
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}
void ExerciceMotsMelanges::afficheMots()
{
    float ratio = abeApp->getAbeApplicationDecorRatio();
    clearScene();
    QFont fonte(abeApp->font().family(),abeApp->font().pointSize()*2*ratio,abeApp->font().weight(),abeApp->font().italic());
    qreal hauteurEtiquette= 0 ; // hauteur des étiquettes
    maxLargeurEtiquette = 0; // Taille maximum des étiquettes créées

    // On va calculer la largeur maximale du mot le plus long
    for(int i = 0; i<m_listeMots.size(); i++)
    {
        QFontMetrics metric(fonte);
        QSize size = QSize((metric.boundingRect(m_listeMots[i])).width(),
                           (metric.boundingRect(m_listeMots[i])).height());
        hauteurEtiquette = size.height() + 5;
        maxLargeurEtiquette=qMax(maxLargeurEtiquette, size.width()+5);
    }
    maxLargeurEtiquette = maxLargeurEtiquette + 10;

    m_listeEtiquettes.clear();
    if(m_listeMots.size()>0)
    {
        // Création des étiquettes
        for(int i=0;i<m_listeMots.size();i++)
        {
            AbulEduPieceV1 *etiq = new AbulEduPieceV1(QSize(maxLargeurEtiquette, hauteurEtiquette));
            connect(etiq, SIGNAL(xChanged()),this, SLOT(slotLimitePieceX()),Qt::UniqueConnection);
            connect(etiq, SIGNAL(yChanged()),this, SLOT(slotLimitePieceY()),Qt::UniqueConnection);
            etiq->setObjectName(m_listeMots[i]);
            etiq->setParent(gv_AireDeJeu->scene());
            etiq->abePieceSetAcceptWordWrap(false);
            etiq->abePieceSetFont(fonte);
            etiq->abePieceSetTexte(m_listeMots[i]); // Le texte visible de l'étiquette
            etiq->abePieceSetTexteAlignement(Qt::AlignCenter);
            etiq->abePieceSetTextePen(QPen(Qt::white));
            etiq->abePieceSetPen(Qt::NoPen);
            etiq->abePieceSetRadius(0,0);
            etiq->abePieceSetBrush(QColor(202,103,0,179));
            etiq->abePiecePlaceFixe(false);
            gv_AireDeJeu->scene()->addItem(etiq);
            m_listeEtiquettes.append(etiq); // Fait la liste des étiquettes
        }
        placeMots();
    }

    // Construction des Cellules (pour déposer les mots)
    for(int i=0;i<m_listeMots.size();i++){
        AbulEduCelluleV1 *zone=new AbulEduCelluleV1(QSizeF(maxLargeurEtiquette + 10, hauteurEtiquette +10));
        connect(zone, SIGNAL(signalAbeCelluleDropResult(abe::ABE_EVAL)), this, SLOT(updateDropResultReceived(abe::ABE_EVAL)),Qt::UniqueConnection);
        zone->abeCelluleSetInitialBrush(QBrush());
        zone->setObjectName(m_listeMots[i]);
        zone->setParent(gv_AireDeJeu->scene());
        zone->abeCelluleSetPen(QPen(QColor("#c38b6b"), 2, Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        zone->setAcceptedMouseButtons(Qt::NoButton);
        zone->setAcceptHoverEvents(false);
        gv_AireDeJeu->scene()->addItem(zone);
        m_listeCellules.append(zone);
    }

    for (int i = 0;i<m_listeCellules.size();i++)
    {
        for(int j=0;j<m_listeEtiquettes.size();j++)
        {
            connect(m_listeEtiquettes[j], SIGNAL(signalAbePieceSurvole(QGraphicsItem*)),m_listeCellules[i], SLOT(slotAbeCellulePieceSurvol(QGraphicsItem*)),Qt::UniqueConnection);
        }
    }

    placeCellules();
}

void ExerciceMotsMelanges::placeMots()
{
    // Les mots sont placés dans des AbulEduPieceV1
    QPointF offsetZoneEtiquette = QPointF(0, gv_AireDeJeu->height()/2); // Les pièces sont placées dans la partie inférieure de l'aire de jeu
    QList<QGraphicsItem *>listeItem = gv_AireDeJeu->scene()->items(); // Récupère tous les items présents sur l'aire de jeu

    foreach(QGraphicsItem *m_item, listeItem){
        if(m_item->type()==AbulEduPieceV1::AbulEduPieceType){ // Faut être sûr que c'est un Mot (une abulEduPieceV1)
            int x = 0;
            int y = 0;
            int k=0;
            do{ // On cherche une place libre
                k++;
                x = qrand()%int(gv_AireDeJeu->width() - 100) + offsetZoneEtiquette.x();
                x=x+20;
                y = qrand()%int(gv_AireDeJeu->height() / 2 -50) + offsetZoneEtiquette.y();
                y = y +20;
                static_cast<AbulEduPieceV1 *>(m_item)->abePieceSetPositionOriginale(QPointF(x,y));
                m_item->setPos(x,y);
            }while(m_item->collidingItems().size() > 0 && k<200); // On regarde s'il y a une étiquette en-dessous (évite le chevauchement)

        }
    }
}

void ExerciceMotsMelanges::placeCellules()
{
    // Les cellules sont destinées à accueillir les mots rangés
    int largeurCellules =0;
    int hauteurCellules =0;
    if(m_listeCellules.size()>0){ // Comme toutes les pièces sont identiques, on en teste une (la première)
        largeurCellules = m_listeCellules[0]->abeCelluleGetSize().width();
        hauteurCellules = m_listeCellules[0]->abeCelluleGetSize().height();
    }
    int margeBord = 5;
    int espaceCellules = 5;

    // On va essayer de placer les cellules harmonieusement
    // Centrée dans la partie supérieure de l'aire de jeu :-D
    QList<QGraphicsItem *>listeItem = gv_AireDeJeu->scene()->items(Qt::AscendingOrder); // Récupère tous les items présents sur l'aire de jeu
    if(listeItem.size()>0)
    {
        int nbCellulesParLigne = int((gv_AireDeJeu->width() - margeBord*2) / (largeurCellules +espaceCellules));
        qreal offsetX = ((gv_AireDeJeu->width() - margeBord*2) - (largeurCellules +espaceCellules)*qMin(m_listeCellules.size(),nbCellulesParLigne))/2;
        int nbLignes = int(m_listeCellules.size() / nbCellulesParLigne)+1;
        int offsetY = int(((gv_AireDeJeu->height()/2 - margeBord*2) - (hauteurCellules + espaceCellules) * nbLignes)/2);

        int i=0;
        foreach(QGraphicsItem *m_item, listeItem)
        {
            if(m_item->type()==AbulEduCelluleV1::AbulEduCelluleType) // Faut être sûr que c'est une Cellule
            {
                AbulEduCelluleV1 *cellule = static_cast<AbulEduCelluleV1 *>(m_item);
                qreal x= margeBord + offsetX + i % nbCellulesParLigne * (largeurCellules + espaceCellules);
                qreal y = margeBord + offsetY + (hauteurCellules + espaceCellules) * int(i/nbCellulesParLigne);
                i++;
                cellule->setPos(x, y);

                // Pour les cellules qui contiennent des étiquettes, on remet les étiquettes à la bonne place
                if(cellule->abeCelluleHasPiece()){
                    AbulEduPieceV1 *piece = cellule->abeCelluleHasPiece();
                    piece->setPos(cellule->x()+(cellule->abeCelluleGetSize().width() - piece->abePieceGetSize().width())/2,
                                  cellule->y()+(cellule->abeCelluleGetSize().height()-piece->abePieceGetSize().height())/2);
                }
            }
        }
    }
}

void ExerciceMotsMelanges::clearScene()
{
    // On fait le ménage de printemps
    m_listeEtiquettes.clear();
    m_listeCellules.clear();
    gv_AireDeJeu->scene()->clear();
}

QString ExerciceMotsMelanges::takeSentence(const QString& unitPath, int exerciseNumber, AbulEduTextV1* text)
{
    QString sentence;
    QSettings* config = new QSettings(unitPath, QSettings::IniFormat);
    config->beginGroup("motsMelanges"); // L'indentation avec {...} n'est là que pour la lisibilité
        config->beginReadArray("exercice");
            config->setArrayIndex(exerciseNumber);
            int nombreMaxiMots = config->value("nombreMotsMax",18).toInt();

            if (config->value("modeManuel").toBool())
            {
                qDebug()<<"Tirage manuel";

 /* ---------------------------- Cas n°1 : mode manuel + numero de phrase à -1 ------------------------
    ---------- C'est le cas où la phrase peut avoir été saisie et ne pas appartenir au texte ----------*/

                if(config->value("numeroPhrase").toInt()==-1)
                {
                    sentence = config->value("phrase").toString();
                }
                else
 /* ------------------- Cas n°2 : mode manuel + numero de phrase différent de -1 -------------------
    ---- La phrase a été choisie dans l'éditeur, on prend le numéro plutôt que le texte (+robuste) ----*/

                {
                    sentence = text->abetextPhrase(config->value("numeroPhrase").toInt()-1);
                }

            }
            else
 /* ----------------------------------- Cas n°3 : mode automatique  ----------------------------------- */
            {
                qDebug()<<"Tirage automatique";
                qsrand(QDateTime::currentDateTime ().toTime_t ());
                QStringList copiePhrases = text->abetextPhrases().values();
                do
                {
                    qDebug()<<"je refais un tour ";
                    qDebug()<<"nb mots : "<<sentence.split(" ",QString::SkipEmptyParts).size()<<" :: "<<nombreMaxiMots;
                    qDebug()<<"vide ?"<<sentence.isEmpty();
                    qDebug()<<"nombre phrases dans copie "<<copiePhrases.size();
                    int tirage = qrand() % copiePhrases.size();
                    sentence = copiePhrases.takeAt(tirage);
                    qDebug()<<tirage;
                    qDebug()<<sentence;
                }
                while((sentence.split(" ",QString::SkipEmptyParts).size() > nombreMaxiMots || sentence.isEmpty()) && copiePhrases.size()>0 );
                if (sentence.split(" ",QString::SkipEmptyParts).size() > nombreMaxiMots)
                {
                    AbulEduMessageBoxV1::showInformation(trUtf8("Le texte ne contient pas de phrase de moins de ")+QString::number(nombreMaxiMots)+trUtf8(" mots..."),0,trUtf8("Réponse incomplète"));
//                    slotQuitterAccueil();
                }
            }
        config->endArray();
    config->endGroup();
    return sentence;
}
