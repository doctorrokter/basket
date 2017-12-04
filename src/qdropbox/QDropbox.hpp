/*
 * QDropbox.hpp
 *
 *  Created on: Nov 25, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOX_HPP_
#define QDROPBOX_HPP_

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>

#include "SharedLink.hpp"
#include "QDropboxFile.hpp"
#include "Account.hpp"
#include "../Logger.hpp"

class QDropbox : public QObject {
    Q_OBJECT
public:
    QDropbox(QObject* parent = 0);
    QDropbox(const QString& accessToken, QObject* parent = 0);
    QDropbox(const QString& appSecret, const QString& appKey, const QString& redirectUri, QObject* parent = 0);
    virtual ~QDropbox();

    const QString& getUrl() const;
    QDropbox& setUrl(const QString& url);

    const int& getVersion() const;
    QDropbox& setVersion(const int& version);

    const QString& getAppSecret() const;
    QDropbox& setAppSecret(const QString& appSecret);

    const QString& getAppKey() const;
    QDropbox& setAppKey(const QString& appKey);

    const QString& getRedirectUri() const;
    QDropbox& setRedirectUri(const QString& redirectUri);

    const QString& getAccessToken() const;
    QDropbox& setAccessToken(const QString& accessToken);

    QString authUrl() const;

    // files
    void listFolder(const QString& path = "", const bool& includeMediaInfo = false, const bool& recursive = false,
                    const bool& includeDeleted = false, const bool& includeHasExplicitSharedMembers = false, const bool& includeMountedFolders = true,
                    const int& limit = 0, SharedLink sharedLink = SharedLink());

    // users
    void getAccount(const QString& accountId);
    void getCurrentAccount();

Q_SIGNALS:
    void accessTokenChanged(const QString& accessToken);

    // files
    void listFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore);

    // users
    void accountLoaded(Account* account);
    void currentAccountLoaded(Account* account);

private slots:
    void onError(QNetworkReply::NetworkError e);

    // files
    void onListFolderLoaded();

    // users
    void onAccountLoaded();
    void onCurrentAccountLoaded();

private:
    static Logger logger;

    QNetworkAccessManager m_network;

    QString m_authUrl;
    QString m_url;
    int m_version;

    QString m_accessToken;
    QString m_appSecret;
    QString m_appKey;
    QString m_redirectUri;

    QString m_fullUrl;

    void init();
    void generateFullUrl();

    QNetworkRequest prepareRequest(const QString& apiMethod);
};

#endif /* QDROPBOX_HPP_ */
