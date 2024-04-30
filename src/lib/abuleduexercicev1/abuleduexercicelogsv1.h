/**
  * Classe AbulEduExerciceLogsV1
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

#ifndef ABULEDUEXERCICELOGSV1_H
#define ABULEDUEXERCICELOGSV1_H

#include <QObject>
#include "maiaXmlRpcClient.h"
#include "abuleduapplicationv1.h"
#include "abuleduloggerv1.h"

class AbulEduExerciceLogsV1 : public QObject
{
    Q_OBJECT

public:
    explicit AbulEduExerciceLogsV1(QObject *parent = 0);
    /** Destructeur de la classe */
    virtual ~AbulEduExerciceLogsV1();
    QVariant abeXMLnotNULL(QVariant input);

public slots:
    void abeLogsPush(QVariantMap tableau);

private slots:
    /** Expédition d'un flux XML avec les logs */
    void rpcPushLogs(QVariant &v, QNetworkReply *reply);

    /** Gestion des erreurs XMLRPC */
    void rpcFault(int code, const QString &errorMessage);

private:
    QString                             *m_exerciceSession;  /** La session de l'exercice, un timestamp tout simple */
    MaiaXmlRpcClient                    *m_rpcAbulEduLogs;
    AbulEduNetworkAccessManagerV1       *m_nam;
};

#endif // ABULEDUEXERCICELOGSV1_H
