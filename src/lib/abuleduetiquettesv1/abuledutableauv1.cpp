/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
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

#include "abuledutableauv1.h"

AbulEduTableauV1::AbulEduTableauV1(QWidget *parent, int nbColonnes, int nbLignes, Entetes flagsEnteteH) :
    QObject(parent)
{
    m_localDebug = false;
    if(m_localDebug) qDebug()  << __FUNCTION__;
    m_nbColonnes            = nbColonnes;
    m_nbLignes              = nbLignes;
    m_flagsEntete           = flagsEnteteH;
    m_position              = QPointF(20,20);
    m_defautLargeurColonnes = (float)80;
    m_defautHauteurlignes   = (float)50;

    abeTableauCreeCellules();
    abeTableauConstruitTableau();
}
AbulEduTableauV1::~AbulEduTableauV1()
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
//    foreach (AbulEduCelluleV1 * cell, creeListeCellules()) {
//        if(cell)
//        {
//            cell->deleteLater();
//        }
//    }
}

void AbulEduTableauV1::abeTableauCreeCellules()
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    m_listeEnteteHaut.clear();
    m_listeEnteteBas.clear();
    m_listeEnteteGauche.clear();
    m_listeEnteteDroit.clear();
    m_listeEnteteCoins.clear();
    m_listeCellules.clear();
    QSizeF tailleDefaut = QSizeF(m_defautLargeurColonnes, m_defautHauteurlignes);
    // Création du coin HautGauche si nécessaire
    if(m_flagsEntete.testFlag(HautGauche)){
        QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
        cellule->abeCelluleAcceptePiece(false);
        m_listeEnteteCoins.insert(AbulEduTableauV1::HautGauche, cellule);
        static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
    }
    // Création du coin HautDroit si nécessaire
    if(m_flagsEntete.testFlag(HautDroit)){
        QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
        cellule->abeCelluleAcceptePiece(false);
        m_listeEnteteCoins.insert(AbulEduTableauV1::HautDroit, cellule);
        static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
    }
    // Création du coin BasDroit si nécessaire
    if(m_flagsEntete.testFlag(BasDroit)){
        QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
        cellule->abeCelluleAcceptePiece(false);
        m_listeEnteteCoins.insert(AbulEduTableauV1::BasDroit, cellule);
        static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
    }
    // Création du coin BasGauche si nécessaire
    if(m_flagsEntete.testFlag(BasGauche)){
        QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
        cellule->abeCelluleAcceptePiece(false);
        m_listeEnteteCoins.insert(AbulEduTableauV1::BasGauche, cellule);
        static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
    }

    //Création Entete Horizontal Haut
    if(m_flagsEntete.testFlag(Haut)){
        for(int i = 0; i < m_nbColonnes; i++){
            QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
            cellule->abeCelluleAcceptePiece(false);
            m_listeEnteteHaut.insert(i, cellule);
            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
        }
    }
    //Création Entete Horizontal Bas
    if(m_flagsEntete.testFlag(Bas)){
        for(int i = 0; i < m_nbColonnes; i++){
            QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
            cellule->abeCelluleAcceptePiece(false);
            m_listeEnteteBas.insert(i, cellule);
            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
        }
    }
    //Création Entete Vertical Gauche
    if(m_flagsEntete.testFlag(Gauche)){
        for(int i = 0; i < m_nbLignes; i++){
            QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
            cellule->abeCelluleAcceptePiece(false);
            m_listeEnteteGauche.insert(i, cellule);
            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
        }
    }
    //Création Entete Vertical Droit
    if(m_flagsEntete.testFlag(Droit)){
        for(int i = 0; i < m_nbLignes; i++){
            QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
            cellule->abeCelluleAcceptePiece(false);
            m_listeEnteteDroit.insert(i, cellule);
            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
        }
    }

    // Création des Cellules
    for(int i=0; i<m_nbLignes * m_nbColonnes; i++){
        QPointer<AbulEduCelluleV1> cellule = new AbulEduCelluleV1(tailleDefaut);
        cellule->abeCelluleAcceptePiece(true);
        cellule->setObjectName(QString::number(i)); //numérotage des cellules
        m_listeCellules.insert(i, cellule);
        static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(cellule);
    }
}

