/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2012 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2014 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#include "allereditorv1.h"
#include "ui_allereditorv1.h"
#include "abuleduoutilsv1.h"

AllerEditorV1::AllerEditorV1(QWidget *parent,QString nomModule) :
    QWidget(parent),
    ui(new Ui::AllerEditorV1),
    m_abuleduFileManager(0),
    m_multiMedia(new AbulEduMultiMediaV1(AbulEduMultiMediaV1::Recorder)),
    m_currentSoundPath(QString()),
    m_abuleduFile(0),
    m_widgetMainwindow(0),
    m_config(0),
    m_nombreExerciceTexteTrous(0),
    m_nombreExerciceOrdreAlphabetique(0),
    m_nombreExerciceMotsMelanges(0),
    m_nombreExercicePhrasesMelangees(0),
    m_nombreExerciceParagraphesMelanges(0),
    m_nombreExercicePhrasesSansEspace(0),
    m_nombreExerciceRechercheRapide(0),
    m_listeMotsIndexes(QMap<int,QString>()),
    m_previousWidget(0),
    m_previousExercice(0),
    m_previousStart(0),
    m_previousSelection(QString()),
    m_isFileSettingsFound(false),
    m_text(new AbulEduTextV1(QString())),
    m_numberWords(0),
    m_numberSentences(0),
    m_numberParagraphs(0),
    m_textEditForHoles(0),
    m_selectedObjectsList(QStringList()),
    m_holeArray(QList< QPair<int,int> >()),
    m_lastSelection(QPair<int,int>(0,0)),
    m_isTextModify(false),
    m_displayedObjectsMap(QMap<int,AbulEduTextEditV1*>()),
    m_nameTabs(QMap<QString,QString>()),
    m_correcteur(0),
    m_curseur(QTextCursor()),
    m_menuContextuel(0),
    m_textFormatSelected(QTextCharFormat()),
    m_textFormatUnSelected(QTextCharFormat())

{
    Q_UNUSED(nomModule)
    ui->setupUi(this);
    foreach(AbulEduFlatBoutonV1* b,ui->gbTutoriels->findChildren<AbulEduFlatBoutonV1*>()){
        b->abeFlatBoutonSetBackgroundAlignement(Qt::AlignVCenter | Qt::AlignLeft);
        b->setTexteAlignement(Qt::AlignVCenter | Qt::AlignLeft);
        b->setTextePaddingLeft(70);
    }

    int w = 0;
    QFontMetrics fm(abeApp->font());
    foreach(QPushButton* ft,ui->allerEditorFrmAccueilButtons->findChildren<QPushButton*>()){
        if(fm.boundingRect(ft->text()).width() > w) w = fm.boundingRect(ft->text()).width();
    }
    foreach(QPushButton* ft,ui->allerEditorFrmAccueilButtons->findChildren<QPushButton*>()){
        ft->setMinimumWidth(w);
    }

//    ui->allerEditorBtnTutoOpenText->setTexteAlignement(Qt::AlignBottom);
    m_previousWidget = ui->wAccueil;
    // Enlever la fermeture par la croix
    Qt::WindowFlags wFlags = this->windowFlags();
    if(Qt::WindowCloseButtonHint == (wFlags & Qt::WindowCloseButtonHint))
    {
        wFlags = wFlags ^ Qt::WindowCloseButtonHint;
        this->setWindowFlags(wFlags);
    }

    installEventFilter(this);
    setWindowTitle(abeApp->getAbeApplicationLongName()+ "[*]");
    /* Mise en place de la détection de modifications */
    {
        connectToHasChanged();
        connect(this, SIGNAL(hasChanged()),this, SLOT(setItHasBeenModified()),Qt::UniqueConnection);
    }

    ui->twEditor->setCurrentWidget(ui->wAccueil);

    /* Pour l'ajout d'un texte grâce au bouton Ouvrir sur la page Texte, on a un QFileDialog vers le disque local ... */
    QFileDialog *f = new QFileDialog(this, trUtf8("Sélectionner un texte brut"),
                                     QDir::homePath(),trUtf8("Fichiers Texte (*.txt *.rtf)"));
    f->setOption(QFileDialog::DontUseNativeDialog, true);
    ui->tabFileDialog->layout()->addWidget(f);
    /* Si on ne fait pas ca le dialog "disparait" quand on clique sur un fichier */
    connect(f, SIGNAL(accepted()), f, SLOT(show()), Qt::UniqueConnection);
    connect(f, SIGNAL(rejected()), f, SLOT(show()), Qt::UniqueConnection);
    connect(f, SIGNAL(fileSelected(QString)), this, SLOT(slotLocalFileDialogSelected(QString)), Qt::UniqueConnection);
    connect(f, SIGNAL(rejected()), this, SLOT(slotAbeEditorShowEditorTextPage()), Qt::UniqueConnection);
    /* ... et un AbulEduMediathequeGetV1 pour récupérer des textes sur data */
    ui->widgetMediathequeGet->abeSetSource("data");
    ui->widgetMediathequeGet->abeSetCustomBouton1("");
    ui->widgetMediathequeGet->abeHideBoutonTelecharger();
    ui->widgetMediathequeGet->abeSetSearchFileMime(AbulEduMediathequeGetV1::abeMediathequeFileMimeText);
    connect(ui->widgetMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(QSharedPointer<AbulEduFileV1>,int)), this, SLOT(slotLoadTextFromData(QSharedPointer<AbulEduFileV1>,int)));
    connect(ui->widgetMediathequeGet,SIGNAL(signalAbeMediathequeGetCloseOrHide()),SLOT(slotAbeEditorShowEditorTextPage()),Qt::UniqueConnection);

    connect(ui->TTRabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);
    connect(ui->OAabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);
    connect(ui->MOMabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);
    connect(ui->PHMabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);
    connect(ui->PAMabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);
    connect(ui->PSEabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);
    connect(ui->RRabeInstruction, SIGNAL(toggled(bool)),this, SLOT(onCheckBoxRecordToogled(bool)),Qt::UniqueConnection);

//    ui->allerEditorBtnTutorials->setVisible(false);
    ui->frmTaillePolice->setVisible(false);
}

void AllerEditorV1::abeEditorSetMainWindow(QWidget *mw)
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    m_widgetMainwindow = mw;
    MainWindow* parent = (MainWindow*) mw;
    /* Si on a déjà un fichier abe ouvert c'est lui qu'on édite */
    if (!parent->abeGetMyAbulEduFile()->abeFileGetFileName().baseName().isEmpty()) {
        m_abuleduFile = parent->abeGetMyAbulEduFile();
        ui->allerEditorBtnCurrent->setText(trUtf8("Editer le module ")+"\n"+m_abuleduFile->abeFileGetFileName().fileName());
        ui->allerEditorBtnCurrent->setEnabled(true);
        ui->allerEditorBtnCurrent->setMinimumHeight(60);
        ui->allerEditorBtnNew->setMinimumHeight(60);
        ui->allerEditorBtnOpen->setMinimumHeight(60);
        ui->allerEditorBtnTutorials->setMinimumHeight(60);

    }
    else {
        m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
        m_abuleduFile->setObjectName("depuis editeur");
        parent->abeSetMyAbulEduFile(m_abuleduFile);
        ui->allerEditorBtnCurrent->setEnabled(false);
    }
    m_abuleduFileManager = parent->abeGetMyAbulEduBoxFileManager();
    m_abuleduFileManager->abeSetFile(m_abuleduFile);
    //    QRect rec(x(),y()+10,width(),height()-50);
    //    m_abuleduFileManager->setGeometry(rec);
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)),this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)),Qt::UniqueConnection);
    //    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), this, SLOT(displaySaveLocation(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),Qt::UniqueConnection);

    ui->wMediathequePush->abeSetSourceEnum(AbulEduMediathequePushV1::abeMediatheque);
    connect(ui->wMediathequePush, SIGNAL(signalMediathequePushCloseOrHide()),this,SLOT(slotAbeEditorShowEditorTextPage()),Qt::UniqueConnection);

    QString selectedText = abe::STYLESELECTION;
    QString unSelectedText = abe::STYLEBLANC;
    m_textFormatSelected.setBackground(QBrush(QColor(selectedText.remove("background-color: "))));
    m_textFormatUnSelected.setBackground(QBrush(QColor(unSelectedText.remove("background-color: "))));


    /** @todo les autres */
    ui->widgetInputChooser->abeDeviceChooserSetMultimedia(m_multiMedia);
    m_multiMedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetButtonVisible(AbulEduControlAudioV1::BtnHide
                                                                                        | AbulEduControlAudioV1::BtnNext
                                                                                        | AbulEduControlAudioV1::BtnPrevious
                                                                                        | AbulEduControlAudioV1::BtnMagnifyingGlass,
                                                                                        false);
    m_multiMedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetButtonVisible(AbulEduControlAudioV1::BtnRecord, true);
    m_multiMedia->abeMultiMediaSetLevelMeterVisible(true);
    m_multiMedia->abeMultiMediaGetAudioControlWidget()->setEnabled(false);
    connect(m_multiMedia->abeMultiMediaGetViewerWidget(), SIGNAL(signalAbeMediaViewerRecordingStarted()), this, SLOT(slotRecordingStarted()),Qt::UniqueConnection);
    connect(m_multiMedia->abeMultiMediaGetViewerWidget(), SIGNAL(signalAbeMediaViewerRecordingStopped()), this, SLOT(slotRecordingStopped()),Qt::UniqueConnection);
    connect(m_multiMedia->abeMultiMediaGetViewerWidget(), SIGNAL(signalAbeMediaViewerLevel(qreal)),
            m_multiMedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioGetAudioLevelMeter(), SLOT(slotAbulEduAudioLevelWidgetSetLevel(qreal)),Qt::UniqueConnection);


//    ABULEDU_LOG_DEBUG()<<trUtf8(" Répertoire de travail editeur : ")<<m_abuleduFile->abeFileGetDirectoryTemp().absolutePath();

    /* Mise en place des noms des onglets dans la bonne langue */
    setTabSettings();

    QFontMetrics fm(abeApp->font());
    QString exo(QString("w").repeated(10));
    ui->TTRcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
    ui->OAcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
    ui->MOMcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
    ui->PHMcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
    ui->PAMcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
    ui->PSEcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
    ui->RRcbNumeroExercice->setMinimumWidth(fm.boundingRect(exo).width());
}

AbulEduTextV1* AllerEditorV1::abeEditorGetText()
{
    return m_text;
}


void AllerEditorV1::setNewUnitSettings(QString unitName)
{
    ABULEDU_LOG_TRACE()<<" ++++++++ "<<  __PRETTY_FUNCTION__;
    /* Copie des consignes par défaut. La méthode va renvoyer false dans le cas de l'ouverture d'un fichier existant mais c'est pas grave, le chemin sera passé à l'AbulEduInstructionV1 */

    if(!createDefaultInstruction(ui->TTRabeInstruction,trUtf8("Guide avec ta souris les mots enlevés vers leur place dans le texte"),QString(":/editeur/ttrconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Texte à trous";
    }
    if(!createDefaultInstruction(ui->OAabeInstruction,trUtf8("Utilise les cases pour ranger les mots proposés dans l'ordre alphabétique."),QString(":/editeur/oaconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Ordre alphabétique";
    }
    if(!createDefaultInstruction(ui->MOMabeInstruction,trUtf8("Remets en ordre les mots proposés pour reconstituer une phrase du texte."),QString(":/editeur/momconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Mots mélangés";
    }
    if(!createDefaultInstruction(ui->PHMabeInstruction,trUtf8("Remets les phrases proposées dans l'ordre du texte."),QString(":/editeur/phmconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Phrases mélangées";
    }
    if(!createDefaultInstruction(ui->PAMabeInstruction,trUtf8("Remets dans l'ordre du texte les paragraphes présentés."),QString(":/editeur/pamconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Paragraphes mélangés";
    }
    if(!createDefaultInstruction(ui->PSEabeInstruction,trUtf8("Sépare les mots dans les phrases affichées."),QString(":/editeur/pseconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Phrases sans espaces";
    }
    if(!createDefaultInstruction(ui->RRabeInstruction,trUtf8("Retrouve dans le texte le mot qui s'affiche en haut et clique sur lui avant la fin du temps imparti."),QString(":/editeur/rrconsignesequence"))){
        //            ABULEDU_LOG_DEBUG()<<"Impossible de copier la consigne par défaut pour Recherche rapide";
    }
    /* --------------------------------------------Instanciation de m_config ------------------------------------------------------ */
    if (!unitName.isEmpty())
    {
        QString nomFichierConf = m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/module.conf";

//        ABULEDU_LOG_DEBUG()<<"Nom du fichier de conf : "<<nomFichierConf;
        QFile* fichierConf = new QFile(nomFichierConf);
        if (!fichierConf->exists())
        {
//            ABULEDU_LOG_DEBUG()<<trUtf8(" Fichier config NON trouvé")<<" dans "<<m_abuleduFile->abeFileGetFileName().baseName();
        }
        else
        {
//            ABULEDU_LOG_DEBUG()<<trUtf8(" Fichier config trouvé")<<" dans "<<m_abuleduFile->abeFileGetFileName().fileName();
            fichierConf->copy(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/copieModule.conf");
            m_isFileSettingsFound = true;
            m_config = new QSettings(nomFichierConf,QSettings::IniFormat);
            while(!m_config->group().isEmpty()){
                m_config->endGroup();
            }
            ui->GEleAuteur->setText(m_config->value("auteur").toString());
            ui->GEleTitre->setText(m_config->value("titreTexte").toString());
            QFile::copy(m_config->fileName(),"copieFichierConfig");

            m_config->beginGroup("texteATrous");
            if(m_config->value("exerciceActive").toBool() == false && ui->TTRcbNumeroExercice->count() == 0) {
                on_TTRbtnAjouterExercice_clicked();
                ui->GEcbTexteTrous->setChecked(false);
            }
            else{
                ui->GEcbTexteTrous->setChecked(true);
            }
            m_config->endGroup();

            m_config->beginGroup("ordreAlphabetique");
            if(m_config->value("exerciceActive").toBool() == false && ui->OAcbNumeroExercice->count() == 0){
                on_OAbtnAjouterExercice_clicked();
                ui->GEcbOrdreAlphabetique->setChecked(false);
            }
            else{
                ui->GEcbOrdreAlphabetique->setChecked(true);
            }
            m_config->endGroup();
            m_config->beginGroup("motsMelanges");
            if(m_config->value("exerciceActive").toBool() == false && ui->MOMcbNumeroExercice->count() == 0){
                on_MOMbtnAjouterExercice_clicked();
                ui->GEcbMotsMelanges->setChecked(false);
            }
            else{
                ui->GEcbMotsMelanges->setChecked(true);
            }
            m_config->endGroup();
            m_config->beginGroup("phrasesMelangees");
            if(m_config->value("exerciceActive").toBool() == false && ui->PHMcbNumeroExercice->count() == 0){
                on_PHMbtnAjouterExercice_clicked();
                ui->GEcbPhrasesMelangees->setChecked(false);
            }
            else{
                ui->GEcbPhrasesMelangees->setChecked(true);
            }
            m_config->endGroup();
            m_config->beginGroup("paragraphesMelanges");
            if(m_config->value("exerciceActive").toBool() == false && ui->PAMcbNumeroExercice->count() == 0){
                on_PAMbtnAjouterExercice_clicked();
                ui->GEcbParagraphesMelanges->setChecked(false);
            }
            else{
                ui->GEcbParagraphesMelanges->setChecked(true);
            }
            m_config->endGroup();
            m_config->beginGroup("phraseSansEspace");
            if(m_config->value("exerciceActive").toBool() == false && ui->PSEcbNumeroExercice->count() == 0){
                on_PSEbtnAjouterExercice_clicked();
                ui->GEcbPhrasesSansEspace->setChecked(false);
            }
            else{
                ui->GEcbPhrasesSansEspace->setChecked(true);
            }
            m_config->endGroup();
            m_config->beginGroup("rechercheRapide");
            if(m_config->value("exerciceActive").toBool() == false && ui->RRcbNumeroExercice->count() == 0){
                on_RRbtnAjouterExercice_clicked();
                ui->GEcbRechercheRapide->setChecked(false);
            }
            else{
                ui->GEcbRechercheRapide->setChecked(true);
            }
            m_config->endGroup();
        }
    }
    else
    {
        m_config = new QSettings(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/module.conf",QSettings::IniFormat);
        m_config->setValue("unitVersion", abeApp->getAbeApplicationUnitVersion());
//        ABULEDU_LOG_DEBUG()<<trUtf8(" Fichier config créé")<<" dans "<<m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/module.conf";
        on_TTRbtnAjouterExercice_clicked();
        on_OAbtnAjouterExercice_clicked();
        on_MOMbtnAjouterExercice_clicked();
        on_PHMbtnAjouterExercice_clicked();
        on_PAMbtnAjouterExercice_clicked();
        on_PSEbtnAjouterExercice_clicked();
        on_RRbtnAjouterExercice_clicked();
    }
    /* ---------------------------------------------------------------------------------------------------------------------------- */


    /* ---------------------------------------------Instanciation de m_text ------------------------------------------------------ */

    m_text = new AbulEduTextV1(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/"+m_config->value("fichierTexte").toString());
    m_listeMotsIndexes = m_text->abetextMotsPonctues();
//    ABULEDU_LOG_DEBUG()<<" m_listeMotsIndexes "<<m_text->abetextMotsPonctues();
    m_numberWords = m_text->abetextCompteMotsPonctues();
    m_numberSentences = m_text->abetextComptePhrases();
    m_numberParagraphs = m_text->abetextParagraphes().count();
    /* ---------------------------------------------------------------------------------------------------------------------------- */

    ui->frmTaillePolice->setVisible(false);

    setTabTextSettings();
    setTabTTRSettings();
    setTabOASettings();
    setTabMOMSettings();
    setTabPHMSettings();
    setTabPAMSettings();
    setTabPSESettings();
    setTabRRSettings();

    /* -------------------------------------- Configuration de base pour l'onglet Propriétés -------------------------------------- */

    ui->cbLangueRessource->addItems(m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().values());
    ui->twEditor->setCurrentWidget(ui->wTexte);
    loadSettings(ui->wProprietes,1);
    /* ---------------------------------------------------------------------------------------------------------------------------- */

    //        updateLabelWidgetName(ui->twEditor->currentIndex());
    setWindowModified(false);
}

void AllerEditorV1::setTabSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    m_nameTabs.insert("texte", trUtf8("Texte"));
    m_nameTabs.insert("texteATrous", trUtf8("Texte à trous"));
    m_nameTabs.insert("ordreAlphabetique", trUtf8("Ordre alphabétique"));
    m_nameTabs.insert("motsMelanges", trUtf8("Mots mélangés"));
    m_nameTabs.insert("phrasesMelangees", trUtf8("Phrases mélangées"));
    m_nameTabs.insert("paragraphesMelanges", trUtf8("Paragraphes mélangés"));
    m_nameTabs.insert("phraseSansEspace", trUtf8("Phrase sans espaces"));
    m_nameTabs.insert("rechercheRapide", trUtf8("Recherche rapide"));
    m_nameTabs.insert("proprietes", trUtf8("Propriétés"));
    m_nameTabs.insert("accueil", trUtf8("Accueil"));

    // Je me demande s'il vaut mieux ou pas remplacer ui->twEditor->widget(0) par wTexte, ui->wClosure par wClosure, etc...
    ui->wTexte->setProperty("titre",m_nameTabs.value("texte"));
    ui->wTexte->setProperty("abrege","Texte");
    ui->wTexte->setProperty("rang",0);
    ui->wTexte->setProperty("label","lblTE");

    ui->wClosure->setProperty("titre",m_nameTabs.value("texteATrous"));
    ui->wClosure->setProperty("abrege","TTR");
    ui->wClosure->setProperty("rang",1);
    ui->wClosure->setProperty("label","lblTTR");

    ui->wOrdreAlpha->setProperty("titre",m_nameTabs.value("ordreAlphabetique"));
    ui->wOrdreAlpha->setProperty("abrege","OA");
    ui->wOrdreAlpha->setProperty("rang",2);
    ui->wOrdreAlpha->setProperty("label","lblOA");

    ui->wMotsMelanges->setProperty("titre",m_nameTabs.value("motsMelanges"));
    ui->wMotsMelanges->setProperty("abrege","MOM");
    ui->wMotsMelanges->setProperty("rang",3);
    ui->wMotsMelanges->setProperty("label","lblMOM");

    ui->wPhrasesMelangees->setProperty("titre",m_nameTabs.value("phrasesMelangees"));
    ui->wPhrasesMelangees->setProperty("abrege","PHM");
    ui->wPhrasesMelangees->setProperty("rang",4);
    ui->wPhrasesMelangees->setProperty("label","lblPHM");

    ui->wParagraphes->setProperty("titre",m_nameTabs.value("paragraphesMelanges"));
    ui->wParagraphes->setProperty("abrege","PAM");
    ui->wParagraphes->setProperty("rang",5);
    ui->wParagraphes->setProperty("label","lblPAM");

    ui->wPhrasesEspaces->setProperty("titre",m_nameTabs.value("phraseSansEspace"));
    ui->wPhrasesEspaces->setProperty("abrege","PSE");
    ui->wPhrasesEspaces->setProperty("rang",6);
    ui->wPhrasesEspaces->setProperty("label","lblPSE");

    ui->wRechercheRapide->setProperty("titre",m_nameTabs.value("rechercheRapide"));
    ui->wRechercheRapide->setProperty("abrege","RR");
    ui->wRechercheRapide->setProperty("rang",7);
    ui->wRechercheRapide->setProperty("label","lblRR");

    ui->wProprietes->setProperty("titre",m_nameTabs.value("proprietes"));
    ui->wProprietes->setProperty("abrege","Propriétés");
    ui->wProprietes->setProperty("rang",8);
    ui->wProprietes->setProperty("label","lblPR");

    ui->wAccueil->setProperty("titre",m_nameTabs.value("accueil"));
    ui->wAccueil->setProperty("abrege","AC");
    ui->wAccueil->setProperty("rang",9);
    ui->wAccueil->setProperty("label","lblAC");

    /*
    foreach(QObject* o,ui->twEditor->children())
    {
        QWidget* w = (QWidget*) o;
        if(m_localDebug) {
            ABULEDU_LOG_DEBUG()<<" ------------------------- ";
            ABULEDU_LOG_DEBUG()<<w->objectName();
            ABULEDU_LOG_DEBUG()<<w->property("titre").toString();
            ABULEDU_LOG_DEBUG()<<" ------------------------- ";
        }
    }*/

    connect(ui->twEditor, SIGNAL(currentChanged(int)),this, SLOT(updateLabelWidgetName(int)),Qt::UniqueConnection);
    connect(ui->twEditor, SIGNAL(currentChanged(int)), this, SLOT(onSwitchOnglet(int)),Qt::UniqueConnection);

}

void AllerEditorV1::setTabTextSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_text->abetextCompteMots();
    ui->TEbtnNettoyer->setVisible(false);
    QFontMetrics fm(abeApp->font());
    ui->TEbtnModifier->setMinimumWidth(fm.boundingRect("Sauvegardeww").width());
    if (m_text->abeGetText().isEmpty())
    {
//        ABULEDU_LOG_DEBUG()<<"Le texte est vide";
        ui->TEteTexte->setReadOnly(false);
        ui->TEbtnNettoyer->setEnabled(true);
        ui->TEbtnModifier->setEnabled(false);
        ui->TEbtnModifier->setText(trUtf8("&Sauvegarde\ndu texte"));
    }
    else
    {
//        ABULEDU_LOG_DEBUG()<<" -----                C'est là que j'écris mon texte                        ----------------------------------" ;
        ui->TEteTexte->setPlainText(m_text->abeGetText().trimmed());
        ui->TEteTexte->setReadOnly(true);
        ui->TEbtnModifier->setText(trUtf8("&Modifier\nle texte"));
        ui->TEbtnNettoyer->setEnabled(false);
    }
    m_isTextModify = false;
    ui->GEcbTexteTrous->setProperty("widgetLie",ui->wClosure->objectName());
    ui->GEcbOrdreAlphabetique->setProperty("widgetLie",ui->wOrdreAlpha->objectName());
    ui->GEcbMotsMelanges->setProperty("widgetLie",ui->wMotsMelanges->objectName());
    ui->GEcbPhrasesMelangees->setProperty("widgetLie",ui->wPhrasesMelangees->objectName());
    ui->GEcbParagraphesMelanges->setProperty("widgetLie",ui->wParagraphes->objectName());
    ui->GEcbPhrasesSansEspace->setProperty("widgetLie",ui->wPhrasesEspaces->objectName());
    ui->GEcbRechercheRapide->setProperty("widgetLie",ui->wRechercheRapide->objectName());

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        ui->GEcbTexteTrous->setChecked(m_config->value("texteATrous/exerciceActive").toBool());
        ui->GEcbOrdreAlphabetique->setChecked(m_config->value("ordreAlphabetique/exerciceActive").toBool());
        ui->GEcbMotsMelanges->setChecked(m_config->value("motsMelanges/exerciceActive").toBool());
        ui->GEcbPhrasesMelangees->setChecked(m_config->value("phrasesMelangees/exerciceActive").toBool());
        ui->GEcbParagraphesMelanges->setChecked(m_config->value("paragraphesMelanges/exerciceActive").toBool());
        ui->GEcbPhrasesSansEspace->setChecked(m_config->value("phraseSansEspace/exerciceActive").toBool());
        ui->GEcbRechercheRapide->setChecked(m_config->value("rechercheRapide/exerciceActive").toBool());
    }

    /*--------------------------Correcteur orthographique jlf 2012/04/11-----------------------------*/
    m_correcteur = new AbulEduCorrecteurV1(ui->TEteTexte);
    connect(ui->TEteTexte,SIGNAL(textChanged()),m_correcteur, SLOT(parseWord()),Qt::UniqueConnection);
    ui->TEteTexte->installEventFilter(this);
    connect(ui->TEteTexte, SIGNAL(textChanged()), this, SLOT(onTEtextModify()),Qt::UniqueConnection);
}

void AllerEditorV1::setTabTTRSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsTexteTrous();
    QFontMetrics fm(abeApp->font());
    QString doubleV("w");
    QString lemot(doubleV.repeated(15));
    ui->TTRlblCommenceA->setFixedWidth(fm.boundingRect(lemot).width());
    ui->TTRlblFinitA->setFixedWidth(fm.boundingRect(lemot).width());

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }

        m_config->beginGroup("texteATrous");
        ui->GEcbTexteTrous->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbTexteTrous);

        if(m_config->value("exerciceActive").toBool())
        {
            ui->TTRchbAfficherTexteSequence->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->TTRchbAfficherMotsBienPlaces->setChecked((m_config->value("afficherMotBienPlaces",true).toBool()));
            ui->TTRabeInstruction->setChecked((m_config->value("afficherConsigneSequence",true).toBool()));
            if (ui->TTRabeInstruction->isChecked())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->TTRabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->TTRcbNumeroExercice->blockSignals(true);
            int size = m_config->beginReadArray("exercice");
            ui->TTRcbNumeroExercice->clear();
            for (int i = 1; i <= size; ++i)
            {
                ui->TTRcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            m_config->endArray();
            ui->TTRcbNumeroExercice->blockSignals(false);

            m_nombreExerciceTexteTrous = size;
        }
        m_config->endGroup();
    }
    else{
        ui->TTRchbAfficherTexteSequence->setChecked(false);
        ui->TTRchbAfficherMotsBienPlaces->setChecked(true);
        ui->TTRabeInstruction->setChecked(false);
        ui->TTRcbNumeroExercice->blockSignals(true);
        ui->TTRcbNumeroExercice->clear();
        ui->TTRcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->TTRcbNumeroExercice->blockSignals(false);
        m_nombreExerciceTexteTrous = 1;
        ui->TTRbtnAjouterExercice->setEnabled(true);
        ui->TTRbtnDeleteCurrentExercise->setEnabled(false);
    }

    //        ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices texte a trous : "<<m_nombreExerciceTexteTrous;
    /* 20150622 Je préfère désactiver le mode "intelligent" : il n'est pas fiable (pas de distinction entre LA pêche -> NOM et JE pêche -> VERBE */
    ui->TTRfrModeIntelligent->setVisible(false);
    ui->TTRrbModeIntelligent->setVisible(false);
}

void AllerEditorV1::setTabOASettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsOrdreAlphabetique();
    ui->OAsbProfondeur->setVisible(false);
    ui->OAlblProfondeur->setVisible(false);
    ui->OAsbProfondeurScolaire->setVisible(false);
    ui->OAlblProfondeurScolaire->setVisible(false);

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        m_config->beginGroup("ordreAlphabetique");
        ui->GEcbOrdreAlphabetique->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbOrdreAlphabetique);
        if(m_config->value("exerciceActive").toBool())
        {
            ui->OAchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->OAchbAfficherMotsBienPlaces->setChecked((m_config->value("afficherMotBienPlaces",true).toBool()));
            ui->OAabeInstruction->setChecked((m_config->value("afficherConsigneSequence",true).toBool()));
            if (ui->OAabeInstruction->isChecked())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->OAabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->OAsbNbMotsAutomatique->setMaximum(m_numberWords);
            ui->OAsbNbMotsIntelligent->setMaximum(m_numberWords);
            ui->OAsbNbMotsManuel->setMaximum(m_numberWords);
            ui->OAsbNbMotsAutomatique->setMinimum(0);
            ui->OAsbNbMotsIntelligent->setMinimum(0);
            ui->OAsbNbMotsManuel->setMinimum(0);
            ui->OAsbNbMotsScolaire->setMinimum(0);

            ui->OAcbNumeroExercice->blockSignals(true);
            int size = m_config->beginReadArray("exercice");
            ui->OAcbNumeroExercice->clear();
            for (int i = 1; i <= size; ++i)
            {
                ui->OAcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            ui->OAcbNumeroExercice->blockSignals(false);
            m_config->endArray();
            m_nombreExerciceOrdreAlphabetique = size;
        }
        m_config->endGroup();
    }
    else{
        ui->OAchbAfficherTexte->setChecked(false);
        ui->OAchbAfficherMotsBienPlaces->setChecked(true);
        ui->OAabeInstruction->setChecked(false);
        ui->OAcbNumeroExercice->blockSignals(true);
        ui->OAcbNumeroExercice->clear();
        ui->OAcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->OAcbNumeroExercice->blockSignals(false);
        m_nombreExerciceOrdreAlphabetique = 1;
        ui->OAbtnAjouterExercice->setEnabled(true);
        ui->OAbtnDeleteCurrentExercise->setEnabled(false);
    }

//    ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices ordre alphabetique : "<<m_nombreExerciceOrdreAlphabetique;
    /* 20150622 Je préfère désactiver le mode "intelligent" : il n'est pas fiable (pas de distinction entre LA pêche et JE pêche */
    ui->OAfrModeIntelligent->setVisible(false);
    ui->OArbModeIntelligent->setVisible(false);
}

void AllerEditorV1::setTabMOMSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsMotsMelanges();

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        m_config->beginGroup("motsMelanges");
        ui->GEcbMotsMelanges->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbMotsMelanges);
        if(m_config->value("exerciceActive").toBool())
        {
            ui->MOMchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->MOMchbAfficherMotsBienPlaces->setChecked((m_config->value("afficherMotBienPlaces",true).toBool()));
            ui->MOMabeInstruction->setChecked((m_config->value("afficherConsigneSequence",true).toBool()));
            if (ui->MOMabeInstruction->isChecked())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->MOMabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }

            ui->MOMsbNumeroPhrase->setMaximum(m_numberSentences);

            ui->MOMcbNumeroExercice->blockSignals(true);
            int size = m_config->beginReadArray("exercice");
            ui->MOMcbNumeroExercice->clear();
            for (int i = 1; i <= size; ++i)
            {
                ui->MOMcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            ui->MOMcbNumeroExercice->blockSignals(false);
            m_config->endArray();
            m_nombreExerciceMotsMelanges = size;
        }
        m_config->endGroup();
    }
    else{
        ui->MOMchbAfficherTexte->setChecked(false);
        ui->MOMchbAfficherMotsBienPlaces->setChecked(true);
        ui->MOMabeInstruction->setChecked(false);
        ui->MOMcbNumeroExercice->blockSignals(true);
        ui->MOMcbNumeroExercice->clear();
        ui->MOMcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->MOMcbNumeroExercice->blockSignals(false);
        m_nombreExerciceMotsMelanges = 1;
        ui->MOMbtnAjouterExercice->setEnabled(true);
        ui->MOMbtnDeleteCurrentExercise->setEnabled(false);
    }

//    ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices mots melanges : "<<m_nombreExerciceMotsMelanges;
}

void AllerEditorV1::setTabPHMSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsPhrasesMelangees();

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        m_config->beginGroup("phrasesMelangees");
        ui->GEcbPhrasesMelangees->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbPhrasesMelangees);
        if(m_config->value("exerciceActive").toBool())
        {
            ui->PHMchbAfficherTexteSequence->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->PHMchbAfficherPhrasesBienPlacees->setChecked((m_config->value("afficherPhrasesBienPlacees",true).toBool()));
            ui->PHMabeInstruction->setChecked((m_config->value("afficherConsigneSequence").toBool()));
            if (ui->PHMabeInstruction->isChecked())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->PHMabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->PHMsbNbPhrasesUtiliseesManuel->setMaximum(m_numberSentences);
            ui->PHMsbNbPhrasesUtiliseesManuel->setMinimum(0);
            ui->PHMsbPhraseDebutManuel->setMaximum(m_numberSentences);
            ui->PHMsbPhraseDebutManuel->setMinimum(1);
            ui->PHMsbPhraseFinManuel->setMaximum(m_numberSentences);
            ui->PHMsbPhraseFinManuel->setMinimum(1);
            ui->PHMsbPhraseDebutAutomatique->setMaximum(m_numberSentences);
            ui->PHMsbPhraseDebutAutomatique->setMinimum(1);
            ui->PHMsbPhraseFinAutomatique->setMaximum(m_numberSentences);
            ui->PHMsbPhraseFinAutomatique->setMinimum(1);

            ui->PHMcbNumeroExercice->blockSignals(true);
            int size = m_config->beginReadArray("exercice");
            ui->PHMcbNumeroExercice->clear();
            for (int i = 1; i <= size; ++i)
            {
                ui->PHMcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            ui->PHMcbNumeroExercice->blockSignals(false);
            m_config->endArray();
            m_nombreExercicePhrasesMelangees = size;
        }
        m_config->endGroup();
    }
    else{
        ui->PHMchbAfficherTexteSequence->setChecked(false);
        ui->PHMchbAfficherPhrasesBienPlacees->setChecked(true);
        ui->PHMabeInstruction->setChecked(false);
        ui->PHMcbNumeroExercice->blockSignals(true);
        ui->PHMcbNumeroExercice->clear();
        ui->PHMcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->PHMcbNumeroExercice->blockSignals(false);
        m_nombreExercicePhrasesMelangees = 1;
        ui->PHMbtnAjouterExercice->setEnabled(true);
        ui->PHMbtnDeleteCurrentExercise->setEnabled(false);
    }

//    ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices phrases melangees : "<<m_nombreExercicePhrasesMelangees;
}

void AllerEditorV1::setTabPAMSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsParagraphesMelanges();

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        m_config->beginGroup("paragraphesMelanges");
        ui->GEcbParagraphesMelanges->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbParagraphesMelanges);
        if(m_config->value("exerciceActive").toBool())
        {
            ui->PAMchbAfficherTexteSequence->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->PAMchbAfficherParagraphesBienPlaces->setChecked((m_config->value("afficherParagraphesBienPlaces",true).toBool()));
            ui->PAMabeInstruction->setChecked((m_config->value("afficherConsigneSequence").toBool()));
            if (ui->PAMabeInstruction->isChecked())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->PAMabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->PAMsbParagrapheDebutManuel->setMaximum(m_numberSentences);
            ui->PAMsbParagrapheDebutManuel->setMinimum(0);
            ui->PAMsbParagrapheFinManuel->setMaximum(m_numberSentences);
            ui->PAMsbParagrapheFinManuel->setMinimum(0);
            ui->PAMsbParagrapheDebutAuto->setMaximum(m_numberSentences);
            ui->PAMsbParagrapheDebutAuto->setMinimum(0);
            ui->PAMsbParagrapheFinAuto->setMaximum(m_numberSentences);
            ui->PAMsbParagrapheFinAuto->setMinimum(0);

            ui->PAMcbNumeroExercice->blockSignals(true);
            int size = m_config->beginReadArray("exercice");
            ui->PAMcbNumeroExercice->clear();
            for (int i = 1; i <= size; ++i)
            {
                ui->PAMcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            ui->PAMcbNumeroExercice->blockSignals(false);
            m_config->endArray();
            m_nombreExerciceParagraphesMelanges = size;
        }
        m_config->endGroup();
    }
    else{
        ui->PAMchbAfficherTexteSequence->setChecked(false);
        ui->PAMchbAfficherParagraphesBienPlaces->setChecked(true);
        ui->PAMabeInstruction->setChecked(false);
        ui->PAMcbNumeroExercice->blockSignals(true);
        ui->PAMcbNumeroExercice->clear();
        ui->PAMcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->PAMcbNumeroExercice->blockSignals(false);
        m_nombreExerciceParagraphesMelanges = 1;
        ui->PAMbtnAjouterExercice->setEnabled(true);
        ui->PAMbtnDeleteCurrentExercise->setEnabled(false);
    }

//    ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices paragraphes melanges : "<<m_nombreExerciceParagraphesMelanges;
}

