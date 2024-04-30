/**
  * Classe
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
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

 * @file abuleduexercicewidgettelecommandev1.h
 * @version 1
 */

#ifndef ABULEDUEXERCICEWIDGETTELECOMMANDE_H
#define ABULEDUEXERCICEWIDGETTELECOMMANDE_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QProgressBar>
#include <QLineEdit>
#include "ui_abuleduexercicewidgettelecommandev1.h"
#include "abuleduapplicationv1.h"
#include "abuledupageaccueilv1.h"
#include "abuleduloggerv1.h"

namespace Ui {
    class AbulEduExerciceWidgetTelecommandeV1;
}

class AbulEduExerciceWidgetTelecommandeV1 : public QWidget
{
    Q_OBJECT

public:

    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le widget parent de l'objet, initialise par defaut a 0
      */
    explicit AbulEduExerciceWidgetTelecommandeV1(QWidget *parent = 0);

    /** Destructeur par defaut de la classe */
    ~AbulEduExerciceWidgetTelecommandeV1();

    /** Acces public pour supprimer l'objet */
    void abeTelecommandeClose();

    Ui::AbulEduExerciceWidgetTelecommandeV1 *ui;

    /** Permet de definir l'image de fond
      * @param chemin, chaine de caractere, le chemin de l'emplacement de l'image */
    void setImageFond(const QString &);

    /** Redimensionne la telecommande */
    void abeTelecommandeResize();

    /** Permet de definir le niveau
      * @param s, chaine de caractere
      */
    void setAbeLevel(const QString &);

    /** Permet de definir le niveau
      * @brief peut-être obsolete
      * à voir si c'est utilisé dans les applications
      * @see setAbeLevel()
      */
    void setMNiveau(int niveau);

    /** Permet de definir la tete d'evaluation
      * @param tete, chaine de caractere, la tete a utiliser
      */
    void setMTeteEval(const QString &);

protected:

    /** Permet de changer d'evenement
      * @param *e, QEvent, un pointeur sur un evenement */
    void changeEvent(QEvent *e);

private:
    QPixmap                m_imageFond;
    int m_niveau;
    QString m_teteEval;

public slots:
    /** Code exécuté lors du clic sur le bouton Fermer de la popup */
    void on_btnQuitterRetourMenuPrincipal_clicked();

private slots :
    /** Gere l'evenement appuie sur la touche */
    void keyPressEvent(QKeyEvent *event);
    /** Gere l'evenement relachement de la touche */
    void keyReleaseEvent(QKeyEvent *event);
    /** Code exécuté lors du clic sur le bouton Quitter de la télécommande */
    void on_btnQuitter_clicked();
    /** Code exécuté lors du clic sur le bouton Annuler de la popup */
    void on_btnQuitterAnnuler_clicked();
    /** Permet de redimensionner le widget à la bonne taille */
    void setDimensionsWidget();

    void on_btnAide_clicked();

signals:
    /** Signal émis lors du clic sur le bouton Quitter de la télécommande */
    void btnTelecommandeQuitterClicked();
    /** Signal émis lors du clic sur le bouton Annuler de la popup */
    void btnTelecommandeQuitterAnnulerClicked();
    /** Signal émis lors du clic sur le bouton Fermer de la popup
      * @note destiné à être connecté au slotQuitterAccueil */
    void btnTelecommandeQuitterFermerClicked();
    /** Signal émis lors du clic sur le bouton Aide de la télécommande */

    void btnTelecommandeAideClicked();

    /** Signal émis lors du clic sur le bouton Niveau de la télécommande */
    void btnTelecommandeNiveauClicked();
    /** Signal émis lors du clic sur le bouton Facile de la popup niveau
      * @note destiné à être connecté au slotChangeNiveauFacile() */
    void btnTelecommandeChangeNiveau(int);
    /** Signal émis lors du clic sur le bouton Difficile de la télécommande
      * @note destiné à être connecté au slotChangeNiveauAnnuler() */
    void btnTelecommandeNiveauAnnulerClicked();

};

#endif // ABULEDUEXERCICEWIDGETTELECOMMANDE_H
