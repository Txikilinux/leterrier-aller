/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
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

#include "abuleduapplicationv1.h"
#include "abuleduetiquettesv1.h"
#include "mainwindow.h"
#include "abuleduconstantesv1.h"
#include "abuledusplashscreenv1.h"
#include "exerciceclosure.h"
#include "exerciceordrealpha1.h"
#include "exercicemotsmelanges.h"
#include "exercicephrasesmelangees.h"
#include "exerciceparagraphesmelanges.h"
#include "exercicephrasesansespaces.h"
#include "exercicerapido.h"
#include "version.h"

#if QT_VERSION >= 0x050200
void setBackground(AbulEduEtiquettesV1* graphicsView,const QString &backgroundPath){
    QPixmap pixFond(backgroundPath);
    graphicsView->setBackgroundBrush(QBrush(pixFond.scaledToWidth(graphicsView->width())));
}

QString loadUnit(const QString& unitName) {
    QString path = QString();
    AbulEduFileV1* abeFile = new AbulEduFileV1();
    if(!abeFile->abeFileOpen(unitName)){
        qDebug()<<"Impossible d'ouvrir le module dans loadUnit("<<unitName<<")";
    }
    else{
        path = abeFile->abeFileGetDirectoryTemp().absolutePath();
    }
    return path;
}

void launchExerciseType(const QString& exerciseType,AbulEduEtiquettesV1* graphicsView, QString &unitName){
    QString unitPath = loadUnit(unitName.replace("~",QDir::homePath()));
    if(unitPath.isNull()){
        AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(QObject::trUtf8("Problème !!"),QObject::trUtf8("Impossible d'ouvrir le module, et donc de lancer un exercice..."),true,graphicsView);
        msg->show();
        QObject::connect(msg, SIGNAL(signalAbeMessageBoxCloseOrHide()),graphicsView, SLOT(close()),Qt::UniqueConnection);
    }
    else{
        if(QString::compare(exerciseType, "TTR", Qt::CaseInsensitive) == 0){
            ExerciceClosure *ex = new ExerciceClosure(graphicsView, unitPath);
            ex->connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "OA", Qt::CaseInsensitive) == 0){
            ExerciceOrdreAlpha1 *ex = new ExerciceOrdreAlpha1(graphicsView, unitPath);
            QObject::connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "MOM", Qt::CaseInsensitive) == 0){
            ExerciceMotsMelanges *ex = new ExerciceMotsMelanges(graphicsView, unitPath);
            QObject::connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "PHM", Qt::CaseInsensitive) == 0){
            ExercicePhrasesMelangees *ex = new ExercicePhrasesMelangees(graphicsView, unitPath);
            QObject::connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "PAM", Qt::CaseInsensitive) == 0){
            ExerciceParagraphesMelanges *ex = new ExerciceParagraphesMelanges(graphicsView, unitPath);
            QObject::connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "PSE", Qt::CaseInsensitive) == 0){
            ExercicePhraseSansEspaces *ex = new ExercicePhraseSansEspaces(graphicsView, unitPath);
            QObject::connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "RR", Qt::CaseInsensitive) == 0){
            ExerciceRapido *ex = new ExerciceRapido(graphicsView, unitPath);
            QObject::connect(ex, SIGNAL(exerciceExited()),graphicsView,SLOT(close()),Qt::UniqueConnection);
        }
        else {
            AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(QObject::trUtf8("Problème !!"),QObject::trUtf8("Impossible de lancer l'exercice spécifié..."),true,graphicsView);
            msg->show();
            QObject::connect(msg, SIGNAL(signalAbeMessageBoxCloseOrHide()),graphicsView, SLOT(close()),Qt::UniqueConnection);
        }
        graphicsView->show();
    }
}
#endif

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050600
    /* Ajout support  highDpi scaling */
    AbulEduApplicationV1::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    AbulEduApplicationV1 a(argc, argv,VER_INTERNALNAME_STR, VER_PRODUCTVERSION_STR, VER_COMPANYDOMAIN_STR, VER_COMPANYNAME_STR, VER_UNITVERSION_STR);
    a.setAbeApplicationLongName(QObject::trUtf8(VER_FILEDESCRIPTION_STR));

#if QT_VERSION >= 0x050200
        QCommandLineOption backgroundOption(QStringList()<<"b"<<"background",
                                            QCoreApplication::translate("main","Background parameter"),
                                             QCoreApplication::translate("main", "The Background path (*.jpg)"), "");
        a.getAbeCommandLineParser()->addOption(backgroundOption);
        QCommandLineOption unitOption(QStringList()<<"m"<<"module",
                                            QCoreApplication::translate("main","Module parameter"),
                                             QCoreApplication::translate("main", "The Unit path (*.abe)"), "");
        a.getAbeCommandLineParser()->addOption(unitOption);
        QCommandLineOption exerciseTypeOption(QStringList()<<"e"<<"exercise",
                                            QCoreApplication::translate("main","Exercise type parameter"),
                                             QCoreApplication::translate("main", "The Exercise type (TTR,OA,MOM,PHM,PAM,PSE or RR"), "");
        a.getAbeCommandLineParser()->addOption(exerciseTypeOption);
        a.initCommandLineParser();
        /* Si aucun des trois arguments prévus n'est présent, on lance l'AbulEduSplashScreenV1 qui lancera l'application */
        if(!a.getAbeCommandLineParser()->isSet(backgroundOption) && !a.getAbeCommandLineParser()->isSet(unitOption) && !a.getAbeCommandLineParser()->isSet(exerciseTypeOption)){
            AbulEduSplashScreenV1 *splash = new AbulEduSplashScreenV1(0,true);
            splash->show();
            MainWindow* w = new MainWindow(0);
            splash->setMainWindow(w);
            abeApp->processEvents();
            splash->launch(4000);

        }
        else{
            AbulEduEtiquettesV1* g = new AbulEduEtiquettesV1(QPointF(0,0));
            g->setGeometry(QDesktopWidget().frameGeometry());
            g->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            if(a.getAbeCommandLineParser()->isSet(backgroundOption)){
               setBackground(g,a.getAbeCommandLineParser()->value(backgroundOption));
            }
            if(a.getAbeCommandLineParser()->isSet(unitOption) && a.getAbeCommandLineParser()->isSet(exerciseTypeOption)){
                QString unit = a.getAbeCommandLineParser()->value(unitOption);
               launchExerciseType(a.getAbeCommandLineParser()->value(exerciseTypeOption),g,unit);
            }
        }
