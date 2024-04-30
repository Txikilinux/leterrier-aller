/**
  * Classe de base en version une pour les exercices AbulEdu permettant de structurer
  * nos logiciels et respecter ainsi un certain nombre de choses dont l'interface publique
  * à laquelle le plugin peut se connecter pour échanger des données (logs pour le moment).
  * La classe AbulEduExerciceV1 hérite de QMainWindow pour que vous puissiez créer une fenêtre
  * qui dérive d'AbulEduExerciceV1 le plus simplement possible.
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/Normalisation_des_%C3%A9changes_entre_l%27application_et_le_plugin
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/Utilisation_de_abuleduexercicev1
  * @author 2009-2011 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GPL)
  *
  * @warning NE MODIFIEZ SURTOUT PAS CES FICHIERS, SI VOUS LE FAITES NE SOYEZ PAS ÉTONNÉS
  *          QUE L'APPLICATION NE S'INTERFACE PLUS CORRECTEMENT AVEC LE PLUGIN !!!!!
  *
  * #AbulEduExerciceV1
  * include(lib/abuleduexercicev1/abuleduexercicev1.pri)
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
 */
#ifndef ABULEDUEXERCICEV1_H
#define ABULEDUEXERCICEV1_H

#include "abuleduexercicecommonv1.h"
#include "abuleduexercicewidgettelecommandev1.h"
#include "abuleduexercicewidgetairedetravailv1.h"
#include "abuleduexercicewidgetmessagev1.h"
#include "abuledumessageboxv1.h"
#include "abuleduloggerv1.h"

/** @file abuleeduexercicev1.h
  * @version 1
  * @brief Cette classe permet de:
  * -gérer le moteur des exercices
  * -gérer les logs
  * Elle se présente sous la forme de widgets (attention, avant la modification du 20 novembre 2011 elle fournissait une QMainWindow).
  * Vous devez maintenant intégrer ces widgets dans votre fenêtre principale.
  * @link https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduExerciceV1
  */

class AbulEduExerciceV1: public QObject, public AbulEduExerciceCommonV1
{
    Q_OBJECT

public:

    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le widget parent de l'objet, initialise par defaut a 0
      */
    AbulEduExerciceV1(QWidget *parent = 0);

    /// Destructeur par defaut de la classe
    virtual ~AbulEduExerciceV1();

    /** Accesseur en lecture de la couleur qui indique une réponse juste
      * @note pas d'accesseur en écriture puisqu'on veut justement que ce soit une couleur commune à tous les logiciels */
    inline const QColor& abeExerciceGetRightColor() {return m_rightColor;}

    /** Accesseur en lecture de la couleur qui indique une réponse fausse
      * @note pas d'accesseur en écriture puisqu'on veut justement que ce soit une couleur commune à tous les logiciels */
    inline const QColor& abeExerciceGetWrongColor() {return m_wrongColor;}

    /// Configure le niveau de l'exercice
    virtual void setAbeLevel(const QString& s);
    /// Renseigne le nombre total de questions
    virtual void setAbeNbTotalQuestions(int n);

    /// Affecte le theme du personnage dans la telecommande
    virtual void setAbeThemeTeteTelecommande(const QString& s);
    /// Affecte la tete du personnage dans la telecommande
    virtual void setAbeTeteTelecommande(const QString& s);
    /// Affecte une tete de personnage dans la telecommande en fonction du rapport entre les deux parametres
    virtual qreal setAbeTeteForResult(int errors, int total);
    /// Affecte le theme du niveau (level) dans la telecommande
    virtual void setAbeThemeLevelTelecommande(const QString& s);

    //Les deux get pour pouvoir manipuler les objets depuis la state machine
    AbulEduExerciceWidgetTelecommandeV1  * getAbeExerciceTelecommandeV1();
    AbulEduExerciceWidgetAireDeTravailV1 * getAbeExerciceAireDeTravailV1();
    AbulEduExerciceWidgetMessageV1*        getAbeExerciceMessageV1();

    const QString& getAbeExerciceUnitLanguage() const;
    void setAbeExerciceUnitLanguage(const QString &getAbeExerciceUnitLanguage);

public slots:
    /// Connecte un signal (par exemple de la mainwindow) pour fermer l'exercice en cours
    void slotAbeFermerExercice();

protected:

    /** Structure permettant de stocker les réponses des utilisateurs et la réponse attendue
      * @attention on met la réponse de l'utilisateur en premier, la réponse attendue en second
      */
    QList<QPair<QVariant,QVariant> > m_answers;

    //// Permer l'affichage à l'utilisateur quand un module donné ne contient pas un type d'exercice
    virtual void displayErrorMissingExercise(const QString& nomExercice, QWidget *place);

private:
    QString                                 m_themeTetes;       /// Le theme des tete de la telecommande (asma/suzu/will/pingu/autre)
    QString                                 m_themeLevel;       /// Le theme des niveaux de la telecommande (asma/suzu/will/pingu/autre)

    /// Une même couleur dans tous les logiciels pour indiquer une réponse juste
    QColor                                  m_rightColor;
    /// Une même couleur dans tous les logiciels pour indiquer une réponse fausse
    QColor                                  m_wrongColor;

    AbulEduExerciceWidgetTelecommandeV1*    m_uiAbulEduTelecommande;
    AbulEduExerciceWidgetAireDeTravailV1*   m_uiAbulEduAireDeTravail;
    AbulEduExerciceWidgetMessageV1*         m_uiAbulEduMessage;

    /// La langue du module codée en ISO 639-2 par conformité avec ScoLOMFR
    QString                                 m_unitLanguage;
};

#endif