void AbulEduTableauV1::abeTableauConstruitTableau()
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    float x = m_position.x();
    float y = m_position.y();
    qreal zoneCentralePosX = 0; // Décalage en x lié aux entêtes de gauche
    qreal zoneEntetDroitPosX = x; // Décalage en X pour les entets situés à droite
    qreal hauteur=0;

    // Calcul du décalage
    if(m_flagsEntete.testFlag(HautGauche) || m_flagsEntete.testFlag(BasGauche) || m_flagsEntete.testFlag(Gauche)){
        // S'il y a au moins une entete dans la colonne de gauche
        if(m_listeEnteteCoins.value(HautGauche)){
            zoneCentralePosX = m_listeEnteteCoins.value(HautGauche)->abeCelluleGetSize().width();
        }
        for(int i = 0; i < m_listeEnteteGauche.size(); i++){
            zoneCentralePosX = qMax(zoneCentralePosX, m_listeEnteteGauche.value(i)->abeCelluleGetSize().width());
        }
        if(m_listeEnteteCoins.value(BasGauche)){
            zoneCentralePosX = qMax(zoneCentralePosX, m_listeEnteteCoins.value(BasGauche)->abeCelluleGetSize().width());
        }
    }

    //Calcul de la position horizontale des entetes de droite s'il y en a
    if(m_flagsEntete.testFlag(HautDroit) || m_flagsEntete.testFlag(BasDroit) || m_flagsEntete.testFlag(Droit)){
        zoneEntetDroitPosX = zoneEntetDroitPosX + zoneCentralePosX;
        // On ajoute la largeur des cellules centrale du tableau
        for(int i=0; i< m_nbColonnes; i++){
            zoneEntetDroitPosX = zoneEntetDroitPosX + m_listeCellules.value(i)->abeCelluleGetSize().width();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //                         Positionnement des entetes du haut
    ///////////////////////////////////////////////////////////////////////////////////////

    // Les entetes en haut

    if(m_flagsEntete.testFlag(HautGauche) || m_flagsEntete.testFlag(Haut) || m_flagsEntete.testFlag(HautDroit)){

        if(m_listeEnteteCoins.value(HautGauche)){ // Il y a une entete de coin à positionner
            //            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeEnteteCoins.value(HautGauche));
            m_listeEnteteCoins.value(HautGauche)->setPos(x, y);
            hauteur = m_listeEnteteCoins.value(HautGauche)->abeCelluleGetSize().height();
        }

        x = x + zoneCentralePosX;
        // Les entetes de colonnes
        for (int i=0; i < m_listeEnteteHaut.size(); i++){
            if(m_listeEnteteHaut.value(i)){ // il y a une entete à positionner
                m_listeEnteteHaut.value(i)->setPos(x, y);
                hauteur = qMax(hauteur,m_listeEnteteHaut.value(i)->abeCelluleGetSize().height());
                //                static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem( m_listeEnteteHaut.value(i));
            }
            x = x + m_listeEnteteHaut.value(i)->abeCelluleGetSize().width();
        }

        // Coin Droit
        if(m_listeEnteteCoins.value(HautDroit)){ // Il y a une entete de coin à positionner
            //            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeEnteteCoins.value(HautDroit));
            m_listeEnteteCoins.value(HautDroit)->setPos(zoneEntetDroitPosX, y);
            hauteur = qMax(hauteur,m_listeEnteteCoins.value(HautDroit)->abeCelluleGetSize().height());
        }
        x = m_position.x();
        y = m_position.y() + hauteur;
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    //                         Positionnement des Lignes
    ///////////////////////////////////////////////////////////////////////////////////////

    hauteur=0;
    for(int i = 0; i<m_nbLignes; i++){
        // Entete de ligne gauche
        if(m_flagsEntete.testFlag(Gauche)){
            if(m_listeEnteteGauche.value(i)){ // il y a une entete à positionner
                //                static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeEnteteGauche.value(i));
                m_listeEnteteGauche.value(i)->setPos(x,y);
                hauteur = m_listeEnteteGauche.value(i)->abeCelluleGetSize().height();
            }
        }
        // Les cellules
        x = x + zoneCentralePosX;
        for(int j = 0; j < m_nbColonnes; j++){
            //            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeCellules.value(i*m_nbColonnes + j));
            m_listeCellules.value(i*m_nbColonnes + j)->setPos(x, y);
            hauteur = qMax(hauteur,  m_listeCellules.value(i*m_nbColonnes + j)->abeCelluleGetSize().height());
            x = x + m_listeCellules.value(i*m_nbColonnes + j)->abeCelluleGetSize().width();
        }
        // Entete de ligne Droit
        if(m_flagsEntete.testFlag(Droit)){
            if(m_listeEnteteDroit.value(i)){ // il y a une entete à positionner
                //                static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeEnteteDroit.value(i));
                m_listeEnteteDroit.value(i)->setPos(x,y);
                hauteur = m_listeEnteteDroit.value(i)->abeCelluleGetSize().height();
            }
        }
        x = m_position.x();
        y = y + hauteur;
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //                         Positionnement des entetes du Bas
    ///////////////////////////////////////////////////////////////////////////////////////
    // Les entetes en Bas

    if(m_flagsEntete.testFlag(BasGauche) || m_flagsEntete.testFlag(Bas) || m_flagsEntete.testFlag(BasDroit)){
        if(m_listeEnteteCoins.value(BasGauche)){ // Il y a une entete de coin à positionner
            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeEnteteCoins.value(BasGauche));
            m_listeEnteteCoins.value(BasGauche)->setPos(x, y);
            hauteur = m_listeEnteteCoins.value(BasGauche)->abeCelluleGetSize().height();
        }

        x = x + zoneCentralePosX;
        // Les entetes de colonnes
        for (int i=0; i < m_listeEnteteBas.size(); i++){
            if(m_listeEnteteBas.value(i)){ // il y a une entete à positionner
                m_listeEnteteBas.value(i)->setPos(x, y);
                hauteur = qMax(hauteur,m_listeEnteteBas.value(Bas)->abeCelluleGetSize().height());
                //                static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem( m_listeEnteteBas.value(i));
            }
            x = x + m_listeEnteteBas.value(i)->abeCelluleGetSize().width();
        }

        // Coin Droit
        if(m_listeEnteteCoins.value(BasDroit)){ // Il y a une entete de coin à positionner
            //            static_cast<AbulEduEtiquettesV1 *>(this->parent())->scene()->addItem(m_listeEnteteCoins.value(AbulEduTableauV1::BasDroit));
            m_listeEnteteCoins.value(BasDroit)->setPos(zoneEntetDroitPosX, y);
            hauteur = qMax(hauteur,m_listeEnteteCoins.value(BasDroit)->abeCelluleGetSize().height());
        }
    }
}

QMap<int, AbulEduCelluleV1 *> AbulEduTableauV1::abeTableauGetEntetes(Entete position)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    switch(position){
    case Haut:
        return m_listeEnteteHaut;
        break;
    case Bas:
        return m_listeEnteteBas;
        break;
    case Gauche:
        return m_listeEnteteGauche;
        break;
    case Droit:
        return m_listeEnteteDroit;
        break;
    default:
        return QMap<int, AbulEduCelluleV1 *>();
    }
}

AbulEduCelluleV1 * AbulEduTableauV1::abeTableauGetEnteteCoin(Entete position)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    switch(position){
    case HautDroit:

    case HautGauche:

    case BasDroit:

    case BasGauche:
        return m_listeEnteteCoins.value(position);
        break;
    default:
        return 0;
    }
}

