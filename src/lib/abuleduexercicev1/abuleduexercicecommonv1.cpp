/**
  * Classe AbulEduExerciceCommonV1
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
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

#include "abuleduexercicecommonv1.h"

#ifndef DEBUG_ABULEDUEXERCICEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduExerciceCommonV1::AbulEduExerciceCommonV1(QObject *parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //Initialisation des différentes variables
    m_parent           = parent;
    m_exerciceName     = "";
    m_level            = "";
    m_nbTotalQuestions = 0;
    setAbeNumQuestion(0);
    m_skill            = "";
    m_isLivretComplete = true;
    m_isAnswerShownBeforeStart=false;
    m_numQuestionInSequence  = 0;
    m_nbTotalQuestionsSequence = 0;
    m_evaluation = abe::evalY;
    m_abuleduExerciceLogs = new AbulEduExerciceLogsV1();
}

AbulEduExerciceCommonV1::~AbulEduExerciceCommonV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete m_abuleduExerciceLogs;
}

void AbulEduExerciceCommonV1::setAbeLineLog(const QString& question, const QString& answer, int score, int nbPrintedQuestions,
                                            abe::ABE_EVAL evaluation, const QString& expected, const QString& answerDuration,
                                            const QString& answerIntermediate, const QString& answerProposed)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //Construction de la structure de données
    QHash <QString, QString> line;
    line.insert("exerciceName", m_exerciceName);
    line.insert("level", m_level);
    line.insert("score", QString::number(score));
    line.insert("nbTotalQuestions", QString::number(m_nbTotalQuestions));
    line.insert("nbPrintedQuestions", QString::number(nbPrintedQuestions));
    line.insert("question", question);
    line.insert("answer", answer);
    line.insert("evaluation", QString::number(evaluation));
    if(expected != "")
        line.insert("expected", expected);
    if(m_skill != "")
        line.insert("skill", m_skill);
    if(answerDuration != "")
        line.insert("answerDuration", answerDuration);
    if(answerIntermediate != "")
        line.insert("answerIntermediate", answerIntermediate);
    if(answerProposed != "")
        line.insert("answerProposed", answerProposed);

    m_arrayLogs.insert(m_arrayLogs.count(),line);
}

void AbulEduExerciceCommonV1::resetAbeLogs()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayLogs.clear();
}

const QHash<int, QHash<QString, QString> >  AbulEduExerciceCommonV1::getPluginLogs(const QString &login,
                                                                                    const QString &group,
                                                                                    const QString &exerciceName)
{
    Q_UNUSED(login)
    Q_UNUSED(group)
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayLogs;

    if(!exerciceName.isEmpty()) {
        QHash<int, QHash<QString, QString> > resultat;
        for(int i=0; i<m_arrayLogs.count(); i++) {
            QHash<QString, QString> ligne = m_arrayLogs[i];
            if(ligne["exerciceName"] == exerciceName) {
                resultat.insert(resultat.count(),ligne);
            }
        }
        return resultat;
    }
    else
        return m_arrayLogs;
}

const QHash<int, QHash<QString, QString> >&  AbulEduExerciceCommonV1::getPluginLogsDownloaded()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_arrayLogsDownloaded;
}

const QHash<int, QString>& AbulEduExerciceCommonV1::getPluginUsers()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayUsers;
    return m_arrayUsers;
}

const QHash<QString, QString> &AbulEduExerciceCommonV1::getPluginGroups()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayGroups;
    return m_arrayGroups;
}

const QHash<QString, QString>& AbulEduExerciceCommonV1::getPluginDomains()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayDomains;
    return m_arrayDomains;
}

const QHash<int, QString>& AbulEduExerciceCommonV1::getPluginApplications()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayApplications;
    return m_arrayApplications;
}

const QHash<int, QString>& AbulEduExerciceCommonV1::getPluginSkills()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arraySkills;
    return m_arraySkills;
}

const QHash<int, QString>& AbulEduExerciceCommonV1::getPluginExercices()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayExercices;
    return m_arrayExercices;
}

const QHash<int, QString>& AbulEduExerciceCommonV1::getPluginLevels()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_arrayLevels;
    return m_arrayLevels;
}

void AbulEduExerciceCommonV1::pushAbulEduLogs(const QString& moduleName)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //2012 on a maintenant notre infra sso, on peut donc expédier les logs sans passer par le plugin
    if(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() == 1) {
        //La variable qui contient tout ce qu'on expedie au serveur
        QVariantMap tableau;

        //Les informations de base
        tableau["module_name"] = moduleName;

        //Et toutes les questions
        for(int i=0; i<m_arrayLogs.count(); i++)
        {
            tableau[QString::number(i)+"_exercice_name"]                 = m_arrayLogs[i]["exerciceName"];
            tableau[QString::number(i)+"_exercice_level"]                = m_arrayLogs[i]["level"];
            tableau[QString::number(i)+"_exercice_skill"]                = m_arrayLogs[i]["skill"];
            tableau[QString::number(i)+"_exercice_score"]                = m_arrayLogs[i]["score"];
            tableau[QString::number(i)+"_exercice_nb_total_questions"]   = m_arrayLogs[i]["nbTotalQuestions"];
            tableau[QString::number(i)+"_exercice_nb_printed_questions"] = m_arrayLogs[i]["nbPrintedQuestions"];
            tableau[QString::number(i)+"_exercice_evaluation"]           = m_arrayLogs[i]["evaluation"];

            tableau[QString::number(i)+"_question_date"]                 = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            tableau[QString::number(i)+"_question_question"]             = m_arrayLogs[i]["question"];
            tableau[QString::number(i)+"_question_answer"]               = m_arrayLogs[i]["answer"];
            tableau[QString::number(i)+"_question_expected"]             = m_arrayLogs[i]["expected"];
            tableau[QString::number(i)+"_question_answer_time"]          = m_arrayLogs[i]["time"];
            tableau[QString::number(i)+"_question_answer_intermediate"]  = m_arrayLogs[i]["intermediate"];
            tableau[QString::number(i)+"_question_proposed"]             = m_arrayLogs[i]["proposed"];
            tableau[QString::number(i)+"_question_evaluation"]           = m_arrayLogs[i]["evaluation"];

        }
        tableau["exercice_nb_questions_log"]     = m_arrayLogs.count();

        m_abuleduExerciceLogs->abeLogsPush(tableau);
    }

    /** @todo on garde ou poubelle ? */
    QEvent* logEvent = new QEvent(AbulEduLogsPush);
    QApplication::postEvent(m_parent, logEvent);
}

