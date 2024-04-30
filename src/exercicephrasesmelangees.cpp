 /**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011-2012 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011-2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#include "exercicephrasesmelangees.h"

ExercicePhrasesMelangees::ExercicePhrasesMelangees(QWidget *parent, QString repertoireBase) :
    AbulEduCommonStatesV1(parent), AbulEduOutilsV1(getAbeExerciceAireDeTravailV1())
{
    m_localDebug = 0;
    m_parent = parent;
    setHelpText("");
    m_messageBox = new AbulEduMessageBoxV1(QString(),QString(),false,getAbeExerciceAireDeTravailV1());
    m_messageBox->setVisible(false);
    if(m_parent->inherits("AbulEduPageAccueilV1")) {
            connect(m_parent, SIGNAL(dimensionsChangees()),this, SLOT(setDimensionsWidgets()),Qt::UniqueConnection);
    }

    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setAcceptDrops(true);
    setAbeTeteTelecommande("attente");
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();

    labelZone = new AbulEduLabelWidgetV1();
    labelZone->setStyleSheet("background-color: rgba(0,0,0,0)");
    labelZone->setFrameShape(QFrame::NoFrame);
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(labelZone); // le proxy va permettre de modifier l'ordre d'affichage des objets
    proxy->setZValue(-1); // labelZone sera en dessous des autres objets
    aireDeJeu = new AbulEduLabelFrameV1(Qt::Vertical, labelZone);
    connect(aireDeJeu, SIGNAL(AbulEduLabel_moved(AbuleduLabelV1*)), this, SLOT(updateHelpTextBecauseMotion(AbuleduLabelV1*)));
    labelZone->labelsScrollArea->setWidget(aireDeJeu);
    labelZone->setVisible(false);
    aireDeJeu->abelabelSetLargeurEtiquettes(200);
    aireDeJeu->abelabelSetMarge(10);

    if(!QDir(repertoireBase).exists()) {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Échec"),trUtf8("Impossible d'ouvrir le fichier spécifié !"));
    }
    else {
        m_config = new QSettings(repertoireBase + "/conf/module.conf",QSettings::IniFormat);
//        m_config->setIniCodec("UTF-8");
        QString fic(QString("%1/data/%2").arg(repertoireBase).arg(m_config->value("fichierTexte").toString()));
        m_texte = new AbulEduTextV1(fic);
        // Préparation des exercices
        if(m_config->value("phrasesMelangees/afficherPhrasesBienPlacees").toBool()){
            JEVEUXCOLORIERETIQUETTESMALPLACEES=1;
        }
        else{
            JEVEUXCOLORIERETIQUETTESMALPLACEES=0;
        }
        if (m_config->value("phrasesMelangees/exerciceActive").toBool() == false)
        {
            displayErrorMissingExercise(QString::fromUtf8("Phrases mélangées"),parent);
            this->deleteLater();
        }
    }
    getAbeExerciceMessageV1()->setParent(m_parent);
    sequenceMachine->start();
}

ExercicePhrasesMelangees::~ExercicePhrasesMelangees()
{
    //On expédie les logs sur le serveur
    pushAbulEduLogs();
    delete m_texte;
    delete m_config;
    emit exerciceExited();

}

void ExercicePhrasesMelangees::slotSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotSequenceEntered()";

    setAbeNbExercices(m_config->value("phrasesMelangees/exercice/size").toInt());
//    boiteTetes->resetTetes(m_prefixeTetes, m_nbExercices);               // Construit la barre de Têtes
//    boiteTetes->setVisible(false);
    setAbeNbTotalQuestions(1);
    m_listePhrases = m_texte->abetextPhrases().values(); // Liste des phrases
    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

    // Determine si on présente la séquence ou pas.
    onPeutPresenterSequence = m_config->value("phrasesMelangees/afficherConsigneSequence").toBool() || m_config->value("phrasesMelangees/afficherTexteSequence").toBool();

    // Determine si on affiche le bilan de l'exercice
    onPeutPresenterBilanExercice = true;

    AbulEduCommonStatesV1::slotSequenceEntered();
}
void ExercicePhrasesMelangees::slotPresenteSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotPresenteSequenceEntered();";
    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Phrases mélangées"));
    if (m_config->value("phrasesMelangees/afficherConsigneSequence").toBool())
    {
        QString debutTableau = "<tr valign=middle>";
        QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
        QString consigne = "<td align = right> " +  m_config->value("phrasesMelangees/consigneSequence").toString() + "</td>";
        QString finTableau = "</tr>";
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);
        QFileInfo fi(m_config->fileName());
        QString instructionFilePath = fi.absolutePath().replace("conf","data/instructions/phmConsigneSequence.ogg");
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
    if(m_config->value("phrasesMelangees/afficherTexteSequence").toBool()){
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(false);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
        for(int i = 0; i<m_listePhrases.size(); i++)
        {
            messageTexte = messageTexte + m_listePhrases[i] +"<br />";
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
    //    boiteTetes->resetTetes(m_prefixeTetes, m_nbExercices);               // Construit la barre de Têtes
    //    boiteTetes->setVisible(false);                                       // On les montrera plus tard !
}

void ExercicePhrasesMelangees::slotExercicesEntered()
{
    if (m_localDebug) qDebug()<<"ExercicePhrasesMelangees::slotExercicesEntered() : "<<m_numExercice+1;
    getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),QString()));
    m_config->beginGroup("phrasesMelangees");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    onPeutPresenterExercice = m_config->value("afficherConsigneExercice").toBool() || m_config->value("afficherTexteExercice").toBool();
    m_config->endArray();
    m_config->endGroup();
    AbulEduStateMachineV1::slotExercicesEntered();
}

void ExercicePhrasesMelangees::slotPresentationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotPresenteExoEntered();------";


    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Exercice n°") + QString::number(m_numExercice + 1));
    m_config->beginGroup("phrasesMelangees");
    m_config->beginReadArray("exercice");
            m_config->setArrayIndex(m_numExercice);
            int phraseDebut = m_config->value("phraseDebut").toInt();
            int phraseFin = m_config->value("phraseFin").toInt();

            if (m_config->value("afficherConsigneExercice").toBool())
            {
                getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(m_config->value("consigneExercice").toString());
            }
            else
            {
                getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne("");
            }
            QString message = "";
            for(int i = phraseDebut-1; i<phraseFin; i++)
            {
                message = message + m_listePhrases[i] +"<br />";
            }
            bool afficherTexte = m_config->value("afficherTexteExercice",false).toBool();
            getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(afficherTexte);
            if (afficherTexte)
            {
                getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(message);
                getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
            }
            else
            {
                getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(QString());
                getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
                getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
            }
        m_config->endArray();
    m_config->endGroup();
    redimensionneConsigne();
    getAbeExerciceMessageV1()->setVisible(true);
    AbulEduCommonStatesV1::slotPresentationExerciceEntered();
}

void ExercicePhrasesMelangees::slotRealisationExerciceEntered()
{
    m_config->beginGroup("phrasesMelangees");
        m_config->beginReadArray("exercice");
        m_config->setArrayIndex(m_numExercice);
            m_numLevel = m_config->value("niveauDifficulte").toInt();
        m_config->endArray();
    m_config->endGroup();
    getAbeExerciceTelecommandeV1()->setAbeLevel(QString::number(m_numLevel-1));
    setAbeExerciceName(trUtf8("Phrases mélangées - Exercice %1").arg(m_numExercice+1)); // Sera utilisé par le système de log

    if (m_localDebug) qDebug()<<" ExercicePhrasesMelangees::slotRealisationExerciceEntered() "<<QString::number(m_numLevel);
    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    setAbeExerciceEvaluation(abe::evalY);
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
}

QStringList ExercicePhrasesMelangees::takeSentences(const QString& unitPath, int exerciseNumber, AbulEduTextV1* text){
    QStringList sentences; //listePhrases
    QStringList allSentences = text->abetextPhrases().values(); // m_listePhrases
    QSettings* config = new QSettings(unitPath, QSettings::IniFormat);
    config->beginGroup("phrasesMelangees");
    config->beginReadArray("exercice");
    config->setArrayIndex(exerciseNumber);
    /* ---------------------------- Cas n°1 : mode Manuel ---------------------------- */
    if (config->value("modeManuel").toBool())
    {
        qDebug()<<"Mode Manuel";
        QStringList phrases(config->value("listePhrases").toString().split(";",QString::SkipEmptyParts));
        for(int i=0; i<phrases.size();i++){
            int num = phrases.at(i).toInt();
            sentences.append(allSentences.at(num-1));
        }
    }
    /* ----------------------------- Cas n°2 : mode Automatique, phrases consécutives ----------------------------- */
    else if(config->value("phrasesConsecutivesAutomatique").toBool())
    {
        qDebug()<<trUtf8("Mode Automatique, phrases consécutives ");
        int numPhraseFin = config->value("phraseFin",allSentences.size()).toInt();
        int numPhraseDebut = config->value("phraseDebut",1).toInt();
        int nombrePhrases =  config->value("nombrePhrasesAutomatique",5).toInt();
        qDebug()<<"numPhraseDebut : "<<numPhraseDebut<<", numPhraseFin : "<<numPhraseFin<<", nombre phrases : "<<nombrePhrases;
        QList<int> indexTires;
        if ((numPhraseFin - numPhraseDebut + 1) < nombrePhrases)
        {
            AbulEduMessageBoxV1::showInformation(trUtf8("Le nombre de phrases disponibles \n est inférieur au nombre de phrases demandé."),0,trUtf8("Problème"));
        }
        else
        {
            int modulo = numPhraseFin - nombrePhrases;
            if(modulo == 0)
                modulo = 1;
            int numPhraseTirage = numPhraseDebut + rand() % (modulo);
            qDebug()<<"Tirage base : "<<numPhraseTirage;
            qDebug()<<allSentences;
            for (int i=0;i<nombrePhrases;i++)
            {qDebug()<<"boucle for pour "<<i<<", soit at("<<numPhraseTirage+i-1;
                sentences.append(allSentences.at(numPhraseTirage + i-1));
                indexTires << numPhraseTirage + i-1;
                qDebug()<<"puis : "<<numPhraseTirage+i-1;
            }
            qDebug()<<"Sortie du for";
        }
        qDebug()<<"Numeros phrases auto consecutif : "<<indexTires;

    }
    /* ---------------------------- Cas n°3 : mode Automatique, phrases non consécutives ---------------------------- */
    else
    {
        qDebug()<<trUtf8("Mode Automatique, tout court...");
        int nbPhrasesTirees = 0;
        bool finAtteinte = false;
        QList<int> numPhrasePresents;
        QList<int> indexTires;
        int numPhraseFin = config->value("phraseFin").toInt();
        int numPhraseDebut = config->value("phraseDebut").toInt();
        for (int i=numPhraseDebut;i<=numPhraseFin;i++)
        {
            numPhrasePresents << i;
        }
        while (nbPhrasesTirees < config->value("nombrePhrasesAutomatique").toInt() && !finAtteinte)
        {
            int numPhraseTirage = rand() % (numPhrasePresents.count());
            int indexUtilise = numPhrasePresents.takeAt(numPhraseTirage);
            nbPhrasesTirees++;
            indexTires << indexUtilise;
            if (numPhrasePresents.count()<=0)
                finAtteinte = true;
        }
        qSort(indexTires);
        foreach(int num,indexTires)
        {
            sentences.append(allSentences.at(num-1));
        }

        qDebug()<<"Numeros phrases auto non consecutif : "<<indexTires;
    }
    config->endArray();
    config->endGroup();
    return sentences;
}

