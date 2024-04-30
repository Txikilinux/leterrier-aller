/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2016 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exerciceordrealpha1.h"
#include "exerciceparagraphesmelanges.h"
#include "exercicephrasesansespaces.h"
#include "exerciceclosure.h"
#include "exercicerapido.h"
#include "allereditorv1.h"
#include <QTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_localDebug = false;

    ui->setupUi(this);
    ui->pageChoosePrint->abeUnitPrinterSetParent(ui->stCentral);

    /* 20140429 #3592 Icham Taille minimale définie ici et dans le .ui */
    setMinimumSize(1024,600);

    installEventFilter(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_isExerciceEnCours = false;
    m_texteBulles.clear();
    m_texteBulles.insert(0, trUtf8("Texte à trous"));
    m_texteBulles.insert(1, trUtf8("Ordre alphabétique"));
    m_texteBulles.insert(2, trUtf8("Mots mélangés"));
    m_texteBulles.insert(3, trUtf8("Phrases mélangées"));
    m_texteBulles.insert(4, trUtf8("Paragraphes mélangés"));
    m_texteBulles.insert(5, trUtf8("Phrases sans espaces"));
    m_texteBulles.insert(6, trUtf8("Recherche rapide"));

    gv_Accueil = new AbulEduPageAccueilV1(&m_texteBulles, ui->fr_principale);
    m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1(this), &QObject::deleteLater);
    setExercisesEnabled();
    m_numberExoCalled = -1;
    m_lastNumberExoCalled = -1;
    m_isDemoAvailable = true;
    m_demoTimeLine = new QTimeLine(8000,this);
    connect(m_demoTimeLine, SIGNAL(finished()),this, SLOT(slotFinDemo()),Qt::UniqueConnection);
    m_messageAide = trUtf8("Choisis un module en cliquant sur le bouton <b>Ouverture</b> du parchemin, les zones de lancement des exercices disponibles deviendront alors actives.");
    m_abuleduMessageBox = new AbulEduMessageBoxV1(trUtf8("On y va ?"),m_messageAide,false,gv_Accueil);
    m_abuleduMessageBox->setWink();
    m_abuleduMessageBox->abeSetModeEnum(AbulEduMessageBoxV1::abeNoButton);
    m_abuleduMessageBox->setVisible(false);
    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());
    connect(gv_Accueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)),Qt::UniqueConnection);
    connect(gv_Accueil, SIGNAL(signalAbePageAccueilMousePressed()), this, SLOT(slotDemo()),Qt::UniqueConnection);
    connect(gv_Accueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)),Qt::UniqueConnection);
    connect(gv_Accueil->abePageAccueilGetMenu(), SIGNAL(btnQuitterTriggered()), this, SLOT(btnQuitterClicked()),Qt::UniqueConnection);
    connect(gv_Accueil->abePageAccueilGetMenu(), SIGNAL(btnAideTriggered()), this, SLOT(btnAideClicked()),Qt::UniqueConnection);
    connect(gv_Accueil->abePageAccueilGetMenu(), SIGNAL(btnOuvrirTriggered()), this, SLOT(btnOuvrirClicked()),Qt::UniqueConnection);
    connect(gv_Accueil->abePageAccueilGetMenu(), SIGNAL(btnFermerTriggered()), this, SLOT(btnFermerClicked()),Qt::UniqueConnection);
    connect(gv_Accueil->abePageAccueilGetMenu(), SIGNAL(btnBoxTriggered()), this, SLOT(btnBoxClicked()),Qt::UniqueConnection);

    actionQuitter = new QAction("Quitter",this);

    m_abuleduFileManager = ui->AbulEduBoxFileManager;
    m_abuleduFileManager->abeMediathequeGetHideCloseBouton(true);
    m_abuleduFileManager->abeSetFile(m_abuleduFile);
    //    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected()),this, SLOT(slotOpenFile()), Qt::UniqueConnection);
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)),this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
    connect(ui->AbulEduBoxFileManager,SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
            ui->wEditor,SLOT(displaySaveLocation(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),Qt::UniqueConnection);
    connect(ui->AbulEduBoxFileManager,SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
            this, SLOT(slotSwitchExitBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),Qt::UniqueConnection);
    connect(ui->AbulEduBoxFileManager,SIGNAL(signalAbeFileCloseOrHide()),this,SLOT(slotExerciceExited()),Qt::UniqueConnection);
    /* Philippe 2013/05/07
     Plus maintenant ... donc j'essaye de connecter le signal au slot exerciceExited(), qui me ramène à la page d'accueil avec m_numberExoCalled = -1
     J'attends un peu avant de supprimer ce code, mais qui à priori... */
    //    /* les cinq lignes de code ci-dessous sont à priori inutiles, puisque le bouton fermer de la l'AbulEduBoxFileManager est disabled */
    //#ifdef __ABULEDUTABLETTEV1__MODE__
    //    connect(m_abuleduFileManager, SIGNAL(signalAbeFileCloseOrHide()), this, SLOT(showFullScreen()),Qt::UniqueConnection);
    //#else
    //#endif

    ui->wEditor->abeEditorSetMainWindow(this);
    connect(ui->wEditor, SIGNAL(editorLeft()),this, SLOT(slotExerciceExited()),Qt::UniqueConnection);
    connect(ui->wEditor, SIGNAL(editorTest()),this, SLOT(debutTestParametres()),Qt::UniqueConnection);
    connect(ui->wEditor, SIGNAL(editorChoose()),this, SLOT(slotAfficheBoxFileManagerOpen()),Qt::UniqueConnection);
    connect(ui->wEditor, SIGNAL(editorSave(QSharedPointer<AbulEduFileV1>)),this, SLOT(slotAfficheBoxFileManagerSave(QSharedPointer<AbulEduFileV1>)),Qt::UniqueConnection);

#ifdef __ABULEDUTABLETTEV1__MODE__
    ui->menuBar->hide();
#endif
    gv_Accueil->abePageAccueilDesactiveZones(true);

#ifndef __ABULEDUTABLETTEV1__MODE__
    ui->pageAbout->abeAproposSetMainWindow(this);
#endif
    connect(ui->pageAbout, SIGNAL(signalAbeAproposSetindex(int)),gv_Accueil->abePageAccueilGetMenu(), SIGNAL(btnAideTriggered()), Qt::UniqueConnection);
    connect(ui->pageAbout, SIGNAL(signalAbeAproposBtnCloseClicked()),this, SLOT(slotShowMainPage()),Qt::UniqueConnection);
    /* Les lignes ci-dessous permettent d'afficher l'application dans la résolution de la Tabuledu1 */
    //    setFixedSize(1600,768);
    //    move(300,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    if(isVisible()){
        if (m_localDebug) qDebug()<<"MainWindow::resizeEvent();"<<e->oldSize()<<" --> "<<e->size();
        setDimensionsWidgets();
    }
}
void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    //    if (m_localDebug) qDebug()<<"MainWindow::showEvent();";
    //    setDimensionsWidgets();
}
void MainWindow::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"MainWindow::setDimensionsWidgets();---Start";
    //    Pour faire les captures vidéo, on fixe la dimension de l'application à 1024 x 768, résolution minimale prise en compte
    //    resize(1024,768);
    gv_Accueil->setDimensionsWidgets();
    ui->wEditor->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    /* 20141031 Philippe : 4 lignes à supprimer dès que c'est validé sans problème d'affichage */
    //    ui->wEditor->setFixedWidth(width()*0.95);
    //    qreal alpha = height()*0.95;
    //    qreal beta = height() - 40;
    //    ui->wEditor->setFixedHeight(qMin(alpha,beta));
}

