/** AbulEduLabelV1
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

#ifndef ABULEDULABELV1_H
#define ABULEDULABELV1_H

#include <QLabel>
#include <QObject>
#include <QDrag>
#include <QMouseEvent>
#include <QFont>
#include <QMimeData>
#include <QApplication>
#include "abuleduconstantesv1.h"
#include "abuleduloggerv1.h"

/** @class AbuleduLabelV1
  * @brief Cette classe hérite de QLabel et permet de gérer une liste d'AbuleduLabel
  * Cette classe hérite de QLabel et permet de gérer une liste de AbuleduLabel's */

class AbuleduLabelV1 : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int intOrdre READ abelabelNumOrdre WRITE abelabelSetNumOrdre)
    Q_PROPERTY(int intPlace READ abelabelNumPlace WRITE abelabelSetNumPlace)
    Q_PROPERTY(QPoint abelabelLastPos READ abelabelLastPos WRITE abelabelSetLastPos)

public:
    explicit AbuleduLabelV1(QWidget *parent = 0);
    explicit AbuleduLabelV1(const QString& texte, int abelabelNumOrdre = -1,
                            int abelabelNumPlace = -1, const QFont& font = QFont(), QWidget *parent = 0);

    ~AbuleduLabelV1();
    /** Attribue un numéro d'ordre de classement à l'étiquette
      * i.e. ordre alphabétique
      * @param intOrdre le numéro d'ordre
      */
    void abelabelSetNumOrdre(int intOrdre);

    /** Retourne le numéro d'ordre de classement de l'étiquette
      * i.e. ordre alphabétique
      * @return le numéro d'ordre -1 par défaut
      */
    int abelabelNumOrdre();

    /** Attribue un rang physique à l'étiquette
      * @param intPlace le numéro d'ordre
      */
    void abelabelSetNumPlace(int intPlace);

    /** Retourne le rang physique de l'étiquette
      * @return le numéro d'ordre -1 par défaut
      */
    int abelabelNumPlace(){ return m_intPlace;}

    /** Stocke la position de l'étiquette après son déplacement
      * @param lastPoint la position a mémoriser
      */
    void abelabelSetLastPos(const QPoint& lastPos){ m_lastPos = lastPos;}

    /** Fonction surchargée
      * @param x,y abscisse et ordonnée de la position à mémoriser
      */
    void abelabelSetLastPos(int x,int y) {m_lastPos=QPoint(x,y);}

    /** Retourne la position mémorisée
      * @return lastPoint la position mémorisée
      */
    const QPoint& abelabelLastPos(){ return m_lastPos;}

    /** Fixe l'espace entre deux étiquettes 10 par défaut
      * @param marge
      */
    void abelabelSetMarge(int marge){ m_marge=marge;}

    /** Retourne l'espace entre deux étiquettes
      * @return
      */
    int abelabelMarge(){ return m_marge;}

    /** Affecte la fonte */
    void abelabelSetFont(const QFont& font){ setFont(font);}

    /** Affecte le style CSS par défaut */
    void abelabelSetDefaultStyleSheet(const QString& CSS){ m_defaultStyleSheet = CSS;}

    /** Retourne le style CSS par défaut */
    const QString& abelabelGetDefaultStyleSheet(){ return m_defaultStyleSheet;}

    /** Affecte le style CSS sélectionnées */
    void abelabelSetSelectedStyleSheet(const QString& CSS){ m_selectedStyleSheet = CSS;}

    /** Retourne le style CSS sélectionnées */
    const QString& abelabelGetSelectedStyleSheet(){ return m_selectedStyleSheet;}

    /** Affecte le style CSS indiquant une erreur */
    void abelabelSetErrorStyleSheet(const QString& CSS){ m_errorStyleSheet = CSS;}

    /** Retourne le style CSS indiquant une erreur */
    const QString& abelabelGetErrorStyleSheet(){ return m_errorStyleSheet;}

    QString abelabelGetText(){return text();}

private:
    /** Le numéro d'ordre dans le classement -1 par défaut */
    int m_intOrdre;

    /** Le rang dans la liste des étiquettes -1 par défaut */
    int m_intPlace;

    /** La position de l'étiquette (avant déplacement) */
    QPoint m_lastPos;


    /** Position de la souris lors du clic */
    QPoint m_lastMousePos;

    /** Espace entre deux étiquettes */
    int m_marge;

    /** Mime type de abuleduLabel */
    QString m_mimecode;

    /** Style CSS par défaut */
    QString m_defaultStyleSheet;

    /** Style CSS pour les étiquettes sélectionnées */
    QString m_selectedStyleSheet;

    /** Style CSS pour les étiquettes indiquant une erreur */
    QString m_errorStyleSheet;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    /** Signal annonçant le rang de l'abuleduLabel sélectionné
      * @param rang Le rang de l'étiquette sélectionnée dans la liste
      */
    void abuleduLabelSelected(int rang);

    /** Signal annonçant l'abuleduLabel qui vient de se déplacer lors d'un survol
      * @param etiq la référence à l'étiquette déplacée
      */
    void AbulEduLabel_moved(AbuleduLabelV1 *etiq);
    /**
      * signal emis lors du clic
      */
    void clicked();

public slots:

};

#endif // ABULEDULABELV1_H