void ExercicePhrasesMelangees::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotInitQuestionEntered();";
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    QStringList listePhrases = takeSentences(m_config->fileName(),m_numExercice,m_texte);
    if (m_localDebug) qDebug() << "###############################################################";
//    if (m_localDebug) qDebug() << m_config->group();
//    if (m_localDebug) qDebug() << m_numExercice;
    if (m_localDebug) qDebug() << listePhrases;
    if (m_localDebug) qDebug() << "###############################################################";
//    boiteTetes->setCouleurs(QPen(Qt::red), QBrush(Qt::yellow));
    m_nbClicSurVerifier = 0;

    AbulEduCommonStatesV1::slotInitQuestionEntered();

    aireDeJeu->abelabelCreeAbulEduLabels(listePhrases,"background-color: #dfded3 ;border-radius: 10px;padding-left:10px;","background-color: #e0c1a9 ;border-radius: 10px;padding-left:10px;","background-color: #ff4f4a ;border-radius: 10px;padding-left:10px;");
    aireDeJeu->abelabelMelangeAbulEduLabels();
    m_ordrePresentation.clear();
    for (int j=0;j<aireDeJeu->abelabelListeAbuleduLabelsByNumPlace().size();j++) // Utiliser un iterateur serait plus sûr !!!
    {
        m_ordrePresentation << QString::number(aireDeJeu->abelabelListeAbuleduLabelsByNumPlace().value(j)->abelabelNumOrdre());
    }
    labelZone->setVisible(true);
    aireDeJeu->show();

}

