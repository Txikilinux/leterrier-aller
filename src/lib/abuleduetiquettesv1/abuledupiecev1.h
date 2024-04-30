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
*/



#ifndef ABULEDUPIECEV1_H
#define ABULEDUPIECEV1_H

#include <QGraphicsObject>
#include <QPainter>
#include <QCursor>
#include <QFont>
#include <QFontMetrics>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include "abuledusceneetiquettesv1.h"
#include "abuleduconstantesv1.h"
#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
#include "abuledumultimediav1.h"
#endif

/** @file abuledupiecev1.h
  * @version 1
  */

class AbulEduCelluleV1;

class AbulEduPieceV1 : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF position READ pos WRITE setPos)
public:
    /** Définition d'un type customisé pour les pièces */
    enum { AbulEduPieceType = UserType + abe::abeEtiquettePiece };

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return AbulEduPieceType;
    }
    explicit AbulEduPieceV1(const QRectF& rect, QGraphicsObject *parent = 0);
    explicit AbulEduPieceV1(const QSizeF& rect, QGraphicsObject *parent = 0);

    ~AbulEduPieceV1();

    /** Positionne la pièce à l'initialisation
      * @param pos la position de la pièce sur la scene est un QPointF
      */
    void abePieceSetPositionOriginale(const QPointF& pos);

    /** Positionne la pièce à l'initialisation
      * @param x et y la position de la pièce sur la scene sont deux réels
      */
    void abePieceSetPositionOriginale(qreal x, qreal y);

    /** Retourne la position originale de la pièce
      * @return position de la pièce
      */
    const QPointF& abePieceGetPositionOriginale(){ return m_positionOriginale;}

    /** Précise si la pièce doit revenir à sa position de départ, ou à sa dernière position(déplacement libre) */
    void abePiecePlaceFixe(bool yesNo){ m_retourPositionOriginale = yesNo;}

    /** Fixe le Pen de contour
      * @param contour le pen de contour taille et couleur
      */
    void abePieceSetPen(const QPen& contour){ m_penContour = contour;}

    /** Fixe la brosse de Fond
      * @param brosseFond la brosse de remplissage du fond
      */
    void abePieceSetBrush(const QBrush& brosseFond){ m_brosseFond = brosseFond;}

    /** Fixe la font de l'étiquette */
    void abePieceSetFont(const QFont& font){ m_font = font;}

    /** Récupère la font de l'étiquette */
    const QFont& abePieceGetFont(){ return m_font;}

    /** Fixe la taille de l'étiquette */
    void abePieceSetSize(const QSizeF& rect){ m_rect = QRectF(0,0,rect.width(), rect.height());}

    QSizeF abePieceGetSize(){ return m_rect.size();}

    /** Fixe l'arrondi de la pièce
      * @param xRadius
      * @param yRadius
      */
    void abePieceSetRadius(int xRadius, int yRadius);

    /** Attribue une image qui sera dessinée sur le fond
      * @param cheminImage le chemin vers le fichier de l'image
      */
    void abePieceSetImage(const QString& cheminImage, const QPointF& positionImage = QPointF(0,0), const QSize& tailleImage = QSize(0,0));


    void abePieceSetPixmap(const QPixmap pixmap, const QPointF& positionImage = QPointF(0,0), const QSize& tailleImage = QSize(0,0));

    /** Alignement de l'image dans l'étiquette
      * Qt::AlignLeft       0x0001	Aligns with the left edge.
      * Qt::AlignRight      0x0002	Aligns with the right edge.
      * Qt::AlignHCenter	0x0004	Centers horizontally in the available space.
      * Qt::AlignJustify	0x0008	Justifies the text in the available space.
      * Qt::AlignTop        0x0020	Aligns with the top.
      * Qt::AlignBottom     0x0040	Aligns with the bottom.
      * Qt::AlignVCenter	0x0080	Centers vertically in the available space.
      * Qt::AlignCenter	AlignVCenter | AlignHCenter	Centers in both dimensions.
      */
    void abePieceSetImageAlignement(const Qt::Alignment& alignement);

    /** Fixe la taille de l'image */
    void abePieceSetImageTaille(const QSize& tailleImage, const Qt::AspectRatioMode& modeRatio = Qt::KeepAspectRatio);

    /** Fixe la position de l'image */
    void abePieceSetImagePosition(const QPointF &positionImage);

    /** Fixe la marge entre le bord et les éléments internes
      * @param marge le nombre de pixels entre le bord et l'image ou le texte
      */
    void abePieceSetMarge(int marge){ m_marge = marge;}

    int abePieceGetMarge(){ return m_marge;}

    /** Fixe le texte de la pièce
      * @param texte le texte de la pîèce
      */
    void abePieceSetTexte(const QString& texte, const QPointF& positionTexte = QPointF(0,0), const QSize& tailleRectTexte = QSize(0,0));

    /** Retourne le texte de la pièce */
    const QString& abePieceGetTexte(){ return m_texte;}

    /** Alignement du texte dans l'étiquette
      * Qt::AlignLeft       0x0001	Aligns with the left edge.
      * Qt::AlignRight      0x0002	Aligns with the right edge.
      * Qt::AlignHCenter	0x0004	Centers horizontally in the available space.
      * Qt::AlignJustify	0x0008	Justifies the text in the available space.
      * Qt::AlignTop        0x0020	Aligns with the top.
      * Qt::AlignBottom     0x0040	Aligns with the bottom.
      * Qt::AlignVCenter	0x0080	Centers vertically in the available space.
      * Qt::AlignCenter	AlignVCenter | AlignHCenter	Centers in both dimensions.
      */
    void abePieceSetTexteAlignement(const Qt::Alignment &alignement);

    /** Fixe la position du texte */
    void abePieceSetTextePosition(const QPointF& position);

    /** Fixe la taille du rectangle contenant le texte */
    void abePieceSetTexteSize(const QSize& tailleTexteRect);

    /** Fixe le Pen de Texte
      * @param contour le pen de contour taille et couleur
      */
    void abePieceSetTextePen(const QPen& texte){ m_penTexte = texte;}

    /** Rend la pièce insensible aux événements souris */
    void abePieceSetAcceptMouse(bool trueFalse){ m_isAcceptMouse = trueFalse;}

    /** Retourne les coordonnées du centre de la pièce */
    const QPointF& abePieceGetCenter();

    /** Autorise ou interdit le retour à la ligne dans l'étiquette */
    inline void abePieceSetAcceptWordWrap(bool trueFalse) {m_isAcceptWordWrap = trueFalse;}

    /** Déplace la piéce dans la cellule passée en paramétre
     * @param cellToGo La cellule a atteindre
     */
    QPointF abePieceMoveToCell(AbulEduCelluleV1 *cellToGo , bool anim);

