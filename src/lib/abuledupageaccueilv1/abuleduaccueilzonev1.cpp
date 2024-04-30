/**
  * Classe AbulEduZoneV1
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

#include "abuleduaccueilzonev1.h"

#ifndef DEBUG_ABULEDUPAGEACCUEILV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduZoneV1::AbulEduZoneV1(int numero, const QRectF &zone, float ratio, const QString &cheminBulle,
                             const QString &texteBulle, const QFont &font, const QPoint &posBulle,
                             const QPoint &posFleche, const QString &typeFleche,  int offsetV,
                             bool actif, QGraphicsItem *parent):
    QObject(0),
    QGraphicsRectItem(parent),
    m_numero(numero),
    m_rect(zone),
    m_cheminBulle(cheminBulle),
    m_texteBulle(texteBulle),
    m_posBulle(posBulle),
    m_posFleche(posFleche),
    m_font(font),
    m_offsetV(offsetV),
    m_actif(actif),
    m_ratio(ratio),
    m_petiteFleche(":/abuledupageaccueilv1/images/"+typeFleche+"fleche.png"),
    m_grandeFleche(":/abuledupageaccueilv1/images/"+typeFleche+"flecheHover.png"),
    m_bulle( new AbulEduBulleV1(m_posBulle, m_cheminBulle, m_texteBulle, m_offsetV, m_font)),
    m_fleche(new AbulEduFlecheV1(m_posFleche-QPoint(10,10),m_petiteFleche,m_grandeFleche))
//
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setBrush(QColor(Qt::transparent));
    abeZoneDrawRect(false);

    m_fleche->setVisible(true);

#ifdef __ABULEDUTABLETTEV1__MODE__
    m_bulle->setVisible(true);
#else
    m_bulle->setVisible(false);
#endif

    if(!m_actif){
        setAcceptHoverEvents(false);
    }
    else
    {
        setAcceptHoverEvents(true);
        setCursor(Qt::PointingHandCursor);
    }
    abeZoneResizeRect(m_ratio);
}

AbulEduZoneV1::~AbulEduZoneV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete m_bulle;
    delete m_fleche;
}

void AbulEduZoneV1::abeZoneSetActif(bool YesNo)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_actif = YesNo;
    if (m_actif)
    {
        setAcceptHoverEvents(true);
        setCursor(Qt::PointingHandCursor);
    }
    else
    {
        setAcceptHoverEvents(false);
        setCursor(Qt::ArrowCursor);
    }
}

void AbulEduZoneV1::abeZoneAfficheZone(bool yesNo)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* Quand on est sur tablette on ne cache pas les bulles */
#ifndef __ABULEDUTABLETTEV1__MODE__
    m_bulle->setVisible(yesNo);
    m_fleche->abeFlecheAfficheGrossie(yesNo);
    setVisible(yesNo);
#endif
}

void AbulEduZoneV1::abeZoneDrawRect(bool YesNo, const QColor &color, int thickness, const Qt::PenStyle &style)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if (YesNo == true)
    {
        setPen(QPen(color,thickness,style));
    }
    else
    {
        setPen(QPen(Qt::NoPen));
    }
    update();
}

#ifdef __ABULEDUTABLETTEV1__MODE__
void AbulEduZoneV1::abeZoneSetImageBulleTablette()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_bulle->abeBulleSetImageBulle(m_cheminBulle.replace("bulle","bulleTablette"));
    m_fleche->abeFlecheAfficheGrossie(false);
    m_bulle->abeBulleUpdateBulle();
}
#endif

void AbulEduZoneV1::abeZoneResizeRect(const float &ratio)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_ratio = ratio;
    setRect(m_rect.x() * ratio,
            m_rect.y() * ratio,
            m_rect.width() * ratio,
            m_rect.height() * ratio);
    m_bulle->abeBulleResize(ratio);
    m_fleche->abeFlecheResize(ratio);
}

void AbulEduZoneV1::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(m_actif){
       m_bulle->setVisible(true);
#ifdef __ABULEDUTABLETTEV1__MODE__
       m_bulle->abeBulleSetImageBulle(m_cheminBulle.replace("bulleTablette","bulle"));
       m_bulle->abeBulleUpdateBulle();
#endif
       m_fleche->abeFlecheAfficheGrossie(true);
    }
}

void AbulEduZoneV1::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* Quand on est sur tablette on ne cache pas les bulles */
#ifdef __ABULEDUTABLETTEV1__MODE__
    abeZoneSetImageBulleTablette();
#else
    m_bulle->setVisible(false);
#endif
    m_fleche->abeFlecheAfficheGrossie(false);
}
