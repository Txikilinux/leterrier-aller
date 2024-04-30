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

#include "abuleduoutilsv1.h"

AbulEduOutilsV1::AbulEduOutilsV1(QWidget* parent)
{
    m_parent = parent;
    qsrand(QTime().currentTime().msec()); // Initialise la fonction qRand() avec une valeur arbitraire

//    dicoNiveaux.insert(Outil,"Outil");
//    dicoNiveaux.insert(CP,"CP");
//    dicoNiveaux.insert(CE1,"CE1");
//    dicoNiveaux.insert(CE2,"CE2");
//    dicoNiveaux.insert(CM1,"CM1");
//    dicoNiveaux.insert(CM2,"CM2");

    m_matchNature.insert(abe::VER,"VER");
    m_matchNature.insert(abe::NOM,"NOM");
    m_matchNature.insert(abe::ADJ,"ADJ");
    m_matchNature.insert(abe::ADV,"ADV");
    m_matchNature.insert(abe::PRE,"PRE");
    m_matchNature.insert(abe::PRO,"PRO");
    m_matchNature.insert(abe::CON,"CON");
    m_matchNature.insert(abe::DET,"DET");

    m_localDebug = 0;
}


QStringList AbulEduOutilsV1::abeoutilsCreerListeMots(QStringList listeMots, int nbMots, abe::ABE_LEVEL niveau, QString nature, int profondeur)
{
    if (m_localDebug) qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<"parametres "<<nbMots<<"::"<<niveau<<"::"<<nature<<"::"<<profondeur;
    QStringList listeComplete=listeMots;
    QStringList m_listeMots;
    m_listeMots.clear();
    nbMots= qMin(nbMots, listeComplete.size());
    nbMots = qMin(nbMots,18);
    if (nbMots == -1)
        nbMots = listeComplete.size();
    int i=0;
    while(i < nbMots && listeComplete.size()!=0)
    {
        int hasard = qrand()%listeComplete.size();
        QString mot=listeComplete.takeAt(hasard);
//        QStringList lmot=mot.split(QRegExp("\\W"),QString::SkipEmptyParts); // On obtient Mot, Niveau, Nature
        QStringList lmot=mot.remove('"').split(";",QString::SkipEmptyParts); // On obtient Mot, Niveau, Nature
        if (lmot.size()<3)
        {
            lmot=mot.split("\t",QString::SkipEmptyParts); // On obtient Mot, Niveau, Nature
            if (lmot.size()<3)
            {
                AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(QObject::trUtf8("Problème"),QObject::trUtf8("Problème avec le dictionnaire des mots...")
                                                                          +"<br/> "
                                                                          +QObject::trUtf8("L'exécution en mode intelligent sera perturbée, voire impossible !!"),
                                                                          true,m_parent);
                messageBox->show();
                return m_listeMots;
            }
        }

        if(( abe::ABE_DONNENIVEAU().key(lmot[1],abe::CM2) <= niveau) && (lmot[2].contains(nature) || nature == ""))
        {
            bool onAjouteLeMot = true;
            if(!profondeur == 0)
            {
                QString debutMot = lmot[0].mid(0,profondeur);

                for(int j=0;j < m_listeMots.size(); j++)
                {
                    // On regarde si le début du mot (fonction de profondeur) est présent dans les mots déjà sélectionnés
                    if(m_listeMots[j].startsWith(debutMot,Qt::CaseInsensitive))
                    {
                        onAjouteLeMot=false;
                    }
                }
            }
            if(onAjouteLeMot)
            {
                m_listeMots.append(lmot[0]);
                i++;
            }
        }
        else
            i++;
    }

    m_listeMots = abeoutilsSortListeMots(m_listeMots);
    return m_listeMots;
}

QStringList AbulEduOutilsV1::abeoutilsCreerListeMots(QStringList listeMots, int nbMots, abe::ABE_LEVEL niveau, abe::ABE_NATURE nature, int profondeur)
{
    if (m_localDebug) qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    return abeoutilsCreerListeMots(listeMots,nbMots,niveau,m_matchNature.value(nature),profondeur);
}

QStringList AbulEduOutilsV1::abeoutilsSortListeMots(QStringList listeMots, bool sensCroissant)
{
//    qDebug()<<" ------ "<< __FILE__ <<  __LINE__ << __FUNCTION__;

    // On va trier la liste de mots pour les avoir dans l'ordre alphabétique
    // On utilise compareLocaleAware pour tenir compte des règles de tri locales
    // On passe au qSort pour pouvoir trier y compris le fichier dictionnaire de 28000 mots
    // Le paramètre sensCroissant indique l'ordre du tri true croissant et false décroissant
    if(sensCroissant) // tri dans l'ordre croissant
    {
        qSort(listeMots.begin(),listeMots.end(),localeAwareCompareToBool);
    }

    else // Tri dans l'ordre décroissant
    {
        qSort(listeMots.begin(),listeMots.end(),localeAwareCompareToBool);
        QStringList inverse;
        for(int i=listeMots.size()-1; i>=0;i--)
        {
            inverse << listeMots[i];
        }
        listeMots = inverse;
    }
    return listeMots;
}

