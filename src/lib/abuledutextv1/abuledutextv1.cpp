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

#include "abuledutextv1.h"

#ifndef DEBUG_ABULEDUTEXTV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduTextV1::AbulEduTextV1(QString fichier, QWidget* parent)
{
    setParent(parent);
    m_texte = QString();
    if(!fichier.isEmpty()) {
        QFile file(fichier);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            AbulEduMessageBoxV1(QObject::trUtf8("Échec"), QObject::trUtf8("Impossible d'ouvrir le fichier spécifié !"));
        }
        QTextStream inputStream(&file);
        inputStream.setCodec("UTF-8");
        m_texte = (inputStream.readAll());
        ABULEDU_LOG_DEBUG()<<"Le texte chargé commence par "<<m_texte.left(50)+"...";
    }
    else
    {
        ABULEDU_LOG_DEBUG()<<"AbulEduTexte vide";
    }
}

QString AbulEduTextV1::abeGetText()
{
    return m_texte;
}

void AbulEduTextV1::abeSetText(QString text)
{
    m_texte = text;
}

QMap<int, QString> AbulEduTextV1::abetextParagraphes()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QMap<int, QString> map;
    map.clear();
    if(!m_texte.isEmpty()){
        QStringList paragraphes =  m_texte.split("\n", QString::SkipEmptyParts);
        int i = 0;
        QString s;
        foreach(s, paragraphes){
            map.insert(i, s.trimmed());
            i++;
        }
    }
    return map;
}

QMap<int, QString> AbulEduTextV1::abetextPhrases()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QMap<int, QString> map;
    QString tmpPhrase;
    map.clear();
    QRegExp sep("[\\.!?\n]"); //Les séparateurs de phrases
    if(!m_texte.isEmpty()){
        /* Trouver les guillemets fermants 1
        QMap<int,int> indexGuillemets;
        int ind = m_texte.indexOf("\"",0);
        while(ind > -1){
            indexGuillemets.insert(indexGuillemets.size()+1,ind);
            ind = m_texte.indexOf("\"",ind+1);
        }
        QMap<int,int> indexGuillemetsFin;
        QMapIterator<int,int> iter(indexGuillemets);
        while(iter.hasNext()){
            iter.next();
            if(iter.key()%2 == 0){
                indexGuillemetsFin.insert(iter.key(),iter.value());
            }
        }
        */

        /* Trouver les guillemets fermants 2
        int nbGuillemets = 0;
        int lastIndex = 0;
        while (m_texte.indexOf("\"",lastIndex)>-1){
            ABULEDU_LOG_DEBUG()<<" //////////////////////////////                                      "<<m_texte.indexOf("\"",lastIndex);
            if(m_texte.indexOf("\"",lastIndex) + 11 < m_texte.length()) ABULEDU_LOG_DEBUG()<<m_texte.mid(m_texte.indexOf("\"",lastIndex),10);
            ABULEDU_LOG_DEBUG()<<nbGuillemets;
            if(nbGuillemets%2 == 1)
            {
                m_texte.replace(m_texte.indexOf("\"",lastIndex),1,"guillemetDeFin");
                ABULEDU_LOG_DEBUG()<<"je remplace";
            }
            else
            {
                ABULEDU_LOG_DEBUG()<<"je ne remplace pas";
            }
            nbGuillemets++;
            lastIndex = m_texte.indexOf("\"",lastIndex)+1;
        }
        */

        tmpPhrase.clear();
        int debut = 0;
        int i = 0;
        int index = m_texte.indexOf(sep,0);
        while (index>-1){
            QString phrase=m_texte.mid(debut,index-debut+1).trimmed().simplified();
            ABULEDU_LOG_DEBUG() << "debug de phrase: " << phrase << " index " << index;
            debut=index+1;
            index = m_texte.indexOf(sep,debut);
            if (index >=0 && index+1 < m_texte.size()){
                if(QString(m_texte.at(index+1)) == QString::fromUtf8("»")){
                    index = index+1;
                }
            }

            /* Trouver les guillemets fermants 3
               Je vais ici utiliser un truc que j'ai remarqué : si le guillemet de début de phrase
               est en fait le guillemet fermant de la phrase précédente, il est suivi d'un espace */
            if(phrase.startsWith("\" ") || phrase =="\""){
                phrase.remove(0,2);
                map.insert(i-1,map.value(i-1)+"\"");
            }
            if(phrase.size()!=0){
                ABULEDU_LOG_DEBUG()<<"phrase"<<phrase.size()<<phrase;
                map.insert(i, phrase);
                i++;
            }
            //Si on est à la fin
            if(index == -1) {
                phrase = m_texte.right(m_texte.size()-debut).trimmed().simplified();
                if(!phrase.isEmpty()) {
                    map.insert(i, phrase);
                }
            }
        }
    }
    ABULEDU_LOG_DEBUG() << map;
    return map;
}

