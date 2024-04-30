/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011 André Connes <andre.connes@wanadoo.fr>
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
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

#include "exerciceclosure.h"

ExerciceClosure::ExerciceClosure(QWidget *parent, QString repertoireBase) :
    AbulEduCommonStatesV1(parent), AbulEduOutilsV1(getAbeExerciceAireDeTravailV1())
{
    m_localDebug = false;
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    m_parent = parent;
    id_temporisation = 0;
    m_maxPieceWidth = 20;
    m_maxCelluleWidth = 0;
    setHelpText("");
    m_receivedDropResults.clear();
    m_listePiecesPlacees.clear();
    m_upButton = 0;
    m_downButton = 0;
    m_pointSize = 24;

    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setAcceptDrops(true);
    //    setWindowTitle(abeApp->getAbeApplicationLongName()+"--"+trUtf8("Texte à trous"));
    setAbeTeteTelecommande("attente");
    // Connecte le signal émis par la page d'accueil quand ses dimensions changent avec le slot setDimensionsWidgets()
    if(m_parent->inherits("AbulEduPageAccueilV1")) {
            connect(m_parent, SIGNAL(dimensionsChangees()),this, SLOT(setDimensionsWidgets()),Qt::UniqueConnection);
    }
    m_aireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));

#ifdef __ABULEDUTABLETTEV1__MODE__
    m_aireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_aireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    m_aireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_aireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
    connect(m_aireDeJeu,SIGNAL(wheelTurn(int)),m_aireDeJeu->verticalScrollBar(),SLOT(setValue(int)), Qt::UniqueConnection);
    connect(m_aireDeJeu->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(movePieceOrdinate(int)), Qt::UniqueConnection);
    m_messageBox = new AbulEduMessageBoxV1(QString(),QString(),false,getAbeExerciceAireDeTravailV1());
    m_messageBox->setVisible(false);
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(m_aireDeJeu); // le proxy va permettre de modifier l'ordre d'affichage des objets
    proxy->setZValue(-1); // labelZone sera en dessous des autres objets
    m_aireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    m_aireDeJeu->setFrameShape(QFrame::NoFrame);

    if(!QDir(repertoireBase).exists())
    {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Échec"),trUtf8("Impossible d'ouvrir le fichier spécifié !"));
//        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Échec"),trUtf8("Impossible d'ouvrir le fichier spécifié !"));
//        messageBox->show();
    }
    else {
        m_config = new QSettings(repertoireBase + "/conf/module.conf",QSettings::IniFormat);
        //        m_config->setIniCodec("UTF-8");
        QString fic(QString("%1/data/%2").arg(repertoireBase).arg(m_config->value("fichierTexte").toString()));
        m_texte = new AbulEduTextV1(fic);
        // Préparation des exercices
        m_config->beginGroup("texteATrous");
        if(m_config->value("afficherMotBienPlaces").toBool())
            JEVEUXCOLORIERETIQUETTESMALPLACEES=1;
        else
            JEVEUXCOLORIERETIQUETTESMALPLACEES=0;
        if (m_config->value("exerciceActive").toBool() == false)
        {
            displayErrorMissingExercise(QString::fromUtf8("Texte à trous"),parent);
            this->deleteLater();
        }
        m_config->endGroup();
    }
    getAbeExerciceMessageV1()->setParent(m_parent);
    sequenceMachine->start();

    m_listeSuperMots = m_texte->abeTextSuperMots();
}

ExerciceClosure::~ExerciceClosure()
{
    delete m_texte;
    delete m_config;
    //    delete getAbeExerciceMessageV1();
    pushAbulEduLogs(abeApp->property("moduleEnCours").toString());
    // Permet à la MainWindow de savoir que l'exercice est terminé
    emit exerciceExited();
}

void ExerciceClosure::slotSequenceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setAbeNbExercices(m_config->value("texteATrous/exercice/size").toInt());
    setAbeNbTotalQuestions(1);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());
    // Determine si on présente la séquence ou pas.
    onPeutPresenterSequence = m_config->value("texteATrous/afficherConsigneSequence").toBool() || m_config->value("texteATrous/afficherTexteSequence").toBool();
    // Determine si on affiche le bilan de l'exercice
    onPeutPresenterBilanExercice = true;
    AbulEduCommonStatesV1::slotSequenceEntered();
}

void ExerciceClosure::slotPresenteSequenceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    getAbeExerciceMessageV1()->setVisible(false);
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Texte à trous"));

    if (m_config->value("texteATrous/afficherConsigneSequence").toBool())
    {
        QString debutTableau = "<tr valign=middle>";
        QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
        QString consigne = "<td align = right> " +  m_config->value("texteATrous/consigneSequence").toString() + "</td>";
        QString finTableau = "</tr>";
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);
        QFileInfo fi(m_config->fileName());
        QString instructionFilePath = fi.absolutePath().replace("conf","data/instructions/ttrConsigneSequence.ogg");
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

    if(m_config->value("texteATrous/afficherTexteSequence").toBool())
    {
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(false);
        if (m_localDebug) qDebug()<<"Affichage du texte demandé";
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(m_texte->abeGetText());
        getAbeExerciceMessageV1()->abeWidgetMessageResize();
    }
    else
    {
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    }
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneConsigne();
}

void ExerciceClosure::slotExercicesEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_numExercice+1;
    getAbeExerciceMessageV1()->abeWidgetMessageSetMediaMedias(AbulEduMediaMedias(QString(),QString()));
    m_config->beginGroup("texteATrous");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    onPeutPresenterExercice = (m_config->value("afficherConsigneExercice").toBool() || m_config->value("afficherTexteExercice").toBool());
    m_config->endArray();
    m_config->endGroup();
    AbulEduStateMachineV1::slotExercicesEntered();
}

void ExerciceClosure::slotPresentationExerciceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    getAbeExerciceMessageV1()->setVisible(false);
    m_config->beginGroup("texteATrous");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    bool affichageConsigne = m_config->value("afficherConsigneExercice").toBool();
    int debutTexteAfficher = m_config->value("marqueurDebutTexte").toInt();
    int finTexteAfficher   = m_config->value("marqueurFinTexte").toInt();
    bool affichageTexteExo = m_config->value("afficherTexteExercice").toBool();
    if(affichageConsigne){
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(m_config->value("consigneExercice").toString());
    }
    else {
        getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(QString());
    }
    m_config->endArray();
    m_config->endGroup();

    if (m_localDebug) qDebug()<<"Affichage texte en début d'exercice : "<<affichageTexteExo;
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Exercice n°%1").arg(m_numExercice+1));
    if(affichageTexteExo)
    {
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(false);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice(m_texte->abeGetText().mid(debutTexteAfficher,finTexteAfficher-debutTexteAfficher));
    }
    else
    {
        getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
        getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    }
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneConsigne();

    AbulEduCommonStatesV1::slotPresentationExerciceEntered();
}

void ExerciceClosure::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<QString::number(m_numLevel);
    m_config->beginGroup("texteATrous");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    m_numLevel = m_config->value("niveauDifficulte",1).toInt();
    setAbeExerciceName(trUtf8("Texte à trous - Exercice %1").arg(m_numExercice+1)); // Sera utilisé par le système de log

    m_config->endArray();
    m_config->endGroup();
    getAbeExerciceTelecommandeV1()->setAbeLevel(QString::number(m_numLevel-1));
    setAbeLevel(QString::number(m_numLevel-1));

    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    setAbeExerciceEvaluation(abe::evalY);
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
}

void ExerciceClosure::slotInitQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<< m_numExercice;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    // Le nombre de clics sur le bouton Vérification servira à évaluer l'exercice
    m_nbClicSurVerifier = 0;

    AbulEduCommonStatesV1::slotInitQuestionEntered();

}

void ExerciceClosure::slotQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    setHelpText(trUtf8("Les étiquettes à droite du texte contiennent les mots enlevés. Déplace-les vers le trou qui contenait ces mots.")+"<br />"
                + trUtf8("Clique sur le bouton <b>Vérification</b> quand tu as fini ou sur le bouton <b>Solution</b> si tu n'y arrives pas..."));

    afficheTexteEtEtiquettes();
    AbulEduCommonStatesV1::slotQuestionEntered();
}

void ExerciceClosure::slotAfficheCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    setHelpText(trUtf8("Les étiquettes ont été replacées au bon endroit.")+"<br />"
                + trUtf8("La couleur verte indique qu'une étiquette était bien placée.")+"<br />"
                + trUtf8("La couleur violette indique qu'une étiquette était mal placée.")+"<br />"
                + trUtf8("La couleur bleue indique qu'une étiquette n'était pas placée."));
    abeStateMachineSetVerifieReponse(donneReponse());
    AbulEduStateMachineV1::slotAfficheCorrectionQuestionEntered();
//    setDimensionsWidgets();
}