void AllerEditorV1::setTabPSESettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsPhrasesSansEspace();

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        m_config->beginGroup("phraseSansEspace");
        ui->GEcbPhrasesSansEspace->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbPhrasesSansEspace);
        if(m_config->value("exerciceActive").toBool())
        {
            ui->PSEchbAfficherClicsMalPlaces->setChecked(m_config->value("afficherClicsMalPlaces",true).toBool());
            ui->PSEabeInstruction->setChecked(m_config->value("afficherConsigneSequence").toBool());
            if (m_config->value("afficherconsigneSequence").toBool())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->PSEabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->PSEcbNumeroExercice->blockSignals(true);
            ui->PSEcbNumeroExercice->clear();
            int size = m_config->beginReadArray("exercice");
            for (int i = 1; i <= size; ++i)
            {
                ui->PSEcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            ui->PSEcbNumeroExercice->blockSignals(false);
            m_config->endArray();
            m_nombreExercicePhrasesSansEspace = size;
        }
        m_config->endGroup();
    }
    else{
        ui->PSEchbAfficherTexte->setChecked(false);
        ui->PSEchbAfficherClicsMalPlaces->setChecked(true);
        ui->PSEabeInstruction->setChecked(false);
        ui->PSEcbNumeroExercice->blockSignals(true);
        ui->PSEcbNumeroExercice->clear();
        ui->PSEcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->PSEcbNumeroExercice->blockSignals(false);
        m_nombreExercicePhrasesSansEspace = 1;
        ui->PSEbtnAjouterExercice->setEnabled(true);
        ui->PSEbtnDeleteCurrentExercise->setEnabled(false);
    }

//    ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices phrases sans espaces : "<<m_nombreExercicePhrasesSansEspace;
}

void AllerEditorV1::setTabRRSettings()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    setDefaultSettingsRechercheRapide();

    if (m_isFileSettingsFound)
    {
        while(!m_config->group().isEmpty()){
            m_config->endGroup();
        }
        m_config->beginGroup("rechercheRapide");
        ui->GEcbRechercheRapide->setChecked(m_config->value("exerciceActive").toBool());
        displayWidget(m_config->value("exerciceActive").toBool(),ui->GEcbRechercheRapide);
        if(m_config->value("exerciceActive").toBool())
        {
            ui->RRchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->RRchbGarderCouleurMotsPasses->setChecked((m_config->value("afficherErreurs",true).toBool()));
            ui->RRabeInstruction->setChecked((m_config->value("afficherConsigneSequence").toBool()));
            if (ui->RRabeInstruction->isChecked())
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->RRabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->RRcbNumeroExercice->blockSignals(true);
            ui->RRcbNumeroExercice->clear();
            int size = m_config->beginReadArray("exercice");
            for (int i = 1; i <= size; ++i)
            {
                ui->RRcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(i)));
            }
            ui->RRcbNumeroExercice->blockSignals(false);
            m_config->endArray();
            m_nombreExerciceRechercheRapide = size;
        }
        m_config->endGroup();
    }
    else{
        ui->RRchbAfficherTexte->setChecked(false);
        ui->RRchbGarderCouleurMotsPasses->setChecked(false);
        ui->RRabeInstruction->setChecked(false);
        ui->RRcbNumeroExercice->blockSignals(true);
        ui->RRcbNumeroExercice->clear();
        ui->RRcbNumeroExercice->addItem(trUtf8("Exercice 1"));
        ui->RRcbNumeroExercice->blockSignals(false);
        m_nombreExerciceRechercheRapide = 1;
        ui->RRbtnAjouterExercice->setEnabled(true);
        ui->RRbtnDeleteCurrentExercise->setEnabled(false);
    }

//    ABULEDU_LOG_DEBUG()<<"--------- >    Nombre d'exercices recherche rapide : "<<m_nombreExerciceRechercheRapide;

    /* 20150622 Je préfère désactiver le mode "intelligent" : il n'est pas fiable (pas de distinction entre LA pêche et JE pêche */
    ui->RRfrModeIntelligent->setVisible(false);
    ui->RRrbModeIntelligent->setVisible(false);
}

bool AllerEditorV1::createDefaultInstruction(AbulEduInstructionV1* abeInstruction, const QString& instruction,const QString &instructionResourceName)
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<instructionResourceName<<abeInstruction->objectName();
    abeInstruction->abeInstructionSetMultimedia(m_multiMedia);
    QString instructionPath = m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/data/instructions/";
    QDir().mkpath(instructionPath);
    return abeInstruction->abeInstructionSetDefaultInstruction(instruction,instructionPath,instructionResourceName);
}

void AllerEditorV1::allerEditorCleanProject()
{
    ABULEDU_LOG_TRACE()<<" ++++++++ "<<  __PRETTY_FUNCTION__;
    m_isFileSettingsFound = false;
    ui->TEteTexte->clear();
    ui->GEleAuteur->clear();
    ui->GEleTitre->clear();
    m_text = new AbulEduTextV1();
    m_abuleduFile->abeCleanDirectory(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath(),m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
//    m_abuleduFile->abeClean();

    /** je le fais deux fois puisque je vais aussi le faire dans la méthode setNewUnitSettings que j'appelle juste après ... */
    m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
    MainWindow* parent = (MainWindow*) m_widgetMainwindow;
    parent->abeSetMyAbulEduFile(m_abuleduFile);


    foreach(QCheckBox* chb,ui->GEgbExercices->findChildren<QCheckBox*>()){
        chb->setChecked(false);
    }
    foreach(QLabel* lbl,ui->frmNavigEditor->findChildren<QLabel*>()){
        if(lbl != ui->lblTE && lbl != ui->lblPR){
            lbl->setVisible(false);
        }
    }

    ui->twEditor->setCurrentWidget(ui->wTexte);
}

void AllerEditorV1::slotAbeEditorShowEditorMainPage()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(sender()){
//        ABULEDU_LOG_DEBUG() << "Slot appelé par un signal";
    }
    else{
        ui->twEditor->setCurrentWidget(ui->wAccueil);
    }
}

AllerEditorV1::~AllerEditorV1()
{
    delete ui;
    delete m_text;
    /* Comme chaque fois que l'objet en dispose, je fais un deleteLater() plutôt qu'un delete */
    m_config->deleteLater();
    emit editorLeft();
}

void AllerEditorV1::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AllerEditorV1::closeEvent(QCloseEvent* event)
{
    if (sender() == 0)
    {
        event->ignore();
        return;
    }
    else
    {
        event->accept();
    }
    emit editorLeft();
}

/* --------------------------------------Fonctions de navigation entre les onglets------------------------------------------ */


void AllerEditorV1::onSwitchOnglet(int nouvelIndex)
{
    QWidget* nouveauWidget = ui->twEditor->widget(nouvelIndex);
    if(nouveauWidget == 0){
        return;
    }
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_previousWidget<<" -> "<<nouveauWidget;
    if(m_previousWidget == nouveauWidget){
        return;
    }
    if(nouveauWidget != ui->wAutreAffichage){
        saveSettings(m_previousWidget,m_previousExercice);
        m_previousWidget = nouveauWidget;
        m_previousExercice = 0;
        loadSettings(nouveauWidget,0);
        ui->TTRcbNumeroExercice->setCurrentIndex(0);
        ui->OAcbNumeroExercice->setCurrentIndex(0);
        ui->MOMcbNumeroExercice->setCurrentIndex(0);
        ui->PHMcbNumeroExercice->setCurrentIndex(0);
        ui->PAMcbNumeroExercice->setCurrentIndex(0);
        ui->PSEcbNumeroExercice->setCurrentIndex(0);
        ui->RRcbNumeroExercice->setCurrentIndex(0);
        if (nouveauWidget == ui->wTexte){
            saveTextAsFile();
        }
    }
}

