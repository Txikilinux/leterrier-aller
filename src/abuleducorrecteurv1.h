/**
  * (le terrier d'AbulEdu)
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

#ifndef ABULEDUCORRECTEURV1_H
#define ABULEDUCORRECTEURV1_H

#include <QWidget>
#include <hunspell/hunspell.hxx>
#include <QMultiHash>
#include <QTextEdit>
#include <QTextCursor>
#include <QSignalMapper>
#include <QMenu>
#include <QObject>
#include <QTextDocumentFragment>

class AbulEduCorrecteurV1 : public QObject
{
Q_OBJECT
public:
    AbulEduCorrecteurV1(QTextEdit *textObject);
    ~AbulEduCorrecteurV1();
//    bool replaceWordRequested(QPoint posCurseur);
    QStringList suggestWords(QString word);
private:
    bool m_localDebug;
    /** Crée une liste de tous les mots
      * Cette liste est très volatile dans le cas où on remplace un mot par un autre
      * ou lors de toute modification du texte
      * @return la liste des mots non trouvés (en clé) et leurs positions de curseur de début et fin de mot (dans une QPair)
      */
    QMultiHash<QString, QPair<int,int> > parseText();

    /** Curseur du texte contenu dans le TextEdit */
    QTextCursor *curseur;

    Hunspell *correcteur;
    char **propositions;
    QStringList *tokenList;
    QTextEdit *m_textObject;
    QMenu *menuContextuelPropositions;
//    QTextCharFormat  highlightFormat;
    QPair<int,int> m_previousSelection;
private slots:
    /** Vérifie au fur et à mesure l'orthographe des mots saisis */
    void parseWord();

};

#endif // ABULEDUCORRECTEURV1_H