QString AbulEduTextV1::abetextPhrase(int numPhrase)
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    return abetextPhrases().value(numPhrase);
}

QString AbulEduTextV1::abetextNettoieText(const QString& text)
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QString texte;
    if(text.isNull()){
        texte = m_texte;
    }
    else {
        texte = text;
    }
    texte.replace(":", " : "); // Ajoute un espace avant et après :
    texte.replace("!", " ! ");
    texte.replace("?", " ? ");
    texte.replace("(", " ("); // Pareil pour les parenthèses
    texte.replace("( ", "(");
    texte.replace(")", ") ");
    texte.replace(" )", ")");
    texte.replace(",", ", ");
    /* Cette classe vient du logiciel ALLER, pour lequel la ligne ci-dessous avait été faite, mais qui provoquait des dégâts pour les versions antérieures à la 5.0.10 */
    if(abeApp->applicationName() == "leterrier-aller"){
        if(abeApp->applicationVersion() >= "5.0.10"){
            texte.replace(QString::fromUtf8("..."), QString::fromUtf8("…"));
        }
    }
    else{
        texte.replace(QString::fromUtf8("..."), QString::fromUtf8("…"));
    }
    texte.replace(QString::fromUtf8("«"), QString::fromUtf8(" « "));
    texte.replace(QString::fromUtf8("»"), QString::fromUtf8(" » "));
    texte.replace(QRegExp(" {1,}"), " "); // Supprime les espaces multiples
    texte.replace("’","'");  // remplace les apostrophes moisies (cf bilbo)
    texte.replace("' ","'"); // enlève les espaces devant et après l'apostrophe
    texte.replace(" '","'");
    texte.replace("- ","-");// enlève les espaces devant et après le tiret
    texte.replace(" -","-");
    texte.replace(QString::fromUtf8("―"), "-");
    texte.replace(" .","."); // enlève l'espace avant le point
    texte.replace(" ,",","); // enlève l'espace avant la virgule
    texte.replace(QString::fromUtf8("« "), QString::fromUtf8(" «"));
    texte.replace(QString::fromUtf8(" »"), QString::fromUtf8("» "));

    //piste mais demande a avoir absolument toute la liste de tous les caractères acceptés ... ca fait long!
    //    QRegExp gardeQueTexteValide("[^a-zA-Z0-9 :!?(),'«»-\"\\.,;@%#\s]");
    //    texte.replace(gardeQueTexteValide,"");

    QRegExp sep("[.!?]");

    QStringList tmpListeParagraphes;
    QString tmpParagraphe;
    tmpListeParagraphes = texte.split("\n", QString::SkipEmptyParts);

    for(int i = 0; i < tmpListeParagraphes.size(); i++)
    {
        tmpParagraphe.clear();
        int debut = 0;
        int index = 0;
        while(debut < tmpListeParagraphes[i].size())
        {
            index = tmpListeParagraphes[i].indexOf(sep,debut);
            if(index == -1)
                index = tmpListeParagraphes[i].size()-1;
            QString phrase=tmpListeParagraphes[i].mid(debut,index-debut+1).trimmed().simplified();
            /* Les lignes ci-dessous provoquent le bug #4361 */
            //            if(phrase.mid(0,1)=="-")
            //            {
            //                phrase.replace(0,1,QString::fromUtf8("― "));
            //            }
            tmpParagraphe = tmpParagraphe + phrase + " ";
            debut=index+1;
        }

        tmpListeParagraphes[i]= tmpParagraphe.trimmed();
        tmpListeParagraphes[i].replace(". . .", QString::fromUtf8("…"));
        tmpListeParagraphes[i].replace("! .", "!");
        tmpListeParagraphes[i].replace("? .", "?");
        tmpListeParagraphes[i].replace(QString::fromUtf8(" »"),QString::fromUtf8("»"));
        //    tmpListeParagraphes[i].replace(" \"", "\"");

    }

    texte.clear();
    for(int i = 0; i<tmpListeParagraphes.size(); i++)
    {
        if(tmpListeParagraphes[i].size()!=0)
        {
            texte = texte + tmpListeParagraphes[i] +"\n";
        }
    }

    // On enlève l'espace de fin
    texte = texte.trimmed();

    return texte;
}

QString AbulEduTextV1::abetextNettoieMot(const QString& mot)
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QString retourMot = mot;
    return retourMot.remove(QString::fromUtf8("…"))
            .remove(".")
            .remove("!")
            .remove("?")
            .remove(";")
            .remove(":")
            .remove(",")
            .remove(QString::fromUtf8("»"))
            .remove(QString::fromUtf8("«")).simplified();
}

