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
 * @file abuledutableauv1.h
 * @version 1
 */

#ifndef ABULEDUTABLEAUV1_H
#define ABULEDUTABLEAUV1_H

#include <QObject>
#include <QPointer>
#include <QDebug>
#include "abuleduetiquettesv1.h"

/** @brief Classe permettant de construire un tableau de AbulEduCellulesV1
 *
 * Le tableau est composé :
 * \li d'une zone centrale dont les cellules sont numérotées de 0 à (nombre colonnes x nombre lignes) -1 en partant
 * du haut à gauche jusqu'au bas à droite.
 * \li de quatre zones d'entetes énumérées dans enum Entete (Haut, Bas, Gauche, Droit)
 * \li de quatre cellules de coins (HautGauche, HautDroit, BasDroit, BasGauche)
 *
 * Un certain nombre de fonctions retournent une QMap de ces zones de cellules.
 */

class AbulEduTableauV1 : public QObject
{
    Q_OBJECT
    Q_FLAGS(Entete Entetes ) // Pour les entetes


public:

    /** @enum Les valeurs des items de l'enum sont fixées de façon à correspondre aux bits 0, 1, 2, 3...
      * d'ou les valeurs 1,2,4,8 soit en binaire 0001, 0010, 0100, 1000...
      */
    enum Entete { Haut       = 1,
                  Bas        = 2,
                  Gauche     = 4,
                  Droit      = 8,
                  HautGauche = 16,
                  HautDroit  = 32,
                  BasDroit   = 64,
                  BasGauche  = 128
                };

    /** Déclaration des Flags associés à l'enumeration correspondante
      */
    Q_DECLARE_FLAGS(Entetes, Entete)

    /** Contructeur
      * @param parent Il faut passer l'abuleduEtiquetteV1 * qui doit recevoir le tableau
      */
    AbulEduTableauV1(QWidget *parent, int nbColonnes, int nbLignes, Entetes flagsEntete = 0);
    ~AbulEduTableauV1();

    /** Positionne les différents éléments du tableau */
    void abeTableauConstruitTableau();

    /** Retourne la ligne d'entete sélectionnée */
    QMap<int, AbulEduCelluleV1 *> abeTableauGetEntetes(Entete position);

    /** Retourne un pointeur vers une cellule de coin */
    AbulEduCelluleV1 * abeTableauGetEnteteCoin(Entete position);

    /** Retourne la liste des cellules "centrales"
      * @return Les cellules sont numérotées de 0 à nbcols x nbLignes -1, du haut gauche au bas droite
      */
    QMap<int, AbulEduCelluleV1 *> abeTableauGetCellules();

    /** Retourne un pointeur vers la cellule centrale choisie */
    AbulEduCelluleV1 * abeTableauGetCellule(int numero);

    /** Retourne la liste des cellules d'une colonne du tableau, y compris les cellules d'entête. S'il n'y a pas de cellule d'entête, la valeur est 0.
     *  @param col La colonne choisie : les numéros de colonne correspondent aux numéros des colonnes de la zone centrale
     * @return Les keys de la QMap varient de -1 (entete haut) à nombre de lignes +1 (entete bas)
     */
    QMap<int, AbulEduCelluleV1 *> abeTableauGetColumnCellules(int numCol);

    /** Retourne la liste des cellules d'une ligne du tableau, y compris les cellules d'entête. S'il n'y a pas de cellule d'entête, la valeur est 0.
       *  @param col La ligne choisie : les numéros de ligne correspondent aux numéros des lignes de la zone centrale
       * @return Les keys de la QMap varient de -1 (entete gauche) à nombre de colonnes +1 (entete droit)
       */
    QMap<int, AbulEduCelluleV1 *> abeTableauGetRowCellules(int numRow);

    /** Retourne le numéro de la colonne de la cellule d'après le numéro de la cellule
     * @param numCell Le numéro de la cellule
     * @return Le numéro de la colonne
     */
    int abeTableauGetCelluleColumn(int numero);

    /** Retourne le numéro de la ligne de la cellule d'après le numéro de la cellule
     * @param numCell Le numéro de la cellule
     * @return Le numéro de la ligne
     */
    int abeTableauGetCelluleRow(int numero);

    /** Redimensionne la colonne choisie
      * Le numérotage commence à 0 avec la colonne d'entete de gauche
      * et se termine à nombre de colonne +1 avec la colonne d'entete de droite
      */
    void abeTableauColonneRedim(int colonne, qreal taille);

    /** Redimensionne la ligne choisie
      * Le numérotage commence à 0 avec la ligne d'entete du haut
      * et se termine à nombre de lignes +1 avec la ligne d'entete du  bas
      */
    void abeTableauLigneRedim(int ligne, qreal taille);

    /** Redimensionne toutes les cellules d'un tableau y compris les entetes
      * @param taille la nouvelle taille des cellules (width, height)
      */
    void abeTableauRedimensionne(QSizeF taille);

    /** Positionne le tableau sur la graphicsscene
     * @param position La position (coin haut gauche) du tableau.
     */
    void abeTableauSetPos(QPointF position);

    QSizeF abeTableauGetSize();
private:

    /** Crée les cellules nécessaire au tableau */
    void abeTableauCreeCellules();

    /** Retourne la liste de toutes les cellules */
    QList<AbulEduCelluleV1 *>creeListeCellules();

    int m_nbColonnes;
    int m_nbLignes;
    Entetes m_flagsEntete;
    QPointF m_position; // La position du tableau
    float m_defautLargeurColonnes;
    float m_defautHauteurlignes;
    QMap<int, AbulEduCelluleV1 *>    m_listeEnteteHaut;
    QMap<int, AbulEduCelluleV1 *>    m_listeEnteteBas;
    QMap<int, AbulEduCelluleV1 *>    m_listeEnteteGauche;
    QMap<int, AbulEduCelluleV1 *>    m_listeEnteteDroit;
    QMap<int, AbulEduCelluleV1 *>    m_listeCellules;
    QMap<Entetes, AbulEduCelluleV1 *>m_listeEnteteCoins;
    bool m_localDebug;

};

    /** Necessaire pour utiliser les combinaisons de flags
      */
    Q_DECLARE_OPERATORS_FOR_FLAGS(AbulEduTableauV1::Entetes)

#endif // ABULEDUTABLEAUV1_H