void AllerEditorV1::loadSettings(QWidget* onglet, int indexExercice)//à initialiser
{
//    if(onglet == m_previousWidget) return;
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<" onglet "<<onglet<<", exercice "<<indexExercice;
    if(m_config == 0) return;
    else ABULEDU_LOG_DEBUG()<<m_config->fileName();
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }

    if(onglet == ui->wTexte){
//        ABULEDU_LOG_DEBUG()<<"Load Case Texte";
        //            ABULEDU_LOG_DEBUG()<<"Fichier de configuration "<<m_config->fileName();
        ui->GEleTitre->setText(m_config->value("titreTexte").toString());
        ui->GEleAuteur->setText(m_config->value("auteur").toString());
        ui->GEsbTaillePolice->setValue(m_config->value("taillePolice",12).toInt());

        m_config->beginGroup("texteATrous");
        ui->GEcbTexteTrous->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
        m_config->beginGroup("ordreAlphabetique");
        ui->GEcbOrdreAlphabetique->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
        m_config->beginGroup("motsMelanges");
        ui->GEcbMotsMelanges->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
        m_config->beginGroup("phrasesMelangees");
        ui->GEcbPhrasesMelangees->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
        m_config->beginGroup("paragraphesMelanges");
        ui->GEcbParagraphesMelanges->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
        m_config->beginGroup("phraseSansEspace");
        ui->GEcbPhrasesSansEspace->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
        m_config->beginGroup("rechercheRapide");
        ui->GEcbRechercheRapide->setChecked(m_config->value("exerciceActive").toBool());
        m_config->endGroup();
    }

    else if(onglet == ui->wClosure){
//        ABULEDU_LOG_DEBUG()<<"Load Case 1";
        m_config->beginGroup("texteATrous");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wClosure->setEnabled(false);
        }
        else
        {
            ui->wClosure->setEnabled(true);
            ui->TTRchbAfficherTexteSequence->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->TTRchbAfficherMotsBienPlaces->setChecked((m_config->value("afficherMotBienPlaces",true).toBool()));
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->TTRabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->TTRabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->TTRabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
                ui->TTRsbDifficulte->setValue(m_config->value("niveauDifficulte",1).toInt());
                ui->TTRchbAfficherTexteExercice->setChecked(m_config->value("afficherTexteExercice").toBool());
                int pointSize = m_config->value("fontSize",24).toInt();
                switch (pointSize) {
                case 24:
                   ui->TTRrbBigPointSize->setChecked(true);
                    break;
                case 18:
                   ui->TTRrbMediumPointSize->setChecked(true);
                    break;
                case 12:
                   ui->TTRrbLittlePointSize->setChecked(true);
                    break;
                default:
                    ui->TTRrbBigPointSize->setChecked(true);
                    break;
                }
                int marqueurDebut = m_config->value("marqueurDebutTexte",0).toInt();
                ui->TTRsbMarqueurDebutTexte->setValue(marqueurDebut);
                QTextCursor cursor(ui->TEteTexte->document());
                cursor.movePosition(QTextCursor::End);
                ui->TTRsbMarqueurFinTexte->setValue(m_config->value("marqueurFinTexte",cursor.position()).toInt());
                ui->TTRrbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                if (m_config->value("modeManuel").toBool())
                {
                    ui->TTRfrModeManuel->setVisible(true);
                    QStringList listeTrousLue = m_config->value("listeTrous").toString().split(";",QString::SkipEmptyParts);
//                    ABULEDU_LOG_DEBUG()<<"Récupération de trous : "<<listeTrousLue.join(";");
//                    ABULEDU_LOG_DEBUG()<<"Avec début "<<marqueurDebut<<":"<<ui->TTRsbMarqueurDebutTexte->value()<<" et fin "<<ui->TTRsbMarqueurFinTexte->value();
//                    ABULEDU_LOG_DEBUG()<<"Transformation de trous : "<<listeTrousCorrigee.join(";");
                    ui->TTRlblTrousManuel->setText(listeTrousLue.join(";"));
                    ui->TTRsbNbTrousManuel->setValue(m_config->value("nombreTrousAAfficher").toInt());
                    /* L'appel des méthodes ci-dessous visait à afficher les mots enlevés à la place de ce qui est stocké : les index des trous.
                     * Méthode matchHolesWords créée pour éviter de faire comme ça */
                    //                    on_TTRbtnChoixMots_clicked();
                    //                    on_AbtnValider_clicked();
                    ui->TTRteTrousModeManuel->setHtml(matchHolesWords(ui->TEteTexte->toPlainText(),listeTrousLue.join(";"),ui->TTRsbMarqueurDebutTexte->value(),ui->TTRsbMarqueurFinTexte->value()).first.join(";"));

                }
                else
                {
                    ui->TTRfrModeManuel->setVisible(false);
                    ui->TTRteTrousModeManuel->clear();
                    ui->TTRsbNbTrousManuel->setValue(0);
                }

                ui->TTRrbModeIntelligent->setChecked(m_config->value("modeIntelligent").toBool());
                if (m_config->value("modeIntelligent").toBool())
                {
                    ui->TTRfrModeIntelligent->setVisible(true);
                    ui->TTRsbNbTrousIntelligent->setValue(m_config->value("nombreTrousAAfficher").toInt());
                    ui->TTRcbNature->setCurrentIndex(m_config->value("indexNature").toInt());
                    ui->TTRcbNiveau->setCurrentIndex(m_config->value("indexNiveau").toInt());
                }
                else
                {
                    ui->TTRfrModeIntelligent->setVisible(false);
                    ui->TTRsbNbTrousIntelligent->setValue(5);
                    ui->TTRcbNature->setCurrentIndex(0);
                    ui->TTRcbNiveau->setCurrentIndex(ui->TTRcbNiveau->count()-1);
                }

                ui->TTRrbModeAutomatique->setChecked(m_config->value("modeAutomatique").toBool());
                if (m_config->value("modeAutomatique").toBool())
                {
                    ui->TTRfrModeAutomatique->setVisible(true);
                    ui->TTRsbNbMotsAutomatique->setValue(m_config->value("nombreTrousAAfficher",5).toInt());
                }

                ui->TTRchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->TTRteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wOrdreAlpha){
//        ABULEDU_LOG_DEBUG()<<"Load Case 2";
        m_config->beginGroup("ordreAlphabetique");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wOrdreAlpha->setEnabled(false);
        }
        else
        {
            ui->wOrdreAlpha->setEnabled(true);
            ui->OAchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->OAchbAfficherMotsBienPlaces->setChecked((m_config->value("afficherMotBienPlaces",true).toBool()));
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->OAabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->OAabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    //                    ui->OAteConsigneSequence->setHtml(m_config->value("consigneSequence").toString());
                    ui->OAabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
                ui->OAsbDifficulte->setValue(m_config->value("niveauDifficulte").toInt());

                ui->OArbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                if (m_config->value("modeManuel").toBool())
                {
                    ui->OAfrModeManuel->setVisible(true);
                    ui->OAteMotsModeManuel->setHtml(m_config->value("listeMots").toString());
                    ui->OAsbNbMotsManuel->setValue(m_config->value("nbMotsARanger").toInt());
                }
                else
                {
                    ui->OAfrModeManuel->setVisible(false);
                    ui->OAteMotsModeManuel->clear();
                    ui->OAsbNbMotsManuel->setValue(0);
                }

                ui->OArbModeIntelligent->setChecked(m_config->value("modeIntelligent").toBool());
                if (m_config->value("modeIntelligent").toBool())
                {
                    ui->OAfrModeIntelligent->setVisible(true);
                    ui->OAsbNbMotsIntelligent->setValue(m_config->value("nbMotsARanger").toInt());
                    ui->OAcbNature->setCurrentIndex(m_config->value("indexNature").toInt());
                    ui->OAcbNiveau->setCurrentIndex(m_config->value("indexNiveau").toInt());
                    ui->OAsbProfondeur->setValue(m_config->value("profondeur").toInt());
                }
                else
                {
                    ui->OAfrModeIntelligent->setVisible(false);
                    ui->OAsbNbMotsIntelligent->setValue(0);
                    ui->OAcbNature->setCurrentIndex(0);
                    ui->OAcbNiveau->setCurrentIndex(0);
                    ui->OAsbProfondeur->setValue(0);
                }

                ui->OArbModeAutomatique->setChecked(m_config->value("modeAutomatique").toBool());
                if (m_config->value("modeAutomatique").toBool())
                {
                    ui->OAfrModeAutomatique->setVisible(true);
                    ui->OAsbNbMotsAutomatique->setValue(m_config->value("nbMotsARanger",6).toInt());
                }

                if (m_config->value("modeScolaire").toBool())
                {
                    ui->OAfrModeScolaire->setVisible(true);
                    ui->OAsbNbMotsScolaire->setValue(m_config->value("nbMotsARanger").toInt());
                    ui->OAcbNatureScolaire->setCurrentIndex(m_config->value("indexNature").toInt());
                    ui->OAcbNiveauScolaire->setCurrentIndex(m_config->value("indexNiveau").toInt());
                    ui->OAsbProfondeurScolaire->setValue(m_config->value("profondeur").toInt());
                }
                else
                {
                    ui->OAfrModeScolaire->setVisible(false);
                    ui->OAsbNbMotsScolaire->setValue(0);
                    ui->OAcbNatureScolaire->setCurrentIndex(0);
                    ui->OAcbNiveauScolaire->setCurrentIndex(0);
                    ui->OAsbProfondeurScolaire->setValue(0);
                }

                ui->OAchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->OAteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wMotsMelanges){
//        ABULEDU_LOG_DEBUG()<<"Load Case 3";
        //            ABULEDU_LOG_DEBUG()<<"Le texte comporte "<<m_numberSentences<<" phrases.";
        m_config->beginGroup("motsMelanges");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wMotsMelanges->setEnabled(false);
        }
        else
        {
            ui->wMotsMelanges->setEnabled(true);
            ui->MOMchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->MOMchbAfficherMotsBienPlaces->setChecked(m_config->value("afficherMotBienPlaces",true).toBool());
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->MOMabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->MOMabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->MOMabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
                ui->MOMsbDifficulte->setValue(m_config->value("niveauDifficulte").toInt());
                ui->MOMsbNumeroPhrase->setMaximum(m_numberSentences);
                ui->MOMchbAfficherPhraseExercice->setChecked(m_config->value("afficherPhrase").toBool());
                ui->MOMchbAfficherPhraseAide->setChecked(m_config->value("afficherPhraseDansAide").toBool());
                ui->MOMrbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                if (m_config->value("modeManuel").toBool())
                {
                    ui->MOMfrModeManuel->setVisible(true);
                    ui->MOMsbNbMotsMaxAuto->setVisible(false);
                    ui->MOMlblNbMotsMaxAuto->setVisible(false);
                    if (!m_config->value("phrase").toString().isEmpty())
                        ui->MOMteMotsModeManuel->setText(m_config->value("phrase").toString());
                    ui->MOMsbNumeroPhrase->setValue(m_config->value("numeroPhrase",1).toInt());
                }
                else
                {
                    ui->MOMfrModeManuel->setVisible(false);
                    ui->MOMteMotsModeManuel->clear();
                    ui->MOMsbNumeroPhrase->setValue(0);
                    ui->MOMsbNbMotsMaxAuto->setValue(m_config->value("nombreMotsMax",18).toInt());
                }

                ui->MOMrbModeAutomatique->setChecked(m_config->value("modeAutomatique").toBool());

                ui->MOMchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->MOMteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wPhrasesMelangees){
//        ABULEDU_LOG_DEBUG()<<"Load Case 4";
        m_config->beginGroup("phrasesMelangees");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wPhrasesMelangees->setEnabled(false);
        }
        else
        {
            ui->wPhrasesMelangees->setEnabled(true);
            ui->PHMchbAfficherTexteSequence->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->PHMchbAfficherPhrasesBienPlacees->setChecked(m_config->value("afficherPhrasesBienPlacees",true).toBool());
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->PHMabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->PHMabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->PHMabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            ui->PHMsbPhraseDebutManuel->setMinimum(1);
            ui->PHMsbPhraseDebutManuel->setMaximum(m_numberSentences);
            ui->PHMsbPhraseDebutManuel->setValue(m_config->value("phraseDebut",1).toInt());
            ui->PHMsbPhraseFinManuel->setMinimum(1);
            ui->PHMsbPhraseFinManuel->setMaximum(m_numberSentences);
            ui->PHMsbPhraseFinManuel->setValue(m_config->value("phraseFin",m_numberSentences).toInt());
            ui->PHMsbNbPhrasesUtiliseesManuel->setMaximum(m_numberSentences);
            ui->PHMsbNbPhrasesUtiliseesManuel->setValue(m_config->value("nbPhrasesUtiliseesManuel",5).toInt());
            ui->PHMsbNbPhrasesAutomatique->setMaximum(m_numberSentences);
            ui->PHMsbNbPhrasesAutomatique->setValue(m_config->value("nombrePhrasesAutomatique",5).toInt());
            ui->PHMsbPhraseDebutAutomatique->setMinimum(1);
            ui->PHMsbPhraseDebutAutomatique->setMaximum(m_numberSentences);
            ui->PHMsbPhraseDebutAutomatique->setValue(m_config->value("phraseDebut",1).toInt());
            ui->PHMsbPhraseFinAutomatique->setMinimum(1);
            ui->PHMsbPhraseFinAutomatique->setMaximum(m_numberSentences);
            ui->PHMsbPhraseFinAutomatique->setValue(m_config->value("phraseFin",m_numberSentences).toInt());
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
//                ABULEDU_LOG_DEBUG()<<"Le texte comporte "<<m_numberSentences<<" phrases.";
                ui->PHMsbDifficulte->setValue(m_config->value("niveauDifficulte").toInt());
                ui->PHMrbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                if (ui->PHMrbModeManuel->isChecked())
                {
                    ui->PHMrbModeManuel->setVisible(true);
                    ui->PHMtePhrasesModeManuel->setHtml(m_config->value("listePhrases").toString());
                    ui->PHMsbNbPhrasesUtiliseesManuel->setMaximum(m_config->value("nbPhrasesUtiliseesManuel").toInt());
                    ui->PHMsbNbPhrasesUtiliseesManuel->setValue(m_config->value("nbPhrasesUtiliseesManuel").toInt());
                    ui->PHMsbPhraseFinManuel->setValue(m_config->value("phraseFin").toInt());
                }
                else
                {
                    ui->PHMfrModeManuel->setVisible(false);
                    ui->PHMtePhrasesModeManuel->clear();
                }

                ui->PHMrbModeAutomatique->setChecked(m_config->value("modeAutomatique").toBool());
                if (ui->PHMrbModeAutomatique->isChecked())
                {
                    ui->PHMfrModeAutomatique->setVisible(true);
                    ui->PHMchbPhrasesConsecutivesAutomatique->setChecked(m_config->value("phrasesConsecutivesAutomatique",true).toBool());
                    ui->PHMsbPhraseDebutAutomatique->setValue(m_config->value("phraseDebut",1).toInt());
                    ui->PHMsbPhraseFinAutomatique->setValue(m_config->value("phraseFin",m_text->abetextComptePhrases()).toInt());
                    ui->PHMsbNbPhrasesAutomatique->setValue(m_config->value("nombrePhrasesAutomatique",5).toInt());
                }
                ui->PHMchbAfficherTexteExercice->setChecked(m_config->value("afficherTexteExercice").toBool());
                ui->PHMchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->PHMteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wParagraphes){
        //            ABULEDU_LOG_DEBUG()<<"Load Case 5";
        //            ABULEDU_LOG_DEBUG()<<"Le texte comporte "<<m_numberParagraphs<<" paragraphes.";
        m_config->beginGroup("paragraphesMelanges");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wParagraphes->setEnabled(false);
            //            ABULEDU_LOG_DEBUG()<<"onglet paragraphe disabled";
        }
        else
        {
            ui->wParagraphes->setEnabled(true);
            //            ABULEDU_LOG_DEBUG()<<"onglet paragraphe enabled";

            ui->PAMchbAfficherTexteSequence->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->PAMchbAfficherParagraphesBienPlaces->setChecked((m_config->value("afficherParagraphesBienPlaces",true).toBool()));
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->PAMabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->PAMabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->PAMabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
                ui->PAMsbDifficulte->setValue(m_config->value("niveauDifficulte").toInt());
                ui->PAMrbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                ui->PAMsbParagrapheDebutManuel->setMinimum(1);
                ui->PAMsbParagrapheDebutManuel->setMaximum(m_text->abetextParagraphes().size());
                ui->PAMsbParagrapheDebutManuel->setValue(m_config->value("paragrapheDebut",0).toInt()+1);
                ui->PAMsbParagrapheFinManuel->setMinimum(1);
                ui->PAMsbParagrapheFinManuel->setMaximum(m_text->abetextParagraphes().count());
                ui->PAMsbParagrapheFinManuel->setValue(m_config->value("paragrapheFin",m_text->abetextParagraphes().size()-1).toInt()+1);
                ui->PAMsbNombreParagraphesAuto->setValue(m_config->value("nombreParagraphes",m_text->abetextParagraphes().size()).toInt());
                ui->PAMsbParagrapheDebutAuto->setMinimum(1);
                ui->PAMsbParagrapheDebutAuto->setMaximum(m_text->abetextParagraphes().size());
                ui->PAMsbParagrapheDebutAuto->setValue(m_config->value("paragrapheDebut",0).toInt()+1);
                ui->PAMsbParagrapheFinAuto->setMinimum(1);
                ui->PAMsbParagrapheFinAuto->setMaximum(m_text->abetextParagraphes().count());
                ui->PAMsbParagrapheFinAuto->setValue(m_config->value("paragrapheFin",m_text->abetextParagraphes().size()).toInt()+1);
                if (m_config->value("modeManuel").toBool())
                {
                    ui->PAMfrModeManuel->setVisible(true);
                    ui->PAMleParagraphesChoisis->setText(m_config->value("listeParagraphes").toString());
                }
                else
                {
                    ui->PAMfrModeManuel->setVisible(false);
                }

                ui->PAMrbModeAuto->setChecked(m_config->value("modeAutomatique").toBool());
                if (m_config->value("modeAutomatique").toBool())
                {
                    ui->PAMfrModeAuto->setVisible(true);
                    ui->PAMrbModeAuto->setChecked(true);
                    ui->PAMleParagraphesChoisis->clear();
                }
                ui->PAMchbAfficherTexteExercice->setChecked(m_config->value("afficherTexteExercice").toBool());
                ui->PAMchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->PAMteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wPhrasesEspaces){
        //            ABULEDU_LOG_DEBUG()<<trUtf8("Chargement des paramètres pour l'onglet ")<<ui->wPhrasesEspaces->property("titre").toString();
        //            ABULEDU_LOG_DEBUG()<<"Load Case 6";
        m_config->beginGroup("phraseSansEspace");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wPhrasesEspaces->setEnabled(false);
        }
        else
        {
            ui->wPhrasesEspaces->setEnabled(true);
            ui->PSEchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->PSEchbAfficherClicsMalPlaces->setChecked((m_config->value("afficherClicsMalPlaces",true).toBool()));
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->PSEabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->PSEabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->PSEabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
                ui->PSEsbDifficulte->setValue(m_config->value("niveauDifficulte").toInt());
                ui->PSEsbNumeroPhrase->setMaximum(m_numberSentences);
                ui->PSErbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                ui->PSErbModeAutomatique->setChecked(m_config->value("modeAutomatique").toBool());
                if (m_config->value("modeManuel").toBool())
                {
                    ui->PSEfrModeManuel->setVisible(true);
//                    ABULEDU_LOG_DEBUG()<<"Lecture de "<<m_config->value("phraseNumero").toInt();
                    if (!m_config->value("phrase").toString().isEmpty())
                    {
                        ui->PSEtePhraseModeManuel->setText(m_config->value("phrase").toString());
                    }
                    ui->PSEsbNumeroPhrase->setValue(m_config->value("phraseNumero",1).toInt());
//                    ABULEDU_LOG_DEBUG()<<"LoadSettings(PSE) -> Lecture de "<<ui->PSEsbNumeroPhrase->value();
                }
                else
                {
                    ui->PSEfrModeManuel->setVisible(false);
                }

                ui->PSEchbAfficherPhrase->setChecked(m_config->value("afficherPhrase").toBool());
                ui->PSEchbIgnorerPonctuation->setChecked(m_config->value("ignorerPonctuation").toBool());
                ui->PSEchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->PSEteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wRechercheRapide){
        //            ABULEDU_LOG_DEBUG()<<"Load Case 7";
        m_config->beginGroup("rechercheRapide");
        if (!m_config->value("exerciceActive").toBool())
        {
            //            ui->wRechercheRapide->setEnabled(false);
        }
        else
        {
            ui->wRechercheRapide->setEnabled(true);
            ui->RRchbAfficherTexte->setChecked(m_config->value("afficherTexteSequence").toBool());
            ui->RRchbGarderCouleurMotsPasses->setChecked((m_config->value("afficherErreurs",true).toBool()));
            bool affichageConsigneSequence = m_config->value("afficherConsigneSequence", true).toBool();
            /* Je suis obligé de force le émit du toggled qui devrait pourtant être émis au setChecked ... */
            ui->RRabeInstruction->setChecked(affichageConsigneSequence);
            emit ui->RRabeInstruction->toggled(affichageConsigneSequence);
            if (affichageConsigneSequence)
            {
                if(!m_config->value("consigneSequence").toString().isEmpty()){
                    ui->RRabeInstruction->abeInstructionSetText(m_config->value("consigneSequence").toString());
                }
            }
            m_config->beginReadArray("exercice");
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en lecture dans le array à l'index ")<<indexExercice;
                int pointSize = m_config->value("fontSize",24).toInt();
                switch (pointSize) {
                case 24:
                   ui->RRrbBigPointSize->setChecked(true);
                    break;
                case 18:
                   ui->RRrbMediumPointSize->setChecked(true);
                    break;
                case 12:
                   ui->RRrbLittlePointSize->setChecked(true);
                    break;
                default:
                    ui->RRrbBigPointSize->setChecked(true);
                    break;
                }
                ui->RRsbDifficulte->setValue(m_config->value("niveauDifficulte").toInt());
                ui->RRsbTempsAccorde->setValue(m_config->value("tempsAffichageMot",7).toInt());
                ui->RRsbMotDebut->setValue(m_config->value("numeroMotDebut",1).toInt());
                if (m_config->value("numeroMotFin").toInt() != 0)
                    ui->RRsbMotFin->setValue(m_config->value("numeroMotFin",m_text->abetextCompteMots()).toInt());

                ui->RRrbModeManuel->setChecked(m_config->value("modeManuel").toBool());
                if (m_config->value("modeManuel").toBool())
                {
                    ui->RRfrModeManuel->setVisible(true);
                    ui->RRteMotsModeManuel->setHtml(m_config->value("listeMots").toString());
                    ui->RRsbNbMotsManuel->setValue(m_config->value("nbMotsATrouver",4).toInt());
                    ui->RRchbOrdreModeManuel->setChecked(m_config->value("ordreTexte",false).toBool());
                }
                else
                {
                    ui->RRfrModeManuel->setVisible(false);
                    ui->RRteMotsModeManuel->clear();
                    ui->RRsbNbMotsManuel->setValue(0);
                }

                ui->RRrbModeIntelligent->setChecked(m_config->value("modeIntelligent").toBool());
                if (m_config->value("modeIntelligent").toBool())
                {
                    ui->RRfrModeIntelligent->setVisible(true);
                    ui->RRsbNbMotsIntelligent->setValue(m_config->value("nbMotsATrouver").toInt());
                    ui->RRcbNature->setCurrentIndex(m_config->value("indexNature").toInt());
                    ui->RRcbNiveau->setCurrentIndex(m_config->value("indexNiveau").toInt());
                }
                else
                {
                    ui->RRfrModeIntelligent->setVisible(false);
                    ui->RRsbNbMotsIntelligent->setValue(0);
                    ui->RRcbNature->setCurrentIndex(0);
                    ui->RRcbNiveau->setCurrentIndex(0);
                }

                ui->RRrbModeAutomatique->setChecked(m_config->value("modeAutomatique").toBool());
                if (m_config->value("modeAutomatique").toBool())
                {
                    ui->RRfrModeAutomatique->setVisible(true);
                    ui->RRsbNbMotsAutomatique->setValue(m_config->value("nbMotsATrouver",4).toInt());
                }

                ui->RRchbInsensibleCasse->setChecked(m_config->value("insensibleCasse").toBool());
                ui->RRchbAfficherConsigneExercice->setChecked(m_config->value("afficherConsigneExercice").toBool());
                ui->RRteConsigneExercice->setHtml(m_config->value("consigneExercice").toString());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wProprietes){
        //            ABULEDU_LOG_DEBUG()<<"Load Case 8";
        QString codeLangue = m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().last();
        if (ui->cbLangueRessource->findText(m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().value(codeLangue)) != -1)
        {
            ui->cbLangueRessource->setCurrentIndex(ui->cbLangueRessource->findText(m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().value(codeLangue)));
        }
        if (ui->cbLicence->findText(m_abuleduFile->abeFileGetLOM()->abeLOMgetRightsDescription(codeLangue)) != -1)
        {
            ui->cbLicence->setCurrentIndex(ui->cbLicence->findText(m_abuleduFile->abeFileGetLOM()->abeLOMgetRightsDescription(codeLangue)));
        }
        ui->leTitre->setText(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralTitle(codeLangue));
        ui->teDescription->setPlainText(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralDescription(codeLangue).last());
        ui->leAuteur->setText(abeApp->getAbeIdentite()->abeGetPreNom()+" "+ abeApp->getAbeIdentite()->abeGetNom());
    }
    setWindowModified(false);
}

