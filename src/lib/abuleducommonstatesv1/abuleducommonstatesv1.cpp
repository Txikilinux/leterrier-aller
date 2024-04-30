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

#include "abuleducommonstatesv1.h"

#ifndef DEBUG_ABULEDUCOMMONSTATESV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduCommonStatesV1::AbulEduCommonStatesV1(QWidget *parent) :
    AbulEduStateMachineV1(parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

AbulEduCommonStatesV1::~AbulEduCommonStatesV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //on detruit les objets qui n'ont pas de parents
    //il ne faut pas detruire les objets contenus dans une scene: c'est son destructeur qui s'en charge
}

void AbulEduCommonStatesV1::slotSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    getAbeExerciceTelecommandeV1()->setVisible(true);
    getAbeExerciceAireDeTravailV1()->setVisible(true);
    boiteTetes = new AbulEduGraphicsTetesWidgetV1(m_nbExercices);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addItem(boiteTetes);
    boiteTetes->setVisible(false);
    boiteTetes->resetTetes(m_nbExercices);

    setDimensionsWidgets();
    // On peut ici "surcharger" le comportement défini dans la classe mère lors de la création de la machine à états

    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    AbulEduStateMachineV1::slotSequenceEntered();
}


void AbulEduCommonStatesV1::slotPresenteSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotPresenteSequenceEntered();
}

void AbulEduCommonStatesV1::slotPresenteSequenceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotPresenteSequenceExited();
}

void AbulEduCommonStatesV1::slotPresentationExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotPresentationExerciceEntered();
}

void AbulEduCommonStatesV1::slotPresentationExerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotPresentationExerciceExited();
}

void AbulEduCommonStatesV1::slotRealisationExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotRealisationExerciceEntered();
    boiteTetes->setVisible(true);
    setAbeTeteTelecommande("attente");
    setDimensionsWidgets();
}

void AbulEduCommonStatesV1::slotInitQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotInitQuestionEntered();
}

void AbulEduCommonStatesV1::slotQuestionEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotQuestionEntered();
}

void AbulEduCommonStatesV1::slotQuestionExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotQuestionExited();
}

void AbulEduCommonStatesV1::slotBilanExerciceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setAbeTeteTelecommande("attente");
    setDimensionsWidgets();
    AbulEduStateMachineV1::slotBilanExerciceEntered();
}

void AbulEduCommonStatesV1::slotBilanExerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotBilanExerciceExited();
}

void AbulEduCommonStatesV1::slotBilanSequenceEntered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    boiteTetes->setVisible(false);
    AbulEduStateMachineV1::slotBilanSequenceEntered();
}

void AbulEduCommonStatesV1::slotBilanSequenceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduStateMachineV1::slotBilanExerciceExited();
}

void AbulEduCommonStatesV1::slotQuitter()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //La frame est directement placee dans le ui
    getAbeExerciceTelecommandeV1()->ui->framePopupQuitter->show();
    getAbeExerciceTelecommandeV1()->ui->btnQuitter->setEnabled(false);
    AbulEduStateMachineV1::slotQuitter();
}

//-----------------------------------------------------------------------
//
//         Trucs pour essayer
//
//-----------------------------------------------------------------------

void AbulEduCommonStatesV1::resizeEvent(QResizeEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(event);
//    setDimensionsWidgets();
}

void AbulEduCommonStatesV1::showEvent(QShowEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(event);
//    setDimensionsWidgets();
}

void AbulEduCommonStatesV1::setDimensionsWidgets()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
//    AbulEduStateMachineV1::setDimensionWidgets();
    if (120*abeApp->getAbeApplicationDecorRatio()> 70)
        boiteTetes->resizeTetes(120*abeApp->getAbeApplicationDecorRatio());
    else boiteTetes->resizeTetes(70);
}