void AbulEduExerciceCommonV1::setAbeExerciceName(const QString& s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_exerciceName = s;
}

void AbulEduExerciceCommonV1::setAbeExerciceEvaluation(const abe::ABE_EVAL& e)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_evaluation = e;
}

void AbulEduExerciceCommonV1::setAbeSkill(const QString& s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_skill = s;
}

const QString& AbulEduExerciceCommonV1::getAbeExerciceName()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << m_exerciceName;
    return m_exerciceName;
}

const abe::ABE_EVAL& AbulEduExerciceCommonV1::getAbeExerciceEvaluation()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_evaluation;
}

const QString& AbulEduExerciceCommonV1::getAbeLevel()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_level;
}

int AbulEduExerciceCommonV1::getAbeNbTotalQuestions() const
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_nbTotalQuestions;
}

const QString& AbulEduExerciceCommonV1::getAbeSkill()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_skill;
}

const QHash<QString, QString>& AbulEduExerciceCommonV1::downloadPluginLogsFilter()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_downloadFilter;
}

void AbulEduExerciceCommonV1::setPluginLogs(QHash<int, QHash<QString, QString> > h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(h.count() > 0)
        m_arrayLogsDownloaded =  h;
}

void AbulEduExerciceCommonV1::setPluginUsers(const QHash<int, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayUsers = h;
}

void AbulEduExerciceCommonV1::setPluginGroups(const QHash<QString, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayGroups = h;
}

void AbulEduExerciceCommonV1::setPluginDomains(const QHash<QString, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayDomains = h;
}

void AbulEduExerciceCommonV1::setPluginApplications(const QHash<int, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayApplications = h;
}