void AllerEditorV1::saveSettings(QWidget* onglet, int indexExercice)
{
    if(onglet == 0) {
        return;
    }
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<" onglet "<<onglet<<", exercice "<<indexExercice;
    if(m_config == 0) return;
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->setValue("taillePolice",ui->GEsbTaillePolice->value());
    m_config->setValue("unitVersion",abeApp->getAbeApplicationUnitVersion());
    if(onglet == ui->wTexte){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 0";
        m_config->setValue("titreTexte",ui->GEleTitre->text());
        m_config->setValue("auteur",ui->GEleAuteur->text());
        //        m_config->setValue("resume",ui->GEteResume->toPlainText());
        m_config->beginGroup("texteATrous");
        m_config->setValue("exerciceActive",ui->GEcbTexteTrous->isChecked());
        m_config->endGroup();
        m_config->beginGroup("ordreAlphabetique");
        m_config->setValue("exerciceActive",ui->GEcbOrdreAlphabetique->isChecked());
        m_config->endGroup();
        m_config->beginGroup("motsMelanges");
        m_config->setValue("exerciceActive",ui->GEcbMotsMelanges->isChecked());
        m_config->endGroup();
        m_config->beginGroup("phrasesMelangees");
        m_config->setValue("exerciceActive",ui->GEcbPhrasesMelangees->isChecked());
        m_config->endGroup();
        m_config->beginGroup("paragraphesMelanges");
        m_config->setValue("exerciceActive",ui->GEcbParagraphesMelanges->isChecked());
        m_config->endGroup();
        m_config->beginGroup("phraseSansEspace");
        m_config->setValue("exerciceActive",ui->GEcbPhrasesSansEspace->isChecked());
        m_config->endGroup();
        m_config->beginGroup("rechercheRapide");
        m_config->setValue("exerciceActive",ui->GEcbRechercheRapide->isChecked());
        m_config->endGroup();
    }

    else if(onglet == ui->wClosure){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 1";
        m_config->beginGroup("texteATrous");
        if (!m_config->value("exerciceActive").toBool())
        {

        }
        else
        {
            m_config->setValue("afficherTexteSequence",ui->TTRchbAfficherTexteSequence->isChecked());
            m_config->setValue("afficherMotBienPlaces",ui->TTRchbAfficherMotsBienPlaces->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->TTRabeInstruction->isChecked());
            if (ui->TTRabeInstruction->isChecked())// et il ne faut pas un else ?
            {
                m_config->setValue("consigneSequence",ui->TTRabeInstruction->abeInstructionGetText());
            }
            m_config->beginWriteArray("exercice",m_nombreExerciceTexteTrous);
            {
                m_config->setArrayIndex(indexExercice);
                //                        ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en écriture dans le array à l'index ")<<indexExercice;
                m_config->setValue("niveauDifficulte",ui->TTRsbDifficulte->value());
                m_config->setValue("afficherTexteExercice",ui->TTRchbAfficherTexteExercice->isChecked());
                if(ui->TTRrbMediumPointSize->isChecked()){
                    m_config->setValue("fontSize",18);
                }
                else if(ui->TTRrbLittlePointSize->isChecked()){
                    m_config->setValue("fontSize",12);
                }
                else {
                    m_config->setValue("fontSize",24);
                }
                m_config->setValue("marqueurDebutTexte",ui->TTRsbMarqueurDebutTexte->value());
                m_config->setValue("marqueurFinTexte",ui->TTRsbMarqueurFinTexte->value());
                m_config->setValue("modeManuel",ui->TTRrbModeManuel->isChecked());
                if (ui->TTRrbModeManuel->isChecked())
                {
                    m_config->setValue("listeTrous",ui->TTRlblTrousManuel->text());
                    m_config->setValue("nombreTrousAAfficher",ui->TTRsbNbTrousManuel->value());
                }
                else
                {
                    m_config->setValue("listeTrous","");
                }

                m_config->setValue("modeIntelligent",ui->TTRrbModeIntelligent->isChecked());
                if (ui->TTRrbModeIntelligent->isChecked())
                {
                    m_config->setValue("nombreTrousAAfficher",ui->TTRsbNbTrousIntelligent->value());
                    m_config->setValue("indexNature",ui->TTRcbNature->currentIndex());
                    m_config->setValue("indexNiveau",ui->TTRcbNiveau->currentIndex());
                    AbulEduMessageBoxV1* msgBox = new AbulEduMessageBoxV1(trUtf8("Attention : mode \"intelligent\" demandé pour Texte à trous."),
                                                                          trUtf8("N'oubliez pas de vérifier si le texte ou l'extrait choisi comporte bien %1 mots de nature %2 étudiés au %3...")
                                                                          .arg(QString::number(ui->TTRsbNbTrousIntelligent->value()))
                                                                          .arg(ui->TTRcbNature->currentText())
                                                                          .arg(ui->TTRcbNiveau->currentText()),
                                                                          true,
                                                                          ui->wProprietes);
                    msgBox->show();
                }
                else
                {
                    m_config->setValue("indexNature","");
                    m_config->setValue("indexNiveau","");
                }

                m_config->setValue("modeAutomatique",ui->TTRrbModeAutomatique->isChecked());
                if (ui->TTRrbModeAutomatique->isChecked())
                {
                    m_config->setValue("nombreTrousAAfficher",ui->TTRsbNbMotsAutomatique->value());
                }

                m_config->setValue("afficherConsigneExercice",ui->TTRchbAfficherConsigneExercice->isChecked());
                m_config->setValue("consigneExercice",ui->TTRteConsigneExercice->toPlainText());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wOrdreAlpha){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 2";
        m_config->beginGroup("ordreAlphabetique");
        if (m_config->value("exerciceActive").toBool())
        {
            m_config->setValue("afficherTexteSequence",ui->OAchbAfficherTexte->isChecked());
            m_config->setValue("afficherMotBienPlaces",ui->OAchbAfficherMotsBienPlaces->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->OAabeInstruction->isChecked());
            if (ui->OAabeInstruction->isChecked())
            {
                //                    m_config->setValue("consigneSequence",ui->OAteConsigneSequence->toPlainText());
                m_config->setValue("consigneSequence",ui->OAabeInstruction->abeInstructionGetPlainText());
            }
            m_config->beginWriteArray("exercice",m_nombreExerciceOrdreAlphabetique);
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en ecriture dans le array à l'index ")<<indexExercice;
                m_config->setValue("niveauDifficulte",ui->OAsbDifficulte->value());

                m_config->setValue("modeManuel",ui->OArbModeManuel->isChecked());
                m_config->setValue("listeMots",ui->OAteMotsModeManuel->toPlainText());
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Mode Manuel : "<<ui->OArbModeManuel->isChecked();
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Liste :"<<ui->OAteMotsModeManuel->toPlainText()<<" et Nombre mots manuel = "<<ui->OAsbNbMotsManuel->value();

                m_config->setValue("modeIntelligent",ui->OArbModeIntelligent->isChecked());
                if (ui->OArbModeIntelligent->isChecked())
                {
                    AbulEduMessageBoxV1* msgBox = new AbulEduMessageBoxV1(trUtf8("Attention : mode \"intelligent\" demandé pour Ordre alphabétique."),
                                                                          trUtf8("N'oubliez pas de vérifier si le texte ou l'extrait choisi comporte bien %1 mots de nature %2 étudiés au %3...")
                                                                          .arg(QString::number(ui->OAsbNbMotsIntelligent->value()))
                                                                          .arg(ui->OAcbNature->currentText())
                                                                          .arg(ui->OAcbNiveau->currentText()),
                                                                          true,
                                                                          ui->wProprietes);
                    msgBox->show();
                    m_config->setValue("indexNature",ui->OAcbNature->currentIndex());
                    m_config->setValue("indexNiveau",ui->OAcbNiveau->currentIndex());
                    m_config->setValue("profondeur",ui->OAsbProfondeur->value());
                    m_config->setValue("nbMotsARanger",ui->OAsbNbMotsIntelligent->value());
                    //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Mode Intelligent : "<<ui->OArbModeIntelligent->isChecked();
                    //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"nbMotsARanger Intelligent : "<<ui->OAsbNbMotsIntelligent->value()<<", Nature : "<<ui->OAcbNature->currentText();
                    //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"niveau : "<<ui->OAcbNiveau->currentText()<<", Profondeur : "<<ui->OAsbProfondeur->value();
                }
                m_config->setValue("modeScolaire",ui->OArbModeScolaire->isChecked());
                if (ui->OArbModeScolaire->isChecked())
                {
                    m_config->setValue("indexNature",ui->OAcbNatureScolaire->currentIndex());
                    m_config->setValue("indexNiveau",ui->OAcbNiveauScolaire->currentIndex());
                    m_config->setValue("profondeur",ui->OAsbProfondeurScolaire->value());
                    m_config->setValue("nbMotsARanger",ui->OAsbNbMotsScolaire->value());
                }
                m_config->setValue("modeAutomatique",ui->OArbModeAutomatique->isChecked());
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Mode Automatique : "<<ui->OArbModeAutomatique->isChecked();
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"nbMotsARanger automatique : "<<ui->OAsbNbMotsAutomatique->value();
                if (ui->OArbModeAutomatique->isChecked())
                    m_config->setValue("nbMotsARanger",ui->OAsbNbMotsAutomatique->value());
                else if (ui->OArbModeManuel->isChecked())
                    m_config->setValue("nbMotsARanger",ui->OAsbNbMotsManuel->value());

                m_config->setValue("afficherConsigneExercice",ui->OAchbAfficherConsigneExercice->isChecked());
                if (ui->OAchbAfficherConsigneExercice->isChecked())
                {
                    m_config->setValue("consigneExercice",ui->OAteConsigneExercice->toPlainText());
                }
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wMotsMelanges){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 3";
        m_config->beginGroup("motsMelanges");
        if (!m_config->value("exerciceActive").toBool())
        {
        }
        else
        {
            m_config->setValue("afficherTexteSequence",ui->MOMchbAfficherTexte->isChecked());
            m_config->setValue("afficherMotBienPlaces",ui->MOMchbAfficherMotsBienPlaces->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->MOMabeInstruction->isChecked());
            if (ui->MOMabeInstruction->isChecked())
            {
                m_config->setValue("consigneSequence",ui->MOMabeInstruction->abeInstructionGetPlainText());
            }
            m_config->beginWriteArray("exercice",m_nombreExerciceMotsMelanges);
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en écriture dans le array à l'index ")<<indexExercice;
                m_config->setValue("niveauDifficulte",ui->MOMsbDifficulte->value());
                m_config->setValue("afficherPhrase",ui->MOMchbAfficherPhraseExercice->isChecked());
                m_config->setValue("afficherPhraseDansAide",ui->MOMchbAfficherPhraseExercice->isChecked());
                m_config->setValue("modeManuel",ui->MOMrbModeManuel->isChecked());
                if (m_config->value("modeManuel").toBool())
                {
                    m_config->setValue("phrase",ui->MOMteMotsModeManuel->toPlainText());
                    m_config->setValue("numeroPhrase",ui->MOMsbNumeroPhrase->value());
                }
                else
                {
                    m_config->setValue("nombreMotsMax",ui->MOMsbNbMotsMaxAuto->value());
                }
                m_config->setValue("modeAutomatique",ui->MOMrbModeAutomatique->isChecked());

                m_config->setValue("afficherConsigneExercice",ui->MOMchbAfficherConsigneExercice->isChecked());
                m_config->setValue("consigneExercice",ui->MOMteConsigneExercice->toPlainText());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wPhrasesMelangees){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 4";
        m_config->beginGroup("phrasesMelangees");
        if (!m_config->value("exerciceActive").toBool())
        {
        }
        else
        {
            m_config->setValue("afficherTexteSequence",ui->PHMchbAfficherTexteSequence->isChecked());
            m_config->setValue("afficherPhrasesBienPlacees",ui->PHMchbAfficherPhrasesBienPlacees->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->PHMabeInstruction->isChecked());
            if (ui->PHMabeInstruction->isChecked())
            {
                m_config->setValue("consigneSequence",ui->PHMabeInstruction->abeInstructionGetPlainText());
            }
            m_config->beginWriteArray("exercice",m_nombreExercicePhrasesMelangees);
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en écriture dans le array à l'index ")<<indexExercice;
                m_config->setValue("niveauDifficulte",ui->PHMsbDifficulte->value());
                m_config->setValue("modeManuel",ui->PHMrbModeManuel->isChecked());
                if (m_config->value("modeManuel").toBool())
                {
                    m_config->setValue("listePhrases",ui->PHMtePhrasesModeManuel->toPlainText());
                    m_config->setValue("nbPhrasesUtiliseesManuel",ui->PHMsbNbPhrasesUtiliseesManuel->value());
                    m_config->setValue("phraseDebut",ui->PHMsbPhraseDebutManuel->value());
                    m_config->setValue("phraseFin",ui->PHMsbPhraseFinManuel->value());
                }

                m_config->setValue("modeAutomatique",ui->PHMrbModeAutomatique->isChecked());
                if (m_config->value("modeAutomatique").toBool())
                {
                    m_config->setValue("nombrePhrasesAutomatique",ui->PHMsbNbPhrasesAutomatique->value());
                    m_config->setValue("phrasesConsecutivesAutomatique",ui->PHMchbPhrasesConsecutivesAutomatique->isChecked());
                    m_config->setValue("phraseDebut",ui->PHMsbPhraseDebutAutomatique->value());
                    m_config->setValue("phraseFin",ui->PHMsbPhraseFinAutomatique->value());
                }

                m_config->setValue("afficherTexteExercice",ui->PHMchbAfficherTexteExercice->isChecked());
                m_config->setValue("afficherConsigneExercice",ui->PHMchbAfficherConsigneExercice->isChecked());
                m_config->setValue("consigneExercice",ui->PHMteConsigneExercice->toPlainText());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wParagraphes){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 5";

        m_config->beginGroup("paragraphesMelanges");
        if (!m_config->value("exerciceActive").toBool())
        {
        }
        else
        {
            m_config->setValue("afficherTexteSequence",ui->PAMchbAfficherTexteSequence->isChecked());
            m_config->setValue("afficherParagraphesBienPlaces",ui->PAMchbAfficherParagraphesBienPlaces->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->PAMabeInstruction->isChecked());
            if (ui->PAMabeInstruction->isChecked())
            {
                m_config->setValue("consigneSequence",ui->PAMabeInstruction->abeInstructionGetPlainText());
            }
            m_config->beginWriteArray("exercice",m_nombreExerciceParagraphesMelanges);
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en écriture dans le array à l'index ")<<indexExercice;
                m_config->setValue("niveauDifficulte",ui->PAMsbDifficulte->value());
                m_config->setValue("modeManuel",ui->PAMrbModeManuel->isChecked());

                if (m_config->value("modeManuel").toBool())
                {
                    m_config->setValue("paragrapheDebut",ui->PAMsbParagrapheDebutManuel->value()-1);
                    m_config->setValue("paragrapheFin",ui->PAMsbParagrapheFinManuel->value()-1);
                    m_config->setValue("listeParagraphes",ui->PAMleParagraphesChoisis->text());
                }

                m_config->setValue("modeAutomatique",ui->PAMrbModeAuto->isChecked());
                if (m_config->value("modeAutomatique").toBool())
                {
                    m_config->setValue("nombreParagraphes",ui->PAMsbNombreParagraphesAuto->value());
                    m_config->setValue("listeParagraphes","");
                    m_config->setValue("paragrapheDebut",ui->PAMsbParagrapheDebutAuto->value()-1);
                    m_config->setValue("paragrapheFin",ui->PAMsbParagrapheFinAuto->value()-1);
                }
                m_config->setValue("afficherTexteExercice",ui->PAMchbAfficherTexteExercice->isChecked());
                m_config->setValue("afficherConsigneExercice",ui->PAMchbAfficherConsigneExercice->isChecked());
                m_config->setValue("consigneExercice",ui->PAMteConsigneExercice->toPlainText());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wPhrasesEspaces){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 6";
        m_config->beginGroup("phraseSansEspace");
        if (!m_config->value("exerciceActive").toBool())
        {
        }
        else
        {
            m_config->setValue("afficherTexteSequence",ui->PSEchbAfficherTexte->isChecked());
            m_config->setValue("afficherClicsMalPlaces",ui->PSEchbAfficherClicsMalPlaces->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->PSEabeInstruction->isChecked());
            if (ui->PSEabeInstruction->isChecked())
            {
                m_config->setValue("consigneSequence",ui->PSEabeInstruction->abeInstructionGetPlainText());
            }
            m_config->beginWriteArray("exercice", m_nombreExercicePhrasesSansEspace);
            {
                m_config->setArrayIndex(indexExercice);
//                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en écriture dans le array à l'index ")<<indexExercice;
                m_config->setValue("niveauDifficulte",ui->PSEsbDifficulte->value());
                m_config->setValue("modeManuel", ui->PSErbModeManuel->isChecked());
                m_config->setValue("modeAutomatique",ui->PSErbModeAutomatique->isChecked());

                if (ui->PSErbModeManuel->isChecked())
                {
                    m_config->setValue("phrase",ui->PSEtePhraseModeManuel->toPlainText());
                    m_config->setValue("phraseNumero",ui->PSEsbNumeroPhrase->value());
                }
                m_config->setValue("afficherPhrase",ui->PSEchbAfficherPhrase->isChecked());
                m_config->setValue("ignorerPonctuation",ui->PSEchbIgnorerPonctuation->isChecked());
                m_config->setValue("afficherConsigneExercice",ui->PSEchbAfficherConsigneExercice->isChecked());
                m_config->setValue("consigneExercice",ui->PSEteConsigneExercice->toPlainText());
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wRechercheRapide){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 7";
        m_config->beginGroup("rechercheRapide");
        if (!m_config->value("exerciceActive").toBool())
        {
        }
        else
        {
            m_config->setValue("afficherTexteSequence",ui->RRchbAfficherTexte->isChecked());
            m_config->setValue("afficherErreurs",ui->RRchbGarderCouleurMotsPasses->isChecked());
            m_config->setValue("afficherConsigneSequence",ui->RRabeInstruction->isChecked());
            if (ui->RRabeInstruction->isChecked())
            {
                m_config->setValue("consigneSequence",ui->RRabeInstruction->abeInstructionGetPlainText());
            }
            m_config->beginWriteArray("exercice",m_nombreExerciceRechercheRapide);
            {
                m_config->setArrayIndex(indexExercice);
                //                ABULEDU_LOG_DEBUG()<< trUtf8("Je suis en ecriture dans le array à l'index ")<<indexExercice;
                if(ui->RRrbMediumPointSize->isChecked()){
                    m_config->setValue("fontSize",18);
                }
                else if(ui->RRrbLittlePointSize->isChecked()){
                    m_config->setValue("fontSize",12);
                }
                else {
                    m_config->setValue("fontSize",24);
                }
                m_config->setValue("niveauDifficulte",ui->RRsbDifficulte->value());
                m_config->setValue("tempsAffichageMot",ui->RRsbTempsAccorde->value());
                m_config->setValue("numeroMotDebut",ui->RRsbMotDebut->value());
                m_config->setValue("numeroMotFin",ui->RRsbMotFin->value());

                m_config->setValue("modeManuel",ui->RRrbModeManuel->isChecked());
                if (m_config->value("modeManuel").toBool())
                {
                    m_config->setValue("listeMots",ui->RRteMotsModeManuel->toPlainText());
                    m_config->setValue("nbMotsATrouver",ui->RRsbNbMotsManuel->value());
                    m_config->setValue("ordreTexte",ui->RRchbOrdreModeManuel->isChecked());
                }
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Mode Manuel : "<<ui->RRrbModeManuel->isChecked();
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Liste :"<<ui->RRteMotsModeManuel->toPlainText()<<" et Nombre mots manuel = "<<ui->RRsbNbMotsManuel->value();

                m_config->setValue("modeIntelligent",ui->RRrbModeIntelligent->isChecked());
                if (m_config->value("modeIntelligent").toBool())
                {
                    AbulEduMessageBoxV1* msgBox = new AbulEduMessageBoxV1(trUtf8("Attention : mode \"intelligent\" demandé pour Recherche rapide."),
                                                                          trUtf8("N'oubliez pas de vérifier si le texte ou l'extrait choisi comporte bien %1 mots de nature %2 étudiés au %3...")
                                                                          .arg(QString::number(ui->RRsbNbMotsIntelligent->value()))
                                                                          .arg(ui->RRcbNature->currentText())
                                                                          .arg(ui->RRcbNiveau->currentText()),
                                                                          true,
                                                                          ui->wProprietes);
                    msgBox->show();m_config->setValue("indexNature",ui->RRcbNature->currentIndex());
                    m_config->setValue("nbMotsATrouver",ui->RRsbNbMotsIntelligent->value());
                    m_config->setValue("indexNiveau",ui->RRcbNiveau->currentIndex());
                }
                //Sup m_config->setValue("niveau",ui->RRcbNiveau->currentText());
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Mode Intelligent : "<<ui->RRrbModeIntelligent->isChecked();
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"nbMotsARanger Intelligent : "<<ui->RRsbNbMotsIntelligent->value()<<", Nature : "<<ui->RRcbNature->currentText();
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"niveau : "<<ui->RRcbNiveau->currentText();

                m_config->setValue("modeAutomatique",ui->RRrbModeAutomatique->isChecked());
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Mode Automatique : "<<ui->RRrbModeAutomatique->isChecked();
                //if (m_localDebug) ABULEDU_LOG_DEBUG()<<"nbMotsATrouver automatique : "<<ui->RRsbNbMotsAutomatique->value();
                if (ui->RRrbModeAutomatique->isChecked())
                    m_config->setValue("nbMotsATrouver",ui->RRsbNbMotsAutomatique->value());
                else if (ui->RRrbModeManuel->isChecked())
                    m_config->setValue("nbMotsATrouver",ui->RRsbNbMotsManuel->value());

                m_config->setValue("insensibleCasse",ui->RRchbInsensibleCasse->isChecked());
                m_config->setValue("afficherConsigneExercice",ui->RRchbAfficherConsigneExercice->isChecked());
                if (ui->RRchbAfficherConsigneExercice->isChecked())
                {
                    m_config->setValue("consigneExercice",ui->RRteConsigneExercice->toPlainText());
                }
            }
            m_config->endArray();
        }
        m_config->endGroup();
    }
    else if(onglet == ui->wProprietes){
        //            ABULEDU_LOG_DEBUG()<<"Save Case 8";
        QString codeLangue = m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().key(ui->cbLangueRessource->currentText());

        m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralTitle(codeLangue,ui->leTitre->text());
        m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralDescription(codeLangue,ui->teDescription->document()->toPlainText());

        vCard vcard;
        vCardProperty name_prop  = vCardProperty::createName(ui->leAuteur->text(), "");
        vcard.addProperty(name_prop);
        vCardProperty fn_prop    = vCardProperty(VC_FORMATTED_NAME, ui->leAuteur->text());
        vcard.addProperty(fn_prop);
        vCardProperty note_prop  = vCardProperty(VC_NOTE, "");
        vcard.addProperty(note_prop);
        vCardProperty org_prop   = vCardProperty::createOrganization("");
        vcard.addProperty(org_prop);
        vCardProperty url_prop   = vCardProperty(VC_URL, "");
        vcard.addProperty(url_prop);

        m_abuleduFile->abeFileGetLOM()->abeLOMaddLifeCycleContributionRole("author", vcard, QDate::currentDate());
        m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsCost("no");
        m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsCopyrightAndOtherRestrictions("yes");
        m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsDescription(codeLangue,ui->cbLicence->currentText());

        m_abuleduFile->abeFileGetLOM()->abeLOMsetAnnotationDate(QDate::currentDate());
        m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralLanguage(codeLangue);
    }
    m_config->sync();
    setWindowModified(false);
}

void AllerEditorV1::connectToHasChanged()
{
    /* Général */
    //    connect(ui->GEleTitre, SIGNAL(textChanged(QString)),ui->TElblTitreTexte, SLOT(setText(QString)));
    connect(ui->GEleTitre, SIGNAL(textChanged(QString)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEleAuteur, SIGNAL(textChanged(QString)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    //    connect(ui->GEteResume, SIGNAL(textChanged()), this, SIGNAL(hasChanged()));
    connect(ui->GEcbTexteTrous, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEcbOrdreAlphabetique, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEcbMotsMelanges, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEcbPhrasesMelangees, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEcbParagraphesMelanges, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEcbPhrasesSansEspace, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEcbRechercheRapide, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->GEsbTaillePolice, SIGNAL(valueChanged(int)),this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TEteTexte, SIGNAL(textChanged()),this,SIGNAL(hasChanged()),Qt::UniqueConnection);
    //plus LOM et Cie
    /* Texte à trous */
    connect(ui->TTRchbAfficherMotsBienPlaces, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRchbAfficherTexteSequence, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRchbAfficherTexteExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRsbMarqueurDebutTexte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRsbMarqueurFinTexte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRsbNbMotsAutomatique, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRsbNbTrousIntelligent, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRsbNbTrousManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /** @todo émettre dans l'AbulEduInstructionV1 un signal "abeInstructionChanged" */
    connect(ui->TTRteTrousModeManuel, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRcbNature, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRcbNiveau, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRrbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRrbModeIntelligent, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->TTRrbModeAutomatique, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /* Ordre alphabétique */
    connect(ui->OAchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAchbAfficherMotsBienPlaces, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAchbAfficherTexte, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OArbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OArbModeIntelligent, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OArbModeAutomatique, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OArbModeScolaire, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAteMotsModeManuel, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbNbMotsAutomatique, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbNbMotsIntelligent, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbNbMotsManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbNbMotsScolaire, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbProfondeur, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAsbProfondeurScolaire, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAcbNature, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAcbNiveau, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAcbNatureScolaire, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->OAcbNiveauScolaire, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /* Mots mélangés */
    connect(ui->MOMchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMchbAfficherMotsBienPlaces, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMchbAfficherTexte, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMchbAfficherPhraseExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMchbAfficherPhraseAide, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMrbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMrbModeAutomatique, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMteMotsModeManuel, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->MOMsbNumeroPhrase, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /* Phrases mélangées */
    connect(ui->PHMchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMchbAfficherPhrasesBienPlacees, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMchbAfficherTexteExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMchbAfficherTexteSequence, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMchbPhrasesConsecutivesAutomatique, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMrbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMrbModeAutomatique, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMtePhrasesModeManuel, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbNbPhrasesAutomatique, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbNbPhrasesUtiliseesManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseDebutAutomatique, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseDebutManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseFinAutomatique, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseFinManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /* Paragraphes mélangés */
    connect(ui->PAMchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMchbAfficherParagraphesBienPlaces, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMchbAfficherTexteExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMchbAfficherTexteSequence, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMrbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMrbModeAuto, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMleParagraphesChoisis, SIGNAL(textChanged(QString)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMsbNombreParagraphesAuto, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMsbParagrapheDebutAuto, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMsbParagrapheDebutManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMsbParagrapheFinAuto, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PAMsbParagrapheFinManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /* Phrases sans espaces */
    connect(ui->PSEchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEchbAfficherClicsMalPlaces, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEchbAfficherPhrase, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEchbAfficherTexte, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEchbIgnorerPonctuation, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSErbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSErbModeAutomatique, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEtePhraseModeManuel, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->PSEsbNumeroPhrase, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    /* Recherche rapide */
    connect(ui->RRchbAfficherConsigneExercice, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRchbAfficherTexte, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRchbGarderCouleurMotsPasses, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRchbInsensibleCasse, SIGNAL(stateChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRrbModeManuel, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRrbModeIntelligent, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRrbModeAutomatique, SIGNAL(clicked()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRteConsigneExercice, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRteMotsModeManuel, SIGNAL(textChanged()), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRsbDifficulte, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRsbNbMotsAutomatique, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRsbNbMotsIntelligent, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRsbNbMotsManuel, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRsbTempsAccorde, SIGNAL(valueChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRcbNature, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
    connect(ui->RRcbNiveau, SIGNAL(currentIndexChanged(int)), this, SIGNAL(hasChanged()),Qt::UniqueConnection);
}


/* ------------------------------------------------------------------------------------------------------------------------ */

/* --------------------------- Fonctions utiles triées par ordre d'exercice dans les onglets  ----------------------------- */

void AllerEditorV1::on_TEbtnModifier_clicked()
{
    if (ui->TEbtnModifier->text()==trUtf8("&Modifier\nle texte") && !ui->TEteTexte->toPlainText().isEmpty())
    {
        AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(QObject::trUtf8("Attention"),
                                                           QObject::trUtf8("Tout changement dans le texte, même les corrections, peuvent affecter les exercices déjà prévus...\nSi vous apportez des modifications, essayez ensuite à nouveau tous vos exercices !!"),
                                                           true,
                                                           ui->wTexte);
        msg->show();

    }
    ui->TEteTexte->setReadOnly(false);
    ui->TEbtnNettoyer->setEnabled(true);
    if (ui->TEbtnModifier->text()==trUtf8("&Sauvegarde\ndu texte"))
    {
        on_TEbtnNettoyer_clicked();
        saveTextAsFile();
    }
}

void AllerEditorV1::on_TEbtnNettoyer_clicked()
{
    m_text->abeSetText(m_text->abetextNettoieText());
    ui->TEteTexte->setText(m_text->abeGetText());
    //    m_correcteur = new AbulEduCorrecteurV1(ui->TEteTexte);
}

void AllerEditorV1::onTEtextModify()
{
    m_isTextModify = true;
    ui->TEbtnModifier->setText(trUtf8("&Sauvegarde\ndu texte"));
    ui->TEbtnModifier->setEnabled(true);
    m_text->abeSetText(ui->TEteTexte->toPlainText());
}

void AllerEditorV1::saveTextAsFile()
{
    if(!m_isTextModify){
        return;
    }
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QDir().mkpath(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"data");
    //    QString filename = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder le fichier texte modifié ?"), m_directoryWork->absolutePath()+QDir::separator()+"data"+QDir::separator()+m_config->value("fichierTexte").toString(), trUtf8("Fichier texte (*.txt)"));
    //    bool ok;
    QString filename = m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"data/texteEncours";
    //    if (!(m_directoryWork->absolutePath()+QDir::separator()+"data"+QDir::separator()+m_config->value("fichierTexte").toString()).isEmpty())
    //    {
    //    filename = QInputDialog::getText(this,
    //                                     trUtf8("Sauvegarder le fichier texte modifié ?"),
    //                                     trUtf8("Nom du fichier texte :"), QLineEdit::Normal,
    //                                     m_config->value("fichierTexte").toString(),
    //                                     &ok);
    //    }
    //    else
    //    {
    //        filename = QInputDialog::getText(this,
    //                                         trUtf8("Sauvegarder le fichier texte modifié ?"),
    //                                         trUtf8("Nom du fichier texte :"),
    //                                         QLineEdit::Normal,
    //                                         QString(),
    //                                         &ok);
    //    }
    //    if (!ok || filename.isEmpty())
    //    {
    //        return;
    //    }
    //    if (filename.right(4) != ".txt")
    //        filename.append(".txt");
    //    filename.prepend(m_directoryWork->absolutePath()+QDir::separator()+"data"+QDir::separator());
    QFile file(filename);
    //On sauvegarde le fichier
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
//        ABULEDU_LOG_DEBUG() << "Erreur d'ouverture en ecriture du fichier texte";
        return;
    }
    QTextStream writeData(&file);
    writeData.setCodec("UTF-8");
    writeData << ui->TEteTexte->toPlainText();
    file.close();
    //    m_config->setValue("fichierTexte",filename);
    //    AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Fichier sauvegardé"),trUtf8("Votre texte a été sauvegardé"));
    //    messageBox->setWink();
    //    messageBox->show();
    QFileInfo fi(filename);
    this->setWindowTitle(abeApp->getAbeApplicationLongName()+" : "+fi.fileName()+"[*]");
    this->setWindowModified(false);
    m_config->setValue("fichierTexte",fi.fileName());
//    ABULEDU_LOG_DEBUG()<<"QString filename = "<<filename<<" et QFileInfo fi = "<<fi.fileName();
    m_isTextModify = false;
    ui->TEbtnNettoyer->setEnabled(false);
    on_TEbtnNettoyer_clicked();
    ui->TEteTexte->setReadOnly(true);
    ui->TEbtnModifier->setText(trUtf8("&Modifier\nle texte"));
    m_text->abeSetText(ui->TEteTexte->toPlainText());

    /* Mise à jour des variables membres liées au texte */
    m_listeMotsIndexes = m_text->abetextMotsPonctues();
//    ABULEDU_LOG_DEBUG()<<"liste mots indexés";
//    ABULEDU_LOG_DEBUG()<<m_listeMotsIndexes;
    m_numberWords = m_text->abetextCompteMotsPonctues();
    m_numberSentences = m_text->abetextComptePhrases();
    m_numberParagraphs = m_text->abetextParagraphes().count();
    /* Je mets les lignes ci-dessous à cet endroit, puisqu'avant m_listeMotsIndexes n'est pas remplie... */
    ui->RRsbMotDebut->setMaximum(m_listeMotsIndexes.count());
    ui->RRsbMotFin->setMaximum(m_listeMotsIndexes.count());
    ui->RRsbMotFin->setValue(m_listeMotsIndexes.count());

    on_RRsbMotDebut_valueChanged(0);
    on_RRsbMotFin_valueChanged(m_listeMotsIndexes.count());
}

QStringList AllerEditorV1::recursiveRide(QString directoryName)
{
    QStringList result;
    QDir dir(directoryName);
    //Attention a ne pas prendre le repertoire "." et ".."
    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        if(fileInfo.isDir()) {
            //C'est ici que le parcours est récursif
            result << recursiveRide(fileInfo.absoluteFilePath());
        }
        else {
            if(!fileInfo.baseName().contains("aeffacer")){
                result << fileInfo.absoluteFilePath();
            }
        }
    }
    return result;
}


QPair<QStringList, QString> AllerEditorV1::matchHolesWords(QString text, QString holes,int beginning, int end)
{
    QStringList sl;
    /* Je crée un QTextEdit avec le texte de la page Texte */
    QTextEdit* te = new QTextEdit();
    te->setPlainText(text);
    /* Je limite le contenu aux valeurs de marqueur début et fin de texte */
    QTextCursor curseur(te->document());
    if(beginning == -1){
        curseur.setPosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        beginning = 0;
    }
    else {
        curseur.setPosition(beginning,QTextCursor::MoveAnchor);

    }
    if(end == -1){
        curseur.setPosition(QTextCursor::End,QTextCursor::KeepAnchor);
    }
    else {
        curseur.setPosition(end,QTextCursor::KeepAnchor);
    }
    te->setText(curseur.selectedText());
    QString texte = te->toPlainText();
    QStringList tabTrous(holes.split(";",QString::SkipEmptyParts));
    if(tabTrous.size() > 1)
    {
        for(int i = 0; i < tabTrous.size()-1; i+=2)
        {
            curseur.setPosition(tabTrous.at(i).toInt()-beginning, QTextCursor::MoveAnchor);
            curseur.setPosition(tabTrous.at(i+1).toInt()-beginning, QTextCursor::KeepAnchor);
            texte.replace(tabTrous.at(i).toInt()-beginning,tabTrous.at(i+1).toInt() - tabTrous.at(i).toInt(),QString("_").repeated(curseur.selectedText().length()));

            sl << curseur.selectedText();
        }
    }
    te->deleteLater();
    return qMakePair(sl,texte);
}


void AllerEditorV1::on_TTRbtnChoixMots_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(ui->wAutreAffichage);
    ui->twEditor->blockSignals(false);

    ui->btnWidgetPrecedent->setEnabled(false);
    ui->btnWigdetSuivant->setEnabled(false);

    m_previousStart = 0;
    m_lastSelection = QPair<int,int>(0,0);

    int fin = ui->TTRsbMarqueurFinTexte->value();
    if(fin > ui->TEteTexte->toPlainText().size()) {
        fin = ui->TEteTexte->toPlainText().size();
        ui->TTRsbMarqueurFinTexte->setMaximum(fin);
        ui->TTRsbMarqueurFinTexte->setValue(fin);
    }
    m_displayedObjectsMap.clear();
    matchHolesWords(ui->TEteTexte->toHtml(),ui->TTRlblTrousManuel->text(),ui->TTRsbMarqueurDebutTexte->value(),ui->TTRsbMarqueurFinTexte->value());

    foreach(AbulEduTextEditV1* w,ui->wAutreAffichage->findChildren<AbulEduTextEditV1*>()){
        w->deleteLater();
    }
    m_textEditForHoles = new AbulEduTextEditV1(ui->TEteTexte->toHtml(),0,ui->scrollAreaAutreAffichageContents);
    m_textEditForHoles->abeSetIgnoreStyleSheet();
    m_textEditForHoles->installEventFilter(this);
    QTextCursor curseur(m_textEditForHoles->document());
    curseur.setPosition(ui->TTRsbMarqueurDebutTexte->value(),QTextCursor::MoveAnchor);
    curseur.setPosition(fin,QTextCursor::KeepAnchor);
    m_textEditForHoles->setText(curseur.selectedText());
    m_textEditForHoles->zoomIn(2);
    m_holeArray.clear();
    //    QStringList tabTrous(ui->TTRlblTrousManuel->text().split(";",QString::SkipEmptyParts));
    //    if(tabTrous.size() > 1)
    //    {
    //            QTextCursor curseur(m_textEditForHoles->document());
    //        for(int i = 0; i < tabTrous.size()-1; i+=2)
    //        {
    //            curseur.setPosition(tabTrous.at(i).toInt(), QTextCursor::MoveAnchor);
    //            curseur.setPosition(tabTrous.at(i+1).toInt(), QTextCursor::KeepAnchor);
    //            curseur.setCharFormat(m_textFormatSelected);
    //            m_holeArray.append(QPair<int,int>(tabTrous.at(i).toInt(),tabTrous.at(i+1).toInt()));
    //        }
    //    }
    connect(m_textEditForHoles, SIGNAL(selectionChanged()),this, SLOT(onTTRtextSelected()),Qt::UniqueConnection);
    m_textEditForHoles->setFixedSize(ui->wAutreAffichage->size());
    m_textEditForHoles->setVisible(true);
}

void AllerEditorV1::onTTRtextSelected()
{
    m_textEditForHoles->blockSignals(true);
    QTextCursor curseur(m_textEditForHoles->document());
    if(m_lastSelection.first != m_lastSelection.second)
    {
        int cursorPos = curseur.position();
        curseur.setPosition(m_lastSelection.first,QTextCursor::MoveAnchor);
        curseur.setPosition(m_lastSelection.second,QTextCursor::KeepAnchor);
        curseur.setCharFormat(m_textFormatUnSelected);
        curseur.setPosition(cursorPos);
    }
    QPair<int,int> listeTamponTrous;
    int debut = m_textEditForHoles->textCursor().selectionStart();
    if (m_previousStart != debut)
    {
        m_previousStart = debut;
    }
    int fin = m_textEditForHoles->textCursor().selectionEnd();
//    ABULEDU_LOG_DEBUG()<<"PreviousStart : "<<m_previousStart<<", Debut : "<<debut<<" et fin : "<<fin;

    if (debut < fin)
    {
        //        if (debut == m_previousStart)
        //        {
        //            if (!listeTamponTrous.isEmpty())
        //                listeTamponTrous.removeLast();
        //        }
        listeTamponTrous= QPair<int, int>(debut,fin);
//        ABULEDU_LOG_DEBUG()<<"Trou"<<listeTamponTrous;
    }
//    ABULEDU_LOG_DEBUG()<<"Trou ajoute : "<<listeTamponTrous;
    if (listeTamponTrous.first != listeTamponTrous.second)
    {
        if (!m_holeArray.isEmpty())
        {
            if (m_holeArray.last().first == listeTamponTrous.first
                    || m_holeArray.last().second == listeTamponTrous.second
                    || m_holeArray.last().first == listeTamponTrous.second
                    || m_holeArray.last().second == listeTamponTrous.first)
            {
                m_holeArray.removeLast();
            }
        }
        m_holeArray.append(listeTamponTrous);
        m_lastSelection = m_holeArray.last();
    }
//    ABULEDU_LOG_DEBUG()<<"Liste produite : "<<m_holeArray;

    for (int i=0;i<m_holeArray.count();i++){
        curseur.setPosition(m_holeArray.at(i).first,QTextCursor::MoveAnchor);
        curseur.setPosition(m_holeArray.at(i).second,QTextCursor::KeepAnchor);
        curseur.setCharFormat(m_textFormatSelected);
    }
    m_textEditForHoles->blockSignals(false);
}

void AllerEditorV1::on_OAbtnChoixMots_clicked()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    m_previousSelection = ui->OAteMotsModeManuel->toPlainText();
    QMap<int,QString> motsIndexes = m_text->abetextMots();
    int cleDebut = 0;
    int cleFin = motsIndexes.count();

    m_selectedObjectsList.clear();

    foreach(QString motLu,ui->OAteMotsModeManuel->toPlainText().split(";",QString::SkipEmptyParts))
    {
        if(motsIndexes.values().contains(motLu))
        {
            m_selectedObjectsList.append(QString::number(motsIndexes.key(motLu)));
        }
    }
    displayToChoice(motsIndexes,cleDebut,cleFin,Qt::Horizontal);
}

void AllerEditorV1::on_OAteMotsModeManuel_textChanged()
{
    ui->OAsbNbMotsManuel->setMaximum(ui->OAteMotsModeManuel->toPlainText().split(";",QString::SkipEmptyParts).count());
    ui->OAsbNbMotsManuel->setValue(ui->OAteMotsModeManuel->toPlainText().split(";",QString::SkipEmptyParts).count());
}

void AllerEditorV1::on_MOMbtnChoixMots_clicked()
{
    m_previousSelection = ui->MOMteMotsModeManuel->toPlainText();
    QMap<int,QString> phrasesIndexees = m_text->abetextPhrases();
    int cleDebut = 0;
    int cleFin = phrasesIndexees.count()-1;

    m_selectedObjectsList.clear();

    if (ui->MOMsbNumeroPhrase->value() > 0){
        m_selectedObjectsList.append(QString::number(ui->MOMsbNumeroPhrase->value()-1));
    }
    displayToChoice(phrasesIndexees,cleDebut,cleFin);;
}

void AllerEditorV1::on_MOMteMotsModeManuel_textChanged()
{
    ui->MOMsbNumeroPhrase->setValue(-1);
}

void AllerEditorV1::on_MOMsbNumeroPhrase_valueChanged(int arg1)
{
    ui->MOMteMotsModeManuel->blockSignals(true);
    if (arg1 > 0){
        ui->MOMteMotsModeManuel->setText(m_text->abetextPhrases().value(arg1-1));
    }
    ui->MOMteMotsModeManuel->blockSignals(false);
}

void AllerEditorV1::on_PHMbtnChoixPhrasesManuel_clicked()
{
    m_previousSelection = ui->PHMtePhrasesModeManuel->toPlainText();
    QMap<int,QString> phrasesIndexees = m_text->abetextPhrases();
    int cleDebut = ui->PHMsbPhraseDebutManuel->value()-1;
    int cleFin = ui->PHMsbPhraseFinManuel->value()-1;

    m_selectedObjectsList.clear();

    foreach (QString nombre, ui->PHMtePhrasesModeManuel->toPlainText().split(";",QString::SkipEmptyParts)) {
        int nb = nombre.toInt()-1;
        m_selectedObjectsList.append(QString::number(nb));
    }
    displayToChoice(phrasesIndexees,cleDebut,cleFin);
}

void AllerEditorV1::on_PHMtePhrasesModeManuel_textChanged()
{
    ui->PHMsbNbPhrasesUtiliseesManuel->setMaximum(ui->PHMtePhrasesModeManuel->toPlainText().split(";",QString::SkipEmptyParts).count());
    ui->PHMsbNbPhrasesUtiliseesManuel->setValue(ui->PHMtePhrasesModeManuel->toPlainText().split(";",QString::SkipEmptyParts).count());
}

void AllerEditorV1::on_PAMbtnChoixParagraphes_clicked()
{
    m_previousSelection = ui->PAMleParagraphesChoisis->text();
    QMap<int,QString> paragraphesIndexes = m_text->abetextParagraphes();
    int cleDebut = ui->PAMsbParagrapheDebutManuel->value()-1;
    int cleFin = ui->PAMsbParagrapheFinManuel->value()-1;

    m_selectedObjectsList.clear();

    foreach (QString nombre, ui->PAMleParagraphesChoisis->text().split(";",QString::SkipEmptyParts))
    {
        int nb = nombre.toInt()-1;
        m_selectedObjectsList.append(QString::number(nb));
    }
    displayToChoice(paragraphesIndexes,cleDebut,cleFin);
}

void AllerEditorV1::on_PSEbtnChoixPhrase_clicked()
{
    m_previousSelection = ui->PSEtePhraseModeManuel->toPlainText();
    QMap<int,QString> phrasesIndexees = m_text->abetextPhrases();
    int cleDebut = 0;
    int cleFin = phrasesIndexees.count()-1;

    m_selectedObjectsList.clear();

    if (ui->PSEsbNumeroPhrase->value() > 0) {
        m_selectedObjectsList.append(QString::number(ui->PSEsbNumeroPhrase->value()-1));
    }
    displayToChoice(phrasesIndexees,cleDebut,cleFin);;
}

void AllerEditorV1::on_PSEtePhraseModeManuel_textChanged()
{
    ui->PSEsbNumeroPhrase->setValue(-1);
}

void AllerEditorV1::on_PSEsbNumeroPhrase_valueChanged(int arg1)
{
    ui->PSEtePhraseModeManuel->blockSignals(true);
    if (arg1 > 0)
    {
        ui->PSEtePhraseModeManuel->setText(m_text->abetextPhrases().value(arg1-1));
    }
    ui->PSEtePhraseModeManuel->blockSignals(false);
}

void AllerEditorV1::on_RRbtnChoixMots_clicked()
{
    QMap<int,QString> motsIndexes = m_text->abetextMotsPonctues();
    int cleDebut = ui->RRsbMotDebut->value()-1;
    int cleFin = ui->RRsbMotFin->value()-1;

    m_selectedObjectsList.clear();

    foreach(QString motLu,ui->RRteMotsModeManuel->toPlainText().split(";",QString::SkipEmptyParts)){
        if(motsIndexes.values().contains(motLu)){
            m_selectedObjectsList.append(QString::number(motsIndexes.key(motLu)));
        }
    }

    displayToChoice(motsIndexes,cleDebut,cleFin,Qt::Horizontal);
}

void AllerEditorV1::on_RRteMotsModeManuel_textChanged()
{
    ui->RRsbNbMotsManuel->setMaximum(ui->RRteMotsModeManuel->toPlainText().split(";",QString::SkipEmptyParts).count());
    ui->RRsbNbMotsManuel->setValue(ui->RRteMotsModeManuel->toPlainText().split(";",QString::SkipEmptyParts).count());
}

void AllerEditorV1::onAbuleduLabelSelected(int rangRecu)
{
    //    int indexWidget = ui->twEditor->currentWidget()->property("rang").toInt();

//    ABULEDU_LOG_DEBUG()<<"onAbuleduLabelSelected("<<rangRecu<<") avec "<<m_selectedObjectsList;
//    ABULEDU_LOG_DEBUG()<<trUtf8("On a cliqué sur le mot, phrase ou paragraphe n°")<<rangRecu+1;
    if (m_selectedObjectsList.contains(QString::number(rangRecu)))
        m_selectedObjectsList.removeOne(QString::number(rangRecu));
    else
    {
        if ((m_previousWidget == ui->wMotsMelanges|| m_previousWidget == ui->wPhrasesEspaces) && !m_selectedObjectsList.isEmpty())
        {
            int precedentIndex = m_selectedObjectsList.last().toInt();
            m_displayedObjectsMap.value(precedentIndex)->setStyleSheet(abe::STYLEBLANC);
            m_selectedObjectsList.clear();
        }
        m_selectedObjectsList << QString::number(rangRecu);
    }

    ABULEDU_LOG_TRACE()<<"onAbuleduLabelSelected(fin) avec "<<m_selectedObjectsList;
}

void AllerEditorV1::displayToChoice(QMap<int,QString> indexed, int begining, int end, int direction)
{
    ABULEDU_LOG_TRACE()<<"AllerEditorV1::displayToChoice "<<indexed;
//    ABULEDU_LOG_DEBUG()<<"Selection prealable : "<<m_selectedObjectsList;
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(ui->wAutreAffichage);
    ui->twEditor->blockSignals(false);
    m_displayedObjectsMap.clear();

    ui->btnWidgetPrecedent->setEnabled(false);
    ui->btnWigdetSuivant->setEnabled(false);

    foreach(AbulEduTextEditV1* w,ui->scrollAreaAutreAffichageContents->findChildren<AbulEduTextEditV1*>()){
//        ABULEDU_LOG_DEBUG()<<"destruction de "<<w->toPlainText();
        w->deleteLater();
    }
    int margeAutourEtiquettes = 10;
    int largeurDispoPourEtiquette = ui->scrollAreaAutreAffichageContents->width()-2*margeAutourEtiquettes;
    int espaceMots = 2;
    int horizontalPadding = 12;
    int verticalPadding = 5;
    QFontMetrics mesureur(abeApp->font());

    //    int largeurWidget = ui->scrollAreaAutreAffichageContents->width();

    if (direction == Qt::Horizontal){
        ui->scrollAreaAutreAffichageContents->setStyleSheet(abe::STYLEBLANC);
    }
    else{
        ui->scrollAreaAutreAffichageContents->setStyleSheet(QString());
    }

    int hauteurTotaleEtiquettes = 0;
    int x = margeAutourEtiquettes;
    int y = margeAutourEtiquettes;
    QStringList copieListeSelection;
    for (int i=begining;i<=end;i++)
    {
        if (indexed.value(i) != "retour_a_la_ligne" && !indexed.value(i).isEmpty())
        {
            AbulEduTextEditV1* elt = new AbulEduTextEditV1(indexed.value(i), i,ui->scrollAreaAutreAffichageContents);
//            ABULEDU_LOG_DEBUG()<<i<<"création pour "<<indexed.value(i);
            elt->setVisible(true);
            elt->setReadOnly(true);
            elt->abeSetMInitialStyleSheet(abe::STYLEBLANC);
            if (m_selectedObjectsList.contains(QString::number(i)))
            {
                elt->setStyleSheet(abe::STYLESELECTION);
            }
            connect(elt, SIGNAL(AbulEduTextClicked(int)),this, SLOT(onAbuleduLabelSelected(int)),Qt::UniqueConnection);
            if (direction == Qt::Vertical){
                //                    ABULEDU_LOG_DEBUG()<<"Qt::Vertical";
                int nombreLignes = mesureur.width(elt->toPlainText())/largeurDispoPourEtiquette +1;
                elt->setFixedSize(largeurDispoPourEtiquette,nombreLignes*mesureur.height()+margeAutourEtiquettes);
                hauteurTotaleEtiquettes += elt->height()+verticalPadding;
                elt->move(x,y);
                y += elt->height()+verticalPadding;
                m_displayedObjectsMap.insert(i,elt);
                ui->scrollAreaAutreAffichageContents->setFixedSize(largeurDispoPourEtiquette,hauteurTotaleEtiquettes + verticalPadding);
            }
            else
            {
                //            if (m_localDebug) ABULEDU_LOG_DEBUG()<<"Qt::Horizontal";
                largeurDispoPourEtiquette = this->width()-ui->saAutreAffichage->verticalScrollBar()->width() - 20;

                if (copieListeSelection.contains(elt->toPlainText()))
                {
                    elt->setStyleSheet(abe::STYLESELECTION);
                    copieListeSelection.removeOne(elt->toPlainText());
                }

                elt->setWordWrapMode(QTextOption::NoWrap);
                elt->setFrameShape(QTextEdit::Box);
                elt->setFrameShadow(QTextEdit::Plain);
                elt->setAlignment(Qt::AlignCenter);
                elt->setFixedSize(mesureur.width(elt->toPlainText())+horizontalPadding,mesureur.height()+verticalPadding);
                elt->move(x,y);
                //                    ABULEDU_LOG_DEBUG()<<"Element de largeur "<<elt->width()<<"et hauteur "<<elt->height()<<" place en ("<<elt->x()<<" X "<<elt->y()<<")";
                if ((x + elt->width()) > largeurDispoPourEtiquette)
                {
                    x = margeAutourEtiquettes;
                    y += mesureur.height() + margeAutourEtiquettes;
                }
                else
                {
                    x += elt->width()+espaceMots;
                }

                m_displayedObjectsMap.insert(i,elt);
                //                if (i==end-1){
                //                    y += mesureur.height();
                //                }
                ui->scrollAreaAutreAffichageContents->setFixedSize(this->width() - 20  , y+mesureur.height() + margeAutourEtiquettes);
            }
        }
    }

//    ABULEDU_LOG_DEBUG()<<"Content : ("<<ui->scrollAreaWidgetContents->width()<<" X "<<ui->scrollAreaWidgetContents->height()<<")";
}

/* --------------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------- Boutons "Ajouter un exercice" --------------------------------------------- */

void AllerEditorV1::on_TTRbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExerciceTexteTrous;
    if(m_nombreExerciceTexteTrous >= 4){
        ui->TTRbtnAjouterExercice->setEnabled(false);
    }
    else{
        ui->TTRbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExerciceTexteTrous >= 5){
        return;
    }
    ui->TTRfrParametresExercice->setVisible(true);
    m_nombreExerciceTexteTrous++;
    if (m_nombreExerciceTexteTrous <= 1){
        ui->TTRbtnDeleteCurrentExercise->setEnabled(false);
    }
    else{
        ui->TTRbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->TTRcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExerciceTexteTrous)));
    ui->TTRcbNumeroExercice->setCurrentIndex(m_nombreExerciceTexteTrous - 1);
    ui->TTRrbModeManuel->setChecked(false);
    ui->TTRrbModeIntelligent->setChecked(false);
    ui->TTRrbModeAutomatique->setChecked(true);
    ui->TTRfrModeAutomatique->setVisible(true);
    ui->TEteTexte->setFocus();
    QTextCursor curseur= ui->TEteTexte->textCursor();
    curseur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    ui->TTRsbMarqueurFinTexte->setValue(curseur.position());
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("texteATrous");
    m_config->beginReadArray("exercice");
    {
        m_config->setArrayIndex(m_nombreExerciceTexteTrous);
        if (m_config->value("marqueurFinTexte").toString() != "")
        {
            ui->TTRsbMarqueurFinTexte->setValue(m_config->value("marqueurFinTexte").toInt());
        }
    }
    m_config->endArray();
    m_config->endGroup();
}

void AllerEditorV1::on_OAbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExerciceOrdreAlphabetique;
    if(m_nombreExerciceOrdreAlphabetique >= 4)
    {
        ui->OAbtnAjouterExercice->setEnabled(false);
    }
    else
    {
        ui->OAbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExerciceOrdreAlphabetique >= 5)
    {
        return;
    }
    ui->OAfrParametresExercice->setVisible(true);
    if (m_nombreExerciceOrdreAlphabetique == 0)
        ui->OAbtnAjouterExercice->setText(trUtf8("Ajouter un exercice"));
    m_nombreExerciceOrdreAlphabetique++;
    if (m_nombreExerciceOrdreAlphabetique <= 1)
    {
        ui->OAbtnDeleteCurrentExercise->setEnabled(false);
    }
    else
    {
        ui->OAbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->OAcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExerciceOrdreAlphabetique)));
    ui->OAcbNumeroExercice->setCurrentIndex(m_nombreExerciceOrdreAlphabetique - 1);
    ui->OArbModeManuel->setChecked(false);
    ui->OArbModeIntelligent->setChecked(false);
    ui->OArbModeAutomatique->setChecked(true);
    ui->OAsbNbMotsAutomatique->setValue(5);
    ui->OArbModeScolaire->setChecked(false);
}

void AllerEditorV1::on_MOMbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExerciceMotsMelanges;
    if(m_nombreExerciceMotsMelanges >= 4)
    {
        ui->MOMbtnAjouterExercice->setEnabled(false);
    }
    else
    {
        ui->MOMbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExerciceMotsMelanges >= 5)
    {
        return;
    }
    ui->MOMfrParametresExercice->setVisible(true);
    if (m_nombreExerciceMotsMelanges == 0)
        ui->MOMbtnAjouterExercice->setText(trUtf8("Ajouter un exercice"));
    m_nombreExerciceMotsMelanges++;
    if (m_nombreExerciceMotsMelanges <= 1)
    {
        ui->MOMbtnDeleteCurrentExercise->setEnabled(false);
    }
    else
    {
        ui->MOMbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->MOMcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExerciceMotsMelanges)));
    ui->MOMcbNumeroExercice->setCurrentIndex(m_nombreExerciceMotsMelanges - 1);
    ui->MOMrbModeManuel->setChecked(false);
    ui->MOMrbModeAutomatique->setChecked(true);
}

void AllerEditorV1::on_PHMbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExercicePhrasesMelangees;
    if(m_nombreExercicePhrasesMelangees >= 4)
    {
        ui->PHMbtnAjouterExercice->setEnabled(false);
    }
    else
    {
        ui->PHMbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExercicePhrasesMelangees >= 5)
    {
        return;
    }
    ui->PHMfrParametresExercice->setVisible(true);
    if (m_nombreExercicePhrasesMelangees == 0)
        ui->PHMbtnAjouterExercice->setText(trUtf8("Ajouter un exercice"));
    m_nombreExercicePhrasesMelangees++;
    if (m_nombreExercicePhrasesMelangees <= 1)
    {
        ui->PHMbtnDeleteCurrentExercise->setEnabled(false);
    }
    else
    {
        ui->PHMbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->PHMcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExercicePhrasesMelangees)));
    ui->PHMcbNumeroExercice->setCurrentIndex(m_nombreExercicePhrasesMelangees - 1);
    ui->PHMrbModeManuel->setChecked(false);
    ui->PHMrbModeAutomatique->setChecked(true);
    ui->PHMsbPhraseDebutAutomatique->setValue(1);
    ui->PHMsbPhraseDebutManuel->setValue(1);
    ui->PHMsbPhraseFinAutomatique->setValue(m_numberSentences);
    ui->PHMsbPhraseFinManuel->setValue(m_numberSentences);
}

void AllerEditorV1::on_PAMbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExerciceParagraphesMelanges;
    if(m_nombreExerciceParagraphesMelanges >= 4)
    {
        ui->PAMbtnAjouterExercice->setEnabled(false);
    }
    else
    {
        ui->PAMbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExerciceParagraphesMelanges >= 5)
    {
        return;
    }
    ui->PAMfrParametresExercice->setVisible(true);
    if (m_nombreExerciceParagraphesMelanges == 0)
        ui->PAMbtnAjouterExercice->setText(trUtf8("Ajouter un exercice"));
    m_nombreExerciceParagraphesMelanges++;
    if (m_nombreExerciceParagraphesMelanges <= 1)
    {
        ui->PAMbtnDeleteCurrentExercise->setEnabled(false);
    }
    else
    {
        ui->PAMbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->PAMcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExerciceParagraphesMelanges)));
    ui->PAMcbNumeroExercice->setCurrentIndex(m_nombreExerciceParagraphesMelanges - 1);
    ui->PAMsbDifficulte->setValue(1);
    ui->PAMrbModeManuel->setChecked(false);
    ui->PAMrbModeAuto->setChecked(true);
    ui->PAMsbParagrapheDebutAuto->setValue(1);
    if (m_text) ui->PAMsbParagrapheFinAuto->setValue(m_text->abetextParagraphes().count());
    ui->PAMleParagraphesChoisis->clear();
    ui->PAMchbAfficherConsigneExercice->setChecked(false);
    ui->PAMteConsigneExercice->clear();
}

void AllerEditorV1::on_PSEbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExercicePhrasesSansEspace;
    if(m_nombreExercicePhrasesSansEspace >= 4)
    {
        ui->PSEbtnAjouterExercice->setEnabled(false);
    }
    else
    {
        ui->PSEbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExercicePhrasesSansEspace >= 5)
    {
        return;
    }
    ui->PSEfrParametresExercice->setVisible(true);
    if (m_nombreExercicePhrasesSansEspace == 0)
        ui->PSEbtnAjouterExercice->setText(trUtf8("Ajouter un exercice"));
    m_nombreExercicePhrasesSansEspace++;
    if (m_nombreExercicePhrasesSansEspace <= 1)
    {
        ui->PSEbtnDeleteCurrentExercise->setEnabled(false);
    }
    else
    {
        ui->PSEbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->PSEcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExercicePhrasesSansEspace)));
    ui->PSEcbNumeroExercice->setCurrentIndex(m_nombreExercicePhrasesSansEspace - 1);
    ui->PSEsbDifficulte->setValue(1);
    ui->PSEsbNumeroPhrase->setValue(1);
    ui->PSErbModeManuel->setChecked(false);
    ui->PSErbModeAutomatique->setChecked(true);
    ui->PSEchbAfficherConsigneExercice->setChecked(false);
    ui->PSEchbAfficherPhrase->setChecked(false);
    ui->PSEteConsigneExercice->clear();
}

void AllerEditorV1::on_RRbtnAjouterExercice_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<m_nombreExerciceRechercheRapide;
    if(m_nombreExerciceRechercheRapide >= 4)
    {
        ui->RRbtnAjouterExercice->setEnabled(false);
    }
    else
    {
        ui->RRbtnAjouterExercice->setEnabled(true);
    }
    if(m_nombreExerciceRechercheRapide >= 5)
    {
        return;
    }
    ui->RRfrParametresExercice->setVisible(true);
    if (m_nombreExerciceRechercheRapide == 0)
        ui->RRbtnAjouterExercice->setText(trUtf8("Ajouter un exercice"));
    m_nombreExerciceRechercheRapide++;
    if (m_nombreExerciceRechercheRapide <= 1)
    {
        ui->RRbtnDeleteCurrentExercise->setEnabled(false);
    }
    else
    {
        ui->RRbtnDeleteCurrentExercise->setEnabled(true);
    }
    ui->RRcbNumeroExercice->addItem(trUtf8("Exercice %1").arg(QString::number(m_nombreExerciceRechercheRapide)));
    ui->RRcbNumeroExercice->setCurrentIndex(m_nombreExerciceRechercheRapide - 1);
    ui->RRsbTempsAccorde->setValue(10);
    ui->RRsbNbMotsAutomatique->setValue(10);
    ui->RRrbModeManuel->setChecked(false);
    ui->RRrbModeIntelligent->setChecked(false);
    ui->RRrbModeAutomatique->setChecked(true);
}
/* ------------------------------------------------------------------------------------------------------------------------- */

