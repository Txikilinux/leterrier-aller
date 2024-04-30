/** AbulEduTextV1
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2012-2015 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#ifndef ABULEDUTEXTV1_H
#define ABULEDUTEXTV1_H
#include "abuledumessageboxv1.h"

/** @brief Cette classe propose des fonctionnalités sur un texte passé en fichier */
class AbulEduTextV1 : public QObject
{
public:

    AbulEduTextV1(QString fichier = QString(), QWidget *parent = 0);

    /** Retourne le texte
      * @return le texte
      */
    QString abeGetText();

    /** Affecte la QString text à m_texte */
    void abeSetText(QString text);

    /** Retourne le texte nettoyé typographiquement
      * @return le texte nettoyé
      * @param text la chaine à nettoyer. S'il est vide, c'est m_texte qui sera nettoyé.
      */
    QString abetextNettoieText(const QString &text = QString());

    static QString abetextNettoieMot(const QString &mot);

    /** Retourne une QMap des paragraphes avec comme key le numéro du paragraphe */
    QMap<int, QString> abetextParagraphes(); // Les paragraphes se terminent par \n

    /** Retourne une QMap des phrases avec comme key le numéro de la phrase */
    QMap<int, QString> abetextPhrases(); // Les phrases se terminent par . ... ! ? \n
    /** Retourne la phrase numéro num
      * @param numPhrase
      */
    QString abetextPhrase(int numPhrase);

    /** Retourne une QMap des mots avec comme key le numéro d'ordre dans le texte */
    QMap<int,QString> abetextMots();

    /** Retourne une QMap des mots avec comme key le numéro d'ordre dans le texte */
    QMap<int,QString> abetextMotsPonctues();

    /** Retourne le nombre de mots du texte
      * @return le nombre de mots
      */
    int abetextCompteMots();

    /** Retourne le nombre de mots du texte
      * @return le nombre de mots
      */
    int abetextCompteMotsPonctues();

    /** Retourne une structure d'éléments sur le modèle suivant :
      * (indexMotDansTexte, (mot,(positionCurseurDebutMot,positionCurseurFinMot))) */
    QMap <int,QPair<QString,QPair<int,int> > > abeTextSuperMots();

    /** Retourne le nombre de phrases
      * @return le nombre de phrases
      */
    int abetextComptePhrases();

    bool isEmpty();

private:
    QString m_texte;
};

#endif // ABULEDUTEXTV1_H