#else
     a.initLogger();
	
     /* On peut désormais lancer Aller avec des arguments
     * par exemple, le lancement en console de "leterrier-aller --exercise=ttr --module=~/leterrier/abebox/leterrier-aller/Heureux_ulysse_ce1.abe --fond=~/leterrier/abebox/leterrier-aller/fondecran.jpg"
     * lancera closure sur le module ulysse en utilisant comme fond d'écran le fichier fondecran.jpg  */

    QStringList arguments = a.arguments();
//    qDebug()<<arguments;
    if (arguments.size() == 1)
    {
        AbulEduSplashScreenV1 *splash = new AbulEduSplashScreenV1(0,true);
        splash->show();
        MainWindow* w = new MainWindow(0);
        splash->setMainWindow(w);
    abeApp->processEvents();
        splash->launch(400);
    }
    else {
        QString exerciseType;
        QString unitName;
        QString background;
        QStringListIterator iter(arguments);
        while(iter.hasNext()){
            QString current = iter.next();
            if(current.contains("--exercise")){
                exerciseType = current.split("=").last();
            }
            else if(current.contains("--module=")){
                unitName = current.split("=").last();
                unitName.replace("~",QDir::homePath());
            }
            else if(current.contains("--fond=")){
                background = current.split("=").last();
                background.replace("~",QDir::homePath());
            }
            else if(current.contains("--help")) {
                QMessageBox::information(new QWidget(),"Aide","Vous pourrez lancer ce logiciel avec les options suivantes:\n\
                                         \n\
                                         --help -> cette aide\n\
                                         \n\
                                         --exercise=type exercice -> lance directement un exercice, choisissez parmi ExerciceClosure, ExerciceOrdreAlpha1, ExerciceMotsMelanges, ExercicePhrasesMelangees, ExerciceParagraphesMelanges, ExercicePhraseSansEspaces ou ExerciceRapido\n\
                                         \n\
                                         --module=nom module -> indiquez son chemin absolu\n\
                        \n\
                        --fond=image de fond -> indiquez son chemin absolu\n\
                        ");
                        return 0;
            }
        }
        AbulEduEtiquettesV1* g = new AbulEduEtiquettesV1(QPointF(0,0));
        g->setGeometry(QDesktopWidget().frameGeometry());
        g->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        if(!background.isEmpty()){
            QPixmap pixFond(background);
            g->setBackgroundBrush(QBrush(pixFond.scaledToWidth(g->width())));
        }
        AbulEduFileV1* abeFile = new AbulEduFileV1();
        if(!abeFile->abeFileOpen(unitName)){
            AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(QObject::trUtf8("Problème !!"),QObject::trUtf8("Impossible d'ouvrir le module..."),true,g);
            msg->show();
            QObject::connect(msg, SIGNAL(signalAbeMessageBoxCloseOrHide()),g, SLOT(close()),Qt::UniqueConnection);
        }

        if(QString::compare(exerciseType, "TTR", Qt::CaseInsensitive) == 0){
            ExerciceClosure *ex = new ExerciceClosure(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            ex->connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "OA", Qt::CaseInsensitive) == 0){
            ExerciceOrdreAlpha1 *ex = new ExerciceOrdreAlpha1(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            QObject::connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "MOM", Qt::CaseInsensitive) == 0){
            ExerciceMotsMelanges *ex = new ExerciceMotsMelanges(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            QObject::connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "PHM", Qt::CaseInsensitive) == 0){
            ExercicePhrasesMelangees *ex = new ExercicePhrasesMelangees(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            QObject::connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "PAM", Qt::CaseInsensitive) == 0){
            ExerciceParagraphesMelanges *ex = new ExerciceParagraphesMelanges(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            QObject::connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "PSE", Qt::CaseInsensitive) == 0){
            ExercicePhraseSansEspaces *ex = new ExercicePhraseSansEspaces(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            QObject::connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else if(QString::compare(exerciseType, "RR", Qt::CaseInsensitive) == 0){
            ExerciceRapido *ex = new ExerciceRapido(g, abeFile->abeFileGetDirectoryTemp().absolutePath());
            QObject::connect(ex, SIGNAL(exerciceExited()),g,SLOT(close()),Qt::UniqueConnection);
        }
        else {
            AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(QObject::trUtf8("Problème !!"),QObject::trUtf8("Impossible de lancer l'exercice spécifié..."),true,g);
            msg->show();
            QObject::connect(msg, SIGNAL(signalAbeMessageBoxCloseOrHide()),g, SLOT(close()),Qt::UniqueConnection);
        }
        qDebug()<<abeFile->abeFileGetContent(0).filePath();
        g->show();
    }
#endif
    return a.exec();
}