void ExerciceClosure::slotFinCorrectionQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    //Le problème du log, c'est d'être sûr que si on a envoyé des choses avant (des clics sur Vérification avant le clic sur Corriger), ils vont bien être envoyés
    int score;//inutile ? non, détourné ;-)

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
    AbulEduStateMachineV1::slotFinCorrectionQuestionEntered();
}

void ExerciceClosure::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nbClicSurVerifier;
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

void ExerciceClosure::slotFinVerificationQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    int score;//inutile ? non, détourné ;-)

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
            //Je mets -1 à la place du nombre de clics, puisque c'est le nombre de clics nécessaires pour arriver à la bonne réponse et que je n'y suis pas
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
            //qDebug()<<getAbeExerciceEvaluation();
            qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
            qDebug()<<m_answers.last().second.toString();
        }
        else
            qDebug()<<"Nibe a envoyer...";
    }
}

void ExerciceClosure::slotFinQuestionEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    AbulEduStateMachineV1::slotFinQuestionEntered();
}

void ExerciceClosure::slotBilanExerciceEntered()
{
    if (m_localDebug)
    {
        qDebug()<<"------------------------------------------------------------------------------------";
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug()<<sequenceMachine->configuration().toList();
        qDebug()<<"------------------------------------------------------------------------------------";
    }
    clearScene();
    // On affiche la bonne tete en fonction du nb de clics sur btnVerifier
    QString imagetete = ":/bilan/";
    switch(m_nbClicSurVerifier)
    {
    case 1:
        /* On met à jour en fonction de la réussite la tête en bas */
        boiteTetes->setEtatTete(m_numExercice, abe::evalA);
        /* On prépare la tête qui sera affichée dans l'AbulEduExerciceWidgetMessageV1 */
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
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}

void ExerciceClosure::slotBilanSequenceEntered()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
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
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Texte à trous - Bilan"));
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(message);
    getAbeExerciceMessageV1()->abeWidgetMessageShowImageFond(true);
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionneBilan();
}

void ExerciceClosure::slotBilanSequenceExited()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    getAbeExerciceMessageV1()->setVisible(false);
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
    AbulEduCommonStatesV1::slotBilanSequenceExited();
}

void ExerciceClosure::slotQuitter()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceClosure::slotAide()
{
    AbulEduStateMachineV1::slotAide();
}

//-------------------------------------------------------------------------
//                      Interface de l'exercice
//-------------------------------------------------------------------------

void ExerciceClosure::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    //    setDimensionsWidgets();
}

void ExerciceClosure::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    //    setDimensionsWidgets();
}

void ExerciceClosure::setDimensionsWidgets()
{
    //    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0, 0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 20 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    m_aireDeJeu->setFixedSize(large - 170 * ratio, haut - 100 * ratio);
    m_aireDeJeu->move((250 * ratio) / 2, 100 * ratio);
    //    m_aireDeJeu->setSceneRect(0,0,m_aireDeJeu->width()-10,m_aireDeJeu->height()-50);

    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 50 * ratio);

    //La largeur de la zone des etiquettes = largeur max de la zone moins 100
    //    m_aireDeJeu->abelabelSetLargeurEtiquettes(labelZone->width() - 150);
    //    m_aireDeJeu->abelabelResizeLabels();
    //    m_aireDeJeu->abelabelResizeFrame();
    //    m_aireDeJeu->abelabelPlaceAbulEduLabelsByNumPlace(); // Car certaines étiquettes peuvent changer de hauteur

    QSet<QAbstractState *> etats = sequenceMachine->configuration(); // On récupère les états de la machine
    if(etats.contains(question)){ //On ne déplace l'exercice que lorsqu'il est visible
        placeMots();
        placeCellules();
    }
    // Redimmensionne le widget de consignes et de bilan
    redimensionneConsigne();
    redimensionneBilan();

    AbulEduCommonStatesV1::setDimensionsWidgets();
    //    getAbeExerciceMessageV1()->move(250*ratio,70*ratio);

    //    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
}



//-------------------------------------------------------------------------
//                      Fonctions de l'exercice
//-------------------------------------------------------------------------

void ExerciceClosure::updateDropResultReceived(abe::ABE_EVAL eval)
{
    //    AbulEduCelluleV1* celluleEmettrice = static_cast<AbulEduCelluleV1 *>(sender());
    //    if (celluleEmettrice->abeCelluleHasPiece())
    //    {
    //        m_listePiecesPlacees << celluleEmettrice->abeCelluleHasPiece();
    //    }
    m_listePiecesPlacees.clear();
    foreach(AbulEduCelluleV1* cellule,m_listeCellulesVides)
    {
        if(cellule->abeCelluleHasPiece())
            m_listePiecesPlacees << cellule->abeCelluleHasPiece();
    }

    setHelpText(trUtf8("Oui, on fait comme ça.")+"<br />"
                +trUtf8("Continue en vérifiant bien le sens de tes phrases."));
    m_receivedDropResults << eval;
    //    if (m_localDebug)
    //    {
    //        qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    //        qDebug()<<"Résultats : "<<m_receivedDropResults;
    //        qDebug()<<"Pieces placées : "<<m_listePiecesPlacees;
    //    }
    if (m_receivedDropResults.contains(abe::evalD))
    {
        setHelpText(trUtf8("Oui, on fait comme ça, mais...")+"<br />"
                    +trUtf8("Relis les phrases avec les mots déjà placés.")+"<br />"+ trUtf8("Le sens te semble-t-il correct ?"));
    }
}

int ExerciceClosure::verifieReponse()
{
    QStringList reponseDonnee;
    QStringList reponseAttendue;
    int nbreponsesFausses = 0;

    // On récupère la liste des mots associés
    QList<AbulEduPieceV1 *> listePiecesPlacees;

    // On construit la liste des pièces placée dans les Cellules
    foreach(AbulEduCelluleV1 *cellule, m_listeCellulesVides)
    {
        if(cellule->abeCelluleHasPiece())
        {
            listePiecesPlacees.append(cellule->abeCelluleHasPiece());
        }
    }

    //    if (m_localDebug) qDebug() << listePiecesPlacees;

    if(listePiecesPlacees.size()<m_listeCellulesVides.size()) //
    {
        if (m_localDebug) qDebug()<<"Toutes les cellules ne sont pas remplies, et on n'est pas dans le cas d'une correction";
        m_messageBox->abeMessageBoxUpdate(trUtf8("Réponse incomplète"),trUtf8("Place tous les mots avant de cliquer sur le bouton <b>Vérification</b>"));

//        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Réponse incomplète"),trUtf8("Place tous les mots avant de cliquer sur <b>Vérification</b>"));
//        messageBox->show();
        // Je sors avec un nombre (1 ou un autre tant que ce n'est pas 0, pour empêcher le log)
        setAbeTeteTelecommande("rien");
//        setDimensionsWidgets();
        return 1;
    }
    else
    {
        //        if (m_localDebug) qDebug()<<"Ou bien toutes les cellules sont remplies, ou bien on demande la correction";
        for(int i=0;i< m_listeCellulesVides.size();i++)
        {

            reponseAttendue.append(m_listeCellulesVides[i]->objectName());
            reponseDonnee.append(m_listeCellulesVides[i]->abeCelluleHasPiece()->objectName());
            //Si c'est bien placé on est content
            if(m_listeCellulesVides[i]->objectName() == m_listeCellulesVides[i]->abeCelluleHasPiece()->objectName())
            {
                if (m_localDebug) qDebug()<<m_listeCellulesVides[i]->objectName()<<" mis a la bonne place (donc compte juste)";
                if(JEVEUXCOLORIERETIQUETTESMALPLACEES==1)
                {
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetMarge(0);
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetBrush(QColor(3,151,3,179));
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->update();
                }

            }
            //Sinon, on est moins content
            else
            {
                if (m_localDebug) qDebug()<<m_listeCellulesVides[i]->objectName()<<" mis a une mauvaise place (donc compte faux)";
                //                    listeCellulesIncorrectes.append(m_listeCellulesVides[i]);
                nbreponsesFausses++;
                if(JEVEUXCOLORIERETIQUETTESMALPLACEES==1)
                {
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetBrush(QColor(234,33,0,179));
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                    m_listeCellulesVides[i]->abeCelluleHasPiece()->update();
                }

            }
        }
    }

    m_answers.append(QPair<QVariant,QVariant>(reponseDonnee.join(","),reponseAttendue.join(",")));
    m_percent = setAbeTeteForResult(nbreponsesFausses,m_listeCellulesVides.count());
    if (m_localDebug)
    {
        qDebug()<<"LOG ajoute :";
        qDebug()<<m_answers.last().first.toString();
        qDebug()<<m_answers.last().second.toString();
        qDebug()<<m_percent;
        qDebug()<<abe::ABE_DONNEEVAL().value(getAbeExerciceEvaluation());
    }

    //    }

//    setDimensionsWidgets();
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);

    if (m_localDebug) qDebug()<<"Je sors de verifieReponse() avec "<<nbreponsesFausses;
    return nbreponsesFausses;
}