void MainWindow::abeLanceExo(int numero)
{
    if(m_isExerciceEnCours){return;}
    //Si aucun module n'a ete ouvert ... on hurle :)
    m_numberExoCalled = numero;
    m_lastNumberExoCalled = numero;
    if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty() && gv_Accueil->abePageAccueilGetBtnRevenirEditeur()->isHidden()) {
        on_action_Ouvrir_triggered();
        return;
    }
    else
        abeAiguillage();
}

void MainWindow::abeAiguillage()
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
    if (m_numberExoCalled >= 0)
    {
        ui->menuBar->setEnabled(false);
        m_isDemoAvailable = false;
    }
    gv_Accueil->abePageAccueilGetBtnRevenirEditeur()->setEnabled(false);

    //Pas sur mac: ca provoque le saut de la fenêtre au milieu de l'écran ... zarb !
#ifndef Q_OS_MAC
    setFixedSize(this->width(), this->height());
#endif

    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

    ui->stCentral->setCurrentWidget(ui->fr_principale);

    switch (m_numberExoCalled){
    case 0:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Closure";
        ExerciceClosure *b = new ExerciceClosure(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    case 1:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Ordre alphabetique";
        ExerciceOrdreAlpha1 *b = new ExerciceOrdreAlpha1(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    case 2:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Mots melanges";
        ExerciceMotsMelanges *b = new ExerciceMotsMelanges(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    case 3:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Phrases melangees";
        ExercicePhrasesMelangees* b = new ExercicePhrasesMelangees(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    case 4:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Paragraphes melanges";
        ExerciceParagraphesMelanges *b = new ExerciceParagraphesMelanges(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    case 5:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Phrases sans espaces";
        ExercicePhraseSansEspaces *b = new ExercicePhraseSansEspaces(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    case 6:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Rapido";
        ExerciceRapido* b = new ExerciceRapido(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(b, SIGNAL(exerciceExited()), this, SLOT(slotExerciceExited()), Qt::UniqueConnection);
        gv_Accueil->abePageAccueilGetMenu()->hide();
        m_isExerciceEnCours = true;
        break;
    }
    default:
    {
        if (m_localDebug) qDebug()<< "Abe Aiguillage defaut case";
        m_numberExoCalled = -1;
        break;
    }
    }
}

void MainWindow::btnQuitterClicked()
{
    if (m_localDebug) qDebug()<<"Bouton Quitter Clicked";
    this->close();
}

void MainWindow::btnAideClicked()
{
    ui->stCentral->setCurrentWidget(ui->pageAbout);
}

void MainWindow::slotSessionAuthenticated(bool enable)
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<< enable;
    if(enable)
        abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
    ui->stCentral->setCurrentWidget(ui->fr_principale);

    m_activityFilter = new ActivityFilter(abeApp);
    m_activityFilter->setObjectName("demoActivityFilter");
    m_activityFilter->setInterval(3000);
    abeApp->installEventFilter(m_activityFilter);
    QObject::connect(m_activityFilter, SIGNAL( userInactive() ),this,  SLOT( slotDemo()),Qt::UniqueConnection);
    QObject::connect(m_activityFilter, SIGNAL(userActive()), this, SLOT(slotFinDemo()), Qt::UniqueConnection);
    connect(abeApp->getAbeNetworkAccessManager(), SIGNAL(ssoAuthStatus(int)), this,SLOT(setTitle(int)));
}

void MainWindow::slotDemo()
{
    if (m_isDemoAvailable == false || ui->stCentral->currentWidget() != ui->fr_principale || m_demoTimeLine->state() == QTimeLine::Running){
        return;
    }
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    float ratio = abeApp->getAbeApplicationDecorRatio();
    //    QKeyEvent* pressSpace = new QKeyEvent(QEvent::KeyPress,Qt::Key_Space,Qt::NoModifier);
    //    QApplication::sendEvent(gv_Accueil,pressSpace);
    //    delete pressSpace;
    m_abuleduMessageBox->setGeometry(10*ratio,10*ratio,400*ratio,250*ratio);
    m_abuleduMessageBox->setVisible(true);
    gv_Accueil->abePageAccueilMontreBulles(true,m_abuleduFile->abeFileGetFileName().fileName().isEmpty());
    foreach(AbulEduZoneV1* zone, gv_Accueil->abePageAccueilGetZones())
    {
        if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty()){
            zone->abeZoneDrawRect(true,QColor("#000028"),4);
        }
        else {
            if(zone->abeZoneIsActif()){
                zone->abeZoneDrawRect(true,QColor("#000028"),4);
            }
        }
    }
    m_demoTimeLine->start();
}

void MainWindow::slotAfficheBoxFileManagerSave(QSharedPointer<AbulEduFileV1> file)
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
    ui->AbulEduBoxFileManager->abeSetSender(sender());
    ui->AbulEduBoxFileManager->abeSetFile(file);
    ui->AbulEduBoxFileManager->abeSetProjectName(file->abeFileGetTitle());
    ui->AbulEduBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeSave);
    ui->AbulEduBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}

void MainWindow::slotFinDemo()
{
    /* Le if sur le m_demoTimeLine->state() permet que le slotFinDemo soit bien exécuté au bon moment, mais sans le test de module chargé ce code empêche l'affichage des bulles */
    if(!m_abuleduFile->abeFileGetFileName().fileName().isEmpty() && sender()->objectName() == "demoActivityFilter"){
        if(m_demoTimeLine->state() != QTimeLine::Running){
            return;
        }
    }
    if(sender()->objectName() != "demoActivityFilter"){
        //        ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    }
    m_demoTimeLine->stop();
    if(m_abuleduMessageBox){
        m_abuleduMessageBox->setVisible(false);
    }
    gv_Accueil->abePageAccueilMontreBulles(false);
    foreach(AbulEduZoneV1* zone, gv_Accueil->abePageAccueilGetZones())
    {
        zone->abeZoneDrawRect(false);
    }
    //    QKeyEvent* releaseSpace = new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier);
    //    QApplication::sendEvent(gv_Accueil,releaseSpace);
    //    delete releaseSpace;
}

void MainWindow::btnOuvrirClicked()
{
    if (m_localDebug) qDebug()<<"Bouton Ouvrir Clicked";
}

void MainWindow::btnBoxClicked()
{
    if (m_localDebug) qDebug()<<"Bouton Box Clicked";
    ui->AbulEduBoxFileManager->abeSetSender(this);
    ui->AbulEduBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeOpen);
    ui->AbulEduBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}

void MainWindow::btnFermerClicked()
{
    if (m_localDebug) qDebug()<<"Bouton Fermer clicked";
}

void MainWindow::slotExerciceExited()
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
#ifdef __ABULEDUTABLETTEV1__MODE__
    gv_Accueil->abePageAccueilRetablisBullesTablette();
#endif
    if(sender() == ui->AbulEduBoxFileManager && ui->AbulEduBoxFileManager->abeGetSender() == ui->wEditor)
    {
        ui->stCentral->setCurrentWidget(ui->pageEditor);
        ui->wEditor->slotAbeEditorShowEditorMainPage();
    }
    else
    {
        ui->stCentral->setCurrentWidget(ui->fr_principale);
    }
    gv_Accueil->abePageAccueilGetMenu()->show();
    m_isExerciceEnCours = false;
    m_isDemoAvailable = ui->actionMode_d_mo->isChecked();
    gv_Accueil->abePageAccueilGetBtnRevenirEditeur()->setEnabled(true);
    ui->menuBar->setEnabled(true);

    m_numberExoCalled = -1;

    //Pour eviter un bug d'affichage mac on ne l'utilise pas
#ifndef Q_OS_MAC
    setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
#endif
    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());
    setExercisesEnabled();
}