void ExercicePhrasesMelangees::slotQuestionEntered()
{if (m_localDebug) qDebug()<<"              ExercicePhrasesMelangees::slotQuestionEntered()";
//    setDimensionsWidgets(); // Place les Widgets à leur place

    AbulEduCommonStatesV1::slotQuestionEntered();
    setHelpText(trUtf8("Déplace par un glisser-déposer avec la souris les phrases pour les remettre dans l'ordre normal du texte. Tu peux aussi en sélectionner une puis cliquer sur les <b>flèches</b> vertes pour la déplacer vers le haut ou le bas. ")+"<br />"
                + trUtf8("Clique sur le bouton <b>Vérification</b> quand tu as fini ou sur le bouton <b>Solution</b> si tu n'y arrives pas..."));
}

void ExercicePhrasesMelangees::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhrasesMelangees::slotAfficheVerificationQuestionEntered() : "<<m_nbClicSurVerifier;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    if (JEVEUXCOLORIERETIQUETTESMALPLACEES == 1)
    {
        setHelpText(trUtf8("La couleur <b>verte</b> indique les phrases <b>bien</b> placées les unes par rapport aux autres.")+"<br />"
                    + trUtf8("Mais pas forcément à leur <b>place</b> définitive.")+"<br />"
                    + trUtf8("Tu peux bouger des phrases puis cliquer sur le bouton <b>Vérification</b> à nouveau."));
    }
    else
    {
        setHelpText(trUtf8("Certaines étiquettes sont mal placées.")+"<br />"
                    + trUtf8("Tu peux bouger des étiquettes puis cliquer sur le bouton <b>Vérification</b> à nouveau."));
    }
    abeStateMachineSetVerifieReponse(verifieReponse());
    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();
}