QVector <int> AbulEduOutilsV1::abeoutilsFindGoodList(QVector<int> &a)
{
    if (m_localDebug) qDebug() << "Appel de findGoodList() sur la liste " << a;
    QVector <int> b;
    QVector <int> p(a.size());

    int u, v;

    if (a.empty()) return b;

    b.push_back(0);

    for (int i = 1; i < a.size(); i++)
    {
        // If next element a[i] is greater than last element of current longest subsequence a[b.back()], just push it at back of "b" and continue
        if (a[b.back()] < a[i])
        {
            p[i] = b.back();
            b.push_back(i);
            continue;
        }

        // Binary search to find the smallest element referenced by b which is just bigger than a[i]
        // Note : Binary search is performed on b (and not a). Size of b is always <=k and hence contributes O(log k) to complexity.
        for (u = 0, v = b.size()-1; u < v;)
        {
            int c = (u + v) / 2;
            if (a[b[c]] < a[i]) u=c+1; else v=c;
        }

        // Update b if new value is smaller then previously referenced value
        if (a[i] < a[b[u]])
        {
            if (u > 0) p[i] = b[u-1];
            b[u] = i;
        }
    }

    for (u = b.size(), v = b.back(); u--; v = p[v]) b[u] = v;

    QVector <int> c;
    for(int z=0;z<a.size();z++) {
        if (b.contains(z)) {
            c.append(a[z]);
        }
    }
    return c;
}

void AbulEduOutilsV1::removeGraphicsPixmaps(QGraphicsItem *item)
{
    QList<QGraphicsItem *>tmpListeItems = item->childItems();
    QGraphicsItem *tmpItem;
    foreach(tmpItem, tmpListeItems)
    {
        if(tmpItem->type()==7)
        {
            delete tmpItem;
        }
    }

}

QStringList AbulEduOutilsV1::abeoutilsCreerListeMotsOutils(QString cheminDictionnaire, abe::ABE_LEVEL niveau, QString nature)
{
    QString fic(QString(cheminDictionnaire +"/mot_aller_cycle3.csv"));
    if (m_localDebug) qDebug()<<fic;
    AbulEduTextV1* motsOutilsAttributs = new AbulEduTextV1(fic);

    QStringList tmpliste;
    tmpliste.clear();

    tmpliste.append(AbulEduOutilsV1::abeoutilsCreerListeMots(motsOutilsAttributs->abetextParagraphes().values(),-1,niveau, nature,0));
    //            tmpliste.append(AbulEduOutilsV1::abeoutilsCreerListeMots(motsOutilsAttributs->abetextParagraphes().values(),motsOutilsAttributs->abetextParagraphes().values().count(),niveau, nature,0));
    return tmpliste;
}

QStringList AbulEduOutilsV1::abeoutilsCreerListeMotsOutils(QString cheminDictionnaire, abe::ABE_LEVEL niveau, abe::ABE_NATURE nature)
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<" niveau "<<niveau<<" et nature "<<nature;

    return abeoutilsCreerListeMotsOutils(cheminDictionnaire,niveau,m_matchNature.value(nature));
}

void AbulEduOutilsV1::abeoutilsCreeAbulEduLabels()
{
    if (m_localDebug) qDebug()<<"ExerciceMelange -- creeAbulEduLabels(1)";
    // Au cas ou il y aurait des abuleduLabels qui trainent, on les efface
//    QMap<int, AbuleduLabel *> aEffacer = ExerciceMelange::creerListeAbuleduLabel();
//    for(int i=0; i<aEffacer.size();i++)
//    {
//         delete aEffacer[i];
//    }
    //Création des abuleduLabel : un par mot

//    int x = m_posListeEtiquettes.x();
//    int y = m_posListeEtiquettes.y();
//    for(int i=0;i<m_listeObjets.size();i++)
//    {
//        AbuleduLabel* etiq = new AbuleduLabel(m_listeObjets[i],i,i,frAireDeJeu);
//        etiq->setObjectName("Etiquette "+QString::number(i));
//            etiq->setFixedSize(frAireDeJeu->width(),fontMetrics().boundingRect(etiq->text() + m_espaceEtiquette).height());
//        etiq->move(x,y);
//        etiq->setLastPos(QPoint(x,y));
//        etiq->setMarge(m_espaceEtiquette);
//        //if (m_localDebug) qDebug()<<"lastpoint"<<etiq->lastPos();
////        if (m_isAnswerShownBeforeStart) {
////            etiq->show();
////        }
////        else {
////            etiq->hide();
////        }
//        connect(etiq,SIGNAL(abuleduLabelSelected(int)),
//                this,SLOT(onAbuleduLabelSelected(int)));
//        connect(etiq,SIGNAL(abuleduLabelMoved(AbuleduLabel  *)),
//                this,SLOT(onAbuleduLabelMoved(AbuleduLabel *)));
//            y = y + etiq->height()+m_espaceEtiquette;

//    }
//    if (m_localDebug) qDebug()<<"x"<<x<<"y"<<y;
//    m_abuleduLabelSelected = -1; // pas d'étiquette sélectionnée
//    ui->btnHaut->setEnabled(false);
//    ui->btnBas->setEnabled(false);
//    ui->btnGauche->setEnabled(false);
//    ui->btnDroite->setEnabled(false);
    //    if (m_localDebug) qDebug()<<"ExerciceMelange -- creeAbulEduLabels(2)";
}

bool AbulEduOutilsV1::localeAwareCompareToBool(const QString &s1, const QString &s2)
{
    if (QString::localeAwareCompare(s1,s2) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