/* bool MainWindow::event(QEvent *event)
{

    switch(event->type()){
    case QEvent::Show:
        if (m_localDebug) qDebug()<<"QEvent::Show";
        break;
    case QEvent::Resize:
        if (m_localDebug) qDebug()<<"QEvent::Resize"<<"Objet visible"<<isVisible();
        break;
    case QEvent::ChildPolished:
        if (m_localDebug) qDebug()<<"QEvent::ChildPolish";
        break;
    default:
                if (m_localDebug) qDebug()<<event->type();
    break;
    }
}
*/

void MainWindow::on_action_Ouvrir_triggered()
{
    btnBoxClicked();
}

void MainWindow::on_action_Editeur_Exercice_triggered()
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
    abeApp->setProperty("geometryMainwindow",geometry());
    m_isDemoAvailable = false;
    ui->wEditor->abeEditorSetMainWindow(this);
    if(ui->stCentral->currentWidget() == ui->pageEditor)
    {
        ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
    }
    else
    {
        ui->stCentral->setCurrentWidget(ui->pageEditor);
        ui->wEditor->slotAbeEditorShowEditorMainPage();
    }
}

void MainWindow::on_action_Quitter_triggered()
{
    btnQuitterClicked();
}

QDir MainWindow::abeGetMyTempDir()
{
    return m_abuleduFile->abeFileGetDirectoryTemp();
    //    return m_tempDir;
}

bool MainWindow::cleanDirectory(QString dirPath, QString dirInitial)
{
    //http://www.siteduzero.com/forum-83-738678-p1-supprimer-repertoire-avec-qt.html

    QDir folder(dirPath);
    //On va lister dans ce répertoire tous les éléments différents de "." et ".."
    //(désignant respectivement le répertoire en cours et le répertoire parent)
    folder.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    foreach(QFileInfo fileInfo, folder.entryInfoList())
    {
        //Si l'élément est un répertoire, on applique la méthode courante à ce répertoire, c'est un appel récursif
        if(fileInfo.isDir())
        {
            if(!cleanDirectory(fileInfo.filePath(),dirInitial)) //Si une erreur survient, on retourne false
                return false;
        }
        //Si l'élément est un fichier, on le supprime
        else if(fileInfo.isFile())
        {
            if(!QFile::remove(fileInfo.filePath()))
            {
                //Si une erreur survient, on retourne false
                return false;
            }
        }
    }

    if (dirPath != dirInitial)
    {
        //Le dossier est maintenant vide, on le supprime
        if(!folder.rmdir(dirPath))
        {
            //Si une erreur survient, on retourne false
            return false;
        }
    }
    //Sinon, on retourne true
    return true;
}

bool MainWindow::abeCleanMyTempDir()
{
    return cleanDirectory(m_tempDir->absolutePath(),m_tempDir->absolutePath());
}

void MainWindow::slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile)
{
    qDebug() << "Nom du fichier passé :" << m_abuleduFileManager->abeGetFile()->abeFileGetFileName().absoluteFilePath();
    m_abuleduFile = abeFile;
    m_lastNumberExoCalled = -1;
    abeApp->setProperty("moduleEnCours",abeFile->abeFileGetFileName().fileName());
    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());
    AbulEduBoxFileManagerV1* box = (AbulEduBoxFileManagerV1*) sender();
    if (box->abeGetSender() > 0)
    {
        if (box->abeGetSender() == ui->wEditor){
            ui->stCentral->setCurrentWidget(ui->pageEditor);
            //            ui->wEditor->slotAbeEditorShowEditorTextPage();
            return;
        }
        else{
            ui->stCentral->setCurrentWidget(ui->fr_principale);
        }
    }
    else{
        ui->stCentral->setCurrentWidget(ui->fr_principale);
    }
    abeAiguillage();
    setExercisesEnabled();
}

