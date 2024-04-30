/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#ifndef ABULEDUOUTILSV1_H
#define ABULEDUOUTILSV1_H
#include <QObject>
#include <QMap>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QTime>
#include <QGraphicsItem>
#include <abuledulabelv1.h>
#include <abuleduconstantesv1.h>
#include "abuledutextv1.h"

class AbulEduOutilsV1
{

    Q_ENUMS(NIVEAU)


public:
    AbulEduOutilsV1(QWidget *parent);
    /** Les niveaux tels que dans le fichiers de Mots fourni
      * @enum Outil, CP, CE1, CE2, CM1, CM2
      */
    enum NIVEAU {Outil, CP, CE1, CE2, CM1, CM2 };



protected:
    /** QMap Dictionnaire des Niveaux */
//    QMap<NIVEAU, QString> dicoNiveaux;

    /** Construit une liste de mots tirés au hazard dans un fichier cvs organisé mot, niveau, nature (séparateur tab)
      * La liste est triée dans l'ordre alphabétique
      * @param listeMots la "liste" de mots (mot, nature,niveau)
      * @param nbMots le nombre de mots à sélectionner dans la liste, si on met "-1" on retourne toutes les occurrences
      * @param niveau le niveau d'aquisition des mots Outils, CP, CE1, CE2, CM1,CM2
      * @param nature NOM nom, VER verbe, ADV adverbe, ADJ adjectif, PRO pronom, PRE préposition, DET déterminant, CON conjonction
      * @param profondeur le nombre de lettres (en début de mot) sur lesquelles porte le classement (0 ignoré)
      */
    QStringList abeoutilsCreerListeMots(QStringList listeMots, int nbMots,abe::ABE_LEVEL niveau, QString nature, int profondeur);

    /** Fonction surchargée pour passer un abe::ABE_NATURE en paramètre */
    QStringList abeoutilsCreerListeMots(QStringList listeMots, int nbMots,abe::ABE_LEVEL niveau, abe::ABE_NATURE nature, int profondeur);

    /** Créée une liste de mots répondant à des caractéristiques précises
      * Fonction destinée à être appelée avec la liste de mots-outils "mot_aller_cycle3.csv" créée par Jean-Louis Frucot
      * @param niveau est le niveau tel que défini dans AbulEduOutilsV1
      * @param nature est une chaine de caractère indiquant la nature du mot recherché
      * @return on va donc retourner la liste de tous les mots du fichier pour le niveau et la nature donnés*/
    QStringList abeoutilsCreerListeMotsOutils(QString cheminDictionnaire, abe::ABE_LEVEL niveau, QString nature = "");

    /** Fonction surchargée pour passer un abe::ABE_NATURE en paramètre */
    QStringList abeoutilsCreerListeMotsOutils(QString cheminDictionnaire, abe::ABE_LEVEL niveau, abe::ABE_NATURE nature);


    /** Fonction triant les mots dans l'ordre alphabétique
      * en respectant les "coutumes locales" localeAwareCompare
      * @param liste QStringListe des mots à classer
      * @param sensCroissant true ordre croissant
      * @return QstringList des mots triés
      */
    static QStringList abeoutilsSortListeMots(QStringList liste, bool sensCroissant=true);

    /**
      * e
      * Adaptation d'un algorithme "classique"
      * @param le premier vecteur d'entiers a est la liste dans laquelle on cherche la plus grande sous-suite croissante
      * @param le deuxième vecteur d'entiers contient les INDEX où se trouvent les éléments de la plus grande sous-suite croissante
      * @note première amélioration possible : détyper (ou QVariant ?) le premier vecteur pour chercher sur autre chose que des int
      * @note deuxième amélioration possible : détyper (ou QVariant ?) le deuxième vecteur et retourner les éléments au lieu de leur index
      */
    QVector <int> abeoutilsFindGoodList(QVector <int> &a);


    /** Supprime les QGraphicsPixmap enfants d'un autre item
      * @param item l'item à nettoyer
      */
    void removeGraphicsPixmaps(QGraphicsItem *item);

    //---------------------------------------------------------------------------------
    //                           Zone des AbulEduLabel
    //---------------------------------------------------------------------------------
    /** Créer les abulEduLabels à partir de la liste m_listeObjets */
    void abeoutilsCreeAbulEduLabels();

private:
    int m_localDebug;
    QWidget* m_parent;
    QMap<abe::ABE_NATURE,QString> m_matchNature;
    static bool localeAwareCompareToBool(const QString &s1, const QString &s2);
};



#endif // ABULEDUOUTILSV1_H
