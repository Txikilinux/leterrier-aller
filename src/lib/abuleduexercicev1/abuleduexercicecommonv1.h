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

#ifndef ABULEDUEXERCICECOMMONV1_H
#define ABULEDUEXERCICECOMMONV1_H

#include <QApplication>
#include <QObject>
#include <QHash>
#include <QEvent>
#include <QDate>
#include "abuleduexercicelogsv1.h"
#include "abuleduloggerv1.h"

/** @file abuleduexercicecommonv1.h
  * @version 1
  */
class AbulEduExerciceCommonV1
{

public:

    /** Constructeur de la classe
      * @param QObject *parent, un pointeur sur le parent de l'objet
      */
    explicit AbulEduExerciceCommonV1(QObject *parent);

    /** Destructeur de la classe
      */
    virtual ~AbulEduExerciceCommonV1();

    //----------------- Connecteurs pour le plugin --------------------- //
    /**
      * Recupere la totalite du tableau des logs en cours
      * Normalement vous ne devriez pas avoir à utiliser ces fonctions, elles
      * ne sont la que pour permettre au plugin de se connecter sur l'application
      */
    virtual const QHash<int, QHash<QString, QString> > getPluginLogs(const QString& login="", const QString &group="", const QString& exerciceName="");
    virtual const QHash<int, QHash<QString, QString> > &getPluginLogsDownloaded();
    /** Le plugin affecte le tableau des logs téléchargés */
    virtual void setPluginLogs(QHash<int, QHash<QString, QString> >);
    /** Permet d'indiquer au plugin ce qu'on veut télécharger comme logs */
    virtual const QHash<QString, QString> &downloadPluginLogsFilter();
    /** Le plugin affecte le tableau du livret */
    virtual void setPluginLivret(const QHash<int, QHash<int, QString> > &);

    QHash<int, QHash<int, QString> > getPluginLivret();

    //----------------- Gestion des logs locaux (avant que ça ne parte sur le serveur) --------------------- //
    /** Récupère les logs locaux pour un exercice et retourne le nombre de résultats
      * @param exerciceName: le nom de l'exercice dont on veut récupérer les logs, laisser vide pour avoir tous les logs (ie la séquence)
      * @return le nombre de lignes de logs
      */
    int getLocalLogs(const QString& exerciceName = "");

    /** Retourne une QStringList contenant les informations de la ligne demandée
      * @param numLigneLog: le numéro de la ligne de log qu'on souhaite récupérer
      * @param masque: permet de ne demander à n'avoir qu'une liste de champs dans le retour, par exemple ("question","nbPrintedQuestions","score")
      * @return une QStringList contenant soit toute la ligne demandée soit uniquement les champs demandés (masque)
      */
    QStringList getLocalLineLog(int numLigneLog, const QStringList& masque=QStringList());

    /** Recupere la totalite du tableau des utilisateurs */
    virtual const QHash<int, QString> &getPluginUsers();
    /** Le plugin affecte le tableau des utilisateurs téléchargés */
    virtual void setPluginUsers(const QHash<int, QString> &);

    /** Recupere la totalite du tableau des groupes */
    virtual const QHash<QString, QString>& getPluginGroups();
    /** Le plugin affecte le tableau des groupes téléchargés */
    virtual void setPluginGroups(const QHash<QString, QString>&);

    /** Recupere la totalite du tableau des domaines */
    virtual const QHash<QString, QString> &getPluginDomains();
    /** Le plugin affecte le tableau des domaines téléchargés */
    virtual void setPluginDomains(const QHash<QString, QString>&);

    /** Recupere la totalite du tableau des applications */
    virtual const QHash<int, QString> &getPluginApplications();
    /** Le plugin affecte le tableau des applications téléchargés */
    virtual void setPluginApplications(const QHash<int, QString> &);

    /** Recupere la totalite du tableau des skills */
    virtual const QHash<int, QString>& getPluginSkills();
    /** Le plugin affecte le tableau des skills téléchargés */
    virtual void setPluginSkills(const QHash<int, QString>&);

    /** Recupere la totalite du tableau des exercices */
    virtual const QHash<int, QString> &getPluginExercices();
    /** Le plugin affecte le tableau des exercices téléchargés */
    virtual void setPluginExercices(const QHash<int, QString>&);

    /** Recupere la totalite du tableau des niveaux */
    virtual const QHash<int, QString>& getPluginLevels();
    /** Le plugin affecte le tableau des niveaux téléchargés */
    virtual void setPluginLevels(const QHash<int, QString>&);

    //----------------- Lanceurs d'evenements --------------------- //
    /**
      * Permet de lancer des évènements a destination du plugin
      * vous pouvez faire appel à ces fonction à n'importe quel moment
      */
    /** Provoque un evenement qui a pour objectif d'envoyer les logs sur le serveur
      * @param moduleName: le nom du module (fichier .abe) utilisé
      */
    virtual void pushAbulEduLogs(const QString& moduleName="");

