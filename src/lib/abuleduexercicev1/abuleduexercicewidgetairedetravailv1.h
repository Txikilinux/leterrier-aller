/**
  * Classe
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  */
/*
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

 * @file abuleduexercicewidgetairedetravail.h
 * @version 1
 */

#ifndef ABULEDUEXERCICEWIDGETAIREDETRAVAILV1_H
#define ABULEDUEXERCICEWIDGETAIREDETRAVAILV1_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include "ui_abuleduexercicewidgetairedetravailv1.h"
#include "abuleduapplicationv1.h"
#include "abuleduloggerv1.h"
#include "abuledupageaccueilv1.h"

namespace Ui {
    class AbulEduExerciceWidgetAireDeTravailV1;
}

class AbulEduExerciceWidgetAireDeTravailV1 : public QWidget
{
    Q_OBJECT

public:

    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le widget parent de l'objet, initialise par defaut a 0
      */
    explicit AbulEduExerciceWidgetAireDeTravailV1(QWidget *parent = 0);

    /** Destructeur par defaut de la classe */
    ~AbulEduExerciceWidgetAireDeTravailV1();

    /** Accesseur public pour fermer / nettoyer / supprimer l'objet */
    void abeAireDeTravailClose();

    Ui::AbulEduExerciceWidgetAireDeTravailV1 *ui;

    /** Permet de definir l'image de fond
      * @param chemin, chaine de caractere, le chemin de l'emplacement de l'image
      */
    void setImageFond(const QString &chemin);

    /** Permet de definir l'image de fond
      * @param fond, la QPixmap à utiliser
      */
    void setImageFond(const QPixmap &pixmap);

protected:

    /** Permet de changer d'evenement
      * @param *e, QEvent, un pointeur sur un evenement
      * @link http://qt-project.org/doc/qt-4.8/QEvent.html
      */
    void changeEvent(QEvent *e);

private:
    QGraphicsScene        *gsPrincipale;
    QPixmap                m_imageFond;
    QGraphicsPixmapItem   *m_fond;

private slots :

    /** Permet de definir la dimension du widget
      * @return void
      */
    void setDimensionsWidget();
};

#endif // ABULEDUEXERCICEWIDGETAIREDETRAVAILV1_H