void MainWindow::setExercisesEnabled()
{
    if (m_localDebug){
        ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    }
    if(m_abuleduFile->abeFileGetFileList().count() > 0)
    {
        QList<QAction *> listMenuEntries = ui->menuExercices->actions();
        QList<AbulEduZoneV1 *> listZonesPageAccueil = gv_Accueil->abePageAccueilGetZones();

        /* Récupération d'un fichier de settings dans le répertoire temporaire */
        QSettings *settings = new QSettings(m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf", QSettings::IniFormat );
        listMenuEntries[0]      ->setEnabled(settings       ->value("texteATrous/exerciceActive",  true).toBool());
        listZonesPageAccueil[0] ->abeZoneSetActif(settings  ->value("texteATrous/exerciceActive",  true).toBool());
        listMenuEntries[1]      ->setEnabled(settings       ->value("ordreAlphabetique/exerciceActive",   true).toBool());
        listZonesPageAccueil[1] ->abeZoneSetActif(settings  ->value("ordreAlphabetique/exerciceActive",   true).toBool());
        listMenuEntries[2]      ->setEnabled(settings       ->value("motsMelanges/exerciceActive",     true).toBool());
        listZonesPageAccueil[2] ->abeZoneSetActif(settings  ->value("motsMelanges/exerciceActive",     true).toBool());
        listMenuEntries[3]      ->setEnabled(settings       ->value("phrasesMelangees/exerciceActive",     true).toBool());
        listZonesPageAccueil[3] ->abeZoneSetActif(settings  ->value("phrasesMelangees/exerciceActive",     true).toBool());
        listMenuEntries[4]      ->setEnabled(settings       ->value("paragraphesMelanges/exerciceActive",     true).toBool());
        listZonesPageAccueil[4] ->abeZoneSetActif(settings  ->value("paragraphesMelanges/exerciceActive",     true).toBool());
        listMenuEntries[5]      ->setEnabled(settings       ->value("phraseSansEspace/exerciceActive",     true).toBool());
        listZonesPageAccueil[5] ->abeZoneSetActif(settings  ->value("phraseSansEspace/exerciceActive",     true).toBool());
        listMenuEntries[6]      ->setEnabled(settings       ->value("rechercheRapide/exerciceActive",     true).toBool());
        listZonesPageAccueil[6] ->abeZoneSetActif(settings  ->value("rechercheRapide/exerciceActive",     true).toBool());
        delete settings;
        m_messageAide = trUtf8("Clique sur la flèche d'une des zones actives pour lancer un exercice.<br/>Tu peux voir les zones actives en appuyant sur la barre d'espace.");
        m_abuleduMessageBox->abeMessageBoxUpdate(m_abuleduMessageBox->abeMessageBoxGetTitle(),m_messageAide,m_abuleduMessageBox->abeMessageBoxGetWink());
        m_abuleduMessageBox->setVisible(false);
        m_activityFilter->setInterval(12000);
        QListIterator<QAction*> it(listMenuEntries);
        bool someEnabled = false;
        while(it.hasNext() && !someEnabled){
            if(it.next()->isEnabled()){
                someEnabled = true;
            }
        }
        ui->menuExercices->setEnabled(someEnabled);
    }
    else
    {
        ui->menuExercices->setEnabled(false);
    }
    ui->actionImprimer->setEnabled(m_abuleduFile->abeFileGetFileList().count() > 0);
}

void MainWindow::on_actionTexte_trous_triggered()
{
    abeLanceExo(0);
}

void MainWindow::on_actionOrdre_alphab_tique_triggered()
{
    abeLanceExo(1);
}

void MainWindow::on_actionMots_m_lang_s_triggered()
{
    abeLanceExo(2);
}

void MainWindow::on_actionPhrases_m_lang_es_triggered()
{
    abeLanceExo(3);
}

void MainWindow::on_actionParagraphes_m_lang_s_triggered()
{
    abeLanceExo(4);
}

void MainWindow::on_actionPhrase_sans_espaces_triggered()
{
    abeLanceExo(5);
}

void MainWindow::on_actionRecherche_rapide_triggered()
{
    abeLanceExo(6);
}

void MainWindow::setTitle(int authStatus)
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<authStatus;
    QString title = abeApp->getAbeApplicationLongName();
    if (m_numberExoCalled >=0)
    {
        title.append(" -- "+m_texteBulles[m_numberExoCalled]);
    }
    if(!m_abuleduFile->abeFileGetFileName().fileName().isEmpty())
    {
        title.append(" -- "+m_abuleduFile->abeFileGetFileName().fileName());
    }
    if (authStatus == 1)
    {
        if(abeApp->getAbeIdentite()->abeGetPreNom().isEmpty() && abeApp->getAbeIdentite()->abeGetNom().isEmpty()){
            title.append(" -- "+abeApp->getAbeNetworkAccessManager()->abeGetSSOLogin());
        }
        else{
            title.append(" -- "+abeApp->getAbeIdentite()->abeGetPreNom()+" "+abeApp->getAbeIdentite()->abeGetNom());
        }
    }
    setWindowTitle(title);
}


void MainWindow::on_actionMode_d_mo_triggered(bool checked)
{
    m_isDemoAvailable = checked;
}

void MainWindow::on_action_Journal_de_mes_activit_s_triggered()
{
    abeApp->getAbeIdentite()->abeGetMyLogsAsPDF();
}

void MainWindow::on_action_Changer_d_utilisateur_triggered()
{
    abeApp->getAbeNetworkAccessManager()->abeSSOLogout();
    abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
}

void MainWindow::slotShowMainPage()
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
    ui->stCentral->setCurrentWidget(ui->fr_principale);
}

void MainWindow::slotShowEditorPage()
{
    ui->stCentral->setCurrentWidget(ui->pageEditor);
}