void ExercicePhrasesMelangees::slotFinVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhrasesMelangees::slotFinVerificationQuestionEntered()";
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
        setAbeLineLog(m_ordrePresentation.join(","),
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
            setAbeLineLog(m_ordrePresentation.join(","),
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
            qDebug()<<m_ordrePresentation.join(",");
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

void ExercicePhrasesMelangees::slotAfficheCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhrasesMelangees::slotAfficheCorrectionQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    setHelpText(trUtf8("Les étiquettes ont été replacées au bon endroit.")+"<br />"
                + trUtf8("La couleur verte indique qu'une étiquette était bien placée.")+"<br />"
                + trUtf8("La couleur violette indique qu'une étiquette était mal placée."));
    abeStateMachineSetVerifieReponse(donneReponse());
    AbulEduStateMachineV1::slotAfficheCorrectionQuestionEntered();
}

void ExercicePhrasesMelangees::slotFinCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ ExercicePhrasesMelangees::slotFinCorrigerQuestionEntered()";
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    //Le problème du log, c'est d'être sûr que si on a envoyé des choses avant (des clics sur Vérification avant le clic sur Corriger), ils vont bien être envoyés
    sequenceMachine->cancelDelayedEvent(id_temporisation);
    int score;

    sequenceMachine->postEvent(new StringEvent("QuestionsDoneCorrect"));
    if ((qCeil(m_percent) - m_percent)>(m_percent - qFloor(m_percent)))
        score = qFloor(m_percent);
    else
        score = qCeil(m_percent);
    if (m_answers.count() > 0)
    {
        setAbeLineLog(m_ordrePresentation.join(","),
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
            qDebug()<<m_ordrePresentation.join(",");
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

void ExercicePhrasesMelangees::slotBilanExerciceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotBilanExerciceEntered();";

    labelZone->setVisible(false);
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
            message = trUtf8("Tu as réussi l'exercice en %1 essais","%1 est supérieur à 1").arg(m_nbClicSurVerifier);
        }
    }
    m_resultats.append(QPair<int, int>(m_numExercice, m_nbClicSurVerifier));

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
    //question meld 20120221
    redimensionneBilan();

}

