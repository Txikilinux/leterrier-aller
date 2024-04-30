/** AbulEduLabelWidgetV1
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2013 Éric Seigne <eric.seigne@ryxeo.com>
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

#ifndef ABULEDULABELWIDGETV1_H
#define ABULEDULABELWIDGETV1_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include "abuledulabelframev1.h"
#include "abuleduflatboutonv1.h"
#include "abuleduloggerv1.h"

#if QT_VERSION > 0x050000
    #include <QScroller>
#endif

class AbulEduLabelWidgetV1 : public QFrame
{
    Q_OBJECT

public:
    explicit AbulEduLabelWidgetV1(QWidget *parent = 0);
    ~AbulEduLabelWidgetV1();
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    AbulEduFlatBoutonV1 *btnGauche;
    AbulEduFlatBoutonV1 *btnDroite;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QScrollArea *labelsScrollArea;
    QWidget *scrollAreaWidgetContents;
    AbulEduFlatBoutonV1 *btnHaut;
    AbulEduFlatBoutonV1 *btnBas;
    QSpacerItem *verticalSpacer_2;

signals:
        void moveSelectedLabelUp();
        void moveSelectedLabelDown();
        void moveSelectedLabelRight();
        void moveSelectedLabelLeft();

public slots:
        void on_btnHaut_clicked();
        void on_btnBas_clicked();
        void on_btnGauche_clicked();
        void on_btnDroite_clicked();
        void btnDroiteSetVisible(bool);
        void btnGaucheSetVisible(bool);
        void btnHautSetVisible(bool);
        void btnBasSetVisible(bool);
        void btnDroiteSetEnabled(bool);
        void btnGaucheSetEnabled(bool);
        void btnHautSetEnabled(bool);
        void btnBasSetEnabled(bool);
        void mousePressEvent(QMouseEvent *);
};

#endif // ABULEDULABELWIDGETV1_H