QMap<int, AbulEduCelluleV1 *> AbulEduTableauV1::abeTableauGetCellules()
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    return m_listeCellules;
}

AbulEduCelluleV1 * AbulEduTableauV1::abeTableauGetCellule(int numero)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    return m_listeCellules.value(numero);
}

QMap<int, AbulEduCelluleV1 *> AbulEduTableauV1::abeTableauGetColumnCellules(int numCol)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    QMap<int, AbulEduCelluleV1*>listCells;
    // Entetehaut
    if(m_flagsEntete.testFlag(Haut))
    {
        listCells.insert(-1, m_listeEnteteHaut.value(numCol));
    }
    else
    {
        listCells.insert(-1, 0);
    }
    // Les cellules centrales
    for(int i=0; i<m_nbLignes; i++)
    {
        listCells.insert(i, m_listeCellules.value(i*m_nbColonnes+numCol));
    }
    // La cellule d'entete bas
    if(m_flagsEntete.testFlag(Bas))
    {
        listCells.insert(m_nbLignes, m_listeEnteteBas.value(numCol));
    }
    else
    {
        listCells.insert(m_nbLignes, 0);
    }
    return listCells;
}

QMap<int, AbulEduCelluleV1 *> AbulEduTableauV1::abeTableauGetRowCellules(int numRow)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    QMap<int, AbulEduCelluleV1*>listCells;
    // Entete Gauche
    if(m_flagsEntete.testFlag(Gauche))
    {
        listCells.insert(-1, m_listeEnteteGauche.value(numRow));
    }
    else
    {
        listCells.insert(-1, 0);
    }
    // Cellules centrales
    for(int i=0; i<m_nbColonnes; i++)
    {
        listCells.insert(i, m_listeCellules.value(numRow*m_nbColonnes + i));
    }
    // Entete droite
    if(m_flagsEntete.testFlag(Droit))
    {
        listCells.insert(m_nbColonnes, m_listeEnteteDroit.value(numRow));
    }
    else
    {
        listCells.insert(m_nbColonnes, 0);
    }
    return listCells;
}