void ExercicePhrasesMelangees::slotBilanSequenceEntered()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotBilanSequenceEntered();------";
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
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Phrases mélangées"));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(message);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneBilan();
}

void ExercicePhrasesMelangees::slotBilanSequenceExited()
{
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    AbulEduCommonStatesV1::slotBilanSequenceExited();
}


void ExercicePhrasesMelangees::slotQuitter()
{
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::slotQuitter";

    AbulEduCommonStatesV1::slotQuitter();
}

void ExercicePhrasesMelangees::slotAide()
{
    AbulEduStateMachineV1::slotAide();
}

//-------------------------------------------------------------------------
//                      Interface de l'exercice
//-------------------------------------------------------------------------

void ExercicePhrasesMelangees::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::resizeEvent()";
//    setDimensionsWidgets();
}
void ExercicePhrasesMelangees::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    if (m_localDebug) qDebug()<<"               ExercicePhrasesMelangees::showEvent()";
//    setDimensionsWidgets();
}
void ExercicePhrasesMelangees::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"                ExercicePhrasesMelangees::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    labelZone->setFixedSize(large - 250 * ratio, haut - 100 * ratio);
    labelZone->move((250 * ratio) / 2, 100 * ratio);
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio);

    //La largeur de la zone des etiquettes = largeur max de la zone moins 100
    aireDeJeu->abelabelSetLargeurEtiquettes(labelZone->width() - 150);
    aireDeJeu->abelabelResizeLabels();
    aireDeJeu->abelabelResizeFrame();
    aireDeJeu->abelabelPlaceAbulEduLabelsByNumPlace(); // Car certaines étiquettes peuvent changer de hauteur

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

    if (m_localDebug) qDebug()<<"                ExercicePhrasesMelangees::setDimensionsWidgets()---end";
}

//-------------------------------------------------------------------------
//                      Fonctions de l'exercice
//-------------------------------------------------------------------------

void ExercicePhrasesMelangees::updateHelpTextBecauseMotion(AbuleduLabelV1 *etiquette)
{
    Q_UNUSED(etiquette);
    setHelpText("Continue à déplacer les étiquettes, et relis-les dans l'ordre pour vérifier si l'enchainement te parait correct.");
}

int ExercicePhrasesMelangees::verifieReponse()
{
    if (m_localDebug) qDebug()<<"          ExercicePhrasesMelangees::verifieReponse()";
    QStringList reponseDonnee;
    QStringList reponseAttendue;
    int nbreponsesFausses = 0;
    // C'est ici que l'on teste la validité des réponses et l'on renvoie le nombre de réponses fausses
        // 0 = tout bon, on peut passer à la suite
        // le nombre retourné sera testé dans le slotVerifieReponse()
    QMap<int, AbuleduLabelV1 *> tmpListebynumPlace = aireDeJeu->abelabelListeAbuleduLabelsByNumPlace();
    // --- Création d'un vecteur avec les rangs des étiquettes rencontrées
    QVector<int> rangEtiquettesProposees(tmpListebynumPlace.size());
    for (int j=0;j<tmpListebynumPlace.size();j++) // Utiliser un iterateur serait plus sûr !!!
    {
        rangEtiquettesProposees[j] = tmpListebynumPlace.value(j)->abelabelNumOrdre();
    }


    if (m_localDebug) qDebug()<<"QVector <int> rangEtiquettesProposees : "<<rangEtiquettesProposees;
    QVector<int> listeRangsCroissants = abeoutilsFindGoodList(rangEtiquettesProposees);
    if (m_localDebug) qDebug()<<"Qvector <int> plus longue sous suite : "<<listeRangsCroissants;

    nbreponsesFausses = tmpListebynumPlace.size() - listeRangsCroissants.size();

    QMapIterator<int, AbuleduLabelV1 *> i(tmpListebynumPlace);
    while(i.hasNext())
    {
        i.next();
        reponseAttendue.append(QString::number(i.value()->abelabelNumPlace()));
        reponseDonnee.append(QString::number(i.value()->abelabelNumOrdre()));
        if(!listeRangsCroissants.contains(i.value()->abelabelNumOrdre()))
        {
            i.value()->setStyleSheet("background-color: rgba(234,33,0,178) ;border-radius: 10px;padding-left:10px;");
        }
        else
        {
            i.value()->setStyleSheet("background-color: rgba(3,151,3,178) ;border-radius: 10px;padding-left:10px;");
        }
    }
    if (m_localDebug)
        qDebug()<<" -- nbreponsesFausses"<<nbreponsesFausses;
    setAbeTeteForResult(nbreponsesFausses,tmpListebynumPlace.count());
    m_answers.append(QPair<QVariant,QVariant>(reponseDonnee.join(","),reponseAttendue.join(",")));
    m_percent = setAbeTeteForResult(nbreponsesFausses,tmpListebynumPlace.count());
    if (m_localDebug)
    {
        qDebug()<<"LOG ajoute :";
        qDebug()<<m_ordrePresentation.join(",");
        qDebug()<<m_answers.last().first.toString();
        qDebug()<<m_percent;
        qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
        qDebug()<<m_answers.last().second.toString();
    }
//    setDimensionsWidgets();
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
    return nbreponsesFausses;
}

