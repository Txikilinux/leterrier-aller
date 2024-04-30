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
* @file AbulEduCelluleV1.h
* @version 1
*/

#ifndef ABULEDUCELLULEV1_H
#define ABULEDUCELLULEV1_H

#include <QGraphicsObject>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include "abuleduconstantesv1.h"
#include "abuledupiecev1.h"

/*
  */
class AbulEduCelluleV1 : public QGraphicsObject
{
    Q_OBJECT
public:
    /** Definition d'un type customisé pour les cellules */
    enum { AbulEduCelluleType = UserType + abe::abeEtiquetteCellule };

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return AbulEduCelluleType;
    }

    /** Constructeur de la classe, qui prend obligatoirement la taille en paramètre */
    explicit AbulEduCelluleV1(const QSizeF& taille, QGraphicsObject *parent = 0);

    ~AbulEduCelluleV1();

    /** Fixe la taille de la cellule */
    void abeCelluleSetSize(const QSizeF& taille){ m_taille = QRectF(0,0,taille.width(), taille.height());}

    /** Retourne la taille de la cellule */
    QSizeF abeCelluleGetSize(){ return QSizeF(m_taille.width(), m_taille.height());}

    /** Garde en mémoire le pointeur de la pièce déposée sur la cellule
      * @param piece le pointeur vers la pièce
      * @note émet abe::evalA si c'est la bonne, abe::evalD sinon
      */
    void abeCelluleSetPiece(AbulEduPieceV1 *piece);

    /** Retourne un pointeur sur la piece occupant la cellule*/
    AbulEduPieceV1 * abeCelluleHasPiece();

    /** Détermine si la cellule accepte le drop de Piece ou pas*/
    void abeCelluleAcceptePiece(bool yesNo);
    bool abeCelluleIsAcceptePiece();

    /** Fixe le Pen de contour
      * @param contour le pen de contour taille et couleur
      */
    void abeCelluleSetPen(const QPen& contour){ m_penContour = contour;}

    /** Fixe la brosse de Fond
      * @param brosseFond la brosse de remplissage du fond
      */
    void abeCelluleSetBrush(const QBrush& brosseFond) { m_brosseFond = brosseFond;}
    void abeCelluleSetInitialBrush(const QBrush& brosseFond) { m_initialBrosseFond = brosseFond; abeCelluleSetBrush(brosseFond);}
    void abeCelluleSetHoverBrush(const QBrush& brosseHover) { m_hoverBrosseFond = brosseHover;}

    /** Attribue une image qui sera dessinée sur le fond
      * @param cheminImage le chemin vers le fichier de l'image
      */
    void abeCelluleSetImage(const QString& cheminImage, const QPointF& positionImage = QPointF(0,0), const QSize& tailleImage = QSize(0,0));

    void abeCelluleSetPixmap(const QPixmap pixmap, const QPointF &positionImage = QPointF(0,0), const QSize &tailleImage = QSize(0,0));

    /** Alignement de l'image dans l'étiquette
      * Qt::AlignLeft	0x0001	Aligns with the left edge.
      * Qt::AlignRight	0x0002	Aligns with the right edge.
      * Qt::AlignHCenter	0x0004	Centers horizontally in the available space.
      * Qt::AlignJustify	0x0008	Justifies the text in the available space.
      * Qt::AlignTop	0x0020	Aligns with the top.
      * Qt::AlignBottom	0x0040	Aligns with the bottom.
      * Qt::AlignVCenter	0x0080	Centers vertically in the available space.
      * Qt::AlignCenter	AlignVCenter | AlignHCenter	Centers in both dimensions.
      */
    void abeCelluleSetImageAlignement(const Qt::Alignment& alignement);

    /** Fixe la taille de l'image */
    void abeCelluleSetImageTaille(const QSize& tailleImage, const Qt::AspectRatioMode& modeRatio = Qt::KeepAspectRatio);

    /** Fixe la position de l'image */
    void abeCelluleSetImagePosition(const QPointF& positionImage);

    /** Fixe la marge entre le bord et les éléments internes
      * @param marge le nombre de pixels entre le bord et l'image ou le texte
      */
    void abeCelluleSetMarge(int marge);

    /** Retourne la marge entre le bord et les éléments internes
      * @return int, entier, le nombre de pixels entre le bord et l'image ou le texte
      */
    int abeCelluleGetMarge();

    /** Fixe la font de l'étiquette */
    void abeCelluleSetFont(const QFont& font){m_font = font;}

    /** Récupère la font de l'étiquette */
    const QFont& abeCelluleGetFont(){ return m_font;}

    /** Recupere le texte de l'etiquette */
    const QString& abeCelluleGetText(){ return m_texte;}

    /** Fixe le texte de la pièce
      * @param texte le texte de la pîèce
      */
    void abeCelluleSetTexte(const QString& texte, const QPointF& positionTexte = QPointF(0,0), const QSize& tailleRectTexte = QSize(0,0));

    /** Alignement du texte dans l'étiquette
      * Qt::AlignLeft	0x0001	Aligns with the left edge.
      * Qt::AlignRight	0x0002	Aligns with the right edge.
      * Qt::AlignHCenter	0x0004	Centers horizontally in the available space.
      * Qt::AlignJustify	0x0008	Justifies the text in the available space.
      * Qt::AlignTop	0x0020	Aligns with the top.
      * Qt::AlignBottom	0x0040	Aligns with the bottom.
      * Qt::AlignVCenter	0x0080	Centers vertically in the available space.
      * Qt::AlignCenter	AlignVCenter | AlignHCenter	Centers in both dimensions.
      */
    void abeCelluleSetTexteAlignement(const Qt::Alignment& alignement);

    /** Fixe la position du texte */
    void abeCelluleSetTextePosition(const QPointF& position);

    /** Fixe la taille du rectangle contenant le texte */
    void abeCelluleSetTexteSize(const QSize& tailleTexteRect);

    /** Fixe le Pen de Texte
      * @param contour le pen de contour taille et couleur
      */
    void abeCelluleSetTextePen(const QPen& texte){ m_penTexte = texte;}

    /** Fixe l'arrondi de la pièce
      * @param xRadius
      * @param yRadius
      */
    void abeCelluleSetRadius(int xRadius, int yRadius);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

private:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
        QRectF m_taille;

        /** Calcule la taille du rectangle contenant le texte */
        QRectF abeCelluleTexteBoundingRect();

        QRectF boundingRect() const;
        bool m_acceptePiece;
        AbulEduPieceV1 *m_piece;
        QPen m_penContour;
        QBrush m_brosseFond;
        QBrush m_initialBrosseFond;
        QBrush m_hoverBrosseFond;
        QImage m_image;
        QSizeF m_tailleImage;
        QPointF m_positionImage;
        Qt::Alignment m_imageAlignement;
        int m_marge;
        QFont m_font;
        QString m_texte;
        QPen m_penTexte;
        QPointF m_positionTexte;
        QSize m_tailleRectTexte;
        Qt::Alignment m_texteAlignement;
        QRectF m_boundingTexteRect;
        int m_xRadius;
        int m_yRadius;
        bool m_localDebug;
        /** QTimer destiné à être lancé au mousePress et arrêté au mouseRelease. Son signal timeout() est réémis en signalAbeCelluleEndTimer() */
        QTimer* m_timer;

private slots:
    void slotAbeCellulePieceSurvol(QGraphicsItem*);
    void slotAbeCelluleEndTimer();

signals:
    void signalAbeCelluleDropResult(abe::ABE_EVAL eval);
    void signalAbeCelluleMouseDoubleClicked();
	void signalAbeCelluleEndTimer();
};

#endif // ABULEDUCELLULEV1_H