void MainWindow::slotBuildRenderPage()
{
    /* 20160210 Philippe -> Pourquoi 580 ?
     * Il me fallait une valeur max au-dessus de laquelle l'impression sort de la page A4 en largeur
     * J'avais trouvé sur un forum 530, 580 ça a l'air de passer ... */
    const int widthMaxForA4 = 580;
    if(ui->pageChoosePrint->abeUnitPrinterGetExercises().isEmpty()){
        ABULEDU_LOG_DEBUG()<<"Pas d'exercices définis, partant pas d'impression";
    }
    else{
        QSettings *settings = new QSettings(m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf", QSettings::IniFormat );
        QString fic(QString("%1/data/%2").arg(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()).arg(settings->value("fichierTexte").toString()));
        AbulEduTextV1* baseText = new AbulEduTextV1(fic);
        ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->document()->clear();
        QTextCursor cursor(ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->document());
        QTextCharFormat titleFormat;
        titleFormat.setFontFamily("Andika Basic");
        titleFormat.setFontPointSize(14);
        titleFormat.setFontUnderline(true);

        QTextCharFormat normalFormat;
        normalFormat.setFontFamily("Andika Basic");
        normalFormat.setFontItalic(false);
        normalFormat.setFontPointSize(11);
        QTextCharFormat instructionFormat;
        instructionFormat.setFontFamily("Andika Basic");
        instructionFormat.setFontItalic(true);
        instructionFormat.setFontPointSize(11);
        QTextBlockFormat cellBlockFormat;
        cellBlockFormat.setAlignment(Qt::AlignHCenter);
        QTextBlockFormat normalBlockFormat;
        normalBlockFormat.setAlignment(Qt::AlignLeft);
        QTextBlockFormat titleBlockFormat;
        titleBlockFormat.setAlignment(Qt::AlignHCenter);
//        titleBlockFormat.setLeftMargin(300);
        titleBlockFormat.setBackground(QBrush(QColor(Qt::lightGray)));
        QTextBlockFormat ExerciseBlockFormat;
        ExerciseBlockFormat.setAlignment(Qt::AlignLeft);
        QTextTableFormat tableFormat;
        tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        tableFormat.setBorder(1);
        tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("texteATrous")){
            settings->beginGroup("texteATrous");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("texteATrous").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
//                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignHCenter);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Texte à trous"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("texteATrous").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("texteATrous"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                int b = settings->value("marqueurDebutTexte").toInt();
                int e = settings->value("marqueurFinTexte").toInt();
                QString holes;
                if(settings->value("modeManuel").toBool()){
                    holes = settings->value("listeTrous").toString();
                }
                else{
                    QListIterator<QPair<int,int> > it(ExerciceClosure::returnHolesFromAutomatic(baseText->abeTextSuperMots(),settings->value("nombreTrousAAfficher",5).toInt(),b,e));
                    QStringList sl;
                    while(it.hasNext()){
                        QPair<int,int> p = it.next();
                        sl.append(QString::number(p.first));
                        sl.append(QString::number(p.second));
                    }
                    holes = sl.join(";");
                }
                //                qDebug()<<balanceTextHoles(ui->wEditor->matchHolesWords(ui->wEditor->abeEditorGetText()->abeGetText(),holes,b,e).second);
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }
                /* On met les mots à replacer
                 * Je crée un tableau de 10 colonnes max pour les mettre */
                QStringList taken = ui->wEditor->matchHolesWords(ui->wEditor->abeEditorGetText()->abeGetText(),holes,b,e).first;
                QStringList shaked;
                while(!taken.isEmpty()){
                    shaked << taken.takeAt(rand()%taken.size());
                }


                /* Je ne centre pas l'ensemble des étiquettes, ça pose problème à l'impression */
//                tableFormat.setAlignment(Qt::AlignCenter);
                /* Par contre il faut que mes cases fassent toutes la même longueur, celle du mot le plus long */
                QVector<QTextLength> lengths;
                int maxlenght = 0;
                QFontMetrics fm(normalFormat.font());
                QStringListIterator iterator(shaked);
                while(iterator.hasNext()){
                    maxlenght = qMax(maxlenght,iterator.next().length());
                }
                int totalWidth = 0;
                int index = 0;
                while(index < shaked.size() && totalWidth < widthMaxForA4){
//                for(int l = 0;l < shaked.size();l++){
                    lengths << QTextLength(QTextLength::FixedLength,fm.boundingRect(QString("w").repeated(maxlenght)).width());
                    totalWidth += fm.boundingRect(QString("w").repeated(maxlenght)).width();
                    index++;
                }
                tableFormat.setColumnWidthConstraints(lengths);

                int columns, rows;
                int maxColumns = lengths.size();

                if(shaked.size() < maxColumns+1){
                    columns = shaked.size();
                    rows = 1;
                }
                else {
                    columns = maxColumns;
                    rows = qCeil(shaked.size()/(float)maxColumns);
                }

                QTextTable* textTable = cursor.insertTable(rows,columns,tableFormat);
                int col = 0;
                int row = 0;
                for( int i = 0; i < shaked.size(); i++ ) {
                    col = i % maxColumns;
                    row = i / maxColumns;
                    QTextTableCell cell = textTable->cellAt( row, col);
                    QTextCursor cellCursor = cell.firstCursorPosition();
                    /* Je veux que les mots soient centrés dans les cases */
                    cellCursor.setBlockFormat(cellBlockFormat);
                    const QString cellText = shaked[i];
                    cellCursor.insertText(cellText);
                }
                cursor.movePosition(QTextCursor::End);
                cursor.insertText("\n");
                cursor.insertText("\n");
                /* On insère le texte à trous */
                cursor.setCharFormat(normalFormat);
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setFontPointSize(11);
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setFontUnderline(false);
                cursor.insertText(balanceTextHoles(ui->wEditor->matchHolesWords(ui->wEditor->abeEditorGetText()->abeGetText(),holes,b,e).second));
                cursor.insertText("\n");
                cursor.insertText("\n");
            }
            settings->endArray();
            settings->endGroup();
            cursor.movePosition( QTextCursor::End );
        }
        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("ordreAlphabetique")){
            settings->beginGroup("ordreAlphabetique");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("ordreAlphabetique").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Ordre alphabétique"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("ordreAlphabetique").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("ordreAlphabetique"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                /* Je vais récupérer et mélanger ma liste de mots. Je ne fais pas le mélange dans la méthode statique parce que l'exercice l'utilise ordonnée
                   La méthode statique pourrait renvoyer la liste mélanger et l'exercice utiliser la méthode sort() mais je n'ai pas l'air d'y avoir accès :-( */
                QStringList words = ExerciceOrdreAlpha1::takeWords(settings->fileName(),number-1,settings->value("nbMotsARanger").toInt(),baseText);
                QStringList randomWords;
                while(!words.isEmpty()){
                    randomWords.insert(randomWords.size(),words.takeAt(rand()%words.size()));
                }
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }

                /* Par contre il faut que mes cases fassent toutes la même longueur, celle du mot le plus long */
                QVector<QTextLength> lengths;
                int maxlenght = 0;
                QFontMetrics fm(normalFormat.font());
                QStringListIterator iterator(randomWords);
                while(iterator.hasNext()){
                    maxlenght = qMax(maxlenght,iterator.next().length());
                }
                int totalWidth = 0;
                int index = 0;
                while(index < randomWords.size() && totalWidth < widthMaxForA4){
//                for(int l = 0;l < shaked.size();l++){
                    lengths << QTextLength(QTextLength::FixedLength,fm.boundingRect(QString("w").repeated(maxlenght)).width());
                    totalWidth += fm.boundingRect(QString("w").repeated(maxlenght)).width();
                    index++;
                }
                tableFormat.setColumnWidthConstraints(lengths);

                /* Je crée un tableau de X colonnes (calcul ci-dessus en fonction de la largeur max) pour mettre les mots de l'exercice */
                int columns, rows;
                int maxColumns = lengths.size();
                if(randomWords.size() < maxColumns+1){
                    columns = randomWords.size();
                    rows = 1;
                }
                else {
                    /* Le nombre de mots dans l'éditeur est limité de mémoire à 18 */
                    columns = maxColumns;
                    rows = qCeil(randomWords.size()/(float)maxColumns);
                }
                QTextTable* textTable = cursor.insertTable(rows,columns,tableFormat );
                int col = 0;
                int row = 0;
                for( int i = 0; i < randomWords.size(); i++ ) {
                    col = i % maxColumns;
                    row = i / maxColumns;
                    QTextTableCell cell = textTable->cellAt( row, col);
                    QTextCursor cellCursor = cell.firstCursorPosition();
                    /* Je veux que les mots soient centrés dans les cases */
                    cellCursor.setBlockFormat(cellBlockFormat);
                    const QString cellText = randomWords[i];
                    cellCursor.insertText(cellText);
                }
                cursor.movePosition(QTextCursor::End);
                /* Et j'insère le même tableau mais vide pour répondre */
                cursor.insertText("\n\n");
                cursor.insertTable(rows,columns,tableFormat);
                cursor.movePosition(QTextCursor::End);
                cursor.insertText("\n\n");
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
            }
            settings->endArray();
            settings->endGroup();
        }

        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("motsMelanges")){

            settings->beginGroup("motsMelanges");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("motsMelanges").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Mots mélangés"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("motsMelanges").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("motsMelanges"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                /* On récupère la phrase */
                QString sentence = ExerciceMotsMelanges::takeSentence(settings->fileName(),number-1,baseText);
                /* On découpe en mots */
                AbulEduTextV1* baseSentence = new AbulEduTextV1();
                baseSentence->abeSetText(sentence);
                QStringList words = baseSentence->abetextMotsPonctues().values();
                /* On mélange */
                QStringList shaked;
                while(!words.isEmpty()){
                    shaked << words.takeAt(rand()%words.size());
                }
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }
                /* Par contre il faut que mes cases fassent toutes la même longueur, celle du mot le plus long */
                QVector<QTextLength> lengths;
                int maxlenght = 0;
                QFontMetrics fm(normalFormat.font());
                QStringListIterator iterator(shaked);
                while(iterator.hasNext()){
                    maxlenght = qMax(maxlenght,iterator.next().length());
                }
                int totalWidth = 0;
                int index = 0;
                while(index < shaked.size() && totalWidth < widthMaxForA4){
//                for(int l = 0;l < shaked.size();l++){
                    lengths << QTextLength(QTextLength::FixedLength,fm.boundingRect(QString("w").repeated(maxlenght)).width());
                    totalWidth += fm.boundingRect(QString("w").repeated(maxlenght)).width();
                    index++;
                }
                tableFormat.setColumnWidthConstraints(lengths);

                /* Je crée un tableau de X colonnes (calcul ci-dessus en fonction de la largeur max) pour mettre les mots de l'exercice */
                int columns, rows;
                int maxColumns = lengths.size();
                if(shaked.size() < maxColumns+1){
                    columns = shaked.size();
                    rows = 1;
                }
                else {
                    /* Le nombre de mots dans l'éditeur est limité de mémoire à 18 */
                    columns = maxColumns;
                    rows = qCeil(shaked.size()/(float)maxColumns);
                }

                QTextTable* textTable = cursor.insertTable(rows,columns,tableFormat );
                int col = 0;
                int row = 0;
                for( int i = 0; i < shaked.size(); i++ ) {
                    col = i % maxColumns;
                    row = i / maxColumns;
                    QTextTableCell cell = textTable->cellAt(row, col);
                    QTextCursor cellCursor = cell.firstCursorPosition();
                    /* Je veux que les mots soient centrés dans les cases */
                    cellCursor.setBlockFormat(cellBlockFormat);
                    const QString cellText = shaked[i];
                    cellCursor.insertText(cellText);
                }
                cursor.movePosition(QTextCursor::End);
                /* Et j'insère le même tableau mais vide pour répondre */
                cursor.insertText("\n\n");
                cursor.insertTable(rows,columns,tableFormat);
                cursor.movePosition(QTextCursor::End);
                cursor.insertText("\n\n");
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                baseSentence->deleteLater();
            }
            settings->endArray();
            settings->endGroup();
            baseText->deleteLater();
        }
        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("phrasesMelangees")){
            settings->beginGroup("phrasesMelangees");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("phrasesMelangees").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Phrases mélangées"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("phrasesMelangees").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("phrasesMelangees"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                /* On récupère les phrases */
                QStringList sentences = ExercicePhrasesMelangees::takeSentences(settings->fileName(),number-1,baseText);
                /* On mélange */
                QStringList shaked;
                while(!sentences.isEmpty()){
                    shaked << sentences.takeAt(rand()%sentences.size());
                }
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }
                /* Je crée un tableau de 2 colonnes pour mettre les mots de l'exercice */
                int columns = 2;
                int rows = shaked.size();

                /* Je vais fixer les largeurs de mes colonnes, à 50 pour la première dans laquelle les enfants sont sensés écrire un nombre
                 * la seconde occupant ce qui me reste de la largeur max autorisée */
                QVector<QTextLength> lengths;
                lengths << QTextLength(QTextLength::FixedLength,50);
                lengths << QTextLength(QTextLength::FixedLength,widthMaxForA4 - 50);
                tableFormat.setColumnWidthConstraints(lengths);
                QTextTable* textTable = cursor.insertTable(rows,columns,tableFormat );
                int row = 0;
                int col = 1;
                for( int i = 0; i < shaked.size(); i++ ) {
                    row = i;
                    QTextTableCell cell = textTable->cellAt(row, col);
                    QTextCursor cellCursor = cell.firstCursorPosition();
                    cellCursor.setBlockFormat(normalBlockFormat);
                    const QString cellText = shaked[i];
                    cellCursor.insertText(cellText);
                }
                cursor.movePosition(QTextCursor::End);
                cursor.insertText("\n\n");
            }
            settings->endArray();
            settings->endGroup();
        }
        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("paragraphesMelanges")){
            settings->beginGroup("paragraphesMelanges");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("paragraphesMelanges").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Paragraphes mélangés"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("paragraphesMelanges").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("paragraphesMelanges"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                /* On récupère les phrases */
                QStringList paragraphs = ExerciceParagraphesMelanges::takeParagraphs(settings->fileName(),number-1,baseText).values();
                /* On mélange */
                QStringList shaked;
                while(!paragraphs.isEmpty()){
                    shaked << paragraphs.takeAt(rand()%paragraphs.size());
                }
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }
                /* Je crée un tableau de 2 colonnes pour mettre les mots de l'exercice */
                int columns = 2;
                int rows = shaked.size();

                /* Je vais fixer les largeurs de mes colonnes, à 50 pour la première dans laquelle les enfants sont sensés écrire un nombre
                 * la seconde occupant ce qui me reste de la largeur max autorisée */
                QVector<QTextLength> lengths;
                lengths << QTextLength(QTextLength::FixedLength,50);
                lengths << QTextLength(QTextLength::FixedLength,widthMaxForA4 - 50);
                tableFormat.setColumnWidthConstraints(lengths);
                QTextTable* textTable = cursor.insertTable(rows,columns,tableFormat );
                int row = 0;
                int col = 1;
                for( int i = 0; i < shaked.size(); i++ ) {
                    row = i;
                    QTextTableCell cell = textTable->cellAt(row, col);
                    QTextCursor cellCursor = cell.firstCursorPosition();
                    cellCursor.setBlockFormat(normalBlockFormat);
                    const QString cellText = shaked[i];
                    cellCursor.insertText(cellText);
                }
                cursor.movePosition(QTextCursor::End);
                cursor.insertText("\n\n");
            }
            settings->endArray();
            settings->endGroup();
        }
        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("phraseSansEspace")){
            settings->beginGroup("phraseSansEspace");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("phraseSansEspace").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Phrases sans espaces"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("phraseSansEspace").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("phraseSansEspace"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                /* On récupère la phrase */
                QString sentence = ExercicePhraseSansEspaces::takeSentence(settings->fileName(),number-1,baseText);
                /* On enlève les espaces pour coller les mots */
                int i = sentence.indexOf(QChar::Space);
                while(i>-1){
                    sentence.remove(i,1);
                    i = sentence.indexOf(" ");
                }
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }
                cursor.setCharFormat(normalFormat);
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setFontPointSize(11);
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setFontUnderline(false);
                cursor.insertText(sentence);
                cursor.insertText("\n");
                cursor.insertText("\n");
            }
            settings->endArray();
            settings->endGroup();
        }
        if(ui->pageChoosePrint->abeUnitPrinterGetExercises().keys().contains("rechercheRapide")){
            settings->beginGroup("rechercheRapide");
            if(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("rechercheRapide").size() > 0){
                cursor.setBlockFormat(titleBlockFormat);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Recherche rapide"));
                //            cursor.insertText("\n");
                cursor.insertText("\n");
                cursor.setBlockFormat(normalBlockFormat);
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed()){
                    QString instruction;
                    if(settings->value("consigneSequence").toString().isEmpty() || ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                        instruction = ui->pageChoosePrint->abeUnitPrinterGetExercises().value("rechercheRapide").second;
                    }
                    else {
                        instruction = settings->value("consigneSequence").toString();
                    }
                    ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                    cursor.setCharFormat(instructionFormat);
                    cursor.insertHtml("<i>"+instruction+"</i>");
                    cursor.setCharFormat(normalFormat);
                    cursor.insertText("\n");
                }
            }
            settings->beginReadArray("exercice");
            QListIterator<int> iter(ui->pageChoosePrint->abeUnitPrinterGetChecked().value("rechercheRapide"));
            while(iter.hasNext()){
                int number = iter.next();
                settings->setArrayIndex(number-1);
                /* On récupère les mots */
                QStringList words = ExerciceRapido::takeExerciseWords(settings->fileName(),number-1,baseText);
                /* On insère le titre */
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);
                cursor.setCharFormat(titleFormat);
                cursor.insertText(trUtf8("Exercice n°%1").arg(number));
                cursor.setCharFormat(normalFormat);
                cursor.insertText("\n");
                if(ui->pageChoosePrint->abeUnitPrinterGetIsInstructionAllowed() && !ui->pageChoosePrint->abeUnitPrinterGetUseGenericInstruction()){
                    if(!settings->value("consigneExercice").toString().isEmpty()){
                        cursor.setCharFormat(instructionFormat);
                        cursor.insertHtml("<i>"+settings->value("consigneExercice").toString()+"</i>");
                        cursor.setCharFormat(normalFormat);
                        cursor.insertText("\n");
                        cursor.insertText("\n");
                    }
                }

                /* Par contre il faut que mes cases fassent toutes la même longueur, celle du mot le plus long */
                QVector<QTextLength> lengths;
                int maxlenght = 0;
                QFontMetrics fm(normalFormat.font());
                QStringListIterator ordrIterator(words);
                while(ordrIterator.hasNext()){
                    maxlenght = qMax(maxlenght,ordrIterator.next().length());
                }
                int totalWidth = 0;
                int index = 0;
                while(index < words.size() && totalWidth < widthMaxForA4){
//                for(int l = 0;l < shaked.size();l++){
                    lengths << QTextLength(QTextLength::FixedLength,fm.boundingRect(QString("w").repeated(maxlenght)).width());
                    totalWidth += fm.boundingRect(QString("w").repeated(maxlenght)).width();
                    index++;
                }
                tableFormat.setColumnWidthConstraints(lengths);

                /* Je crée un tableau pour mettre les mots de l'exercice */
                int columns, rows;
                int maxColumns = lengths.size();
                if(words.size() < maxColumns+1){
                    columns = words.size();
                    rows = 1;
                }
                else {
                    columns = maxColumns;
                    rows = qCeil(words.size()/(float)maxColumns);
                }

                QTextTable* textTable = cursor.insertTable(rows,columns,tableFormat );
                int col = 0;
                int row = 0;
                for( int i = 0; i < words.size(); i++ ) {
                    col = i % maxColumns;
                    row = i / maxColumns;
                    QTextTableCell cell = textTable->cellAt( row, col);
                    QTextCursor cellCursor = cell.firstCursorPosition();
                    /* Je veux que les mots soient centrés dans les cases */
                    cellCursor.setBlockFormat(cellBlockFormat);
                    const QString cellText = words[i];
                    cellCursor.insertText(cellText);
                }
                cursor.movePosition(QTextCursor::End);
                cursor.insertText("\n\n");
                ui->pageChoosePrint->abeUnitPrinterGetTextEdit()->setAlignment(Qt::AlignLeft);            }
            settings->endArray();
            settings->endGroup();
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    /* Pas localDebug car il y a trop de message */
    if (event->type() == QEvent::KeyRelease && ui->stCentral->currentWidget() == ui->fr_principale)
    {
        //        qDebug()<<"Event filter KeyRelease sur la Mainwindow";
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        /* Protection contre les clics frénétiques */
        if(!keyEvent->isAutoRepeat())
        {
            switch(keyEvent->key())
            {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if(m_lastNumberExoCalled == m_texteBulles.size()-1)
                {
                    abeLanceExo(0);
                }
                else
                {
                    abeLanceExo(m_lastNumberExoCalled+1);
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

void MainWindow::on_stCentral_currentChanged(int arg1)
{
//    qDebug()<<" --->  passage dans le widget "<<ui->stCentral->widget(arg1)->objectName();
    if(ui->stCentral->widget(arg1) == ui->fr_principale){
        ui->menuBar->setEnabled(true);
    }
    else {
        ui->menuBar->setEnabled(false);
    }
}

void MainWindow::slotSwitchExitBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation, QString, bool)
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
    if(ui->AbulEduBoxFileManager->abeGetSender() == ui->wEditor)
    {
        ABULEDU_LOG_DEBUG() <<" on venait de l'éditeur, on y retourne ...";
        ui->stCentral->setCurrentWidget(ui->pageEditor);
        setExercisesEnabled();
    }
    else
    {
        ABULEDU_LOG_DEBUG() <<" on venait de la page d'accueil, on y retourne ...";
        ui->stCentral->setCurrentWidget(ui->fr_principale);
    }
}

void MainWindow::debutTestParametres()
{
    if(m_localDebug) {
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    }
    gv_Accueil->abePageAccueilGetBtnRevenirEditeur()->setVisible(true);
    connect(gv_Accueil->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),this,SLOT(slotShowEditorPage()),Qt::UniqueConnection);
    connect(gv_Accueil->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),gv_Accueil->abePageAccueilGetBtnRevenirEditeur(),SLOT(hide()),Qt::UniqueConnection);
    ui->stCentral->setCurrentWidget(ui->fr_principale);
    setExercisesEnabled();
}

