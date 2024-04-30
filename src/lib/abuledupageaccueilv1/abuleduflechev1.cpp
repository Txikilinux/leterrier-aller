/**
  * Classe AbulEduFlecheV1
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduPageAccueilV1
  * @author 2013-2014 philippe.cadaugade <philippe.cadaugade@ryxeo.com>
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

#include "abuleduflechev1.h"

#ifndef DEBUG_ABULEDUPAGEACCUEILV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduFlecheV1::AbulEduFlecheV1(const QPointF &position, const QString &petiteImage,
                                 const QString &grandeImage, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    mousePressedPos(QPointF()),
    m_position(position),
    m_image(petiteImage),
    m_petiteImage(petiteImage),
    m_grandeImage(grandeImage),
    m_ratio(0)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

AbulEduFlecheV1::~AbulEduFlecheV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduFlecheV1::abeFlecheResize(const float& ratio)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_ratio = ratio;
}

void AbulEduFlecheV1::abeFlecheSetCheminImage(const QString& chemin)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_image = chemin;
    update();
}

const QString &AbulEduFlecheV1::abeFlecheGetCheminImage()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_image;
}

void AbulEduFlecheV1::abeFlecheAfficheGrossie(bool trueFalse)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(trueFalse)
    {
        abeFlecheSetCheminImage(m_grandeImage);
    }
    else
    {
        abeFlecheSetCheminImage(m_petiteImage);
    }
    prepareGeometryChange();
}

void AbulEduFlecheV1::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(!m_image.isNull())
    {
        QImage image = QImage(m_image).scaled(QImage(m_image).width()*m_ratio,QImage(m_image).height()*m_ratio,
                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation);


        painter->drawImage(QPoint(m_position.x() * m_ratio, m_position.y() * m_ratio), image);
    }
}

QRectF AbulEduFlecheV1::boundingRect() const
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return QRectF(QPointF(m_position.x() * m_ratio - 20, m_position.y() * m_ratio - 20),
                  QSizeF(QImage(m_image).width()+20 * m_ratio,QImage(m_image).height()*m_ratio + 20));
}


void AbulEduFlecheV1::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    grabMouse();
    mousePressedPos = event->pos();
}

void AbulEduFlecheV1::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ungrabMouse();
    if(this->flags().testFlag(QGraphicsItem::ItemIsMovable)){
    QPointF deplacement(event->scenePos() - mousePressedPos);
    emit posFlecheChanged(deplacement);
    }
}
