/**
  * Classe AbulEduCommonStatesV1
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduCommonStatesV1
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Éric Seigne <eric.seigne@ryxeo.com>
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
  * with this program; if not, write to the Free Software Foundation, Inc.,²
  * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */

#ifndef ABULEDUCOMMONSTATESV1_H
#define ABULEDUCOMMONSTATESV1_H

#include "abuledustatemachinev1.h"
#include "abuleduapplicationv1.h"
#include <QDebug>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include "abuledustatemachinev1.h"
#include "abuleduloggerv1.h"

/**
  * @file abuleducommonstatesv1.h
  * @version 1
  * @brief Cette classe s'utilise avec la AbulEduStateMachineV1, elle permet de personnaliser quelques aspects de la machine à états finis.
  * Entre autre, les animations associées aux transitions et la visibilité globale de certains widgets.
  * Une bonne pratique serait de modifier aussi peu que possible cette classe (sinon vous verrez bien !)
  * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduCommonStatesV1
  */

class AbulEduCommonStatesV1 :public AbulEduStateMachineV1
{
    Q_OBJECT

public:
    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le composant graphique parent de cet objet, initialise par defaut a 0
      */
    explicit AbulEduCommonStatesV1(QWidget *parent = 0);

    /** Destructeur par defaut de la classe
      */
    virtual ~AbulEduCommonStatesV1();

protected:
    /** Stocke le préfixe de thème de l'application
      * @obsolete */
    QString m_prefixeTetes;

    /** Stocke la taille des têtes d'évaluation et de bilan
      * @obsolete */
    int m_tailleTetes;

signals:

protected slots:
    /** Slot de commencement de la sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la sequence
      * @return void
      */
    virtual void slotSequenceEntered();

    /** Slot de commencement de la presentation sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la presentation de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresenteSequenceEntered();

    /** Slot de fin de presentation sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de presentation de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresenteSequenceExited();

//    void slotExercicesEntered();

    /** Slot de commencement de presentation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la presentation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresentationExerciceEntered();

    /** Slot de fin de presentation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de presentation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotPresentationExerciceExited();

    /** Slot de commencement de realisation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la realisation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotRealisationExerciceEntered();

    /** Slot de commencement de realisation exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de la realisation de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotInitQuestionEntered();

    /** Slot de commencement question
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement de(s) question(s)
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotQuestionEntered();

    /** Slot de fin question
      * @brief Ce slot contient le code qui s'executera a l'etat de fin de(s) question(s)
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotQuestionExited();

    /** Slot de commencement bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement du bilan de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanExerciceEntered();

    /** Slot de fin bilan exercice
      * @brief Ce slot contient le code qui s'executera a l'etat de fin du bilan de l'exercice
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    void slotBilanExerciceExited();

    /** Slot de commencement bilan sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de commencement du bilan de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanSequenceEntered();

    /** Slot de fin bilan sequence
      * @brief Ce slot contient le code qui s'executera a l'etat de fin du bilan de la sequence
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotBilanSequenceExited();

    /** Slot quitter
      * @brief Ce slot contient le code qui s'executera a l'etat quitter
      * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/BoiteAOutilsV1
      * @return void
      */
    virtual void slotQuitter();

    /** @todo Methode a implementer
      */
    void resizeEvent(QResizeEvent *event);

    /** @todo Methode a implementer
      */
    void showEvent(QShowEvent *event);

    /** Redimensionnne les différents objets de l'exercice
      * @brief Les têtes contenues dans le AbulEduGraphicsTetesWidgetV1 de pied d'Aire de travail ne seront pas redimensionnées en dessous de 70 x 70
      * (minimum décidé par le graphiste)
      * @return void
      */
    virtual void setDimensionsWidgets();
};

#endif // ABULEDUCOMMONSTATESV1_H