void MainWindow::slotAfficheBoxFileManagerOpen()
{
    /* Le fait de commenter cette ligne provoque une petite anomalie de fonctionnement : quand dans l'éditeur on clique sur Ouvrir un module, une fois le module choisi,
     * on revient à la page d'accueil du logiciel (et plus directement dans l'éditeur. Par contre on n'a plus les bugs de contenu de l'éditeur de passage d'un abe à un autre */
    //    ui->AbulEduBoxFileManager->abeSetSender(sender());
    ui->AbulEduBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeOpen);
    ui->AbulEduBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->AbulEduBoxFileManager->abeSetSender(sender());
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}

void MainWindow::on_actionImprimer_triggered()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;

    if (abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() != 1)
    {
        abeApp->getAbeNetworkAccessManager()->abeOnLoginSuccessGoto(this,SLOT(slotGoToPrint()));
        abeApp->getAbeNetworkAccessManager()->abeOnLoginFailureGoto(this,SLOT(slotDontGoToPrint()));
        abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
    }
    else
    {
        slotGoToPrint();
    }
}

void MainWindow::slotGoToPrint()
{
    /// En fait, il faut aller vers une page qui récapitule les exercices présents et là on peut basculer vers cette pagePrint

    ui->stCentral->setCurrentWidget(ui->pageChoosePrint);
    ui->pageChoosePrint->abeUnitPrintSetRenderSlot(this,SLOT(slotBuildRenderPage()));
    ui->pageChoosePrint->abeUnitPrinterClear();
    QSettings* settings = new QSettings(m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",QSettings::IniFormat);
    if(settings->childGroups().contains("texteATrous") && settings->value("texteATrous/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Texte à trous"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "texteATrous",
                                                           trUtf8("Replace les mots enlevés."),
                                                           "exercice");
    }
    if(settings->childGroups().contains("ordreAlphabetique") && settings->value("ordreAlphabetique/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Ordre alphabétique"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "ordreAlphabetique",
                                                           trUtf8("Remets ces mots dans l'ordre alphabétique."),
                                                           "exercice");
    }
    if(settings->childGroups().contains("motsMelanges") && settings->value("motsMelanges/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Mots mélangés"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "motsMelanges",
                                                           trUtf8("Remets les mots dans l'ordre pour reconstituer une phrase."),
                                                           "exercice");
    }
    if(settings->childGroups().contains("phrasesMelangees") && settings->value("phrasesMelangees/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Phrases mélangées"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "phrasesMelangees",
                                                           trUtf8("Remets ces phrases dans l'ordre du texte."),
                                                           "exercice");
    }
    if(settings->childGroups().contains("paragraphesMelanges") && settings->value("paragraphesMelanges/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Paragraphes mélangés"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "paragraphesMelanges",
                                                           trUtf8("Remets ces paragraphes dans l'ordre du texte."),
                                                           "exercice");
    }
    if(settings->childGroups().contains("phraseSansEspace") && settings->value("phraseSansEspace/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Phrase sans espaces"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "phraseSansEspace",
                                                           trUtf8("Sépare les mots."),
                                                           "exercice");
    }
    if(settings->childGroups().contains("rechercheRapide") && settings->value("rechercheRapide/exerciceActive").toBool()){
        ui->pageChoosePrint->abeUnitPrinterAddExerciseType(trUtf8("Recherche rapide"),
                                                           m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/module.conf",
                                                           "rechercheRapide",
                                                           trUtf8("Retrouve ces mots dans le texte."),
                                                           "exercice");
    }
}

void MainWindow::slotDontGoToPrint()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;

    AbulEduMessageBoxV1::abeMessageBoxShowInformation(trUtf8("La fonctionnalité d'<b>Impression</b> d'exercices d'après le contenu du module n'est accessible qu'aux utilisateurs authentifiés."));
}