int ExercicePhrasesMelangees::donneReponse()
{
    setAbeExerciceEvaluation(abe::evalZ);
    QStringList reponseAttendue;
    QMapIterator<int, AbuleduLabelV1 *> i(aireDeJeu->abelabelListeAbuleduLabelsByNumOrdre());
    while(i.hasNext())
    {
        i.next();
        reponseAttendue.append(QString::number(i.value()->abelabelNumOrdre()));
    }
    m_answers.append(QPair<QVariant,QVariant>(" --- ",reponseAttendue.join(",")));
    setAbeTeteTelecommande("rien");

    QMap<int, AbuleduLabelV1 *> tmpListebynumPlace = aireDeJeu->abelabelListeAbuleduLabelsByNumPlace();
    // --- Création d'un vecteur avec les rangs des étiquettes rencontrées
    QVector<int> rangEtiquettesProposees(tmpListebynumPlace.size());
    for (int j=0;j<tmpListebynumPlace.size();j++) // Utiliser un iterateur serait plus sûr !!!
    {
        rangEtiquettesProposees[j] = tmpListebynumPlace.value(j)->abelabelNumOrdre();
    }

    if (m_localDebug) qDebug()<<"QVector <int> rangEtiquettesProposees : "<<rangEtiquettesProposees;
    QVector<int> listeRangsCroissants = abeoutilsFindGoodList(rangEtiquettesProposees);
    if (m_localDebug) qDebug()<<"Qvector <int> plus longue sous suite : "<<listeRangsCroissants;

    foreach(AbuleduLabelV1* paragraphe,aireDeJeu->abelabelListeAbuleduLabelsByNumPlace())
    {
        if (listeRangsCroissants.contains(paragraphe->abelabelNumOrdre()))
        {
            paragraphe->setStyleSheet("background-color: rgba(1,125,116,178) ;border-radius: 10px;padding-left:10px;");
        }
        else
            paragraphe->setStyleSheet("background-color: rgba(134,45,176,178) ;border-radius: 10px;padding-left:10px;");
    }
    setDimensionsWidgets();
    //Attention : il faut faire le abelabelPlaceAbulEduLabelsByNumOrdre() après setDimensionsWidgets() parce que setDimensionsWidgets() contient abelabelPlaceAbulEduLabelsByNumPlace()
    aireDeJeu->abelabelPlaceAbulEduLabelsByNumOrdre();
    if (m_localDebug)
    {
        qDebug()<<"LOG ajoute :";
        qDebug()<<m_ordrePresentation.join(",");
        qDebug()<<m_answers.last().first.toString();
        qDebug()<<m_percent;
        qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
        qDebug()<<m_answers.last().second.toString();
    }
    return -1;
}

QMap<int, QStringList> ExercicePhrasesMelangees::abeoutilsCreerListeMots(QStringList listeMots, int nbMots, abe::ABE_LEVEL niveau, QString nature, int profondeur)
{
    QMap<int, QStringList>tmpliste;
    tmpliste.clear();

    for(int k = 0;k < m_nbExercices; k++)
    {
        tmpliste.insert(k,AbulEduOutilsV1::abeoutilsCreerListeMots(listeMots,nbMots,niveau, nature, profondeur));
    }
    return tmpliste;
}

void ExercicePhrasesMelangees::redimensionneConsigne()
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

void ExercicePhrasesMelangees::redimensionneBilan()
{
    //question meld 20120221
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}