    /**
      * Déclanche le téléchargement des logs en émettant un évènement
      * @see setAbeDownloadLogsFilter qui permet de gérer les filtres de téléchargement
      */
    virtual void downloadAbulEduLogs();

    /**
      * Déclenche le téléchargement du livret en émettant un évènement
      * affecte au passage la variable niveau (cm1, etc.) et demande un livret complet ou synthetique (sans les competences "AB")
      * @see setAbeDownloadLogsFilter qui permet de gérer les filtres de téléchargement
      */
    virtual void downloadAbulEduLivret(const QString& niveau, bool isComplete = true);

    /**
      * Déclenche le téléchargement de la liste des utilisateurs
      */
    virtual void downloadAbulEduUsers();

    /**
      * Déclenche le téléchargement de la liste des groupes
      */
    virtual void downloadAbulEduGroups();

    /**
      * Déclenche le téléchargement de la liste des domaines
      */
    virtual void downloadAbulEduDomains();

    /**
      * Déclenche le téléchargement de la liste des applications
      */
    virtual void downloadAbulEduApplications();

    /**
      * Déclenche le téléchargement de la liste des skill
      */
    virtual void downloadAbulEduSkills();

    /**
      * Déclenche le téléchargement de la liste des exercices
      */
    virtual void downloadAbulEduExercices();


    /**
      * Déclenche le téléchargement de la liste des niveaux
      */
    virtual void downloadAbulEduLevels();

    //----------------- Les evements --------------------- //
    /**
      * Enregistrement de deux évènements type permettant à l'application de lancer des
      * évènements à n'importe quel moment
      */
    static const QEvent::Type AbulEduLogsPush              = (QEvent::Type)(QEvent::User+abe::LogsPush);
    static const QEvent::Type AbulEduLogsDownload          = (QEvent::Type)(QEvent::User+abe::LogsDownload);

    static const QEvent::Type AbulEduUsersDownload         = (QEvent::Type)(QEvent::User+abe::UsersDownload);
    static const QEvent::Type AbulEduGroupsDownload        = (QEvent::Type)(QEvent::User+abe::GroupsDownload);
    static const QEvent::Type AbulEduDomainsDownload       = (QEvent::Type)(QEvent::User+abe::DomainsDownload);
    static const QEvent::Type AbulEduApplicationsDownload  = (QEvent::Type)(QEvent::User+abe::ApplicationsDownload);
    static const QEvent::Type AbulEduSkillsDownload        = (QEvent::Type)(QEvent::User+abe::SkillsDownload);
    static const QEvent::Type AbulEduExercicesDownload     = (QEvent::Type)(QEvent::User+abe::ExercicesDownload);
    static const QEvent::Type AbulEduLevelsDownload        = (QEvent::Type)(QEvent::User+abe::LevelsDownload);

    //Le livret ... c'est quasiment un module a part
    static const QEvent::Type AbulEduLivretDownload        = (QEvent::Type)(QEvent::User+abe::LivretDownload);
    //----------------- Manipulation des variables membres --------------------- //
    /**
      * @note Ces fonctions sont à utiliser comme vous le souhaitez. Pour plus de simpicité leur nom est normalisé:
      * @note - setAbe: permet d'affecter des valeurs
      * @note - getAbe: permet de récupérer des valeurs
      */

    /**
      * Stocke une ligne de log dans le tableau local en prevision de l'envoi sur le serveur de logs
      * @param question: la question posée
      * @param answer: la réponse fournie
      * @param score: le score
      * @param nbPrintedQuestions: le nombre de question qui ont déjà été affichées
      * @param evaluation: l'évaluation de la réponse, a: bien, b: bof, c: bofbof d: pas bien, z: pas de réponse
      * @param expected: la réponse qu'on attendait "bonne réponse"
      * @param answerDuration: le temps que l'utilisateur a mis pour fournir la réponse
      * @param answerIntermediate: réponse intermédiaire (en cas de construction étape par étape d'une réponse)
      * @param answerProposed: la liste des réponses que le logiciel proposait (sous la forme d'une liste de
      *                        réponses sépparées par des ";" (csv)
      */
    virtual void setAbeLineLog(const QString& question, const QString& answer,
                               int score=-1,        int nbPrintedQuestions=0, abe::ABE_EVAL evaluation=abe::evalY,
                               const QString& expected="", const QString& answerDuration="", const QString& answerIntermediate="",
                               const QString& answerProposed="");

    /** Découpage en plusieurs fonctions TODO */
    void setAbeLogsAnswer(const QString& reponseFournie, qreal pourcentageReussite);
    void setAbeLogsQuestion(int numeroQuestion, const QString& questionPosee, const QString& reponseAtendue, abe::ABE_EVAL evaluation);
    void setAbeLogsExercise(int numeroExercice, int difficulte, int nombreQuestions, const QString& theme="");
    void setAbeLogsSequence(const QString& module, const QString& typeExercice);


