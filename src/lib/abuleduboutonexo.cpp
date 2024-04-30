/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)
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
 */

#include "abuleduboutonexo.h"
#include <QDebug>
#include <QPen>

abuleduBoutonExo::abuleduBoutonExo(float scale, QRectF rectZoneSensible,
                                   QString cheminBulle, QPointF posBulle,
                                   QString texte, QFont fontTexte)
{
//    setAcceptDrops(true);
    setAcceptsHoverEvents(true);
    hover = false;

    m_scale              = scale;
    m_rectZoneSensible   = rectZoneSensible;
    m_cheminBulle        = cheminBulle;
    m_posBulle           = posBulle;
    m_texte              = texte;
    m_fontTexte          = fontTexte;

//    m_zoneSensible = new QGraphicsRectItem(m_rectZoneSensible);
//    m_zoneSensible->setPen(QPen(QColor(255,255,255)));
    m_bulle = new QImage(cheminBulle);
//    m_texteBulle = new QGraphicsSimpleTextItem(m_texte, m_bulle);

    qDebug()<<"bouton cree";

}

QRectF abuleduBoutonExo::boundingRect() const
{
    return QRectF(m_rectZoneSensible.x() * m_scale,
                  m_rectZoneSensible.y() * m_scale,
                  m_rectZoneSensible.width() * m_scale,
                  m_rectZoneSensible.height() * m_scale);
}

void abuleduBoutonExo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawRect(m_rectZoneSensible.x() * m_scale,
                      m_rectZoneSensible.y() * m_scale,
                      m_rectZoneSensible.width() * m_scale,
                      m_rectZoneSensible.height() * m_scale);
    if(hover)
    {
    QImage image = m_bulle->scaledToWidth(m_bulle->width()*m_scale, Qt::SmoothTransformation);

    painter->drawImage(QPoint(m_posBulle.x() * m_scale, m_posBulle.y() * m_scale), image);
    }
}

void abuleduBoutonExo::resizeBoutonExo(float scale)
{
    m_scale = scale;
}

void abuleduBoutonExo::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hover = true;
    update();
    qDebug()<<"dragEnterEvent";
}

void abuleduBoutonExo::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hover = false;
    qDebug()<<"dragLeaveEvent";
}

void abuleduBoutonExo::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
    qDebug()<<"abuleduBoutonExo::MousePressEvent";
}
