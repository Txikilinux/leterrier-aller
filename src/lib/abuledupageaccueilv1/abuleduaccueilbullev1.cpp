/**
  * Classe AbulEduBulleV1
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduPageAccueilV1
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

#include "abuleduaccueilbullev1.h"

#ifndef DEBUG_ABULEDUPAGEACCUEILV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduBulleV1::AbulEduBulleV1(const QPointF& position, const QString& cheminBulle, const QString& texte,
                               int offsetVertical, const QFont& font, float ratio, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    m_position(position),
    m_texte(texte),
    m_offsetVertical(offsetVertical),
    m_bulle(new QImage(cheminBulle)),
    m_cheminBulle(cheminBulle),
    m_font(font),
    m_ratio(ratio),
    m_couleurFont(QColor(Qt::black))
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setVisible(true);
}

AbulEduBulleV1::~AbulEduBulleV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete(m_bulle);
}

void AbulEduBulleV1::abeBulleResize(const float &ratio)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_ratio = ratio;
    prepareGeometryChange();
}

void AbulEduBulleV1::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QFont font = m_font;
    int taillepixel = m_font.pixelSize() *m_ratio;
    if(taillepixel > 0)
        font.setPixelSize(taillepixel);
    painter->setFont(font);
    QFontMetrics fontMetric(font);
    int texteSize = fontMetric.width(m_texte);
    QImage image = m_bulle->scaled(texteSize +40 * m_ratio,
                                   m_bulle->height() * m_ratio,
                                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    painter->drawImage(QPoint(m_position.x() * m_ratio, m_position.y() * m_ratio), image);
    painter->setPen(m_couleurFont);
    painter->drawText(QPoint((m_position.x() + 20)* m_ratio,
                             (m_position.y() + 50 + m_offsetVertical) * m_ratio),
                      m_texte);
}

QRectF AbulEduBulleV1::boundingRect() const
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    const int pixelWidth = QFontMetrics(m_font).width(m_texte);

    return QRectF(m_position.x() * m_ratio , m_position.y() * m_ratio,
                   (m_bulle->width() + pixelWidth) * m_ratio, m_bulle->height() * m_ratio);
}

QPainterPath AbulEduBulleV1::shape() const
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void AbulEduBulleV1::abeBulleSetImageBulle(const QString& cheminBulle)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_bulle = new QImage(cheminBulle);
    m_cheminBulle = cheminBulle;
}