/* ------------------------------------ Slots changements dans les comboBox "Numéro Exercice  -------------------------------------- */

void AllerEditorV1::on_TTRcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wClosure,m_previousExercice);
    loadSettings(ui->wClosure,index);
    m_previousExercice = index;
}

void AllerEditorV1::on_OAcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wOrdreAlpha,m_previousExercice);
    loadSettings(ui->wOrdreAlpha,index);
    m_previousExercice = index;
}

void AllerEditorV1::on_MOMcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }
    
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wMotsMelanges,m_previousExercice);
    loadSettings(ui->wMotsMelanges,index);
    m_previousExercice = index;
}

void AllerEditorV1::on_PHMcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wPhrasesMelangees,m_previousExercice);
    loadSettings(ui->wPhrasesMelangees,index);
    m_previousExercice = index;
}

void AllerEditorV1::on_PAMcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wParagraphes,m_previousExercice);
    loadSettings(ui->wParagraphes,index);
    m_previousExercice = index;
}

void AllerEditorV1::on_PSEcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wPhrasesEspaces,m_previousExercice);
    loadSettings(ui->wPhrasesEspaces,index);
    m_previousExercice = index;
}

void AllerEditorV1::on_RRcbNumeroExercice_currentIndexChanged(int index)
{
    if(m_previousExercice == index){
        return;
    }

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    saveSettings(ui->wRechercheRapide,m_previousExercice);
    loadSettings(ui->wRechercheRapide,index);
    m_previousExercice = index;
}
/* ---------------------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------- Paramétrage par défaut des onglets -------------------------------------------------- */

