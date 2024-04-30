/**
  * Outils(le terrier d'AbulEdu)
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduTeteV1
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011-2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GPL)

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
  * @brief AbulEduTeteV1 affiche sur série d'images (têtes) symbolisant la réussite à un exercice
  * Une tête peut avoir quatre état : Neutre, Bien, Passable, Mal
  * Cette classe retourne une QHBoxLayout comportant n "têtes" Neutres, d'une taille spécifiée ou 51px par défaut
  * Il est possible de modifier l'état de chaque tête et de retrouver son état
  * de même, il est possible de redimensionner les têtes (elles auront toutes la meme taille)
  * Il est possible de spécifier un thème pour les têtes à condition qu'il ait été enregistré
  * dans le fichier de ressources tetes.qrc avec un prefix qui servira à les identifier et pour chacune des quatre images un alias : Neutre, Bien, Passable ou Mal
  */

#ifndef ABULEDUGRAPHICSTETESWIDGETV1_H
#define ABULEDUGRAPHICSTETESWIDGETV1_H

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QPainter>
#include <QFileInfo>
#include <QDebug>
#include "abuleduconstantesv1.h"
#include "abuledugraphicsellipseitem.h"
#include "abuledugraphicstextitem.h"
#include "abuleduloggerv1.h"

class AbulEduGraphicsTetesWidgetV1 : public QGraphicsWidget
{
    Q_OBJECT

public:
    /** Crée une QGraphicsWidget composée de n têtes
      * @param prefixe reférence au groupe de têtes défini dans tetes.qrc
      * @param nbTete le nombre de têtes a afficher (état Neutre au lancement)
      * @param taille en pixel 80 par défaut (les proportions de l'image sont conservées)
      */
//    AbulEduTeteV1(QString prefixeTete, int nbTete, int taille = 51, QWidget *parent = 0);
    explicit AbulEduGraphicsTetesWidgetV1(int nbTete, int taille = 80,QGraphicsWidget *parent = 0);

    ~AbulEduGraphicsTetesWidgetV1();
    /** Attribue un état à la tête d'indice numTete
      * @param numTete le numéro de la tête dans le widget
      * @param tete l'évaluation sous forme d'énum (cf abuleduconstantesv1)
      * @param isAppraisal la valeur true correspond à une tête de petite taille (rappel des exercices passés) et la valeur false correspond à une tête agrandie : exercice en cours
      * @param numQuestion est le numéro de la question; il vaut -1 s'il n'y a qu'une question dans l'exercice; Si la valeur est différente de -1, elle s'affiche dans un rond en haut à gauche de la tête
      * @note si on choisit d'afficher un nombre correspondant à la question, il est adroit d'afficher le nombre de question restante plutôt que le numéro de la question courante : on ne saurait pas combien il y en a en tout
      */
    void setEtatTete(int numTete, abe::ABE_EVAL tete,bool isAppraisal = true, int numQuestion = -1);

    /** Change le nombre de têtes et les réaffiche dans l'état neutre
      * @param nbTete le nombre de têtes a afficher (état Neutre)
      */
    void setNbTetes(int nbTete);

    /** Réaffiche toutes les têtes dans l'état neutre
      * @param prefixe reférence au groupe de têtes défini dans tetes.qrc
      * @param nbTete le nombre de têtes a afficher (état Neutre au lancement)
      */
    void resetTetes(int nbTete, QGraphicsWidget *parent = 0);

    /** Redimensionne les têtes
      * @param taille la taile en pixel (les proportions de l'image sont conservées)
      */
    void resizeTetes(int taille);

    /** Retourne la taille en pixel des tetes (images carrées) */
    int getSizeTetes();

    /** Dimensionne la marge autour des têtes */
    void setMarges(int marge);

    /** Lis l'état d'une tête
      * @param le numéro de la tête
      * @return EVAL cf abuleduconstantesv1
      */
    abe::ABE_EVAL getEtatTete(int numTete);

    /** Lit l'état de toutes les têtes
      * @return Qlist<ENUM> les numéro de l'état de chaque tête :  cf abuleduconstantesv1
      */
    QList<abe::ABE_EVAL> getEtatTetes();

    /** Fixe les couleurs de contour et de fond
      * @param contour la couleur du contour
      * @param background la brosse de remplissage du fond
      */
    void setCouleurs(QPen contour, QBrush brosse);

    QMap<int, QString> *suffixe;

private:

    QList<abe::ABE_EVAL> m_listeTetes;
    QString m_prefixe;
    QSize m_taille;
    int margeH;
    int margeV;
    QList<QGraphicsPixmapItem *> listePixmapTetes;
    QPen m_contour;
    QBrush m_background;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect();
    int m_currentTete;
    bool m_isAppraisal;
    int m_numQuestion;

};

#endif // ABULEDUGRAPHICSTETESWIDGETV1_H