QString MainWindow::balanceTextHoles(QString text)
{
    QList<QList<int> > l;
    int index = 0;
    int max = 0;
    QList<int> indexes;
    while(text.indexOf("_",index) > 0){
        if(text.indexOf("_",index) - index > 0){
            if(!indexes.isEmpty() && !l.contains(indexes)){
                l << indexes;
                if(indexes.size() > max){
                    max = indexes.size();
                }
                indexes.clear();
            }
        }
        if(text.indexOf("_",index) - index > 0){
            index = text.indexOf("_",index);
        }
        else{
            indexes << text.indexOf("_",index);
            index++;
        }
    }
    if(!indexes.isEmpty() && !l.contains(indexes)){
        if(indexes.size() > max){
            max = indexes.size();
        }
        l << indexes;
    }
    QString underscore("_");
    QString uniforme(underscore.repeated(max));
    QString uniformeDouble(uniforme.repeated(2));
    int decalage = 0;
    QListIterator<QList<int> > iterator(l);
    while(iterator.hasNext()){
        QList<int> currentList = iterator.next();
        text.replace(currentList.first()+decalage,currentList.last()-currentList.first()+1,uniforme);
        decalage = decalage + max - (currentList.last()-currentList.first()+1);
    }
    return text.replace(uniforme,uniformeDouble);
}

void MainWindow::on_unitPrinterBtnCancelSelect_clicked()
{
    ui->stCentral->setCurrentWidget(ui->fr_principale);
}