void AbulEduExerciceCommonV1::setPluginSkills(const QHash<int, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arraySkills = h;
}

void AbulEduExerciceCommonV1::setPluginExercices(const QHash<int, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayExercices = h;
}

void AbulEduExerciceCommonV1::setPluginLevels(const QHash<int, QString> &h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayLevels = h;
}

void AbulEduExerciceCommonV1::downloadAbulEduLogs()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduLogsDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduUsers()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduUsersDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduGroups()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduGroupsDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduDomains()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduDomainsDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduApplications()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduApplicationsDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduSkills()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduSkillsDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduExercices()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduExercicesDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::downloadAbulEduLevels()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QEvent* downloadEvent = new QEvent(AbulEduLevelsDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::setAbeDownloadLogsFilter(const QDate &dateBegin, const QDate &dateEnd, const QString &login,
                                                       const QString &group, const QString &evaluation, const QString &skill)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_downloadFilter.insert("login",login);
    m_downloadFilter.insert("group",group);
    m_downloadFilter.insert("dateBegin",dateBegin.toString("yyyy-MM-dd"));
    m_downloadFilter.insert("dateEnd",dateEnd.toString("yyyy-MM-dd"));
    m_downloadFilter.insert("evaluation",evaluation);
    m_downloadFilter.insert("skill",skill);
}

void AbulEduExerciceCommonV1::downloadAbulEduLivret(const QString &niveau, bool isComplete)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_level = niveau;
    m_isLivretComplete = isComplete;

    QEvent* downloadEvent = new QEvent(AbulEduLivretDownload);
    QApplication::postEvent(m_parent, downloadEvent);
}

void AbulEduExerciceCommonV1::setPluginLivret(const QHash<int, QHash<int, QString> >& h)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_arrayLivret.empty();
    m_arrayLivret = h;
}

QHash<int, QHash<int, QString> > AbulEduExerciceCommonV1::getPluginLivret()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_arrayLivret;
}

bool AbulEduExerciceCommonV1::getAbeIsLivretComplete()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_isLivretComplete;
}

void AbulEduExerciceCommonV1::setAbeIsLivretComplete(bool isComplete)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_isLivretComplete = isComplete;
}

int AbulEduExerciceCommonV1::getLocalLogs(const QString &exerciceName)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(exerciceName != "") {
        for(int i=0; i<m_arrayLogs.count(); i++) {
            QHash<QString, QString> ligne = m_arrayLogs[i];
            if(ligne["exerciceName"] == exerciceName) {
                m_cache_arrayLogs.insert(m_cache_arrayLogs.count(),ligne.values());
            }
        }
        //        qDebug() << " ****************************************************************** ";
        //        qDebug() << m_cache_arrayLogs;
        //        qDebug() << " ****************************************************************** ";
        return m_cache_arrayLogs.count();
    }
    else
        return m_arrayLogs.count();
}

QStringList AbulEduExerciceCommonV1::getLocalLineLog(int numLigneLog, const QStringList &masque)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QList < int > indicesAEnvoyer;
    QStringList retour;

    if(m_cache_arrayLogs.count() > 0 && numLigneLog>=0) {
        //On cherche les indices du masque
        if(masque.count()>0) {
            QStringList entete = m_arrayLogs[0].keys();

            //Parcours du masque
            QStringListIterator iMasque(masque);
            while(iMasque.hasNext()) {
                //On ajoute ce numero d'index a notre liste d'indexes a retourner
                indicesAEnvoyer.append(entete.indexOf(iMasque.next()));
            }

            if(indicesAEnvoyer.count()>0) {
                QListIterator < int > iIndice(indicesAEnvoyer);
                while(iIndice.hasNext()) {
                    retour.append(m_cache_arrayLogs[numLigneLog][iIndice.next()]);
                }
            }
        }
        else {
            retour = m_cache_arrayLogs[numLigneLog];
        }
    }
    return retour;
}

void AbulEduExerciceCommonV1::setAbeLevel(const QString& s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_level = s;
}

void AbulEduExerciceCommonV1::setAbeNbTotalQuestions(int n)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_nbTotalQuestions = n;
}