QMap<int,QString> AbulEduTextV1::abetextMots()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QStringList listeMots;
    QString texte(m_texte);
    //    texte.replace(QString::fromUtf8("…"), ".");
    //    texte.replace(QString::fromUtf8("―"), "");
    //    texte.replace("(", "");
    //    texte.replace(")", "");
    //    texte.replace("\"", "");
    //    texte.replace(QString::fromUtf8("«"), "");
    //    texte.replace(QString::fromUtf8("»"), "");
    //    texte.replace("-t-"," ");
    texte.replace("\n"," ");

    QRegExp sep("[.,!?;: ]");
    //Astuce pour que l'arbre soit découpé en deux "mots" "l'" et "arbre"
    if(texte.contains("'")) {
        QStringList listeMots1 = texte.split("'",QString::SkipEmptyParts);

//        ABULEDU_LOG_DEBUG() << "AbulEduTexte::abetextMots !! " << listeMots1;

        for (int i=0;i<listeMots1.size();i++)
        {
            //Si c'est pas le dernier on ajoute "'"
//            ABULEDU_LOG_DEBUG()<<"taille "<<listeMots1.size();
            if(i < listeMots1.size()-1) {
//                ABULEDU_LOG_DEBUG() << i<<" on ajout une ' a la fin du mot " << listeMots1[i];
                listeMots1[i].append("'");
            }
            listeMots.append(listeMots1[i].split(sep,QString::SkipEmptyParts));
            //        ABULEDU_LOG_DEBUG()<<listeMots1[i].split(sep,QString::SkipEmptyParts);
        }
    }
    else {
        listeMots.append(texte.split(sep,QString::SkipEmptyParts));
    }


    QMap<int,QString> motsIndexes;
    for (int k=0;k<listeMots.size();k++)
    {
        //        if (listeMots[k].trimmed() !="'")
        //        {
        motsIndexes.insertMulti(k,listeMots[k]);
        //        }
    }
    ABULEDU_LOG_DEBUG()<<"nombre de mots"<<motsIndexes.size();
    //    ABULEDU_LOG_DEBUG()<<motsIndexes;
    return motsIndexes;
}

