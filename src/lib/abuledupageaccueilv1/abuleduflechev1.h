/**
  * Classe AbulEduFlecheV1
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduPageAccueilV1
  * @author TO FIX
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

#ifndef ABULEDUFLECHEV1_H
#define ABULEDUFLECHEV1_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "abuleduloggerv1.h"

class AbulEduFlecheV1 : public QGraphicsObject
{
    Q_OBJECT
public:

    explicit AbulEduFlecheV1(const QPointF& position, const QString& petiteImage,
                             const QString& grandeImage, QGraphicsItem *parent = 0);

    ~AbulEduFlecheV1();

    void abeFlecheResize(const float &ratio);

    /** Fixe la position de la flèche
      * @param position la position de la flèche en flottant (sommet haut gauche)
      */
    inline void abeFlecheSetPosition(const QPointF& position) {m_position = position;}

    /** Retourne la position de la flèche
      * @return la position de la flèche en flottant
      */
    const QPointF& abeFlecheGetPosition() {return m_position;}

    void abeFlecheSetCheminImage(const QString &);

    const QString& abeFlecheGetCheminImage();

    void abeFlecheAfficheGrossie(bool trueFalse);

private:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;

    QPointF mousePressedPos;
    QPointF m_position;
    QString m_image;
    QString m_petiteImage;
    QString m_grandeImage;
    float   m_ratio;

public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void posFlecheChanged(const QPointF& );
};

#endif // ABULEDUFLECHEV1_H