int ExerciceClosure::donneReponse()
{
    setAbeExerciceEvaluation(abe::evalZ);
    QStringList reponseAttendue;
    setAbeTeteTelecommande("rien");
    setDimensionsWidgets();

    /* Dans un premier temps je mets la bonne couleur */
    m_listePiecesPlacees.clear();
    for(int i=0;i< m_listeCellulesVides.size();i++)
    {
        if (m_listeCellulesVides[i]->abeCelluleHasPiece() != 0)
        {
            if (m_listeCellulesVides[i]->objectName() == m_listeCellulesVides[i]->abeCelluleHasPiece()->objectName())
            {
                m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetBrush(QColor(1,125,116,179));
                m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                m_listeCellulesVides[i]->abeCelluleHasPiece()->setFlag(QGraphicsItem::ItemIsMovable,false);
                m_listeCellulesVides[i]->abeCelluleHasPiece()->update();
                m_listePiecesPlacees.append(m_listeCellulesVides[i]->abeCelluleHasPiece());
            }
            else
            {
                m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetBrush(QColor(134,45,176,179));
                m_listeCellulesVides[i]->abeCelluleHasPiece()->abePieceSetRadius(0,0);
                m_listeCellulesVides[i]->abeCelluleHasPiece()->setFlag(QGraphicsItem::ItemIsMovable,false);
                m_listeCellulesVides[i]->abeCelluleHasPiece()->update();
                m_listePiecesPlacees.append(m_listeCellulesVides[i]->abeCelluleHasPiece());
            }
            m_listeCellulesVides[i]->abeCelluleSetPiece(0);
        }
    }
    foreach(AbulEduPieceV1* piece,m_listePieces)
    {
        if (!m_listePiecesPlacees.contains(piece))
        {
            piece->abePieceSetBrush(QColor(0,108,192,179));
            piece->abePieceSetRadius(0,0);
            piece->setFlag(QGraphicsItem::ItemIsMovable,false);
            piece->update();
        }
        piece->setVisible(true);
    }

    /* Dans un deuxième temps seulement je les bouge si elles ne sont pas au bon endroit */
    m_listePiecesPlacees.clear();
    QList<AbulEduPieceV1*> listePiecesDispo = m_listePieces;
    foreach(AbulEduCelluleV1* cellule, m_listeCellulesVides)
    {
        int i = 0;
        bool trouve = false;
        while(i <listePiecesDispo.size() && trouve == false)
        {
            qDebug()<<" on regarde "<<listePiecesDispo[i]->objectName();
            if(listePiecesDispo[i]->objectName() == cellule->objectName())
            {
                trouve = true;
                int deltaX = (cellule->abeCelluleGetSize().width() - listePiecesDispo[i]->abePieceGetSize().width())/2;
                int deltaY = (cellule->abeCelluleGetSize().height() - listePiecesDispo[i]->abePieceGetSize().height())/2;

                listePiecesDispo[i]->setPos(cellule->pos()+QPoint(deltaX,deltaY));
                cellule->abeCelluleSetPiece(listePiecesDispo[i]);
                m_listePiecesPlacees.append(listePiecesDispo[i]);
                listePiecesDispo.removeOne(listePiecesDispo[i]);
            }
            i++;
        }
        reponseAttendue.append(cellule->objectName());
        m_answers.append(QPair<QVariant,QVariant>(" --- ",reponseAttendue.join(",")));
    }
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);

    /* Suppression des boutons Up et Down s'ils sont là */
    foreach(AbulEduPieceButtonV1* pb,m_aireDeJeu->scene()->findChildren<AbulEduPieceButtonV1*>()){
        pb->deleteLater();
    }

    return -1;
}

QMap<int, QStringList> ExerciceClosure::abeoutilsCreerListeMots(QStringList listeMots, int nbMots, abe::ABE_LEVEL niveau, QString nature, int profondeur)
{
    QMap<int, QStringList>tmpliste;
    tmpliste.clear();

    for(int k = 0;k < m_nbExercices; k++)
    {
        tmpliste.insert(k,AbulEduOutilsV1::abeoutilsCreerListeMots(listeMots,nbMots,niveau, nature, profondeur));
    }
    return tmpliste;
}