#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    /** Ajout un AbulEduMultiMediaAudioControlWidgetV1 à l'AbulEduPieceV1 */
    void abePieceAddAbeMultiMediaAudioControlWidget(QGraphicsScene *scene, QString picture = QString(), QString sound = QString(), QString text = QString());

    /** Getteur de l'AbulEduMultiMediaV1 */
    inline AbulEduMultiMediaV1* abePieceGetAbeMultiMedia() {return m_multiMedia;}

    /** Met à jour les paramètres de l'AbulEduMultiMediaV1 */
    void abePieceUpdateAbeMultiMedia(QString picture, QString sound, QString text);
#endif

private:
    /** Calcule la taille du rectangle contenant le texte */
    QRectF abePieceTexteBoundingRect();
    void initConstructeur();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

//    QGraphicsDropShadowEffect *ombre;
    QVariant itemChange(const GraphicsItemChange &change, const QVariant &value);
    QRectF boundingRect() const;
    QRectF  m_rect;
    QPointF m_hotspot;
    QPointF m_positionOriginale;
    QPointF m_lastPosition;
    QPointF m_center;
    bool    m_retourPositionOriginale;

    /** Indique si la pièce est sensible ou non au événements souris
      * Initialisée à true dans le constructeur */
    bool    m_isAcceptMouse;

    QPen    m_penContour;
    QBrush  m_brosseFond;
    QImage  m_image;
    QSizeF  m_tailleImage;
    QPointF m_positionImage;
    Qt::Alignment m_imageAlignement;
    QString m_texte;
    QFont   m_font;
    QPen    m_penTexte;
    QPointF m_positionTexte;
    QSize   m_tailleRectTexte;
    Qt::Alignment m_texteAlignement;
    QRectF  m_boundingTexteRect;
    int     m_marge;
    int     m_xRadius;
    int     m_yRadius;
    /** QTimer destiné à être lancé au mousePress et arrêté au mouseRelease. Son signal timeout() est réémis en signalAbeCelluleEndTimer() */
    QTimer* m_timer;

    /** Point où a été pressed la souris */
    QPointF m_pressPos;

    bool    m_isAcceptWordWrap;
    QGraphicsObject *m_lastCell;

#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    AbulEduMultiMediaV1* m_multiMedia;
#endif

public slots:
    void slotAbePieceFakeRelease();

protected slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void slotAbePieceEndTimer();

signals:
    void signalAbePieceDeposee(const QString& nom, int numCellule);
    void signalAbePieceCliquee(AbulEduPieceV1*);
    void signalPiecePressed(AbulEduPieceV1*);
    void signalPieceReleased(AbulEduPieceV1*);
    void signalAbePieceDoubleCliquee();
    void signalAbePieceSurvole(QGraphicsItem*);
    void signalAbePieceSurvolCellule(QGraphicsObject*, QGraphicsObject*);
    void signalAbePieceEndTimer();
    void signalAbePieceMediaButtonClicked();

};

#endif // ABULEDUPIECEV1_H