    /**
      * Permet au développeur de filtrer ce qu'on veut télécharger comme logs
      * @param dateBegin: date de début
      * @param dateEnd: date de fin
      * @param login: l'identifiant de l'utilisateur dont on veut récupérer les logs
      * @param group: le nom du groupe dont on veut récupérer les logs
      * @param evaluation: permet de ne récupérer que les logs dont l'évaluation a une certaine valeur
      * @param skill: nom de la compétence (au sens abuledu) qu'on recherche
      */
    virtual void setAbeDownloadLogsFilter(const QDate& dateBegin=QDate::currentDate(), const QDate& dateEnd=QDate::currentDate(),
                                          const QString& login="", const QString& group="", const QString& evaluation="", const QString& skill="");
    /** Reset le tableau des logs en cours: remet à zéro le tableau des logs */
    virtual void resetAbeLogs();
    /** Donne un nom à l'exercice */
    virtual void setAbeExerciceName(const QString &s);
    /** Donne une évaluation à l'exercice */
    virtual void setAbeExerciceEvaluation(const abe::ABE_EVAL &e);
    /** Configure le niveau de l'exercice */
    virtual void setAbeLevel(const QString &s);
    /** Renseigne le nombre total de questions */
    virtual void setAbeNbTotalQuestions(int n);
    /** Renseigne la compétence au sens abuledu */
    virtual void setAbeSkill(const QString &s);

    /** Renvoie le nom de l'exercice */
    virtual const QString &getAbeExerciceName();
    /** Renvoie l'évaluation de l'exercice */
    virtual const abe::ABE_EVAL &getAbeExerciceEvaluation();
    /** Renvoie le niveau de l'exercice */
    virtual const QString &getAbeLevel();
    /** Renvoie le nombre total de questions */
    virtual int getAbeNbTotalQuestions() const;
    /** Renvoie la compétence au sens abuledu */
    virtual const QString &getAbeSkill();
    /** Retourne vrai si il faut telecharger le livret complet (avec les competences AB) */
    virtual bool getAbeIsLivretComplete();
    void setAbeIsLivretComplete(bool isComplete);

    // a faire ... to become en version v1
    //    virtual void setAbulEduParameters(QSettings s, QString login, QString group);
    //    /** Recupere les paramètres sur le serveur */
    //    virtual QSettings getAbulEduParameters(QString login, QString group);
    //    /** Provoque un evenement pour envoyer les paramètres sur le serveur */
    //    virtual void pushAbulEduParameters();


    /** Lecture du numéro de question */
    inline int getAbeNumQuestion() {return m_numQuestion;}
    /** Ecriture du numéro de question */
    inline void setAbeNumQuestion(int num) { m_numQuestion = num;}
    /** Incrémentation du numéro de question */
    inline void increaseAbeNumQuestion() { m_numQuestion++;}


protected:
    //----------------- Variables internes --------------------- //
    /** @todo vérifier si les deux variables ci-dessous ont une utilité,
        les déplacer en private après avoir fait des accesseurs si oui, les supprimer sinon */
    int                              m_nbTotalQuestionsSequence; //nombre total de questions dans la sequence complete
    int                              m_numQuestionInSequence;    //numero de la question de la sequence complete

private:
    QString                          m_exerciceName;             //nom de l'exercice
    int                              m_numQuestion;              //numero de la question de l'exercice en cours
    int                              m_nbTotalQuestions;         //nombre total de questions
    QString                          m_level;                    //niveau -> texte libre
    QString                          m_skill;                    //competence atomique leterrier
    bool                             m_isAnswerShownBeforeStart; //affiche la reponse tant que l'utilisateur n'a pas clique sur le bouton "commencer"
    QHash<int, QHash<QString, QString> > m_arrayLogs;            //stockage des logs
    QHash<int, QHash<int, QString> > m_arrayLivret;              //stockage du livret (temporaire en protected pour eviter de coder les accesseurs)
    abe::ABE_EVAL                    m_evaluation;

    QObject                             *m_parent;               //on garde le lien pour pouvoir lancer des event
    QHash<int, QStringList>              m_cache_arrayLogs;      //stockage des logs en cache lorsqu'on fait une requette de selection (logs locaux)
    QHash<int, QHash<QString, QString> > m_arrayLogsDownloaded;
    QHash<int, QString>                  m_arrayUsers;
    QHash<QString, QString>              m_arrayGroups;
    QHash<QString, QString>              m_arrayDomains;
    QHash<int, QString>                  m_arrayApplications;
    QHash<int, QString>                  m_arraySkills;
    QHash<int, QString>                  m_arrayExercices;
    QHash<int, QString>                  m_arrayLevels;
    QSettings                            m_parameters;       /** parametres -> a detailler */
    QHash<QString, QString>              m_downloadFilter;   /** filtre de telechargement des logs */
    bool                                 m_isLivretComplete; /** permet de choisir si on veut telecharger un livret complet ou synthetique */
    AbulEduExerciceLogsV1               *m_abuleduExerciceLogs; /** pour expédier les logs */
};

#endif // ABULEDUEXERCICECOMMONV1_H