int AbulEduTableauV1::abeTableauGetCelluleColumn(int numero)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    return numero % m_nbColonnes;
}

int AbulEduTableauV1::abeTableauGetCelluleRow(int numero)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    return numero / m_nbColonnes;
}
void AbulEduTableauV1::abeTableauColonneRedim(int colonne, qreal taille)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;

    // Retrouve les cellules concernées
    if(colonne == 0){
        // Redimensionne la colonne d'entete de gauche
        if(m_flagsEntete.testFlag(HautGauche)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(HautGauche)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);
            m_listeEnteteCoins.value(HautGauche)->abeCelluleSetSize(nouvelletaille);
        }

        if(m_flagsEntete.testFlag(BasGauche)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(BasGauche)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);
            m_listeEnteteCoins.value(BasGauche)->abeCelluleSetSize(nouvelletaille);
        }
        if(m_flagsEntete.testFlag(Gauche)){
            for(int i=0; i<m_nbLignes; i++){
                QSizeF nouvelletaille = m_listeEnteteGauche.value(i)->abeCelluleGetSize();
                nouvelletaille.setWidth(taille);
                m_listeEnteteGauche.value(i)->abeCelluleSetSize(nouvelletaille);
            }
        }

    }
    if(colonne == m_nbColonnes +1){
        // Redimensionne la colonne d'entete de droite

        if(m_flagsEntete.testFlag(HautDroit)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(HautDroit)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);
            m_listeEnteteCoins.value(HautDroit)->abeCelluleSetSize(nouvelletaille);
        }

        if(m_flagsEntete.testFlag(BasDroit)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(BasDroit)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);
            m_listeEnteteCoins.value(BasDroit)->abeCelluleSetSize(nouvelletaille);
        }
        if(m_flagsEntete.testFlag(Droit)){
            for(int i=0; i<m_nbLignes; i++){
                QSizeF nouvelletaille = m_listeEnteteDroit.value(i)->abeCelluleGetSize();
                nouvelletaille.setWidth(taille);
                m_listeEnteteDroit.value(i)->abeCelluleSetSize(nouvelletaille);
            }
        }
    }

    if(colonne > 0 && colonne < m_nbColonnes + 1){
        // les entetes haut
        if(m_flagsEntete.testFlag(Haut)){
            QSizeF nouvelletaille = m_listeEnteteHaut.value(colonne -1)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);
            m_listeEnteteHaut.value(colonne - 1)->abeCelluleSetSize(nouvelletaille);
        }
        // Les entetes bas
        if(m_flagsEntete.testFlag(Bas)){
            QSizeF nouvelletaille = m_listeEnteteHaut.value(colonne - 1)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);            
            m_listeEnteteBas.value(colonne -1)->abeCelluleSetSize(nouvelletaille);
        }

        // Les cellules
        for(int i=0; i< m_nbLignes; i++){
            QSizeF nouvelletaille = m_listeEnteteHaut.value(colonne - 1)->abeCelluleGetSize();
            nouvelletaille.setWidth(taille);
            m_listeCellules.value(i*m_nbColonnes + colonne - 1)->abeCelluleSetSize(nouvelletaille);
        }
    }
    abeTableauConstruitTableau();
}

