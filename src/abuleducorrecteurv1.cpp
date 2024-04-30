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

#include "abuleducorrecteurv1.h"
#include <QDebug>

AbulEduCorrecteurV1::AbulEduCorrecteurV1(QTextEdit *textObject) :
    QObject(textObject)
{
    m_localDebug = false;
#if !defined(Q_OS_ANDROID)
    m_textObject=textObject;
    correcteur = new Hunspell("data/dictionnaires/fr-moderne.aff","data/dictionnaires/fr-moderne.dic");
    curseur = new QTextCursor(textObject->document());
    // On y va
    parseText();
#endif
}
AbulEduCorrecteurV1::~AbulEduCorrecteurV1()
{
    delete correcteur;
    delete curseur;
}

void AbulEduCorrecteurV1::parseWord()
{
#if !defined(Q_OS_ANDROID)
    if(m_localDebug) qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    // Style des vaguelettes pour souligner les mots inconnus
    QTextCharFormat highlightFormat;
    highlightFormat.setUnderlineColor(Qt::red);
    highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

        if(m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;

//        curseur->movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
        curseur->select(QTextCursor::WordUnderCursor);
        QString mot = curseur->selectedText();
        if(m_localDebug) qDebug()<<mot;
/*        if(mot=="(")
        {
            curseur->setPosition(curseur->position());
            curseur->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            mot = curseur->selectedText();
        }*/
        if(mot.size()==0)
        {
/*            if(mot.endsWith(QString::fromUtf8("…")) || mot.endsWith(QString::fromUtf8("»")))
            {
                mot=mot.left(mot.size()-1);
                curseur->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,1);
            }
            if(mot.startsWith(QString::fromUtf8("«")))
            {
                int anchorPos = curseur->anchor() + 1;
                int cursPos = curseur->position();
                curseur->setPosition(anchorPos);
                curseur->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, cursPos - anchorPos );
            }
            mot = curseur->selectedText();*/
            if(m_localDebug) qDebug()<<"a sélectionner : "<<m_previousSelection;
            QTextCursor* autreCurseur = new QTextCursor(m_textObject->document());
            autreCurseur->setPosition(m_previousSelection.first,QTextCursor::MoveAnchor);
            autreCurseur->setPosition(m_previousSelection.second,QTextCursor::KeepAnchor);
            if(!correcteur->spell(autreCurseur->selection().toPlainText().toUtf8().data())) // On vérifie dans le dictionnaire
            {
                disconnect(m_textObject,SIGNAL(textChanged()),this,SLOT(parseWord()));

                autreCurseur->setCharFormat(highlightFormat);

                connect(m_textObject,SIGNAL(textChanged()),this,SLOT(parseWord()));
            }
        }
        m_previousSelection = QPair<int,int>(curseur->selectionStart(),curseur->selectionEnd());
                if(m_localDebug) qDebug()<<m_previousSelection;
#endif
}

QMultiHash<QString, QPair<int, int> > AbulEduCorrecteurV1::parseText()
{
    QMultiHash<QString, QPair<int, int> > listeMotsNonTrouves;
    listeMotsNonTrouves.clear();

#if !defined(Q_OS_ANDROID)
    // Style des vaguelettes pour souligner les mots inconnus
    QTextCharFormat highlightFormat;
    highlightFormat.setUnderlineColor(Qt::red);
    highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    // On se place au début
    curseur->movePosition(QTextCursor::Start);

    while(!curseur->atEnd())
    {
        // On sélectionne le mot
        curseur->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        // Todo Traiter le cas des : » ( et d'autre ;-(
        QString mot = curseur->selectedText();
        if(mot=="("){
            curseur->setPosition(curseur->position());
            curseur->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            mot = curseur->selectedText();
        }
        if(mot.size()>1){
            if(mot.endsWith(QString::fromUtf8("…")) ||
                    mot.endsWith(QString::fromUtf8("»"))){
                mot=mot.left(mot.size()-1);
                curseur->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,1);
            }
            if(mot.startsWith(QString::fromUtf8("«"))){
                int anchorPos = curseur->anchor() + 1;
                int cursPos = curseur->position();
                curseur->setPosition(anchorPos);
                curseur->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, cursPos - anchorPos );
            }
            mot = curseur->selectedText();
            if(!correcteur->spell(mot.toUtf8().data())) // On vérifie dans le dictionnaire
            {
                listeMotsNonTrouves.insert(mot, QPair<int, int>(curseur->position(), curseur->anchor()));
                curseur->setCharFormat(highlightFormat);
            }

        }
        //On se déplace au mot suivant
        curseur->movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor);

    }
#endif
    return listeMotsNonTrouves;
}

QStringList AbulEduCorrecteurV1::suggestWords(QString word)
{
    if(m_localDebug) qDebug()<<"AbulEduCorrecteurV1::suggestWords : "<<word;
    QStringList listeMots;
#if !defined(Q_OS_ANDROID)
    if(word.size()>1){
        if(word.endsWith(QString::fromUtf8("…"))){
            word=word.left(word.size()-1);
            if(m_localDebug) qDebug()<<"AbulEduCorrecteurV1::suggestWords : "<<word;
        }
    }
    char **propositions;
    int nbPropositions = correcteur->suggest(&propositions,word.toUtf8().data() );
    for(int i=0; i<nbPropositions;i++)
    {
        listeMots.append(QString::fromUtf8(propositions[i]));
    }
#endif
    return listeMots;
}