void AllerEditorV1::setDefaultSettingsTexteTrous()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    connect(ui->TTRrbModeManuel,SIGNAL(toggled(bool)),ui->TTRfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->TTRrbModeIntelligent, SIGNAL(toggled(bool)), ui->TTRfrModeIntelligent, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->TTRrbModeAutomatique, SIGNAL(toggled(bool)), ui->TTRfrModeAutomatique, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->TTRteTrousModeManuel->setEnabled(false);
    ui->TTRlblTrousManuel->setVisible(false);
    ui->TTRcbNiveau->addItems(abe::ABE_DONNENIVEAU().values());
    ui->TTRcbNature->addItems(abe::ABE_DONNENATURE().values());
    ui->TTRchbAfficherMotsBienPlaces->setChecked(true);
    ui->TTRabeInstruction->setChecked(true);
    ui->TTRabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->TTRabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Guide avec ta souris les mots enlevés vers leur place dans le texte"), instructionPath + "ttrConsigneSequence.ogg");
    /** @todo supprimer les lignes ci-dessous : les slots ne sont pas encore supprimés mais ne font plus rien */
    connect(ui->TTRchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->TTRteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->TTRteConsigneExercice->setVisible(ui->TTRchbAfficherConsigneExercice->isChecked());
//    ui->wClosure->setEnabled(false);
    connect(ui->GEcbTexteTrous, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wClosure);
    ui->lblTTR->hide();
}

void AllerEditorV1::setDefaultSettingsOrdreAlphabetique()
{
    connect(ui->OArbModeManuel,SIGNAL(toggled(bool)),ui->OAfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->OArbModeIntelligent, SIGNAL(toggled(bool)), ui->OAfrModeIntelligent, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->OArbModeAutomatique, SIGNAL(toggled(bool)), ui->OAfrModeAutomatique, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->OArbModeScolaire, SIGNAL(toggled(bool)), ui->OAfrModeScolaire, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->OArbModeScolaire->setVisible(false);
    ui->OAfrModeScolaire->setVisible(false);
    ui->OAcbNiveau->addItems(abe::ABE_DONNENIVEAU().values());
    ui->OAcbNature->addItems(abe::ABE_DONNENATURE().values());
    ui->OAcbNiveauScolaire->addItems(abe::ABE_DONNENIVEAU().values());
    ui->OAcbNatureScolaire->addItems(abe::ABE_DONNENATURE().values());
    ui->OAchbAfficherMotsBienPlaces->setChecked(true);
    ui->OAabeInstruction->setChecked(true);
    ui->OAabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->OAabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Utilise les cases pour ranger les mots proposés dans l'ordre alphabétique."),instructionPath + "oaConsigneSequence.ogg");
    connect(ui->OAchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->OAteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->OAteConsigneExercice->setVisible(ui->OAchbAfficherConsigneExercice->isChecked());
    //    ui->wOrdreAlpha->setEnabled(false);
    connect(ui->GEcbOrdreAlphabetique, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wOrdreAlpha);
    ui->lblOA->hide();
}

void AllerEditorV1::setDefaultSettingsMotsMelanges()
{
    connect(ui->MOMrbModeManuel,SIGNAL(toggled(bool)),ui->MOMfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->MOMchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->MOMteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->MOMrbModeAutomatique, SIGNAL(toggled(bool)),ui->MOMsbNbMotsMaxAuto, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->MOMrbModeAutomatique, SIGNAL(toggled(bool)),ui->MOMlblNbMotsMaxAuto, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->MOMabeInstruction->setChecked(true);
    ui->MOMabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->MOMabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Remets en ordre les mots proposés pour reconstituer une phrase du texte."),instructionPath + "momConsigneSequence.ogg");
    ui->MOMteConsigneExercice->setVisible(ui->MOMchbAfficherConsigneExercice->isChecked());
    ui->MOMsbNumeroPhrase->setMaximum(m_numberSentences);
    ui->MOMchbAfficherMotsBienPlaces->setChecked(true);
    //    ui->wMotsMelanges->setEnabled(false);
    connect(ui->GEcbMotsMelanges, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wMotsMelanges);
    ui->lblMOM->hide();
}

void AllerEditorV1::setDefaultSettingsPhrasesMelangees()
{
    connect(ui->PHMrbModeManuel,SIGNAL(toggled(bool)),ui->PHMfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->PHMrbModeAutomatique, SIGNAL(toggled(bool)), ui->PHMfrModeAutomatique, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->PHMabeInstruction->setChecked(true);
    ui->PHMabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->PHMabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Remets les phrases proposées dans l'ordre du texte."),instructionPath + "phmConsigneSequence.ogg");
    connect(ui->PHMchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->PHMteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->PHMteConsigneExercice->setVisible(ui->PHMchbAfficherConsigneExercice->isChecked());
    ui->PHMsbPhraseDebutAutomatique->setMinimum(1);
    ui->PHMsbPhraseDebutAutomatique->setValue(1);
    ui->PHMsbPhraseDebutManuel->setMinimum(1);
    ui->PHMsbPhraseDebutManuel->setValue(1);
    ui->PHMsbPhraseFinAutomatique->setMaximum(m_numberSentences);
    ui->PHMsbPhraseFinAutomatique->setValue(m_numberSentences);
    ui->PHMsbPhraseFinManuel->setMaximum(m_numberSentences);
    ui->PHMsbPhraseFinManuel->setValue(m_numberSentences);
    connect(ui->PHMsbPhraseDebutAutomatique, SIGNAL(valueChanged(int)),this,SLOT(adjustPHMsbValues(int)),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseFinAutomatique, SIGNAL(valueChanged(int)),this,SLOT(adjustPHMsbValues(int)),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseDebutManuel, SIGNAL(valueChanged(int)),this,SLOT(adjustPHMsbValues(int)),Qt::UniqueConnection);
    connect(ui->PHMsbPhraseFinManuel, SIGNAL(valueChanged(int)),this,SLOT(adjustPHMsbValues(int)),Qt::UniqueConnection);
    ui->PHMchbAfficherPhrasesBienPlacees->setChecked(true);
    //    ui->wPhrasesMelangees->setEnabled(false);
    ui->PHMchbPhrasesConsecutivesAutomatique->setChecked(true);
    connect(ui->GEcbPhrasesMelangees, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wPhrasesMelangees);
    ui->lblPHM->hide();
}

void AllerEditorV1::setDefaultSettingsParagraphesMelanges()
{
    connect(ui->PAMrbModeManuel,SIGNAL(toggled(bool)),ui->PAMfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->PAMrbModeAuto, SIGNAL(toggled(bool)), ui->PAMfrModeAuto, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->PAMabeInstruction->setChecked(true);
    ui->PAMabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->PAMabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Remets dans l'ordre du texte les paragraphes présentés."),instructionPath + "pamConsigneSequence.ogg");
    connect(ui->PAMchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->PAMteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->PAMteConsigneExercice->setVisible(ui->PAMchbAfficherConsigneExercice->isChecked());
    ui->PAMsbParagrapheDebutAuto->setMinimum(1);
    ui->PAMsbParagrapheDebutAuto->setValue(1);
    ui->PAMsbParagrapheDebutManuel->setMinimum(1);
    ui->PAMsbParagrapheDebutManuel->setValue(1);
    ui->PAMsbParagrapheFinAuto->setMaximum(m_numberParagraphs);
    ui->PAMsbParagrapheFinAuto->setValue(m_numberParagraphs);
    ui->PAMsbParagrapheFinManuel->setMaximum(m_numberParagraphs);
    ui->PAMsbParagrapheFinManuel->setValue(m_numberParagraphs);
    ui->PAMchbAfficherParagraphesBienPlaces->setChecked(true);
    //    ui->wParagraphes->setEnabled(false);
    connect(ui->GEcbParagraphesMelanges, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wParagraphes);
    ui->lblPAM->hide();
}

void AllerEditorV1::setDefaultSettingsPhrasesSansEspace()
{
    connect(ui->PSErbModeManuel, SIGNAL(toggled(bool)), ui->PSEfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->PSEsbNumeroPhrase->setMaximum(m_text->abetextComptePhrases());
    ui->PSEabeInstruction->setChecked(true);
    ui->PSEabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->PSEabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Sépare les mots dans les phrases affichées."),instructionPath + "pseConsigneSequence.ogg");
    connect(ui->PSEchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->PSEteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->PSEteConsigneExercice->setVisible(ui->PSEchbAfficherConsigneExercice->isChecked());
    ui->PSEchbIgnorerPonctuation->setVisible(false);//Utilisation non implémentée à ce jour
    ui->PSEsbNumeroPhrase->setMaximum(m_numberSentences);
    ui->PSEchbAfficherClicsMalPlaces->setChecked(true);
    //    ui->wPhrasesEspaces->setEnabled(false);
    connect(ui->GEcbPhrasesSansEspace, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wPhrasesEspaces);
    ui->lblPSE->hide();
}

void AllerEditorV1::setDefaultSettingsRechercheRapide()
{
    connect(ui->RRrbModeManuel,SIGNAL(toggled(bool)),ui->RRfrModeManuel, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->RRrbModeIntelligent, SIGNAL(toggled(bool)), ui->RRfrModeIntelligent, SLOT(setVisible(bool)),Qt::UniqueConnection);
    connect(ui->RRrbModeAutomatique, SIGNAL(toggled(bool)), ui->RRfrModeAutomatique, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->RRcbNiveau->addItems(abe::ABE_DONNENIVEAU().values());
    ui->RRcbNature->addItems(abe::ABE_DONNENATURE().values());
    ui->RRabeInstruction->setChecked(true);
    ui->RRabeInstruction->abeInstructionSetTitle(trUtf8("Afficher une consigne pour la séquence"));
//    ui->RRabeInstruction->abeInstructionSetDefaultInstruction(trUtf8("Retrouve dans le texte le mot qui s'affiche en haut et clique sur lui avant la fin du temps imparti."),instructionPath + "rrConsigneSequence.ogg");
    connect(ui->RRchbAfficherConsigneExercice, SIGNAL(toggled(bool)),ui->RRteConsigneExercice, SLOT(setVisible(bool)),Qt::UniqueConnection);
    ui->RRteConsigneExercice->setVisible(ui->RRchbAfficherConsigneExercice->isChecked());
    ui->RRsbMotDebut->setMaximum(m_listeMotsIndexes.count());
    ui->RRsbMotFin->setMaximum(m_listeMotsIndexes.count());
    ui->RRsbMotFin->setValue(m_listeMotsIndexes.count());
    ui->RRchbGarderCouleurMotsPasses->setChecked(true);
    ui->RRteMotsModeManuel->setReadOnly(true);
    if (!m_listeMotsIndexes.isEmpty())
    {
        ui->RRlblMotDebut->setText("("+m_listeMotsIndexes[ui->RRsbMotDebut->value()-1]+")");
        ui->RRlblMotFin->setText("("+m_listeMotsIndexes[ui->RRsbMotFin->value()-1]+")");
    }
    int longueurMotMax = 0;
    for(int i = 0; i < m_listeMotsIndexes.count(); i++)
    {
        int tailleMot = m_listeMotsIndexes.value(i).length();
        if(tailleMot > longueurMotMax)
            longueurMotMax = tailleMot;
    }


//    ABULEDU_LOG_DEBUG()<<"Le mot le plus long mesurera "<<longueurMotMax<<" lettres,";;
    QString m("w");
    QString lemot(m.repeated(longueurMotMax));
    QFontMetrics fm(font());
    //    ui->wRechercheRapide->setEnabled(false);
    connect(ui->GEcbRechercheRapide, SIGNAL(toggled(bool)),this, SLOT(displayWidget(bool)),Qt::UniqueConnection);
    ui->twEditor->removeWidget(ui->wRechercheRapide);
    ui->lblRR->hide();
}

/* --------------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------- Actions du menu -------------------------------------------------------- */

void AllerEditorV1::on_action_Nouveau_triggered()
{

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    MainWindow* parent = (MainWindow*) m_widgetMainwindow;
    m_abuleduFile->abeCleanDirectory(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath(),m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
    m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
    parent->abeSetMyAbulEduFile(m_abuleduFile);
}

void AllerEditorV1::on_actionEditer_un_projet_triggered()
{

    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    m_abuleduFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeOpen);
    m_abuleduFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    //    QRect rec(x(),y()+10,width(),height()-50);
    //    m_abuleduFileManager->setGeometry(rec);
    //    m_abuleduFileManager->show();
    setWindowModified(false);
}

void AllerEditorV1::on_actionEnregistrer_le_projet_en_cours_triggered()
{
    if (prepareSauvegarde())
    {
        m_abuleduFile->abeFileSetTitle(abeApp->sluggify(ui->leTitre->text().simplified()));
        emit editorSave(m_abuleduFile);
        QFile::remove(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/copieModule.conf");
    }
}

void AllerEditorV1::on_actionEnregistrer_le_projet_sous_triggered()
{
    /** @todo différencier la fonction prepareSauvegarde() si le paramètre n'est pas vide */
    on_actionEnregistrer_le_projet_en_cours_triggered();
}

void AllerEditorV1::displayWidget(bool yesNo, QCheckBox *from)
{
//    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<yesNo;
    QCheckBox* chBox;
    if(from){
        chBox = from;
    }
    else {
        chBox = qobject_cast<QCheckBox*>(sender());
    }
    if (chBox == 0){
//        ABULEDU_LOG_DEBUG()<<"Echec conversion du sender en QCheckBox* ";
        return;
    }
    else{
//        ABULEDU_LOG_DEBUG()<<chBox->property("widgetLie").toString();
    }

    QWidget* widgetTraite = findChild<QWidget*>(chBox->property("widgetLie").toString());
    QLabel* labelLie = findChild<QLabel*>(widgetTraite->property("label").toString());
    //    ABULEDU_LOG_DEBUG()<<"J'ai trouvé "<<widgetTraite->objectName()<<" et "<<labelLie->objectName();

    if (yesNo == false){
        ui->twEditor->removeWidget(widgetTraite);
        labelLie->hide();

//        ABULEDU_LOG_DEBUG()<<"J'ai enlevé "<<widgetTraite<<" et caché "<<labelLie;
        updateLabelWidgetName(ui->twEditor->currentIndex());
    }
    else
    {
        int i = 0;
        bool trouvePlace = false;
        while (i<ui->twEditor->count() && trouvePlace == false)
        {
            if (ui->twEditor->widget(i)->property("rang").toInt() < widgetTraite->property("rang").toInt()){
                //                ABULEDU_LOG_DEBUG()<<"je passe "<<ui->twEditor->widget(i);
                i++;
            }
            else {
                ui->twEditor->insertWidget(i,widgetTraite);
                labelLie->show();

//                ABULEDU_LOG_DEBUG()<<"J'ai ajouté "<<widgetTraite<<"à l'index "<<i<<" et montré "<<labelLie;
                updateLabelWidgetName(ui->twEditor->currentIndex());
                trouvePlace = true;
            }
        }
    }
}

bool AllerEditorV1::prepareSauvegarde(QString nouveau)
{
    Q_UNUSED(nouveau)
    if (m_config->value("fichierTexte").toString().isEmpty()){
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Enregistrement module refusé..."),
                                                                  trUtf8("Le texte n'a pas été enregistré."),true,ui->twEditor->currentWidget());
        messageBox->show();
        return false;
    }

    saveSettings(m_previousWidget,m_previousExercice);

    if(ui->leTitre->text().trimmed().isEmpty() || ui->leAuteur->text().trimmed().isEmpty()){
        if(ui->leTitre->text().trimmed().isEmpty()){
            AbulEduMessageBoxV1 *alertBox = new AbulEduMessageBoxV1(trUtf8("Pas de titre !"),
                                                                    trUtf8("Vous n'avez pas renseigné le champ <b>Titre</b> de l'onglet <b>Propriétés</b> !"),true,ui->twEditor->currentWidget());
            alertBox->show();
            ui->lblTitreModule->setStyleSheet("color:red");
            ui->leTitre->setStyleSheet("border:1px solid red;border-radius:3px");
        }
        else{
            ui->lblTitreModule->setStyleSheet("color:black");
            ui->leTitre->setStyleSheet("border:1px solid grey;border-radius:3px");
        }
        if(ui->leAuteur->text().trimmed().isEmpty()){
            AbulEduMessageBoxV1 *alertBox = new AbulEduMessageBoxV1(trUtf8("Pas d'auteur !"),
                                                                    trUtf8("Vous n'avez pas renseigné le champ <b>Votre nom</b> de l'onglet <b>Propriétés</b> !"),true,ui->twEditor->currentWidget());
            alertBox->show();
            ui->lblNom->setStyleSheet("color:red");
            ui->leAuteur->setStyleSheet("border:1px solid red;border-radius:3px");
        }
        else{
            ui->lblNom->setStyleSheet("color:black");
            ui->leAuteur->setStyleSheet("border:1px solid grey;border-radius:3px");
        }
        //        ui->twEditor->setCurrentWidget(ui->wProprietes);
        return false;
    }
    else{
        ui->lblNom->setStyleSheet("color:black");
        ui->leAuteur->setStyleSheet("border:1px solid grey;border-radius:3px");
        ui->lblTitreModule->setStyleSheet("color:black");
        ui->leTitre->setStyleSheet("border:1px solid grey;border-radius:3px");
    }
    m_config->setValue("version",abeApp->applicationVersion());
    m_config->sync();

    setWindowModified(false);
    QString fileName = "data/dictionnaires/mot_aller_cycle3.csv";
    QFileInfo fi(fileName);
    qDebug()<<fi.absoluteFilePath();
    bool isCopyOK;
    if(!fi.exists()){
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Enregistrement refusé..."),
                                                                  trUtf8("Etape 1 : %1 non trouvé").arg("/data/dictionnaires/mot_aller_cycle3.csv"),true,ui->twEditor->currentWidget());
        messageBox->show();
        return false;
    }
    else{

        if (QFile(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/dictionnaires/mot_aller_cycle3.csv").exists()) {

//            ABULEDU_LOG_DEBUG() << "On a deja " << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/dictionnaires/mot_aller_cycle3.csv";
            isCopyOK = true;
        }
        else{
            QDir().mkpath(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/dictionnaires");

//            ABULEDU_LOG_DEBUG() << "On essaye de copier ici: " << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/dictionnaires/mot_aller_cycle3.csv";
            isCopyOK = QFile(fileName).copy(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/dictionnaires/mot_aller_cycle3.csv");

//            ABULEDU_LOG_DEBUG()<<isCopyOK;
        }
    }

    if (!isCopyOK)
    {
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Enregistrement refusé..."), trUtf8("Etape 2 : Copie ratée"),true,ui->twEditor->currentWidget());
        messageBox->show();
        return false;
    }
    QStringList mesfichiers;
    mesfichiers << recursiveRide(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());

//    ABULEDU_LOG_DEBUG() << "on lui expédie les fichiers " << mesfichiers;

    m_abuleduFile->abeFileExportPrepare(mesfichiers,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath(),QString("abe"));

    return true;
}

void AllerEditorV1::on_actionPublier_sur_AbuleduMediatheque_triggered()
{

}

void AllerEditorV1::on_action_Quitter_triggered()
{
    /** @todo gérer le
    if (this->isWindowModified())
    */
    emit editorLeft();
}

void AllerEditorV1::on_btnAnnulerTout_clicked()
{
    QFile* copieConf = new QFile(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/copieModule.conf");
    if (copieConf->exists()){
        QFile::remove(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/module.conf");
        QFile::rename(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/copieModule.conf",m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/module.conf");
    }
    emit editorLeft();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                   Correcteur Orthographique
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Le filtre d'event est installé sur le TextEdit de la zone Texte ET sur le TextEdit ouvert au bouton choix dans Texte à trous
bool AllerEditorV1::eventFilter(QObject *obj, QEvent *event)
{
    // On filtre les événements sur le TextEdit de la Zone Texte
    if(obj==ui->TEteTexte){
        if(event->type()== QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint position = mouseEvent->pos();

            if(mouseEvent->button()==Qt::RightButton){
                m_curseur = ui->TEteTexte->cursorForPosition(position);
                //                m_curseur->setPosition(curs.position());

//                ABULEDU_LOG_DEBUG()<<"m_curseur.selectedtext"<<m_curseur.selectedText()<<"Format"<<m_curseur.charFormat().underlineStyle();
                if(m_curseur.charFormat().underlineStyle()==QTextCharFormat::SpellCheckUnderline)
                {
                    m_curseur.select(QTextCursor::WordUnderCursor);
                    // On cherche des propositions de correction
                    QStringList suggestedWords = m_correcteur->suggestWords(m_curseur.selectedText());
                    //On crée un signalMapper qui établira le lien entre l'entrée de menu sélectionnée et le fichier à charger
                    QSignalMapper *signalMapper = new QSignalMapper(this);
                    connect(signalMapper, SIGNAL(mapped(QString)),this, SLOT(remplaceMot(QString)),Qt::UniqueConnection);
                    m_menuContextuel = new QMenu();
                    for(int i=0; i<suggestedWords.size();i++)
                    {
                        // Créer le menu contextuel et remplacer par le choix.
                        //et on l'affecte à l'item de menu
                        QAction *act=m_menuContextuel->addAction(suggestedWords[i]);
                        // Reste à définir les actions elles-mêmes et à les connecter !!!
                        signalMapper->setMapping(act,suggestedWords[i]);
                        connect(act, SIGNAL(triggered()), signalMapper, SLOT(map()),Qt::UniqueConnection);
                        m_menuContextuel->popup((mouseEvent->globalPos()));
                    }
                }
            }
        }
    }
    else if(obj == m_textEditForHoles)
    {
        //            ABULEDU_LOG_DEBUG()<<"m_holeArray : "<<m_holeArray;

        if(event->type()== QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint position = mouseEvent->pos();

            if(mouseEvent->button()==Qt::RightButton)
            {

//                ABULEDU_LOG_DEBUG()<<"EventFilter niveau 3b";
                m_curseur = m_textEditForHoles->cursorForPosition(position);

//                ABULEDU_LOG_DEBUG()<<"curseur en "<<m_curseur.position();
                if(m_curseur.charFormat() == m_textFormatSelected )
                {

//                    ABULEDU_LOG_DEBUG()<<"Condition de background remplie";
                    for (int j=0;j<m_holeArray.size();j++)
                    {
                        if (m_curseur.position() > m_holeArray[j].first && m_curseur.position() < m_holeArray[j].second)
                        {
                            m_curseur.setPosition(m_holeArray[j].first, QTextCursor::MoveAnchor);
//                            ABULEDU_LOG_DEBUG()<<"Curseur en "<<m_holeArray[j].first;
                            m_curseur.setPosition(m_holeArray[j].second, QTextCursor::KeepAnchor);
//                            ABULEDU_LOG_DEBUG()<<"Curseur en "<<m_holeArray[j].second;
                        }
                    }
                    m_menuContextuel = new QMenu();
                    QAction* actionDeselection = m_menuContextuel->addAction(trUtf8("Déselectionner"));
                    connect(actionDeselection, SIGNAL(triggered()), this, SLOT(deselectionne()),Qt::UniqueConnection);
                    m_menuContextuel->popup((mouseEvent->globalPos()));
                }
            }
        }
    }
    if (obj == this)  {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Escape) {
                on_btnAnnulerTout_clicked();
            }
        }
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            /* Très improbable double clic sur le bouton du milieu */
            if(mouseEvent->button() & Qt::MidButton){
                    ui->widgetInputChooser->setVisible(!ui->widgetInputChooser->isVisible());
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void AllerEditorV1::remplaceMot(QString proposition)
{
    if(m_curseur.selectedText().size()>1 ){
        if( m_curseur.selectedText().endsWith(QString::fromUtf8("…")) ||
                m_curseur.selectedText().endsWith(QString::fromUtf8("»")) ||
                m_curseur.selectedText().endsWith(QString::fromUtf8(")"))){
            m_curseur.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,1);
        }
        if(m_curseur.selectedText().startsWith(QString::fromUtf8("«")) ||
                m_curseur.selectedText().startsWith(QString::fromUtf8("("))){
            int anchorPos = m_curseur.anchor() + 1;
            int cursPos = m_curseur.position();
            m_curseur.setPosition(anchorPos);
            m_curseur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, cursPos - anchorPos );
        }
    }
    m_curseur.removeSelectedText();
    m_curseur.insertText(proposition);
    return;
}

void AllerEditorV1::deselectionne()
{
    ABULEDU_LOG_TRACE()<<"Avant : "<<m_holeArray;
    m_curseur.setCharFormat(m_textFormatUnSelected);
    m_holeArray.removeOne(QPair<int,int>(m_curseur.selectionStart(),m_curseur.selectionEnd()));
}

void AllerEditorV1::setItHasBeenModified()
{
    setWindowModified(true);
}

void AllerEditorV1::on_RRsbMotDebut_valueChanged(int arg1)
{
    ui->RRlblMotDebut->setText("("+m_listeMotsIndexes.value(arg1-1)+")");
}

void AllerEditorV1::on_RRsbMotFin_valueChanged(int arg1)
{
    ui->RRlblMotFin->setText("("+m_listeMotsIndexes.value(arg1-1)+")");
}

void AllerEditorV1::adjustPHMsbValues(int inutilise)
{
    Q_UNUSED(inutilise)
    ui->PHMsbNbPhrasesAutomatique->setMaximum(ui->PHMsbPhraseFinAutomatique->value()+1-ui->PHMsbPhraseDebutAutomatique->value());
    ui->PHMsbNbPhrasesUtiliseesManuel->setMaximum(ui->PHMsbPhraseFinManuel->value()+1-ui->PHMsbPhraseDebutManuel->value());
}

void AllerEditorV1::slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile)
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<< " Nom du fichier passé :" << abeFile->abeFileGetFileName().absoluteFilePath();
    ABULEDU_LOG_DEBUG()<<" ** Dossier temporaire : "<<abeFile->abeFileGetDirectoryTemp().absolutePath();
    MainWindow* parent = (MainWindow*) m_widgetMainwindow;
    parent->abeSetMyAbulEduFile(abeFile);
    //    m_widgetMainwindow->abeGetMyAbulEduFile()->abeFileOpen(abeFile->abeFileGetFileName().absoluteFilePath());
    //    *m_directoryWork = m_widgetMainwindow->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp();
    setNewUnitSettings(abeFile->abeFileGetFileName().fileName());
    setWindowModified(false);
}

void AllerEditorV1::on_btnWigdetSuivant_clicked()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    if (ui->twEditor->currentWidget() == ui->wTexte && m_isTextModify)
    {
        saveTextAsFile();
    }
    ui->twEditor->setCurrentWidget(ui->twEditor->widget(ui->twEditor->currentIndex()+1));
}

void AllerEditorV1::on_btnWidgetPrecedent_clicked()
{
    ui->twEditor->setCurrentWidget(ui->twEditor->widget(ui->twEditor->currentIndex()-1));
}

void AllerEditorV1::updateLabelWidgetName(int index)
{
    /* Je ne fais pas ça si j'arrive dans la page wAffichage */
    if(index == 10){
        return;
    }
    ui->btnWidgetPrecedent->setEnabled(ui->twEditor->currentWidget() != ui->wTexte);
    ui->btnWigdetSuivant->setEnabled(ui->twEditor->currentWidget() != ui->wProprietes);

    //        ABULEDU_LOG_DEBUG()<<index<<"/"<<ui->twEditor->count()<<" entree dans la page "<<ui->twEditor->widget(index)->property("titre").toString();
    ui->lblTE->setText(ui->wTexte->property("abrege").toString());
    ui->lblTE->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblTTR->setText(ui->wClosure->property("abrege").toString());
    ui->lblTTR->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblOA->setText(ui->wOrdreAlpha->property("abrege").toString());
    ui->lblOA->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblMOM->setText(ui->wMotsMelanges->property("abrege").toString());
    ui->lblMOM->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblPHM->setText(ui->wPhrasesMelangees->property("abrege").toString());
    ui->lblPHM->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblPAM->setText(ui->wParagraphes->property("abrege").toString());
    ui->lblPAM->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblPSE->setText(ui->wPhrasesEspaces->property("abrege").toString());
    ui->lblPSE->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblRR->setText(ui->wRechercheRapide->property("abrege").toString());
    ui->lblRR->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");
    ui->lblPR->setText(ui->wProprietes->property("abrege").toString());
    ui->lblPR->setStyleSheet("font-size:1em; font-weight:100; color:Gray; padding:0 1px 0 1px");

    if (index > 0)
    {
        QLabel* labelPrecedent = findChild<QLabel*>(ui->twEditor->widget(index-1)->property("label").toString());
        if(labelPrecedent){
            labelPrecedent->setText(ui->twEditor->widget(index-1)->property("titre").toString());
            labelPrecedent->setStyleSheet("font-size:2em; font-weight:700; color:Gray; padding:0 1px 0 1px");
        }
    }
    /* Il n'existe pas de QLabel qui s'appelle lblAC, car le chemin n'a pas à être affiché dans ce cas */
    if(ui->twEditor->widget(index)->property("label").toString() != "lblAC"){
        QLabel* labelCourant = findChild<QLabel*>(ui->twEditor->widget(index)->property("label").toString());
        if(labelCourant){
            labelCourant->setText(ui->twEditor->widget(index)->property("titre").toString());
            labelCourant->setStyleSheet("font-size:2em; font-weight:700; color:black; padding:0 1px 0 1px");
        }
    }
    /* Je mets -2 car le dernier est la page accueil qui n'a pas de label */
    if(index < ui->twEditor->count()-2){
        QLabel* labelSuivant = findChild<QLabel*>(ui->twEditor->widget(index+1)->property("label").toString());
        if(labelSuivant){
            labelSuivant->setText(ui->twEditor->widget(index+1)->property("titre").toString());
            labelSuivant->setStyleSheet("font-size:2em; font-weight:700; color:Gray; padding:0 1px 0 1px");
        }
    }
}

void AllerEditorV1::displaySaveLocation(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation loc,QString name,bool success)
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    QString emplacement;
    if (loc == AbulEduBoxFileManagerV1::abePC)
    {
        emplacement = trUtf8("votre ordinateur");
    }
    else if (loc == AbulEduBoxFileManagerV1::abeBoxPerso)
    {
        emplacement = trUtf8("votre abeBox personnelle");
    }
    else if (loc == AbulEduBoxFileManagerV1::abeBoxShare)
    {
        emplacement = trUtf8("une abeBox partagée");
    }
    else if (loc == AbulEduBoxFileManagerV1::abeMediatheque)
    {
        emplacement = trUtf8("AbulEdu-Médiathèque");
    }
    else
    {
        emplacement = trUtf8("un endroit inconnu");
    }

    QString message;
    if (success == true)
    {
        message = trUtf8("Votre fichier a été enregistré dans ")+emplacement;
        if (!name.isEmpty())
        {
            message.append(trUtf8(" sous le nom : ")+name.split("/").last());
        }
        MainWindow* mw = (MainWindow*) m_widgetMainwindow;
        if(m_abuleduFile != mw->abeGetMyAbulEduFile())
        {
            mw->abeSetMyAbulEduFile(m_abuleduFile);

//            ABULEDU_LOG_DEBUG()<<"maj de l'AbulEduFileV1 de la MainWindow";
//            ABULEDU_LOG_DEBUG()<<mw->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath();
        }
        else{

//            ABULEDU_LOG_DEBUG()<<"AbulEduFileV1 de la MainWindow déjà mis à jour";
//            ABULEDU_LOG_DEBUG()<<mw->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath();
        }
    }
    else
    {
        message = trUtf8("Votre fichier n'a pas pu être enregistré...");
    }
    AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"), message,true,ui->twEditor->currentWidget());
    if (success == true)
    {
        msgEnregistrement->setWink();
    }
    msgEnregistrement->show();
    //    m_abuleduFileManager->hide();
}

void AllerEditorV1::on_TTRsbMarqueurDebutTexte_valueChanged(int arg1)
{
    QTextCursor curs(ui->TEteTexte->document());
    curs.setPosition(arg1);
    curs.movePosition(QTextCursor::NextWord,QTextCursor::KeepAnchor);
    ui->TTRlblCommenceA->setText(curs.selectedText());
}

void AllerEditorV1::on_TTRsbMarqueurFinTexte_valueChanged(int arg1)
{
    QTextCursor curs(ui->TEteTexte->document());
    curs.setPosition(arg1);
    curs.movePosition(QTextCursor::PreviousWord,QTextCursor::KeepAnchor);
    ui->TTRlblFinitA->setText(curs.selectedText());
}


void AllerEditorV1::on_btnPublier_clicked()
{
    if (prepareSauvegarde()){
        MainWindow* parent = (MainWindow*) m_widgetMainwindow;
        ui->wMediathequePush->abeSetFile(m_abuleduFile);
        ui->twEditor->blockSignals(true);
        ui->twEditor->setCurrentWidget(ui->wMediathequePush);
        ui->twEditor->blockSignals(false);
        QFile::remove(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/copieModule.conf");
        if(m_abuleduFile != parent->abeGetMyAbulEduFile()){
            parent->abeSetMyAbulEduFile(m_abuleduFile);
        }
    }
}

void AllerEditorV1::on_btnEnregistrer_clicked()
{
    if (prepareSauvegarde())
    {
        m_abuleduFile->abeFileSetTitle(abeApp->sluggify(ui->leTitre->text().simplified()));
        emit editorSave(m_abuleduFile);
        QFile::remove(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/copieModule.conf");
    }
}

void AllerEditorV1::on_btnEssayer_clicked()
{
    if (prepareSauvegarde())
    {
        MainWindow* parent = (MainWindow*) m_widgetMainwindow;
        QString messageEssai;
        if(QFile(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+QDir::separator()+"conf/copieModule.conf").exists()){
            messageEssai = trUtf8("Votre module n'est pas enregistré. Si les paramètres des exercices vous conviennent, revenez dans l'éditeur pour enregistrer ou publier.");
        }
        else {
            messageEssai = trUtf8("Si les paramètres des exercices ne vous conviennent pas, revenez dans l'éditeur pour les modifier.");
        }
        AbulEduMessageBoxV1 *alertBox = new AbulEduMessageBoxV1(trUtf8("Passage en mode essai..."),messageEssai,true,parent->abeGetMyAbulEduAccueil());
        alertBox->abeSetModeEnum(AbulEduMessageBoxV1::abeOKButton);
        alertBox->show();
        emit editorTest();
    }
}

void AllerEditorV1::on_actionEssayer_ces_param_tres_triggered()
{
    on_btnEssayer_clicked();
}


void AllerEditorV1::deleteCurrentExercise(int num, QString typ)
{
    QMap<int,QMap<QString,QVariant> > allExercisesSettings;
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup(typ);
    int total = m_config->beginReadArray("exercice");
    int newTotal = 0;
    {
        for(int i = 0;i < total;i++)
        {
            m_config->setArrayIndex(i);
            //! si i est différent, on crée une map à enquiller dans la super map
            if(i != num)
            {
                newTotal++;
                QMap<QString,QVariant> exerciseSettings;
                foreach(QString cle,m_config->allKeys())
                {
                    exerciseSettings.insert(cle,m_config->value(cle));
                }
                allExercisesSettings.insert(newTotal,exerciseSettings);
            }
        }
        //        ABULEDU_LOG_DEBUG()<<allExercisesSettings;
    }
    m_config->endArray();
    m_config->remove("exercice");
    m_config->beginWriteArray("exercice");
    QMapIterator<int,QMap<QString,QVariant> > iter(allExercisesSettings);
    while(iter.hasNext())
    {
        iter.next();
        m_config->setArrayIndex(iter.key()-1);
        foreach(QString clef,iter.value().keys())
        {
            m_config->setValue(clef,iter.value().value(clef).toString());
        }
    }
    m_config->endArray();
    m_config->endGroup();
}

void AllerEditorV1::on_TTRbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->TTRcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"texteATrous");
    ui->TTRcbNumeroExercice->blockSignals(true);
    ui->TTRcbNumeroExercice->removeItem(ui->TTRcbNumeroExercice->count()-1);
    ui->TTRcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->TTRcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("texteATrous");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExerciceTexteTrous = nb;
    if (m_nombreExerciceTexteTrous < 2) {
        ui->TTRbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->TTRbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExerciceTexteTrous > 4){
        ui->TTRbtnAjouterExercice->setEnabled(false);
    }
    else{
        ui->TTRbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wClosure,0);
}

void AllerEditorV1::on_OAbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->OAcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"ordreAlphabetique");
    ui->OAcbNumeroExercice->blockSignals(true);
    ui->OAcbNumeroExercice->removeItem(ui->OAcbNumeroExercice->count()-1);
    ui->OAcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->OAcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("ordreAlphabetique");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExerciceOrdreAlphabetique = nb;
    if (m_nombreExerciceOrdreAlphabetique < 2){
        ui->OAbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->OAbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExerciceOrdreAlphabetique > 4) {
        ui->OAbtnAjouterExercice->setEnabled(false);
    }
    else {
        ui->OAbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wOrdreAlpha,0);
}

void AllerEditorV1::on_MOMbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->MOMcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"motsMelanges");
    ui->MOMcbNumeroExercice->blockSignals(true);
    ui->MOMcbNumeroExercice->removeItem(ui->MOMcbNumeroExercice->count()-1);
    ui->MOMcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->MOMcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("motsMelanges");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExerciceMotsMelanges = nb;
    if (m_nombreExerciceMotsMelanges < 2) {
        ui->MOMbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->MOMbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExerciceMotsMelanges > 4) {
        ui->MOMbtnAjouterExercice->setEnabled(false);
    }
    else {
        ui->MOMbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wMotsMelanges,0);
}

void AllerEditorV1::on_PHMbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->PHMcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"phrasesMelangees");
    ui->PHMcbNumeroExercice->blockSignals(true);
    ui->PHMcbNumeroExercice->removeItem(ui->PHMcbNumeroExercice->count()-1);
    ui->PHMcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->PHMcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("phrasesMelangees");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExercicePhrasesMelangees = nb;
    if (m_nombreExercicePhrasesMelangees < 2) {
        ui->PHMbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->PHMbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExercicePhrasesMelangees > 4) {
        ui->PHMbtnAjouterExercice->setEnabled(false);
    }
    else {
        ui->PHMbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wPhrasesMelangees,0);
}

void AllerEditorV1::on_PAMbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->PAMcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"paragraphesMelanges");
    ui->PAMcbNumeroExercice->blockSignals(true);
    ui->PAMcbNumeroExercice->removeItem(ui->PAMcbNumeroExercice->count()-1);
    ui->PAMcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->PAMcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("paragraphesMelanges");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExerciceParagraphesMelanges = nb;
    if (m_nombreExerciceParagraphesMelanges < 2) {
        ui->PAMbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->PAMbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExerciceParagraphesMelanges > 4) {
        ui->PAMbtnAjouterExercice->setEnabled(false);
    }
    else {
        ui->PAMbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wParagraphes,0);
}

void AllerEditorV1::on_PSEbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->PSEcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"phraseSansEspace");
    ui->PSEcbNumeroExercice->blockSignals(true);
    ui->PSEcbNumeroExercice->removeItem(ui->PSEcbNumeroExercice->count()-1);
    ui->PSEcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->PSEcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("phraseSansEspace");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExercicePhrasesSansEspace = nb;
    if (m_nombreExercicePhrasesSansEspace < 2) {
        ui->PSEbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->PSEbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExercicePhrasesSansEspace > 4){
        ui->PSEbtnAjouterExercice->setEnabled(false);
    }
    else {
        ui->PSEbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wPhrasesEspaces,0);
}

void AllerEditorV1::on_RRbtnDeleteCurrentExercise_clicked()
{
    int numExercise = ui->RRcbNumeroExercice->currentIndex();
    deleteCurrentExercise(numExercise,"rechercheRapide");
    ui->RRcbNumeroExercice->blockSignals(true);
    ui->RRcbNumeroExercice->removeItem(ui->RRcbNumeroExercice->count()-1);
    ui->RRcbNumeroExercice->setCurrentIndex(0);
    m_previousExercice = 0;
    ui->RRcbNumeroExercice->blockSignals(false);
    while(!m_config->group().isEmpty()){
        m_config->endGroup();
    }
    m_config->beginGroup("rechercheRapide");
    int nb = m_config->beginReadArray("exercice");
    m_config->endArray();
    m_config->endGroup();
    m_nombreExerciceRechercheRapide = nb;
    if (m_nombreExerciceRechercheRapide < 2) {
        ui->RRbtnDeleteCurrentExercise->setEnabled(false);
    }
    else {
        ui->RRbtnDeleteCurrentExercise->setEnabled(true);
    }
    if(m_nombreExerciceRechercheRapide > 4) {
        ui->RRbtnAjouterExercice->setEnabled(false);
    }
    else {
        ui->RRbtnAjouterExercice->setEnabled(true);
    }
    loadSettings(ui->wRechercheRapide,0);
}

void AllerEditorV1::on_TEbtnOuvrirTexte_clicked()
{
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(ui->wOuvrirTexte);
    ui->twEditor->blockSignals(false);
}

void AllerEditorV1::slotAbeEditorShowEditorTextPage()
{
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(ui->wTexte);
    ui->twEditor->blockSignals(false);
}

void AllerEditorV1::resizeEvent(QResizeEvent *e)
{
//    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<e->oldSize()<<" --> "<<e->size();
}

void AllerEditorV1::slotLoadTextFromData(QSharedPointer<AbulEduFileV1> abeFile, int code)
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<abeFile->abeFileGetContent(0).absoluteFilePath()<<code;
    if(code == 1){
        ui->GEleTitre->setText(abeFile->abeFileGetLOM()->abeLOMgetGeneralTitle(abeFile->abeFileGetLanguage()));
        QStringList authors;
        QMap<QString, QList<vCard> > contribs = abeFile->abeFileGetLOM()->abeLOMgetLifeCycleContributions();
        foreach(QString role,contribs.keys()){
            if(role.contains("author")){
                for(int j = 0;j < contribs.value(role).size();j++){
                    vCard author = contribs.value(role).at(j);
                    if(!authors.contains(author.getFullName())){
                        authors.append(author.getFullName());
                    }
                }
            }
        }
        ui->GEleAuteur->setText(authors.join(";"));
        slotLocalFileDialogSelected(abeFile->abeFileGetContent(0).absoluteFilePath());
    }
    else {
        AbulEduMessageBoxV1* msgProblem = new AbulEduMessageBoxV1(trUtf8("Problème"),trUtf8("Erreur lors du chargement du texte."),true,ui->wTexte);
        msgProblem->show();
    }
}

