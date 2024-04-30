/** Menu graphique qui s'affiche en haut des logiciels
  * AbulEduGraphicMenuV1
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduGraphicMenuV1
  *
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2013 Éric Seigne <eric.seigne@ryxeo.com>
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

#ifndef ABULEDUGRAPHICMENUV1_H
#define ABULEDUGRAPHICMENUV1_H

#include <QStringList>
#include <QWidget>
#include <QToolTip>
#include <QSettings>
#include "abuleduflatboutonv1.h"
#include "abuleduapplicationv1.h"
#include "ui_abuledugraphicmenuv1.h"
#include "abuleduloggerv1.h"

namespace Ui {
    class AbulEduGraphicMenuV1;
}

class AbulEduGraphicMenuV1 : public QWidget
{
    Q_OBJECT

public:
    explicit AbulEduGraphicMenuV1(QWidget *parent = 0);
    ~AbulEduGraphicMenuV1();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AbulEduGraphicMenuV1 *ui;
    QSize m_sizeWidget;
    //entier de 0 a 4 (pour le moment)
    //permettant de savoir a quelle étape de l'animation on se
    //trouve = 0 menu fermé et 9 = menu complet ouvert
    int m_animationStep;
    QPixmap m_imageFond;
    QMap<int,QString> m_mapComposants;
    QSettings* m_config;

public slots:
    void setDimensionsWidget();

private slots:
    void plierDeplierMenu(bool deplier);
    void plierMenu();
    void deplierMenu();
    void afficherCacher(const QString& composant, bool deplier);
    void on_abeGraphicMenuBtnBox_clicked();

    void on_abeGraphicMenuBtnQuit_clicked();
    void on_abeGraphicMenuBtnClose_clicked();
    void on_abeGraphicMenuBtnHelp_clicked();
    void on_abeGraphicMenuBtnReport_clicked();
    void on_abeGraphicMenuBtnSettings_clicked();
    void on_abeGraphicMenuBtnOpenMenu_clicked();

signals:
    void btnQuitterTriggered();
    void btnAideTriggered();
    void btnFermerTriggered();
    void btnBoxTriggered();
    void btnLivretTriggered();
    void btnParametresTriggered();
    void btnOuvrirTriggered();
};

#endif // ABULEDUGRAPHICMENUV1_H
