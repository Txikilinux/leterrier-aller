/** AbulEduLabelFrameV1
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2013 Éric Seigne <eric.seigne@ryxeo.com>
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

#ifndef ABULEDULABELFRAMEV1_H
#define ABULEDULABELFRAMEV1_H

#include <QFrame>
#include <QEvent>
#include <QFont>
#include "abuledulabelwidgetv1.h"
#include "abuledulabelv1.h"
#include "abuleduloggerv1.h"

class AbulEduLabelFrameV1 : public QFrame
{
    Q_OBJECT

public:
    explicit AbulEduLabelFrameV1(int orientation = Qt::Vertical, QWidget *parent = 0);
    ~AbulEduLabelFrameV1();

    /** Fixe le point de départ de la liste
      * @param position la position du point de la première étiquette de la liste
      */
    void abelabelSetPosListeLabels(const QPoint &position);
    void abelabelSetPosListeLabels(int x, int y);

    /** Fixe la marge séparant deux étiquettes
      * @param marge la distance en pixel séparant deux étiquettes
      */
    void abelabelSetMarge(const int marge);

    /** Fixe l'orientation de la liste
      * @param orientation Qt::Vertical ou Qt::Horizontal
      */
    void abelabelSetOrientation(const int orientation);
    int abelabelOrientation() const;

    /** Retourne la liste des AbulEduLabels présents sur la QFrame dans l'ordre numPlace
      * @return  QMap<int, AbuleduLabel *> AbulEduLabel *
      */
    QMap<int, AbuleduLabelV1 *>abelabelListeAbuleduLabelsByNumPlace();

    /** Retourne la liste des AbulEduLabels présents sur la QFrame dans l'ordre numOrdre
      * @return QMap<int, AbuleduLabel *> AbulEduLabel *
      */
      QMap<int, AbuleduLabelV1 *>abelabelListeAbuleduLabelsByNumOrdre();

    /** Crée les AbulEduLAbels
      * @param listeTextes la liste des textes à passer aux AbulEduEduLabels
      */
      void abelabelCreeAbulEduLabels(const QStringList &listeTextes, const QString &defaultCSS = "",
                                     const QString &selectedCSS = "background-color: #FFDAB9;",
                                     const QString &errorCSS = "background-color: #FF7F50",
                                     const QFont &fonte = QFont());

    /** Fixe la taille des AbulEduLAbels à une même valeur
      * @param largeur la taille des AbulEduLabels
      */
    void abelabelSetLargeurEtiquettes(const int largeur);

    /** Montre les AbulEduLabels
      * @param onOff true, montre les AbulEduLabels, false les cache
      */
    void abelabelMontreEtiquettes(const bool onOff);

    /** Déplace l'étiquette sélectionnée de x étiquettes
      * @param deplacement +1 vers le haut -1 vers le bas
      */
    void abelabelDeplaceEtiquette(const int deplacement);

    /** Fonction qui échange les positions de deux abuleduLabels (étiquettes)
      * @ param rangEtiq1 rang de la première étiquette
      * @ param rangEtiq2 rang de la deuxième étiquette
      * @return true si la permutation a eu lieu, false sinon
      */
    bool abelabelSwapAbuleduLabel(int rangEtiq1, int rangEtiq2);

    /** Déselectionne toutes les étiquettes */
    void abelabelDeselectionneLabels();

    /** Mélange les étiquettes en modifiant leur numéro de Place
      */
    void abelabelMelangeAbulEduLabels();

    /** Positionne les AbulEduLabels dans l'ordre de leur numPlace */
    void abelabelPlaceAbulEduLabelsByNumPlace();

    /** Positionne les AbulEduLabels dans l'ordre de leur numOrdre */
    void abelabelPlaceAbulEduLabelsByNumOrdre();

    /** Fixe la taille de la Qframe en fonction du nombre d'étiquettes
      * @return size la taille de la QFrame
      */
    QSize abelabelResizeFrame();

    /** Fixe la largeur des AbulEduLabels
      */
    void abelabelResizeLabels();

private:
    /** L'orientation de la liste Qt::Vertical ou Qt::Horizontal */
    int m_orientation;

    /** Largeur des étiquettes */
    int m_largeurEtiquettes;

    /** Coordonnées du point de départ de la liste en repère local */
    QPoint m_PosListeLabels;

    /** La marge entre deux étiquettes */
    int m_marge;

    /** Le rang de l'abuleduLabel sélectionnée (-1 si pas de sélection) */
    int m_abuleduLabelSelected;

    /** La liste des AbuEduLabels présents sur la QFrame */
    QMap<int, AbuleduLabelV1 *> m_listeAbulEduLabels;

    void traceEtiquettes();

signals:
    void btnDroiteSetVisible(bool);
    void btnGaucheSetVisible(bool);
    void btnHautSetVisible(bool);
    void btnBasSetVisible(bool);
    void btnDroiteSetEnabled(bool);
    void btnGaucheSetEnabled(bool);
    void btnHautSetEnabled(bool);
    void btnBasSetEnabled(bool);
    void AbulEduLabel_moved(AbuleduLabelV1 *etiq);

public slots:
    void moveSelectedAbulEduLabelUp();
    void moveSelectedAbulEduLabelDown();
    void moveSelectedAbulEduLabelLeft();
    void moveSelectedAbulEduLabelRight();
    void on_abuleduLabelSelected(int rang);
    void on_AbulEduLabel_moved(AbuleduLabelV1 *etiq);

private slots:
    void childEvent(QChildEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // ABULEDULABELFRAMEV1_H