void AllerEditorV1::slotRecordingStarted()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
}

void AllerEditorV1::slotRecordingStopped()
{
    ABULEDU_LOG_TRACE()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<" : "<<m_currentSoundPath;
}

void AllerEditorV1::onCheckBoxRecordToogled(bool trueFalse)
{
    /* Code remonté dans la lib le 20151006 -> à supprimer d'ici quelques semaines sans bug apparu */
//    if(trueFalse){
//        AbulEduInstructionV1* wConsigne = (AbulEduInstructionV1*) sender();
//        QString radical = wConsigne->objectName();
//        /* chop(14), c'est la longueur de la chaine "abeInstruction" commune aux checkBox de tous les onglets. Le chop transforme donc la QString radical en "ttr", "oa", etc... */
//        radical.chop(14);

////        ABULEDU_LOG_DEBUG()<<radical.toLower();
//        m_multiMedia->setParent(0);
//        wConsigne->abeInstructionSetMultimedia(m_multiMedia);
////        wConsigne->abeInstructionSetInstruction(wConsigne->abeInstructionGetText(),m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/instructions/"+radical.toLower()+"ConsigneSequence.ogg");
////        m_currentSoundPath = m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/instructions/"+radical.toLower()+"ConsigneSequence.ogg";
////        ABULEDU_LOG_DEBUG()<<" &&&& je viens de mettre le path pour les instructions à "<<m_currentSoundPath;
////        m_multiMedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),m_currentSoundPath));
//    }
}

