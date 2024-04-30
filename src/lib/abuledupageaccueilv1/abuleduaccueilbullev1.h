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

#ifndef ABULEDUBULLEV1_H
#define ABULEDUBULLEV1_H

#include <QGraphicsObject>
#include <QFont>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "abuleduloggerv1.h"

class AbulEduBulleV1 : public QGraphicsObject
{
    Q_OBJECT
public:
    /** Constructeur de la bulle (toutes les positions sont à donner par rapport à l'image à l'échelle 1)
      * @param position la position de la bulle dans l'image (échelle 1)
      * @param fichier le chemin vers l'image de la bulle
      * @param texte le texte de la bulle
      * @param offsetVertical le décallage vertical du texte par rapport à l'image de la bulle(en haut à gauche)
      * @param font la fonte du texte
      * @param le taux d'agrandissement de l'image de fond
      */
    AbulEduBulleV1(const QPointF& position, const QString& fichier,
                   const QString& texte, int offsetVertical, const QFont& font, float ratio = 1, QGraphicsObject *parent = 0);

    ~AbulEduBulleV1();

    inline void abeBulleUpdateBulle() {update(boundingRect());}

    /** Fixe le taux d'agrandissement de la bulle
      * @param ratio le taux d'agrandissement en flottant
      */
    void abeBulleResize(const float& ratio);

    /** Fixe la position de la bulle
      * @param position la position de la bulle en flottant (sommet haut gauche)
      */
    void abeBulleSetPosition(const QPointF& position){ m_position = position;}

    /** Retourne la position de la bulle
      * @return la position de la bulle en flottant
      */
    const QPointF& abeBulleGetPosition(){ return m_position;}

    /** Fixe l'image de la bulle
      * @param cheminBulle le nom de la bulle
      */
    void abeBulleSetImageBulle(const QString &cheminBulle);

    /** Retourne le nom dela bulle
      * @return le nom de la bulle
      */
    const QString& abeBulleGetImageBulle(){ return m_cheminBulle;}

    /** Fixe le texte de la bulle */
    void abeBulleSetTexte(const QString& texte){ m_texte = texte;}

    /** Retourne le texte de la bulle */
    const QString& abeBulleGetText(){ return m_texte;}

    /** Fixe la font de la bulle
      * @param font la Qfont de la bulle
      */
    void abeBulleSetFont(const QFont &font){ m_font = font;}

    /** Retourne la font de la bulle */
    const QFont& abeBulleGetFont(){ return m_font;}

    /** Fixe la couleur du texte dela bulle
      * @param color la couleur du Texte
      */
    void abeBulleSetColorPen(const QColor& color){ m_couleurFont = color;}

    /** Fixe le décalage verticale du texte dans la bulle
      * @param offsetV le décalage en pixels
      */
    void abeBulleSetOffsetV(int offsetV){ m_offsetVertical = offsetV;}

    /** Retourne le décalage vertical
      * @return le décalage vertical en pixels
      */
    int abeBulleGetOffsetV(){ return m_offsetVertical;}

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath shape() const;

    QPointF     m_position;
    QString     m_texte;
    int         m_offsetVertical;
    QImage     *m_bulle;
    QString     m_cheminBulle;
    QFont       m_font;
    float       m_ratio;
    QPointF     mousePressedPos;
    QColor      m_couleurFont;
};

#endif // ABULEDUBULLEV1_H