void AbulEduTableauV1::abeTableauLigneRedim(int ligne, qreal taille)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    // Retrouve les cellules concernées
    if(ligne == 0){
        // Redimensionne le coin haut gauche
        if(m_flagsEntete.testFlag(HautGauche)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(HautGauche)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeEnteteCoins.value(HautGauche)->abeCelluleSetSize(nouvelletaille);
        }

        if(m_flagsEntete.testFlag(HautDroit)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(HautDroit)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeEnteteCoins.value(HautDroit)->abeCelluleSetSize(nouvelletaille);
        }
        if(m_flagsEntete.testFlag(Haut)){
            for(int i=0; i<m_nbColonnes; i++){
                QSizeF nouvelletaille = m_listeEnteteHaut.value(i)->abeCelluleGetSize();
                nouvelletaille.setHeight(taille);
                m_listeEnteteHaut.value(i)->abeCelluleSetSize(nouvelletaille);
            }
        }

    }
    if(ligne == m_nbLignes +1){
        // Redimensionne la ligne d'entete du bas

        if(m_flagsEntete.testFlag(BasGauche)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(BasGauche)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeEnteteCoins.value(BasGauche)->abeCelluleSetSize(nouvelletaille);
        }

        if(m_flagsEntete.testFlag(BasDroit)){
            QSizeF nouvelletaille = m_listeEnteteCoins.value(BasDroit)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeEnteteCoins.value(BasDroit)->abeCelluleSetSize(nouvelletaille);
        }
        if(m_flagsEntete.testFlag(Bas)){
            for(int i=0; i<m_nbColonnes; i++){
                QSizeF nouvelletaille = m_listeEnteteBas.value(i)->abeCelluleGetSize();
                nouvelletaille.setHeight(taille);
                m_listeEnteteBas.value(i)->abeCelluleSetSize(nouvelletaille);
            }
        }
    }
    if(ligne > 0 && ligne < m_nbLignes + 1){
        // les entetes gauche
        if(m_flagsEntete.testFlag(Gauche)){
            QSizeF nouvelletaille = m_listeEnteteGauche.value(ligne -1)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeEnteteGauche.value(ligne - 1)->abeCelluleSetSize(nouvelletaille);
        }
        // Les entetes droit
        if(m_flagsEntete.testFlag(Droit)){
            QSizeF nouvelletaille = m_listeEnteteDroit.value(ligne - 1)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeEnteteDroit.value(ligne -1)->abeCelluleSetSize(nouvelletaille);
        }

        // Les cellules
        for(int i=0; i< m_nbColonnes; i++){
            QSizeF nouvelletaille = m_listeCellules.value((ligne -1)*m_nbColonnes + i)->abeCelluleGetSize();
            nouvelletaille.setHeight(taille);
            m_listeCellules.value((ligne -1)*m_nbColonnes + i)->abeCelluleSetSize(nouvelletaille);
        }
    }


    abeTableauConstruitTableau();
}

void AbulEduTableauV1::abeTableauRedimensionne(QSizeF taille)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    // Construire la liste de toutes les cellules
    QList<AbulEduCelluleV1 *> listeCellules = creeListeCellules();
    foreach(AbulEduCelluleV1 *cellule, listeCellules){
        cellule->abeCelluleSetSize(taille);
        abeTableauConstruitTableau();
    }
}

void AbulEduTableauV1::abeTableauSetPos(QPointF position)
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    if(m_position != position)
    {
        m_position = position;
        abeTableauConstruitTableau();
    }
}

QSizeF AbulEduTableauV1::abeTableauGetSize()
{
    if(m_localDebug)
    {
        qDebug()<<__FUNCTION__;
    }
    // Retourne la taille du tableau y compris les entetes

    // On calcule la largeur
    // On récupère la liste des cellules de la premiere ligne du tableau hors entete
    QMap<int, AbulEduCelluleV1 *>listcells = abeTableauGetRowCellules(0);
    float width = 0.0;
    QMapIterator<int, AbulEduCelluleV1 *> it(listcells);
    while(it.hasNext())
    {
        it.next();
        if(it.value())
        {
            width += it.value()->abeCelluleGetSize().width();
        }
    }

    // On calcule la hauteur
    QMap<int, AbulEduCelluleV1 *>listcells2 = abeTableauGetColumnCellules(0);
    float height = 0.0;
    QMapIterator<int, AbulEduCelluleV1 *> it2(listcells2);
    while(it2.hasNext())
    {
        it2.next();
        if(it2.value())
        {
            height += it2.value()->abeCelluleGetSize().width();
        }
    }
    // On retourne le tout
    return QSizeF(width, height);
}

QList<AbulEduCelluleV1 *> AbulEduTableauV1::creeListeCellules()
{
    if(m_localDebug) qDebug()  << __FUNCTION__;
    QList<AbulEduCelluleV1 *>listeCellules;
    listeCellules.append(m_listeEnteteHaut.values());
    listeCellules.append(m_listeEnteteBas.values());
    listeCellules.append(m_listeEnteteGauche.values());
    listeCellules.append(m_listeEnteteDroit.values());
    listeCellules.append(m_listeEnteteCoins.values());
    listeCellules.append(m_listeCellules.values());

    return listeCellules;
}