QMap<int,QString> AbulEduTextV1::abetextMotsPonctues()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QString texte(m_texte);
    texte = texte.trimmed();
    //    texte.replace(QString::fromUtf8("…"), ".");
    //    texte.replace(QString::fromUtf8("―"), "");
    //    texte.replace("(", "");
    //    texte.replace(")", "");
    //    texte.replace("\"", "");
    //    texte.replace(QString::fromUtf8("«"), "");
    //    texte.replace(QString::fromUtf8("»"), "");
    //    texte.replace("-t-"," ");
    texte.replace("\n","retour_a_la_ligne");
    /* On va coller toutes les ponctuations au mot précédent si elle ne le sont pas (détection des mots). Il faudra à la fin décoller les ponctuations doubles */
    /* Je le commente tant qu'il semble poser plus de problèmes qu'il n'en résout */
    //    texte.replace("...",QString::fromUtf8("…"));
    texte.replace(" "+QString::fromUtf8("…"),QString::fromUtf8("…"));
    texte.replace(" ,",",");
    texte.replace(" ;",";");
    texte.replace(" :",":");
    texte.replace(" .",".");
    texte.replace(" !","!");
    texte.replace(" ?","?");

    QChar lastChar;
    if(!texte.isEmpty()){
        lastChar = texte.at(texte.length()-1);
    }

    QStringList listeMots1 = texte.split("'",QString::SkipEmptyParts);

    QStringList listeMots;
    //Ajout d'une "'" a la fin de chaque mot, sauf le dernier
    for (int i=0;i<listeMots1.size()-1;i++)
    {
        listeMots1[i].append("'");
    }

    QStringList separateursDoubles;
    separateursDoubles<<"?"<<"!"<<";"<<":";
    QStringList separateurs;
    separateurs <<"."<<","<<QString::fromUtf8("…")<<separateursDoubles;
    //Pour chaque type de séparateur on coupe le mot et on ajoute le séparateur à la fin du "mot précédent"
    foreach(QString sep,separateurs)
    {
        foreach(QString morceau,listeMots1)
        {
            QStringList listeMots2 = morceau.split(sep,QString::SkipEmptyParts);
            for (int i=0;i<listeMots2.size();i++)
            {
                //-1 pour n'ajouter de séparateur que jusqu'à l'avant dernier mot
                if(i < listeMots2.size()-1) {
                    listeMots2[i].append(sep);
                }
            }
            listeMots.append(listeMots2);
        }
        listeMots1 = listeMots;
        listeMots.clear();
    }

    /* Par contre, le dernier mot n'a pas sa ponctuation... donc je la rajoute à la main */
    if(separateurs.contains(lastChar) && listeMots1.size() > 0)
    {
        listeMots1.last().append(lastChar);
    }

    foreach(QString morceau,listeMots1)
    {
        QStringList listeMots2 = morceau.split(" ",QString::SkipEmptyParts);
        listeMots.append(listeMots2);
    }


    QStringList copieListeMots = listeMots;
    listeMots.clear();
    foreach(QString str,copieListeMots)
    {
        /* Je décolle les séparateurs doubles */
        foreach (QString sep, separateursDoubles) {
            str.replace(sep," "+sep);
        }
        if(str.contains("retour_a_la_ligne"))
        {
            if(str.startsWith("retour_a_la_ligne"))
            {
                listeMots.append("retour_a_la_ligne");
                listeMots.append(str.remove("retour_a_la_ligne"));
            }
            else
            {
                QStringList splt = str.split("retour_a_la_ligne",QString::SkipEmptyParts);
                if(splt.size() != 2)
                {
                    ABULEDU_LOG_DEBUG()<<"Gros problème rencontré dans le parcours du texte...";
                    return QMap<int,QString>();
                }
                listeMots.append(splt.first());
                listeMots.append("retour_a_la_ligne");
                listeMots.append(splt.last());
            }
        }
        else
        {
            listeMots.append(str);
        }
    }

    ABULEDU_LOG_DEBUG()<<"Mots ponctués";
    foreach(QString bout,listeMots){
        bout.replace(":", " :");
        ABULEDU_LOG_DEBUG()<<bout;
    }
    QMap<int,QString> motsIndexes;
    for (int k=0;k<listeMots.size();k++)
    {
        if (listeMots[k].trimmed() !="'")
        {
            motsIndexes.insertMulti(k,listeMots[k]);
        }
    }
    //    ABULEDU_LOG_DEBUG()<<"nombre de mots"<<motsIndexes.size();
    //    ABULEDU_LOG_DEBUG()<<motsIndexes;
    return motsIndexes;
}
int AbulEduTextV1::abetextCompteMots()
{
    return abetextMots().size();
}

int AbulEduTextV1::abetextCompteMotsPonctues()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    QMap<int,QString> copie;
    QMapIterator<int,QString> iterator(abetextMotsPonctues());
    while(iterator.hasNext())
    {
        iterator.next();
        if(iterator.value() != "retour_a_la_ligne")
        {
            copie.insert(iterator.key(),iterator.value());
        }
    }
    return copie.size();
}

QMap<int, QPair<QString, QPair<int, int> > > AbulEduTextV1::abeTextSuperMots()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    int debutRecherche = 0;
    QMap<int, QPair<QString, QPair<int, int> > > mapARetourner;
    QMap<int,QString> listeMots = abetextMotsPonctues();
    //    QMap<int,QString> listeMots = abetextMots();

    ABULEDU_LOG_DEBUG() << " AbulEduTexte::abetextMotsPonctues " << listeMots;

    QTextEdit* editeur = new QTextEdit(abeGetText());
    for (int i=0;i<listeMots.count();i++)
    {
        if(listeMots.value(i) != "retour_a_la_ligne")
        {
            ABULEDU_LOG_DEBUG()<<listeMots.value(i);
            QTextCursor curseur = editeur->document()->find(listeMots.value(i),debutRecherche);

            QPair<QString, QPair<int, int> > mapIntermediaire(curseur.selectedText(),QPair<int,int>(curseur.selectionStart(),curseur.selectionEnd()));
            mapARetourner.insert(i,mapIntermediaire);
            debutRecherche = curseur.selectionEnd();
            ABULEDU_LOG_DEBUG()<<"if début recherche "<<debutRecherche;
        }
        else {
            ABULEDU_LOG_DEBUG()<<"else début recherche "<<debutRecherche;
            QPair<QString, QPair<int, int> > mapIntermediaire("retour_a_la_ligne",QPair<int,int>(debutRecherche,debutRecherche+1));
            mapARetourner.insert(i,mapIntermediaire);
            debutRecherche++;
        }
    }
    //    ABULEDU_LOG_DEBUG()<<mapARetourner;
    return mapARetourner;
}

int AbulEduTextV1::abetextComptePhrases()
{
    return abetextPhrases().size();
}

bool AbulEduTextV1::isEmpty()
{
    if(m_texte.isEmpty()) return true;
    else return false;
}
