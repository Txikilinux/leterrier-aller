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

#ifndef ABULEDUZONEV1_H
#define ABULEDUZONEV1_H
#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QDebug>
#include <QFileInfo>
#include <QCursor>
#include <QBitmap>
#include "abuleduaccueilbullev1.h"
#include "abuleduflechev1.h"
#include "abuleduloggerv1.h"

class AbulEduZoneV1 : public QObject, public  QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit AbulEduZoneV1(int numero, const QRectF& zone, float ratio,
                           const QString& cheminBulle, const QString& texteBulle, const QFont& font,
                           const QPoint& posBulle,const QPoint &posFleche, const QString &typeFleche,
                           int offsetV, bool actif = 1, QGraphicsItem *parent = 0);

    ~AbulEduZoneV1();

    /** Modifie la taille de la zone sensible d'un facteur ratio
      * @param ratio le taux d'agrandissement
      */
    void abeZoneResizeRect(const float& ratio);

    /** Fixe la taille de la zone sensible
      * @param rect le rect de la zone
      */
    inline void abeZoneSetRect(const QRectF& rect){ m_rect = rect;}

    /** Récupère un pointeur vers la bulle
      * @return le pointeur de la bulle
      */
    inline AbulEduBulleV1* abeZoneGetBulle(){ return m_bulle;}

    /** Récupère un pointeur vers la flèche
      * @return le pointeur de la flèche
      */
    inline AbulEduFlecheV1 *abeZoneGetFleche(){return m_fleche;}

    /** Récupère le rect de la zone sensible
      * @return le rect de la zone sensible
      */
    const QRectF& abeZoneGetRect(){ return m_rect;}

    /** Retourne le numéro de la zone sensible
      * @return le numéro de la zone sensible
      */
    inline int abeZoneGetNumero(){ return m_numero;}

    /** Détermine si la zone est sensible ou pas
      * @param YesNo true : la zone est sensible, false : la zone n'est pas sensible ni visible
      */
    void abeZoneSetActif(bool YesNo);

    /** teste si la zone est active */
    inline bool abeZoneIsActif(){ return m_actif;}

    /** Affiche la zone et sa bulle */
    void abeZoneAfficheZone(bool yesNo);

    void abeZoneDrawRect(bool YesNo, const QColor& color = Qt::black, int thickness = 2, const Qt::PenStyle& style = Qt::DashLine);

#ifdef __ABULEDUTABLETTEV1__MODE__
    void abeZoneSetImageBulleTablette();
#endif

private:
    int m_numero;
    QRectF m_rect;
    QString m_cheminBulle;
    QString m_texteBulle;
    QPoint m_posBulle;
    QPoint m_posFleche;
    QFont m_font;
    int m_offsetV;
    bool m_actif;
    float m_ratio;
    QString m_petiteFleche;
    QString m_grandeFleche;
    AbulEduBulleV1 *m_bulle;
    AbulEduFlecheV1* m_fleche;

public slots:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};

#endif // ABULEDUZONEV1_H
