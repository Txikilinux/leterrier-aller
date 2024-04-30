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

#include "abuleduexercicelogsv1.h"

#ifndef DEBUG_ABULEDUEXERCICEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduExerciceLogsV1::AbulEduExerciceLogsV1(QObject *parent) :
    QObject(parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_exerciceSession = new QString();
    m_nam = abeApp->getAbeNetworkAccessManager();
    m_rpcAbulEduLogs = new MaiaXmlRpcClient(QUrl("https://leterrier.abuledu.net/index.php"), this);
    //    m_rpcAbulEduLogs = new MaiaXmlRpcClient(QUrl("http://localhost/xmlrpc/index.php"), this);
}

AbulEduExerciceLogsV1::~AbulEduExerciceLogsV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete m_rpcAbulEduLogs;
    delete m_exerciceSession;
}

QVariant AbulEduExerciceLogsV1::abeXMLnotNULL(QVariant input)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return input;
}

void AbulEduExerciceLogsV1::abeLogsPush(QVariantMap tableau)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QString monip;
    foreach (QNetworkInterface ni, QNetworkInterface::allInterfaces ())
    {
        foreach (QNetworkAddressEntry nae, ni.addressEntries())
        {
            if(nae.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                monip = nae.ip().toString();
                if(monip.startsWith("127.0"))
                    monip = "";
                else if(monip.startsWith("0"))
                    monip = "";
            }
        }
    }

#if defined(Q_WS_WIN)
    tableau["application_os"] = "windows";
#elif defined(Q_OS_ANDROID)
    tableau["application_os"] = "android";
#elif defined(Q_OS_LINUX)
    tableau["application_os"] = "linux";
#elif defined(Q_WS_MAC)
    tableau["application_os"] = "osx";
#endif

    tableau["application_name"]      = abeXMLnotNULL(abeApp->applicationName());
    tableau["application_version"]   = abeXMLnotNULL(abeApp->applicationVersion());
    tableau["application_ip_locale"] = abeXMLnotNULL(monip);
    tableau["application_hostname"]  = abeXMLnotNULL(QHostInfo::localHostName());
    tableau["application_code"]      = abeXMLnotNULL(abeApp->applicationName());

    ABULEDU_LOG_DEBUG()<< "  >> logs envoyés "  << tableau;

    QList<QVariant> params = QList<QVariant>() << tableau;
    m_rpcAbulEduLogs->call("abuledu.logs.push", params,
                           this, SLOT(rpcPushLogs(QVariant&, QNetworkReply*)),
                           this, SLOT(rpcFault(int, const QString &)));

}

void AbulEduExerciceLogsV1::rpcPushLogs(QVariant &v, QNetworkReply *reply)
{
    Q_UNUSED(reply);
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__
                        << "============================================================"
                        << v.toMap()
                        << "============================================================";
    /**
      * @todo gérer l'erreur éventuelle: le code retour du serveur (la transmission réseau est ptet ok mais c'est pas pour autant que le fichier est bien stocké "la bas"
      */
}

void AbulEduExerciceLogsV1::rpcFault(int code, const QString &errorMessage)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << code << errorMessage;
}

