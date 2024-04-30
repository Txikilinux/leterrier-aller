/** Le terrier d'AbulEdu
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
*
  * @file abuledusceneetiquettesV1.h
  * @version 1
  */

#ifndef ABULEDUSCENEETIQUETTESV1_H
#define ABULEDUSCENEETIQUETTESV1_H

#include <QGraphicsScene>
#include <QEvent>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

class AbulEduSceneEtiquettesV1 : public QGraphicsScene
{
    Q_OBJECT
public:

    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le composant graphique parent de cet objet, initialise par defaut a 0
      */
    explicit AbulEduSceneEtiquettesV1(QObject *parent = 0);
    ~AbulEduSceneEtiquettesV1();

private:
    bool m_localDebug;

protected slots:
    void wheelEvent(QGraphicsSceneWheelEvent *event);
};

#endif // ABULEDUSCENEETIQUETTESV1_H