void AllerEditorV1::slotLocalFileDialogSelected(QString fileName)
{
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly)) {
//            ABULEDU_LOG_DEBUG() << "Erreur";
            return;
        }
        QByteArray data = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextStream in(&data);
        in.setAutoDetectUnicode(false);
        in.setCodec(codec);
        QString decodedStr = in.readAll();
        ui->TEteTexte->setPlainText(decodedStr);
    }
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(ui->wTexte);
    ui->twEditor->blockSignals(false);
}

void AllerEditorV1::on_twEditor_currentChanged(int arg1)
{
    ABULEDU_LOG_TRACE()<<" ++++++++ "<<  __PRETTY_FUNCTION__<<" :: "<<ui->twEditor->widget(arg1);
    bool isPushPage = (ui->twEditor->widget(arg1) == ui->wMediathequePush || ui->twEditor->widget(arg1) == ui->wBoutonsTutoriels);
    bool isMainPage = (ui->twEditor->widget(arg1) == ui->wAccueil || ui->twEditor->widget(arg1) == ui->wBoutonsTutoriels);
    bool isDisplayPage = (ui->twEditor->widget(arg1) == ui->wAutreAffichage || ui->twEditor->widget(arg1) == ui->wAffichageTutoriels);
    if(ui->leTitre->text().simplified().isEmpty() && !ui->GEleTitre->text().simplified().isEmpty()){
        ui->leTitre->setText(ui->GEleTitre->text());
    }
    ui->btnEssayer->setHidden(isPushPage || isMainPage || isDisplayPage);
    ui->btnEnregistrer->setHidden(isPushPage || isMainPage || isDisplayPage);
    ui->btnPublier->setHidden(isPushPage || isMainPage || isDisplayPage);
    ui->btnAnnulerTout->setHidden(isPushPage || isDisplayPage);
//    qDebug()<<" __ "<<isDisplayPage;
    ui->frmNavigEditor->setHidden(isPushPage || isMainPage || isDisplayPage);
}

void AllerEditorV1::on_allerEditorBtnCurrent_clicked()
{
    ABULEDU_LOG_TRACE()<<" ++++++++ "<<  __PRETTY_FUNCTION__;
    slotOpenFile(m_abuleduFile);
}

void AllerEditorV1::on_allerEditorBtnNew_clicked()
{
    /* Créer un module à partir de ressources */
    ABULEDU_LOG_TRACE()<<" ++++++++ "<<  __PRETTY_FUNCTION__;
    allerEditorCleanProject();
    setNewUnitSettings();
    emit editorNew();
}

void AllerEditorV1::on_allerEditorBtnOpen_clicked()
{
    ABULEDU_LOG_TRACE()<<" ++++++++ "<<  __PRETTY_FUNCTION__;
    emit editorChoose();
}

void AllerEditorV1::on_AAbtnAnnuler_clicked()
{    
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__<<m_previousWidget;
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(m_previousWidget);
    ui->twEditor->blockSignals(false);

    ui->btnWidgetPrecedent->setEnabled(true);
    ui->btnWigdetSuivant->setEnabled(true);

    if(m_previousWidget == ui->wClosure){

    }
    else if(m_previousWidget == ui->wOrdreAlpha){
        ui->OAteMotsModeManuel->clear();
        m_selectedObjectsList.sort();
        ui->OAteMotsModeManuel->setText(m_previousSelection);
    }
    else if(m_previousWidget == ui->wMotsMelanges){
        ui->MOMteMotsModeManuel->blockSignals(true);
        ui->MOMsbNumeroPhrase->blockSignals(true);
        ui->MOMteMotsModeManuel->clear();
        m_selectedObjectsList.sort();
        ui->MOMteMotsModeManuel->setText(m_previousSelection);
        ui->MOMsbNumeroPhrase->setMaximum(m_displayedObjectsMap.count());
        ui->MOMteMotsModeManuel->blockSignals(false);
        ui->MOMsbNumeroPhrase->blockSignals(false);
    }
    else if(m_previousWidget == ui->wPhrasesMelangees){
        ui->PHMtePhrasesModeManuel->clear();
        m_selectedObjectsList.sort();
        ui->PHMtePhrasesModeManuel->setText(m_previousSelection);
    }
    else if(m_previousWidget == ui->wParagraphes){
        ui->PAMleParagraphesChoisis->clear();
        m_selectedObjectsList.sort();
        ui->PAMleParagraphesChoisis->setText(m_previousSelection);
    }
    else if(m_previousWidget == ui->wPhrasesEspaces){
        ui->PSEtePhraseModeManuel->blockSignals(true);
        ui->PSEsbNumeroPhrase->blockSignals(true);
        ui->PSEtePhraseModeManuel->clear();
        m_selectedObjectsList.sort();
        ui->PSEtePhraseModeManuel->setText(m_previousSelection);
        ui->PSEsbNumeroPhrase->setMaximum(m_displayedObjectsMap.count());
        ui->PSEtePhraseModeManuel->blockSignals(false);
        ui->PSEsbNumeroPhrase->blockSignals(false);
    }
    else if(m_previousWidget == ui->wRechercheRapide){
        ui->RRteMotsModeManuel->clear();
        //        m_selectedObjectsList.sort();
        ui->RRteMotsModeManuel->setText(m_previousSelection);
    }
    else {
//        ABULEDU_LOG_DEBUG()<<"Fonction appelée sur un onglet pas prévu pour ! ";
    }
}

void AllerEditorV1::on_AAbtnReinitialiser_clicked()
{    
    ABULEDU_LOG_TRACE()<<  __PRETTY_FUNCTION__<<m_previousWidget;
    m_selectedObjectsList.clear();
    if(m_previousWidget == ui->wClosure){
        QTextCursor curseur(m_textEditForHoles->document());
        curseur.select(QTextCursor::Document);
        curseur.setCharFormat(m_textFormatUnSelected);
        m_holeArray.clear();
    }
    else if(m_previousWidget == ui->wOrdreAlpha){
        ui->OAteMotsModeManuel->clear();
    }
    else if(m_previousWidget == ui->wMotsMelanges){
        ui->MOMteMotsModeManuel->clear();
    }
    else if(m_previousWidget == ui->wPhrasesMelangees){
        ui->PHMtePhrasesModeManuel->clear();
    }
    else if(m_previousWidget == ui->wParagraphes){
        ui->PAMleParagraphesChoisis->clear();
    }
    else if(m_previousWidget == ui->wPhrasesEspaces){
        ui->PSEtePhraseModeManuel->clear();
    }
    else if(m_previousWidget == ui->wRechercheRapide){
        ui->RRteMotsModeManuel->clear();
    }
    else {
//        ABULEDU_LOG_DEBUG()<<"Fonction appelée sur un onglet pas prévu pour ! ";
    }
    foreach(AbulEduTextEditV1* elt,m_displayedObjectsMap)
    {
        elt->setStyleSheet(abe::STYLEBLANC);
    }
}

void AllerEditorV1::on_AbtnValider_clicked()
{
    ABULEDU_LOG_TRACE()<<  __PRETTY_FUNCTION__<<m_previousWidget<<m_holeArray.size();
    QString chaineAffichage = "";
    int tailleSelection =0;
    ui->twEditor->blockSignals(true);
    ui->twEditor->setCurrentWidget(m_previousWidget);
    ui->twEditor->blockSignals(false);

    ui->btnWidgetPrecedent->setEnabled(true);
    ui->btnWigdetSuivant->setEnabled(true);

    if(m_previousWidget == ui->wClosure){
        QTextCursor curseur(m_textEditForHoles->document());
        for (int i=0;i<m_holeArray.count();i++){
            curseur.setPosition(m_holeArray.at(i).first,QTextCursor::MoveAnchor);
            curseur.setPosition(m_holeArray.at(i).second,QTextCursor::KeepAnchor);
            if(curseur.selection().toPlainText().contains("\n")){
                AbulEduMessageBoxV1* pbMsg = new AbulEduMessageBoxV1(trUtf8("Problème ..."),trUtf8("Les sélections sur plusieurs lignes ne sont pas acceptées."),true,m_textEditForHoles);
                pbMsg->show();
                return;
            }
        }
        qSort(m_holeArray.begin(),m_holeArray.end());
        QString ajoutPourEnregistrementIndex;
        for (int i=0;i<m_holeArray.count();i++){
            ajoutPourEnregistrementIndex.append(QString::number(m_holeArray.at(i).first+ui->TTRsbMarqueurDebutTexte->value())+";"+QString::number(m_holeArray.at(i).second+ui->TTRsbMarqueurDebutTexte->value())+";");
        }
        ui->TTRlblTrousManuel->setText(ajoutPourEnregistrementIndex);
        //        ui->TTRlblTrousManuel->setVisible(false);
        ui->TTRsbNbTrousManuel->setMaximum(m_holeArray.count());
        ui->TTRsbNbTrousManuel->setValue(m_holeArray.count());

        QString ajoutPourCadre;
        for (int i=0;i<m_holeArray.count();i++){
            curseur.setPosition(m_holeArray.at(i).first,QTextCursor::MoveAnchor);
            curseur.setPosition(m_holeArray.at(i).second,QTextCursor::KeepAnchor);
//            ABULEDU_LOG_DEBUG()<<curseur.selection().toPlainText();
            ajoutPourCadre.append(curseur.selectedText()+";");
        }
        ui->TTRteTrousModeManuel->clear();
        ui->TTRteTrousModeManuel->setText(ajoutPourCadre);
    }
    else if(m_previousWidget == ui->wOrdreAlpha){
        if (!m_selectedObjectsList.isEmpty())
        {
            ui->OAteMotsModeManuel->clear();
            m_selectedObjectsList.sort();
            foreach(QString rangEnString,m_selectedObjectsList)
            {
                chaineAffichage.append(m_displayedObjectsMap.value(rangEnString.toInt())->toPlainText()+";");
                tailleSelection++;
            }
            ui->OAteMotsModeManuel->setText(chaineAffichage );
            ui->OAsbNbMotsManuel->setMaximum(tailleSelection);
            ui->OAsbNbMotsManuel->setValue(tailleSelection);
        }
    }
    else if(m_previousWidget == ui->wMotsMelanges){
        if (!m_selectedObjectsList.isEmpty())
        {
            ui->MOMteMotsModeManuel->blockSignals(true);
            ui->MOMsbNumeroPhrase->blockSignals(true);
            ui->MOMteMotsModeManuel->setText(m_displayedObjectsMap.value(m_selectedObjectsList.last().toInt())->toPlainText());
            ui->MOMsbNumeroPhrase->setMaximum(m_displayedObjectsMap.count());
            ui->MOMsbNumeroPhrase->setValue(m_selectedObjectsList.last().toInt()+1);
            ui->MOMteMotsModeManuel->blockSignals(false);
            ui->MOMsbNumeroPhrase->blockSignals(false);
        }
    }
    else if(m_previousWidget == ui->wPhrasesMelangees){
        if (!m_selectedObjectsList.isEmpty())
        {
            ui->PHMtePhrasesModeManuel->clear();
            QList<int> rangs;
            foreach(QString rangString,m_selectedObjectsList)
            {
                rangs << rangString.toInt();
            }
            qSort(rangs.begin(), rangs.end());
            foreach (int rangEntier,rangs)
            {
                chaineAffichage.append(QString::number(rangEntier+1)+";");
                tailleSelection++;
            }
            ui->PHMtePhrasesModeManuel->setText(chaineAffichage);
            ui->PHMsbNbPhrasesUtiliseesManuel->setMaximum(tailleSelection);
        }
    }
    else if(m_previousWidget == ui->wParagraphes){
        if (!m_selectedObjectsList.isEmpty())
        {
            ui->PAMleParagraphesChoisis->clear();
            QList<int> rangs;
            foreach(QString rangString,m_selectedObjectsList)
            {
                rangs << rangString.toInt();
            }
            qSort(rangs.begin(), rangs.end());
            foreach (int rangEntier,rangs)
            {
                chaineAffichage.append(QString::number(rangEntier+1)+";");
                tailleSelection++;
            }
            ui->PAMleParagraphesChoisis->setText(chaineAffichage);
        }
    }
    else if(m_previousWidget == ui->wPhrasesEspaces){
        if (!m_selectedObjectsList.isEmpty())
        {
            ui->PSEtePhraseModeManuel->blockSignals(true);
            ui->PSEsbNumeroPhrase->blockSignals(true);
            ui->PSEsbNumeroPhrase->setValue(m_selectedObjectsList.last().toInt()+1);
            ui->PSEtePhraseModeManuel->setText(m_displayedObjectsMap.value(m_selectedObjectsList.last().toInt())->toPlainText());
            ui->PSEsbNumeroPhrase->setMaximum(m_displayedObjectsMap.count());
            ui->PSEtePhraseModeManuel->blockSignals(false);
            ui->PSEsbNumeroPhrase->blockSignals(false);
        }
    }
    else if(m_previousWidget == ui->wRechercheRapide){
        if (!m_selectedObjectsList.isEmpty())
        {
            ui->RRteMotsModeManuel->clear();
            //            m_selectedObjectsList.sort();
            foreach(QString rangEnString,m_selectedObjectsList)
            {
                chaineAffichage.append(m_displayedObjectsMap.value(rangEnString.toInt())->toPlainText()+";");
                tailleSelection++;
            }
            ui->RRteMotsModeManuel->setText(chaineAffichage);
            ui->RRsbNbMotsManuel->setMaximum(tailleSelection);
        }
    }
    else {
//        ABULEDU_LOG_DEBUG()<<"Fonction appelée sur un onglet pas prévu pour ! ";
    }
}

void AllerEditorV1::on_allerEditorBtnTutorials_clicked()
{
    ui->twEditor->setCurrentWidget(ui->wBoutonsTutoriels);
}

void AllerEditorV1::allerEditorShowTutorial(QUrl url)
{
    QDesktopServices::openUrl(url);
}

void AllerEditorV1::on_allerEditorBtnTutoGetTexte_clicked()
{
    allerEditorShowTutorial(QUrl("https://videos.abuledu.org/embed/135"));
}

void AllerEditorV1::on_allerEditorBtnTutoAutoSettings_clicked()
{
    allerEditorShowTutorial(QUrl("https://videos.abuledu.org/embed/137"));
}

void AllerEditorV1::on_allerEditorBtnTutoChangeInstruction_clicked()
{
    allerEditorShowTutorial(QUrl("https://videos.abuledu.org/embed/138"));
}

void AllerEditorV1::on_allerEditorBtnTutoManualSettings_clicked()
{
    allerEditorShowTutorial(QUrl("https://videos.abuledu.org/embed/139"));
}

void AllerEditorV1::on_allerEditorBtnTutoSaveUnit_clicked()
{
    allerEditorShowTutorial(QUrl("https://videos.abuledu.org/embed/140"));
}

void AllerEditorV1::on_allerEditorBtnCloseTutorials_clicked()
{
    ui->twEditor->setCurrentWidget(ui->wAccueil);
}
