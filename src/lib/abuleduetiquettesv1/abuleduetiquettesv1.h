/**
  * Le terrier d'AbulEdu
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.abuledu.org/projects/ryxeo/wiki/AbulEduEtiquettesV1
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

#ifndef ABULEDUETIQUETTESV1_H
#define ABULEDUETIQUETTESV1_H

#include <QGraphicsView>
#include <QDebug>
#include <QScrollBar>
#include "abuleducellulev1.h"
#include <QMouseEvent>
#if QT_VERSION > 0x050000
    #include <QScroller>
#endif

/** @file abuleduetiquettesv1.h
  * @version 1
  * @brief Cette librairie introduit quatre classes permettant la gestion du Drag&Drop d'étiquettes (texte et/ou image).
  */
class AbulEduEtiquettesV1 : public QGraphicsView
{
    Q_OBJECT
public:

    /** Constructeur de la classe
      * @param position, QPointF, definit un point sur le plan en utilisant une précision en float
      * @param QWidget *parent, un pointeur sur le composant graphique parent de cet objet, initialise par defaut a 0
      */
    explicit AbulEduEtiquettesV1(const QPointF& position, QWidget *parent = 0);

    ~AbulEduEtiquettesV1();

    /** Permet de fixer le taille du widget
      * @param taille, la taille en pixel
      * @return void
      */
    void abeEtiquettesSetDimensionsWidget(const QSize& taille);

    /** Fonction de convenance dessinant un tableau de cellules
      * @return  QMap retournant la liste des pointeurs des cellules avec le numéro de la cellule comme clé
      */
    QMap<int, AbulEduCelluleV1 *> abeEtiquettesDessineTableau(const QPointF& position, int nbColonnes, int largeurColonne, int nbLignes, int hauteurLigne);

private:

    AbulEduSceneEtiquettesV1 *m_scene;
    QPointF m_position;
    bool m_localDebug;

signals:
    int wheelTurn(int);

protected slots:
    void wheelEvent(QWheelEvent *event);
};

#endif // ABULEDUETIQUETTESV1_H