void ExerciceClosure::afficheTexteEtEtiquettes()
{
    if(m_localDebug) {
        qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    }
    //! Pas envie de batailler plus : je vois que c'est dans cette méthode que ma liste de super mots est perturbée et entraine le défaut d'affichage
    //! pour les exercices suivants... je ne sais pas pourquoi mais en tout cas avec cette sauvegardeListe j'évite le problème
    QMap <int,QPair<QString,QPair<int,int> > > sauvegardeListe = m_listeSuperMots;
    clearScene();
    float ratio = abeApp->getAbeApplicationDecorRatio();
    /*
     * Idee generale: une liste de QPair definit les "trous" a creer de type position_debut, position_fin
     * ensuite on parcours le texte a la recherche des zones a trouer
     * et des qu'on en trouve on cree deux objets: une zone sensible (AbulEduCelluleV1) pour recevoir une piece
     *
     * pour savoir si la piece est posee sur la bonne zone on utilisera les noms des objets (nom cellule == nom piece alors bonne réponse)
     *
     * attention pour l'instant cette fonction créé des objets, donc il ne faut pas l'appeller tel-quel quand on resize
     * la fenêtre ... sinon on entasse des objets les uns au dessus des autres
     */

    QTextEdit* textEdit = new QTextEdit(m_texte->abeGetText());
    QTextCursor curseur(textEdit->document());

    m_config->beginGroup("texteATrous");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    m_pointSize = qMax(8.0,m_config->value("fontSize",24).toInt()*1.1*ratio);
    QFont fonteUtilisee(abeApp->font().family(),m_pointSize,abeApp->font().weight(),abeApp->font().italic());
    int debutTexteAfficher = m_config->value("marqueurDebutTexte").toInt();
    int finTexteAfficher   = m_config->value("marqueurFinTexte").toInt();

    //En cas de pb on se rabat sur la taille du texte
    if(finTexteAfficher > textEdit->toPlainText().size())
        finTexteAfficher = textEdit->toPlainText().size();

    m_config->endArray();
    m_config->endGroup();

    QList< QPair<int,int> > tableauDeTrous = createHoleInText();
    if(m_localDebug) {
        qDebug()<<"@é@ tableauDeTrous"<<tableauDeTrous;
    }

    if (tableauDeTrous.isEmpty())
    {
        m_messageBox->abeMessageBoxUpdate(trUtf8("Exercice absent du module"),trUtf8("Impossible d'accéder à un tableau de trous cohérent"));
        return;
    }

    //Cherche le mot le plus long pour calculer la taille de ce mot et faire le nécessaire pour que tous les trous soient de la même taille
    int longueurMotMax = 0;
    for(int i = 0; i < tableauDeTrous.count(); i++)
    {
        int tailleMot = tableauDeTrous[i].second - tableauDeTrous[i].first;
        if(tailleMot > longueurMotMax)
            longueurMotMax = tailleMot;
    }

    if (m_localDebug) qDebug()<<"Le trou le plus long mesurera "<<longueurMotMax<<" lettres,";
    //Et on demande a FontMetrics la place que prendrait ce mot
    //m etant le mot le plus long ?
    QString m("w");
    QString lemot(m.repeated(longueurMotMax));
    QFontMetrics fm(fonteUtilisee);
    int pixelsWideMotMax = fm.boundingRect(lemot).width();
    int pixelsHigh = fm.boundingRect(lemot).height();

    if (m_localDebug) qDebug()<<"C'est a dire"<<pixelsWideMotMax<<" pixels.";

    QPointF positionMarge(50,m_aireDeJeu->height()-60);
    int espaceEntreEtiquettes = 2;
    int margeInterneEtiquette = 5;
    int espaceEntreLignes     = m_pointSize;

    /* QPair<int,int> dernierTrou = tableauDeTrous.last();*/

    bool finDesTrous = false;

    /* Je vais démarrer à la position indiquée dans le module comme marqueur de début de texte, lue dans le fichier de conf sous "marqueurDebutTexte" */
    int indice = debutTexteAfficher;
    curseur.setPosition(indice,QTextCursor::MoveAnchor);
    /* Je cherche dans lequel des mots du texte je suis à cet indice-là */
    int indiceMot = findWordWhereIndex(indice,0);
    /* Je cherche quel est le prochain trou à cet indice-là */
    int indiceTrou = findHoleWhereIndex(indice,tableauDeTrous);

    /* Là je gérais le fait que les retours charriot n'étaient pas indexés dans m_listeSuperMots mais je fais autrement maintenant */
//    QList<int> retoursCharriot;
//    int precedent = 0;
//    QMapIterator <int, QPair<QString, QPair<int, int> > > iter(m_listeSuperMots);
//    while(iter.hasNext())
//    {
//        iter.next();
//        if(iter.key() != precedent+1)
//        {
//            retoursCharriot << precedent+1;
//        }
//        precedent = iter.key();
//    }
//    retoursCharriot.removeFirst();

    bool wordHasChanged = false;
    while (indice <= finTexteAfficher )
    {
        /* Tant que je n'ai pas atteint le prochain trou (ou la fin du texte à afficher) */
        while ((indice < tableauDeTrous.at(indiceTrou).first || finDesTrous) && indice <= finTexteAfficher)
        {
            wordHasChanged=false;
            if (indice < m_listeSuperMots[indiceMot].second.first)
            {
                if(m_localDebug) {
                    qDebug()<<"Hors mot : Indice "<<indice<<" --> "<<curseur.selectedText();
                }
                //Je ne suis ni dans un trou ni dans un mot
            }
            else if (indice == m_listeSuperMots[indiceMot].second.first)
            {
                //Je suis au début d'un mot ou d'un morceau de mot
                curseur.setPosition(indice, QTextCursor::MoveAnchor);
                if(m_localDebug) {
                    qDebug()<<"Début mot : Indice "<<indice<<" --> "<<curseur.selectedText();
                }
            }
            else
            {
                /* Je suis à l'intérieur d'un mot ou d'un morceau mot */
                curseur.setPosition(indice, QTextCursor::KeepAnchor);
                if(m_localDebug) {
                    qDebug()<<indiceMot;
                    qDebug()<<"Prochain trou :"<<indiceTrou<< tableauDeTrous.at(indiceTrou);
                    qDebug()<<"A l'intérieur d'un mot : Indice "<<indice<<" --> "<<curseur.selectedText();
                }
            }
            if (indice >= m_listeSuperMots[indiceMot].second.second)
            {//ça devrait être "==" mais ">=" blinde le saut d'index
                if(m_localDebug) {
                    qDebug()<<" ## j'ai fini "<<m_listeSuperMots[indiceMot].first;
                }
                QString contenuMot;
                if(m_listeSuperMots[indiceMot].first == "retour_a_la_ligne"){
                    contenuMot = "retour_a_la_ligne";
                }
                else{
                    contenuMot = curseur.selectedText();
                }
                int pixelsWide = fm.boundingRect(contenuMot).width();
                int pixelsHigh = fm.boundingRect(contenuMot).height();
                if (wordHasChanged)
                {
                    //                    AbulEduCelluleV1(contenuMot,m_listeSuperMots[indiceMot].second.first)// --> et là j'ai un problème, puisque il aurait fallu garder le bon endroit qui n'est pas bon
                    //if (m_localDebug) qDebug()<<trUtf8("Création d'une cellule (cas 1) - mot à garder - avec ")<<contenuMot<<" en "<<m_listeSuperMots[indiceMot].second.first;
                    AbulEduCelluleV1 *cellulePleine = new AbulEduCelluleV1(QSize(pixelsWide+margeInterneEtiquette+5,pixelsHigh+espaceEntreLignes));
                    cellulePleine->setProperty("mitoyen","non");
//                    if(retoursCharriot.contains(indiceMot))
//                    {
//                        cellulePleine->setObjectName("retour_a_la_ligne");

//                    }
//                    else
//                    {
                        cellulePleine->setObjectName(contenuMot);
//                    }
                    if(m_localDebug) {
                        qDebug()<<"1 Creation d'une cellule pleine avec "<<contenuMot;
                    }
                    cellulePleine->abeCelluleSetTexte(contenuMot);
                    cellulePleine->abeCelluleSetTexteAlignement(Qt::AlignVCenter);
                    cellulePleine->abeCelluleSetFont(fonteUtilisee);
                    cellulePleine->abeCelluleAcceptePiece(false);
                    cellulePleine->setAcceptDrops(false);
                    cellulePleine->setParent(m_aireDeJeu->scene());
                    cellulePleine->abeCelluleSetPen(QPen(QColor(Qt::transparent), 0, Qt::SolidLine));
                    if ((!contenuMot.trimmed().isEmpty()) || cellulePleine->objectName() == "retour_a_la_ligne")
                    {
                        m_listeCellulesPleines.append(cellulePleine);
                        m_listeCellules.append(cellulePleine);
                    }
                }
                else
                {
                    //                    AbulEduCelluleV1(contenuMot,finDernierTrou)// --> et là j'ai un problème, puisque il aurait fallu garder le bon endroit qui n'est pas bon
                    //if (m_localDebug) qDebug()<<trUtf8("Création d'une cellule (cas 2) - mot à garder - avec ")<<contenuMot;//<<" en "<<finDernierTrou;
                    AbulEduCelluleV1 *cellulePleine = new AbulEduCelluleV1(QSize(pixelsWide+margeInterneEtiquette+5,pixelsHigh+espaceEntreLignes));
                    cellulePleine->abeCelluleSetFont(fonteUtilisee);
                    cellulePleine->setProperty("mitoyen","fin");
//                    if(retoursCharriot.contains(indiceMot))
//                    {
//                        cellulePleine->setObjectName("retour_a_la_ligne");

//                    }
//                    else
//                    {
                        cellulePleine->setObjectName(contenuMot);
//                    }
                    //                    cellulePleine->setObjectName(contenuMot);
                    if(m_localDebug) {
                        qDebug()<<"2 Creation d'une cellule pleine contenant "<<contenuMot<<" nommee "<<cellulePleine->objectName();
                    }
                    cellulePleine->setAcceptDrops(false);

                    cellulePleine->abeCelluleSetTexte(contenuMot);
                    cellulePleine->abeCelluleSetTexteAlignement(Qt::AlignVCenter);
                    //                    cellulePleine->abeCelluleSetTexteAlignement(Qt::AlignRight);
                    cellulePleine->abeCelluleAcceptePiece(false);
                    cellulePleine->setParent(m_aireDeJeu->scene());
                    cellulePleine->abeCelluleSetPen(QPen(QColor(Qt::transparent), 0, Qt::SolidLine));
                    if ((!contenuMot.trimmed().isEmpty()) || cellulePleine->objectName() == "retour_a_la_ligne")
                    {
                        m_listeCellulesPleines.append(cellulePleine);
                        m_listeCellules.append(cellulePleine);
                    }
                }
                indiceMot++;
                if(m_localDebug) {
                    qDebug()<<"Indice "<<indice<<" - Le prochain mot est "<<m_listeSuperMots[indiceMot].first<<"{"<<indiceMot<<"}";
                }
                curseur.setPosition(indice, QTextCursor::MoveAnchor);
                wordHasChanged = true;
            }
            indice++;
        }
        if (m_localDebug) qDebug()<<wordHasChanged;
        //Je suis sorti de mon while, c'est que j'ai atteint le premier trou
        //SI JE N'AI PAS FINI LE MOT EN COURS, IL FAUT QUE JE FASSE UNE CELLULE QUI AFFICHE LE DEBUT DE MOT
        if (!wordHasChanged)
        {
            curseur.setPosition(indice, QTextCursor::KeepAnchor);
            QString contenuMot = curseur.selectedText();
            int pixelsWide = fm.boundingRect(contenuMot).width();
            int pixelsHigh = fm.boundingRect(contenuMot).height();
            //if (m_localDebug) qDebug()<<trUtf8("Si contenuMot pas vide, création d'une cellule  (cas 3)- mot à garder - avec ")<<contenuMot<<" en "<<m_listeSuperMots[indiceMot].second.first;
            AbulEduCelluleV1 *cellulePleine = new AbulEduCelluleV1(QSize(pixelsWide+margeInterneEtiquette+5,pixelsHigh+espaceEntreLignes));
            cellulePleine->abeCelluleSetFont(fonteUtilisee);
            cellulePleine->setProperty("mitoyen","debut");
//            if(retoursCharriot.contains(indiceMot))
//            {
//                cellulePleine->setObjectName("retour_a_la_ligne");

//            }
//            else
//            {
                cellulePleine->setObjectName(contenuMot);
//            }
            if(m_localDebug) {
                qDebug()<<"3 Creation d'une cellule pleine avec "<<contenuMot;
            }
            cellulePleine->setAcceptDrops(false);

            cellulePleine->abeCelluleSetTexte(contenuMot);
            cellulePleine->abeCelluleSetTexteAlignement(Qt::AlignVCenter);
            cellulePleine->abeCelluleAcceptePiece(false);
            cellulePleine->setParent(m_aireDeJeu->scene());
            cellulePleine->abeCelluleSetPen(QPen(QColor(Qt::transparent), 0, Qt::SolidLine));
            if ((!contenuMot.trimmed().isEmpty()) || cellulePleine->objectName() == "retour_a_la_ligne")
            {
                m_listeCellulesPleines.append(cellulePleine);
                m_listeCellules.append(cellulePleine);
            }
        }
        //        AbulEduCelluleV1(contenuMot,m_listeSuperMots[indiceMot].second.first);
        if (indice >= m_listeSuperMots[indiceMot].second.second)
        {
            indiceMot++;
            if(m_localDebug) {
                qDebug()<<"Indice "<<indice<<" - Je rentre dans "<<m_listeSuperMots[indiceMot].first<<"("<<indiceMot<<")";
            }

            curseur.setPosition(indice, QTextCursor::MoveAnchor);
        }
        curseur.setPosition(indice, QTextCursor::MoveAnchor);
        while (indice < tableauDeTrous.at(indiceTrou).second)
        {
            if (indice >= m_listeSuperMots[indiceMot].second.second)
            {
                indiceMot++;
                if(m_localDebug) {
                    qDebug()<<"Indice "<<indice<<" - Je rentre dans "<<m_listeSuperMots[indiceMot].first<<"["<<indiceMot<<"]";
                }

                wordHasChanged = true;
            }
            curseur.setPosition(indice, QTextCursor::KeepAnchor);
            indice++;
        }
        curseur.setPosition(indice, QTextCursor::KeepAnchor);
        QString contenuTrou = curseur.selectedText();
        if (!contenuTrou.isEmpty())
        {
            AbulEduPieceV1 *p = new AbulEduPieceV1(QSize(pixelsWideMotMax+margeInterneEtiquette-5,pixelsHigh+espaceEntreLignes-5));
            p->setObjectName(contenuTrou);
            p->abePieceSetAcceptWordWrap(false);
            if(m_localDebug) {
                qDebug()<<"Creation d'une pièce avec "<<contenuTrou;
            }
            p->setParent(m_aireDeJeu->scene());
            int largeurPiece = p->abePieceGetSize().width();
            if (largeurPiece > m_maxPieceWidth)
            {
                m_maxPieceWidth = largeurPiece;
            }
            //            p->abePieceSetPositionOriginale(positionMarge);
            p->abePieceSetFont(fonteUtilisee);
            p->abePieceSetTextePen(QPen(Qt::white));
            p->abePieceSetTexte(contenuTrou);
            p->abePieceSetTexteAlignement(Qt::AlignCenter);
            p->abePieceSetPen(Qt::NoPen);
            p->abePieceSetRadius(0,0);
            p->abePieceSetBrush(QColor(202,103,0,179));
            m_aireDeJeu->scene()->addItem(p);
            p->show();

            AbulEduCelluleV1 *celluleVide = new AbulEduCelluleV1(QSize(pixelsWideMotMax+margeInterneEtiquette,pixelsHigh+espaceEntreLignes));
            celluleVide->setObjectName(contenuTrou);
            if(m_localDebug) {
                qDebug()<<"Creation d'une cellule VIDE pour "<<contenuTrou;
            }
            connect(celluleVide, SIGNAL(signalAbeCelluleDropResult(abe::ABE_EVAL)), this, SLOT(updateDropResultReceived(abe::ABE_EVAL)));
            celluleVide->setProperty("mitoyen","debut");
            celluleVide->abeCelluleSetPen(QPen(QColor("#c38b6b"), 2, Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
            celluleVide->setAcceptedMouseButtons(Qt::NoButton);
            celluleVide->setFlag(QGraphicsItem::ItemIsMovable,false);
            celluleVide->setAcceptHoverEvents(false);
            celluleVide->abeCelluleSetInitialBrush(QBrush());
            m_listeCellulesVides.append(celluleVide);
            m_listeCellules.append(celluleVide);
            m_listePieces.append(p);
            if (indice >= m_listeSuperMots[indiceMot].second.second)
            {
                indiceMot++;
            }
            positionMarge.setX(positionMarge.x()+pixelsWideMotMax+espaceEntreEtiquettes+margeInterneEtiquette);
            //        AbulEduPieceV1(contenuTrou);
        }
        curseur.setPosition(indice, QTextCursor::MoveAnchor);
        //        finDernierTrou = indice;
        if (indiceTrou >= tableauDeTrous.size()-1)
        {
            finDesTrous = true;
        }
        else
        {
            indiceTrou++;
            if(m_localDebug) {
                qDebug()<<"Indice "<<indice<<" - Le prochain trou est "<<indiceTrou<<tableauDeTrous.at(indiceTrou);
            }

        }
        if (!contenuTrou.endsWith("'"))indice++;
    }
    for (int i = 0;i<m_listeCellulesVides.size();i++)
    {
        for(int j=0;j<m_listePieces.size();j++)
        {
            connect(m_listePieces[j], SIGNAL(signalAbePieceSurvole(QGraphicsItem*)),m_listeCellulesVides[i], SLOT(slotAbeCellulePieceSurvol(QGraphicsItem*)));
        }
    }
    m_listeSuperMots = sauvegardeListe;
    placeMots();
    placeCellules();
}

void ExerciceClosure::placeCellules()
{
    int decalageInitial = 50;
    QRect positionPiece(decalageInitial,0,0,0);
    int positionXmaximale = m_aireDeJeu->width() - m_maxCelluleWidth - m_maxPieceWidth -20;
    int espaceEntreEtiquettes = 2;
    int espaceEntreLignes     = m_pointSize;

    QRectF nouvelleScene(m_aireDeJeu->sceneRect().x(),m_aireDeJeu->sceneRect().y(),m_aireDeJeu->sceneRect().width(),10);
    int hauteur = 10;
    foreach(AbulEduCelluleV1* cellule,m_listeCellules)
    {
        if((positionPiece.x()+cellule->abeCelluleGetSize().width()) > positionXmaximale || cellule->objectName() == "retour_a_la_ligne")
        {
            if(cellule->objectName() == "retour_a_la_ligne"){
                cellule->abeCelluleSetTexte("");
                cellule->abeCelluleSetSize(QSizeF(0,cellule->abeCelluleGetSize().height()));
            }
            positionPiece.setX(decalageInitial);
            positionPiece.setY(positionPiece.y()+cellule->abeCelluleGetSize().height()+espaceEntreLignes);
            positionPiece.setWidth(cellule->abeCelluleGetSize().width());
            positionPiece.setHeight(cellule->abeCelluleGetSize().height());
            if (m_localDebug)
            {
                //                qDebug()<<"Ajout de ligne pour "<<cellule->objectName();
                //                qDebug()<<m_aireDeJeu->sceneRect();
            }
            hauteur = positionPiece.y()+cellule->abeCelluleGetSize().height()+espaceEntreLignes;
        }
        //        cellule->setParent(frameCellules);
//        cellule->setParent(m_aireDeJeu->scene());
        m_aireDeJeu->scene()->addItem(cellule);
        m_aireDeJeu->updateSceneRect(nouvelleScene);
        cellule->setPos(positionPiece.x()+espaceEntreEtiquettes,positionPiece.y());
        positionPiece.setX(positionPiece.x()+cellule->abeCelluleGetSize().width());
    }
    nouvelleScene = QRectF(m_aireDeJeu->sceneRect().x(),m_aireDeJeu->sceneRect().y(),m_aireDeJeu->sceneRect().width(),hauteur);
    m_aireDeJeu->setSceneRect(nouvelleScene);
}

void ExerciceClosure::placeMots()
{
    m_placesPourPieces.clear();
    m_ordrePresentation.clear();
    /* Les mots sont placés dans des AbulEduPieceV1 */
    QList<AbulEduPieceV1*> copieListeItem = m_aireDeJeu->scene()->findChildren<AbulEduPieceV1*>();
    /* Mélange des items */
    QList<AbulEduPieceV1 *>listeItem;
    while(copieListeItem.size() > 0)
    {
        int hasard = qrand()%copieListeItem.size();
        listeItem.append(copieListeItem.takeAt(hasard));
    }

    int x = m_aireDeJeu->width() - m_maxPieceWidth -20;
    /* Je vais mettre la première pièce à ce qu'était avant la deuxième position, pour garder cette place pour le bouton up */
    int y = 0;
    int yMove = 0;
    int yDown = 0;
    int nbMove = 0;
    int gapLabels = 10*abeApp->getAbeApplicationDecorRatio();
    if(!listeItem.isEmpty()){
        int w = listeItem.first()->abePieceGetSize().width();
        yMove = gapLabels + listeItem.first()->abePieceGetSize().height();
        while(nbMove*yMove-gapLabels < m_aireDeJeu->height() && nbMove < listeItem.size()-1){
            nbMove++;
        }
        yDown = yMove*(nbMove - 2);
        if(yMove * listeItem.size() -gapLabels < m_aireDeJeu->height()){
            /* On est dans le cas où on n'a pas besoin de boutons Up et Down */
        }
        else {
            /* On est dans le cas où on a besoin de boutons Up et Down */
            y = yMove;
            qDebug()<<"gap + piece "<<yMove;
            QPixmap resizedUp = QPixmap(":/abuledudisplaypieces/arrowUp").scaledToHeight(QPixmap(":/abuledudisplaypieces/arrowUp").height()*abeApp->getAbeApplicationDecorRatio(),Qt::SmoothTransformation);
            m_upButton = new AbulEduPieceButtonV1(resizedUp.size());
            int decalagePieceBnt = (w-m_upButton->abePieceGetSize().width())/2;
            m_upButton->setObjectName("Up");
            m_upButton->setParent(m_aireDeJeu->scene());
            m_upButton->abePieceSetPositionOriginale(x+decalagePieceBnt,0);
            m_upButton->abePieceSetBrush(resizedUp);
            m_aireDeJeu->scene()->addItem(m_upButton);
            connect(m_upButton, SIGNAL(signalAbePieceCliquee(AbulEduPieceV1*)),this, SLOT(slotMoveDisplayPieces(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_upButton, SIGNAL(signalAbePieceButtonEnterHover(AbulEduPieceV1*)),this, SLOT(slotPaintHoverPiece(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_upButton, SIGNAL(signalAbePieceButtonLeaveHover(AbulEduPieceV1*)),this, SLOT(slotUnPaintHoverPiece(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_upButton, SIGNAL(signalPiecePressed(AbulEduPieceV1*)),this, SLOT(slotPaintPressedPiece(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_upButton, SIGNAL(signalPieceReleased(AbulEduPieceV1*)),this, SLOT(slotUnPaintPressedPiece(AbulEduPieceV1*)),Qt::UniqueConnection);
            slotPaintDisabledPiece(m_upButton);

            QPixmap resizedDown = QPixmap(":/abuledudisplaypieces/arrowDown").scaledToHeight(QPixmap(":/abuledudisplaypieces/arrowDown").height()*abeApp->getAbeApplicationDecorRatio(),Qt::SmoothTransformation);
            m_downButton = new AbulEduPieceButtonV1(QSizeF(resizedDown.size()));
            m_downButton->setObjectName("Down");
            m_downButton->setParent(m_aireDeJeu->scene());
            m_downButton->abePieceSetPositionOriginale(x+decalagePieceBnt,yDown);
            m_downButton->abePieceSetBrush(resizedDown);
            m_aireDeJeu->scene()->addItem(m_downButton);
            connect(m_downButton, SIGNAL(signalAbePieceCliquee(AbulEduPieceV1*)),this, SLOT(slotMoveDisplayPieces(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_downButton, SIGNAL(signalAbePieceButtonEnterHover(AbulEduPieceV1*)),this, SLOT(slotPaintHoverPiece(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_downButton, SIGNAL(signalAbePieceButtonLeaveHover(AbulEduPieceV1*)),this, SLOT(slotUnPaintHoverPiece(AbulEduPieceV1*)),Qt::UniqueConnection);/* Je cache les pièces dont la position est supérieure à 6 */
            connect(m_downButton, SIGNAL(signalPiecePressed(AbulEduPieceV1*)),this, SLOT(slotPaintPressedPiece(AbulEduPieceV1*)),Qt::UniqueConnection);
            connect(m_downButton, SIGNAL(signalPieceReleased(AbulEduPieceV1*)),this, SLOT(slotUnPaintPressedPiece(AbulEduPieceV1*)),Qt::UniqueConnection);

        }
    }
    QSizeF pieceSize;
    m_precedentDebutPlaces = 0;
    for (int i=0;i<listeItem.size();i++)
    {
        AbulEduPieceV1* pieceCourante = static_cast<AbulEduPieceV1 *>(listeItem[i]);
        connect(pieceCourante, SIGNAL(xChanged()),this, SLOT(slotLimitePieceX()),Qt::UniqueConnection);
        pieceCourante->abePieceSetPositionOriginale(QPointF(x,y));
        m_placesPourPieces << QPair<QPointF,AbulEduPieceV1*>(pieceCourante->pos(),pieceCourante);
        y = y +gapLabels + pieceCourante->abePieceGetSize().height();
        m_ordrePresentation << pieceCourante->objectName();
        pieceCourante->setProperty("numOrdre",i);//Je pense que c'est inutile
        connect(pieceCourante,SIGNAL(signalAbePieceDeposee(QString,int)),this,SLOT(changeOriginalPos(QString,int)));
        pieceSize = pieceCourante->abePieceGetSize();
    }

    if(m_upButton > 0){
    for(int i = nbMove-3;i < m_placesPourPieces.size();i++){
        qDebug()<<" je cache la pièce  : "<< m_placesPourPieces.at(i).second->abePieceGetTexte();
        m_placesPourPieces.at(i).second->setVisible(false);
    }
    }
    qDebug()<<" Hauteur de l'aire de jeu : "<<m_aireDeJeu->height();
    m_aireDeJeu->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void ExerciceClosure::movePieceOrdinate(int y)
{
    if (m_localDebug)
    {
        qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<< " de "<<y;
        //        qDebug()<<m_listePiecesPlacees;
    }

    QList<QGraphicsItem *>listeItem = m_aireDeJeu->scene()->items(); // Récupère tous les items présents sur l'aire de jeu

    //Si un graphicsitem de la scène est une pièce non encore placée dans une cellule, je la bouge de y - m_precedentDebutPlaces
    foreach(QGraphicsItem *item, listeItem)
    {
        if(item->type()==AbulEduPieceV1::AbulEduPieceType)
        {
            AbulEduPieceV1* itemPiece = static_cast<AbulEduPieceV1 *>(item);
            if (!m_listePiecesPlacees.contains(itemPiece))
            {
                itemPiece->abePieceSetPositionOriginale(itemPiece->abePieceGetPositionOriginale().x(),itemPiece->abePieceGetPositionOriginale().y() + y-m_precedentDebutPlaces);
            }
        }
    }

    // Je corrige dans la liste m_placesPourPieces les positions de + ou - y
    QList<QPair<QPointF,AbulEduPieceV1*> >nouvelleListe;
    for (int j=0;j<m_placesPourPieces.size();j++)
    {
        nouvelleListe << QPair<QPointF,AbulEduPieceV1*>(QPointF(m_placesPourPieces.at(j).first.x(),m_placesPourPieces.at(j).first.y()+y-m_precedentDebutPlaces),m_placesPourPieces.at(j).second);
    }
    m_placesPourPieces = nouvelleListe;
    m_precedentDebutPlaces = y;
}

void ExerciceClosure::changeOriginalPos(QString nom, int numCellule)
{
    Q_UNUSED(nom);
    //    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<"::"<<nom<<"::"<<numCellule;
    AbulEduPieceV1* piece = static_cast<AbulEduPieceV1 *>(sender());
    int i = 0;
    bool trouve = false;
    while( trouve == false && i<m_placesPourPieces.size())//indiquer si elle n'en faisait pas partie ?!
    {
        if (piece == m_placesPourPieces.at(i).second)
        {
            trouve = true;
            i--;
        }
        i++;
    }
    if (trouve == true)
    {
        if (m_localDebug) qDebug() << "La pièce bougée était en position "<<i;
    }
    else
    {
        i=-1;
        if (m_localDebug) qDebug() << "La pièce bougée n'était pas à droite, je mets i = "<<i;

    }
    if (numCellule > -1)
    {
        if (i > -1)
        {
            // ici numCellule > -1 et i >-1 , on a donc mis dans une cellule vide une pièce venant de la liste de droite
            qDebug()<<"Cas 1 : piece de droite "<<i<<" dans cellule vide";
            QList<QPair<QPointF,AbulEduPieceV1*> >nouvelleListe;
            for(int z=0;z<i;z++)
            {
                nouvelleListe << m_placesPourPieces[z];
            }
            for(int k=i;k<m_placesPourPieces.size()-1;k++)
            {
                m_placesPourPieces[k+1].second->abePieceSetPositionOriginale(m_placesPourPieces[k].first);
                nouvelleListe << QPair<QPointF,AbulEduPieceV1*>(m_placesPourPieces[k].first,m_placesPourPieces[k+1].second);
                qDebug()<<"Pour k="<<k<<", on ajoute "<<QPair<QPointF,AbulEduPieceV1*>(m_placesPourPieces[k].first,m_placesPourPieces[k+1].second);
                m_placesPourPieces[k+1].second->update();
            }
            m_placesPourPieces = nouvelleListe;
            m_listePiecesPlacees.append(piece);
        }
        else
        {
            qDebug()<<"Cas 2 : piece changée de cellule vide";
            //            piece->abePieceSetPositionOriginale(QPointF(m_placesPourPieces.last().first+QPointF(0,10+piece->abePieceGetSize().height())));
            //            m_placesPourPieces << QPair<QPointF,AbulEduPieceV1*>(piece->abePieceGetPositionOriginale(),piece);
            //            foreach (AbulEduCelluleV1* cell,m_listeCellulesVides)
            //            {
            //                if (cell->abeCelluleHasPiece() == piece) cell->abeCelluleSetPiece(0);
            //            }

            // ici numCellule > -1 et i  = -1 , on a donc mis dans une cellule vide une pièce qui était déjà dans la scène
        }
    }
    else
    {
        if (i > -1)
        {
            qDebug()<<"Cas 3 : piece de droite retour à droite";
            foreach (AbulEduCelluleV1* cell,m_listeCellulesVides)
            {
                if (cell->abeCelluleHasPiece() == piece) cell->abeCelluleSetPiece(0);
            }
            // ici numCellule = -1 et i >-1 , on a donc laché à un endroit non prévu pour une pièce venant de la liste de droite
        }
        else
        {
            qDebug()<<"Cas 4 : piece de la scene retour à droite";
            if (m_placesPourPieces.size() > 0)
            {
                qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__;

                piece->abePieceSetPositionOriginale(QPointF(m_placesPourPieces.last().first+QPointF(0,10+piece->abePieceGetSize().height())));
                //                piece->setPos(QPointF(m_placesPourPieces.last().first+QPointF(0,10+piece->abePieceGetSize().height())));
                //                piece->abePieceChangePositionOriginale(piece->pos());
                qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__<<" :: "<<m_placesPourPieces.last().first;

            }
            else
            {
                piece->abePieceSetPositionOriginale(QPointF(m_aireDeJeu->width() - m_maxPieceWidth -20,m_precedentDebutPlaces));
            }

            m_placesPourPieces << QPair<QPointF,AbulEduPieceV1*>(piece->abePieceGetPositionOriginale(),piece);
            foreach (AbulEduCelluleV1* cell,m_listeCellulesVides)
            {
                if (cell->abeCelluleHasPiece() == piece) cell->abeCelluleSetPiece(0);
            }
            m_listePiecesPlacees.removeOne(piece);
            // ici numCellule = -1 et i = -1 , on a donc laché à un endroit non prévu pour une pièce qui était déjà dans la scène
        }
    }
    if(m_localDebug) {
        qDebug()<<m_placesPourPieces.size()<<" places à droite";
        qDebug()<<m_listePiecesPlacees.size()<<"pieces placees";
    }
    //    qDebug()<<" ************************************************************************************ ";
    //    foreach (AbulEduCelluleV1* cell,m_listeCellulesVides)
    //    {
    //        qDebug()<<"Cellule "<<cell->objectName()<<" avec "<<cell->abeCelluleHasPiece();
    //    }
}

void ExerciceClosure::clearScene()
{
    m_listeCellulesVides.clear();
    m_listeCellulesPleines.clear();
    m_listeCellules.clear();
    m_listePieces.clear();
    m_aireDeJeu->scene()->clear();
}

void ExerciceClosure::redimensionneConsigne()
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

void ExerciceClosure::redimensionneBilan()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
#ifdef Q_WS_MAC
    m_parent->repaint();
#endif
}

int ExerciceClosure::findWordWhereIndex(int index, int indexDepart)
{
    int i=indexDepart;
    while (i<m_listeSuperMots.count())
    {
        if (m_listeSuperMots.value(i).second.second > index)
        {
            return m_listeSuperMots.key(m_listeSuperMots.value(i));
        }
        i++;
    }
    return -1;
}

int ExerciceClosure::findHoleWhereIndex(int index, QList<QPair<int, int> > listeTrous)
{
    int i=0;
    while(i<listeTrous.count())
    {
        if (listeTrous.value(i).second > index)
        {
            return i;
        }
        i++;
    }
    return i;
}

QList<QPair<int, int> > ExerciceClosure::createHoleInText()
{

    QList< QPair<int,int> > tableauDeTrous;
    m_config->beginGroup("texteATrous");
    m_config->beginReadArray("exercice");
    m_config->setArrayIndex(m_numExercice);
    int debutTexteAfficher = m_config->value("marqueurDebutTexte").toInt();
    int finTexteAfficher   = m_config->value("marqueurFinTexte").toInt();

    if (m_config->value("modeManuel").toBool())
        /* -------------- Cas n°1 : mode manuel (une liste existe dans le fichier module.conf) -------------- */
    {
        if (m_localDebug) qDebug()<<"Mode Manuel";
        QStringList tabTrous(m_config->value("listeTrous").toString().split(";",QString::SkipEmptyParts));
        if(tabTrous.size() > 1) {
            //On deserialise, deux par deux
            for(int i = 0; i < tabTrous.size()-1; i+=2) {
                tableauDeTrous.append(QPair<int,int>(tabTrous.at(i).toInt(),tabTrous.at(i+1).toInt()));
            }
        }
        else
        {
            m_messageBox->abeMessageBoxUpdate(trUtf8("Erreur"),trUtf8("Erreur de lecture de la liste des trous à faire dans le texte !"));
//            AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Erreur"),trUtf8("Erreur de lecture de la liste des trous à faire dans le texte !"));
//            messageBox->show();
        }
        if (m_localDebug)qDebug()<<"---------------- Manuel --------------------- "<<tableauDeTrous;

        while (tableauDeTrous.size() > m_config->value("nombreTrousAAfficher").toInt())
        {
            int alea = rand() % (tableauDeTrous.size());
            tableauDeTrous.takeAt(alea);
        }
    }
    /* --------------------------------------------------------------------------------------------------- */
    else
    {
        if (m_config->value("modeIntelligent").toBool())
            /* ------ Cas n°2 : mode intelligent (tirage aléatoire mais présence dans mot_aller_cycle3.csv) ------ */
        {
            if (m_localDebug) qDebug()<<"Mode Intelligent";
            bool fini = false;
            QString fic(QString("%1/data/%2").arg(m_config->fileName().remove("/conf/module.conf")).arg("dictionnaires/mot_aller_cycle3.csv"));
            AbulEduTextV1* motsOutilsAttributs = new AbulEduTextV1(fic);
            int nat = m_config->value("indexNature").toInt();//qstring
            int niv = m_config->value("indexNiveau").toInt();//abelevel
            QStringList motsOutilsAttributsTrimmed;
            foreach(QString ligne,motsOutilsAttributs->abetextParagraphes().values())
            {
                if (!ligne.trimmed().isEmpty())
                {
                    motsOutilsAttributsTrimmed << ligne;
                }
            }

            QStringList motsOutilsNatureNiveauDonne = AbulEduOutilsV1::abeoutilsCreerListeMots(motsOutilsAttributsTrimmed,-1,abe::ABE_LEVEL(niv), abe::ABE_NATURE(nat),0);
            int nombreTrous = 0;
            QList<int> indexExistants = m_listeSuperMots.keys();

            int indexTirageEric;

            QList<int> indexUtilises;
            int indexTirage;
            if (indexExistants.size() < m_config->value("nombreTrousAAfficher").toInt()) {
                m_messageBox->abeMessageBoxUpdate(trUtf8("Accès impossible à l'exercice"),trUtf8("Plus de trous demandés que de mots dans l'exercice..."));
//                AbulEduMessageBoxV1* alerte = new AbulEduMessageBoxV1(trUtf8("Accès impossible à l'exercice"),trUtf8("Plus de trous demandés que de mots dans l'exercice..."));
//                alerte->show();
                return QList<QPair<int, int> >();
            }
            while (nombreTrous < m_config->value("nombreTrousAAfficher").toInt() && !fini)
            {
                indexTirage = rand() % (indexExistants.count());
                indexTirageEric = indexExistants.takeAt(indexTirage);

                if (m_localDebug) qDebug()<<"Premiere boucle tour "<<indexUtilises.count()+1 << " tirage aleatoire de " << indexTirage << " correspondance a " << indexTirageEric;
                if (!indexUtilises.contains(indexTirageEric)  && motsOutilsNatureNiveauDonne.contains(m_listeSuperMots.value(indexTirageEric).first))
                {
                    if (m_listeSuperMots.value(indexTirageEric).second.second > debutTexteAfficher
                            && m_listeSuperMots.value(indexTirageEric).second.second < finTexteAfficher)
                    {
                        if (m_localDebug) qDebug()<<m_listeSuperMots.value(indexTirageEric).first<<" :: "<<m_listeSuperMots.value(indexTirageEric).second<<" ajoute";//le mot d'index "indexTirageEric" dans m_listeSuperMots
                        tableauDeTrous.append(m_listeSuperMots.value(indexTirageEric).second);
                        nombreTrous++;
                    }
                    indexUtilises<<indexTirageEric;
                }
                if(indexExistants.count() <= 0)
                {
                    fini = true;
                    QString msg = trUtf8("Le texte ne contient pas ")
                                    +QString::number(m_config->value("nombreTrousAAfficher").toInt())
                                    +trUtf8(" mots de type ")
                                    +abe::ABE_DONNENATURE().value(abe::ABE_NATURE(nat))
                                    +trUtf8(" devant être sûs à la fin du ")
                                    + abe::ABE_DONNENIVEAU().value(abe::ABE_LEVEL(niv));
                    m_messageBox->abeMessageBoxUpdate(trUtf8("Problème"),msg);
//                    AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Problème"),trUtf8("Le texte ne contient pas ")
//                                                                              +QString::number(m_config->value("nombreTrousAAfficher").toInt())
//                                                                              +" mots de type "
//                                                                              +abe::ABE_DONNENATURE().value(abe::ABE_NATURE(nat))
//                                                                              +trUtf8(" devant être sûs à la fin du ")
//                                                                              + abe::ABE_DONNENIVEAU().value(abe::ABE_LEVEL(niv)));
//                    messageBox->show();
                }
            }
        }
        else
            /* ------------------ Cas n°3 : mode automatique (tirage aléatoire de mots du texte) ------------------ */
        {
            qDebug()<<"Mode Automatique, pour "<<m_config->value("nombreTrousAAfficher").toInt()<<" mots sur un total de "<<m_listeSuperMots.size();
            tableauDeTrous = returnHolesFromAutomatic(m_listeSuperMots,m_config->value("nombreTrousAAfficher").toInt(),debutTexteAfficher, finTexteAfficher);
        }
    }
    qSort(tableauDeTrous.begin(),tableauDeTrous.end());
    if (m_localDebug) qDebug() << "Exercice "<<m_numExercice;
    if (m_localDebug) qDebug()<<" ---------------- ";
    if (m_localDebug) qDebug() << "Tableau de trous : "<<tableauDeTrous;
    if (m_localDebug) qDebug()<<" ---------------- ";
    m_config->endArray();
    m_config->endGroup();
    return tableauDeTrous;
}

QList< QPair<int,int> > ExerciceClosure::returnHolesFromAutomatic(QMap <int,QPair<QString,QPair<int,int> > > superMots, int holesNumber, int beginning, int end){
    QList< QPair<int,int> > holes;
    int nombreTrous = 0;
    QList<int> indexExistants = superMots.keys();
    QList<int> indexUtilises;
    int indexTirage;
    if (superMots.size() < holesNumber) {
        AbulEduMessageBoxV1::showInformation(trUtf8("Plus de trous demandés que de mots dans l'exercice..."),0,trUtf8("Accès impossible à l'exercice"));
        return QList<QPair<int, int> >();
    }
    while (nombreTrous < holesNumber)
    {
        //indexTirage = rand() % indexExistants.count();
        indexTirage = rand() % (indexExistants.count());
        if (!indexUtilises.contains(indexTirage)
                && superMots.value(indexTirage).first.simplified() != ":"
                && superMots.value(indexTirage).first.simplified() != ";"
                && superMots.value(indexTirage).first.simplified() != "!"
                && superMots.value(indexTirage).first.simplified() != "?"
                && superMots.value(indexTirage).first.simplified() != QString::fromUtf8("…"))
        {
            if (superMots.value(indexTirage).second.second > beginning
                    && superMots.value(indexTirage).second.second < end)
            {
                qDebug()<<superMots.value(indexTirage).first<<" :: "<<superMots.value(indexTirage).second<<" ajoute";//le mot d'index "indexTirage" dans superMots
                if (holes.isEmpty())
                {
                    holes.append(superMots.value(indexTirage).second);
                }
                else
                {
                    bool ajoute = false;
                    int j=0;
                    while (j<holes.count() && !ajoute)
                    {
                        if (holes[j].first > superMots.value(indexTirage).second.first)
                        {
                            holes.insert(j,superMots.value(indexTirage).second);
                            ajoute = true;
                        }
                        j++;
                    }
                    if (!ajoute)
                        holes.append(superMots.value(indexTirage).second);
                }
                nombreTrous++;
                indexUtilises<<indexTirage;
            }
        }
    }
    return holes;
}


void ExerciceClosure::slotMoveDisplayPieces(AbulEduPieceV1* piece)
{
    int movement = 0;
    int gapLabels = 10*abeApp->getAbeApplicationDecorRatio();
    if(piece->objectName() == "Up"){
        movement = 1;
    }
    else if(piece->objectName() == "Down"){
        movement = -1;
    }
    QList<QPair<QPointF,AbulEduPieceV1*> > nouvellesPlaces;
    for(int i=0;i<m_placesPourPieces.size();i++){
        QPointF deltaY(0,movement*(gapLabels+m_placesPourPieces.at(i).second->abePieceGetSize().height()));
        m_placesPourPieces.at(i).second->abePieceSetPositionOriginale(m_placesPourPieces.at(i).first+deltaY);
        nouvellesPlaces.append(QPair<QPointF,AbulEduPieceV1*>(m_placesPourPieces.at(i).second->abePieceGetPositionOriginale(),m_placesPourPieces.at(i).second));
        if(nouvellesPlaces.last().first.y() < m_upButton->pos().y()+gapLabels+nouvellesPlaces.last().second->abePieceGetSize().height()
            || m_placesPourPieces.at(i).second->y() >= m_downButton->y()){
            m_placesPourPieces.at(i).second->setVisible(false);
        }
        else {
            m_placesPourPieces.at(i).second->setVisible(true);
        }
    }
    m_placesPourPieces = nouvellesPlaces;
    for(int i = 0;i < m_placesPourPieces.size();i++){
        qDebug()<<"plus un";
    }

    if(m_placesPourPieces.size() > 0){
        if(m_placesPourPieces.first().first.y() >= m_upButton->y() + gapLabels + m_placesPourPieces.first().second->abePieceGetSize().height()){
            slotPaintDisabledPiece(m_upButton);
        }
        else {
            slotUnPaintDisabledPiece(m_upButton);
        }
        if(m_placesPourPieces.last().first.y() < m_downButton->y()){
            slotPaintDisabledPiece(m_downButton);
        }
        else {
            slotUnPaintDisabledPiece(m_downButton);
        }
    }
    qDebug()<<"bouton Up en "<<m_upButton->pos().y();
    for(int p=0;p<m_placesPourPieces.size();p++){
        qDebug()<<"piece "<<m_placesPourPieces.at(p).second->objectName()<<" en "<<m_placesPourPieces.at(p).second->pos().y()<<" ("<<m_placesPourPieces.at(p).first.y()<<")";
    }
    qDebug()<<"bouton Down en "<<m_downButton->pos().y();
}

void ExerciceClosure::slotPaintHoverPiece(AbulEduPieceV1 *p)
{
    if(p->isEnabled()){
        QString pixName = ":/abuledudisplaypieces/arrow"+p->objectName()+"Hover";
        p->abePieceSetBrush(QPixmap(pixName).scaledToHeight(p->abePieceGetSize().height()));
        p->update();
    }
}

void ExerciceClosure::slotUnPaintHoverPiece(AbulEduPieceV1 *p)
{
    if(p->isEnabled()){
        QString pixName = ":/abuledudisplaypieces/arrow"+p->objectName();
        p->abePieceSetBrush(QPixmap(pixName).scaledToHeight(p->abePieceGetSize().height()));
        p->update();
    }
}

void ExerciceClosure::slotPaintPressedPiece(AbulEduPieceV1 *p)
{
    if(p->isEnabled()){
        QString pixName = ":/abuledudisplaypieces/arrow"+p->objectName()+"Pressed";
        p->abePieceSetBrush(QPixmap(pixName).scaledToHeight(p->abePieceGetSize().height()));
        p->update();
    }
}

void ExerciceClosure::slotUnPaintPressedPiece(AbulEduPieceV1 *p)
{
    if(p->isEnabled()){
        QString pixName = ":/abuledudisplaypieces/arrow"+p->objectName()+"Hover";
        p->abePieceSetBrush(QPixmap(pixName).scaledToHeight(p->abePieceGetSize().height()));
        p->update();
    }
}

void ExerciceClosure::slotPaintDisabledPiece(AbulEduPieceV1 *p)
{
    QString pixName = ":/abuledudisplaypieces/arrow"+p->objectName()+"Disabled";
    p->abePieceSetBrush(QPixmap(pixName).scaledToHeight(p->abePieceGetSize().height()));
    p->setEnabled(false);
    p->update();
}

void ExerciceClosure::slotUnPaintDisabledPiece(AbulEduPieceV1 *p)
{
    QString pixName = ":/abuledudisplaypieces/arrow"+p->objectName();
    p->abePieceSetBrush(QPixmap(pixName).scaledToHeight(p->abePieceGetSize().height()));
    p->setEnabled(true);
    p->update();
}

void ExerciceClosure::slotLimitePieceX()
{
    AbulEduPieceV1* p = (AbulEduPieceV1*) sender();
    if(p->x() > m_aireDeJeu->width()-p->abePieceGetSize().width()){
        p->setX(m_aireDeJeu->width()-p->abePieceGetSize().width());
    }
    if(p->x() < 0){
        p->setX(0);
    }
}

void ExerciceClosure::slotLimitePieceY()
{
    AbulEduPieceV1* p = (AbulEduPieceV1*) sender();
    if(p->y() > m_aireDeJeu->height()-p->abePieceGetSize().height()){
        p->setY(m_aireDeJeu->height()-p->abePieceGetSize().height());
    }
    if(p->y() < 0){
        p->setY(0);
    }
}
