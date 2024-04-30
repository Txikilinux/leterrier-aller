/** Classe AbulEduPageAccueilV1
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduPageAccueilV1
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
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

#ifndef ABULEDUPAGEACCUEILV1_H
#define ABULEDUPAGEACCUEILV1_H

#include <QGraphicsView>
#include <QSettings>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDir>
#include <QGridLayout>
#include <QMainWindow>
#include <QGraphicsProxyWidget>
#include "abuleduapplicationv1.h"
#include "abuleduaccueilzonev1.h"
#include "abuleduaccueilbullev1.h"
#include "abuledugraphicmenuv1.h"
#include "abuleduloggerv1.h"

class AbulEduPageAccueilV1 : public QGraphicsView
{
    Q_OBJECT

public:
    /** Constructeur : on lui passe le fichier de conf à partir duquel
      * la page d'accueil sera construite.
      * @param config pointeur vers le fichier de conf
      * @param listeTexteBulles pointeur vers la Qmap des numéro des zone et du texte correspondant (utile pour traduction)
      */
    explicit AbulEduPageAccueilV1(QMap<int, QString> *listeTexteBulles, QWidget *parent, QMainWindow* mainwindow = 0);

    explicit AbulEduPageAccueilV1(QWidget *parent);

    ~AbulEduPageAccueilV1();

    /// Affiche les bulles actives ou les masque
    void abePageAccueilMontreBulles(bool vraiFaux,bool isDemo = false);

    /// Desactive les zones sensibles
    void abePageAccueilDesactiveZones(bool vraiFaux);

#ifdef __ABULEDUTABLETTEV1__MODE__
    /// Remet la bulle style tablette à toutes les zones
    void abePageAccueilRetablisBullesTablette();
#endif

    /// Retourne le pointeur du menu graphique
    AbulEduGraphicMenuV1 * abePageAccueilGetMenu(){ return m_parchemin;}

    /// Retourne la liste des zones cliquables de la page d'accueil
    const QList<AbulEduZoneV1 *> &abePageAccueilGetZones(){ return m_listeZones;}

    /// Pointeur vers le bouton qui permet de revenir à un éditeur masqué
    QPushButton* abePageAccueilGetBtnRevenirEditeur(){ return m_btnRevenirEditeur;}

    void setDimensionsWidgets();

    /// Réécrit les textes des bulles : utilisé pour traduire la page d'accueil par exemple
    void abePageAccueilRetranslate();

    QMainWindow *abePageAccueilGetMainWindow() const;

protected :
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private:
    void installPageAccueil();
    void initAbulEduPageAccueilV1();


    QGraphicsScene          *m_scene;
    QSettings               *m_config;
    QMap<int, QString>      *m_listeTexteBulles;
    QPixmap                 m_imageFond;
    QSize                   m_tailleImageFond;
    float                   m_proportionImageFond;
    float                   m_ratio;
    AbulEduZoneV1           *m_zone;
    QList<AbulEduZoneV1 *>  m_listeZones;
    AbulEduGraphicMenuV1    *m_parchemin;
    QPushButton             *m_btnRevenirEditeur;
    QMainWindow             *m_mainWindow;


signals:
    /** Signal émis par le bouton pressé
      * @param numero le numéro du bouton pressé
      * @param nom le texte visible sur le bouton pressé
      */
    void boutonPressed(const int numero, const QString& nom);

    /// deprecated, remplacee par boutonPressed(int numero, QString nom);
    void boutonPressed(const int numero);

    void dimensionsChangees();
    void signalAbePageAccueilMousePressed();
};

#endif // ABULEDUPAGEACCUEILV1_H
