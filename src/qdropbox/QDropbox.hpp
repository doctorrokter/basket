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
#include <QVariantMap>
#include <QList>
#include <QtGui/QImage>
#include <QFile>

#include "SharedLink.hpp"
#include "QDropboxFile.hpp"
#include "Account.hpp"
#include "QDropboxSpaceUsage.hpp"
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

    const QString& getDownloadsFolder() const;
    QDropbox& setDownloadsFolder(const QString& downloadsFolder);

    QDropbox& setReadBufferSize(qint64 readBufferSize);

    QString authUrl() const;

    // files
    void listFolder(const QString& path = "", const bool& includeMediaInfo = false, const bool& recursive = false,
                    const bool& includeDeleted = false, const bool& includeHasExplicitSharedMembers = false, const bool& includeMountedFolders = true,
                    const int& limit = 0, SharedLink sharedLink = SharedLink());
    void listFolderContinue(const QString& cursor);
    void createFolder(const QString& path, const bool& autorename = false);
    void deleteFile(const QString& path);
    void move(const QString& fromPath, const QString& toPath, const bool& allowSharedFolder = false, const bool& autorename = false, const bool& allowOwnershipTransfer = false);
    void rename(const QString& fromPath, const QString& toPath, const bool& allowSharedFolder = false, const bool& autorename = false, const bool& allowOwnershipTransfer = false);
    void getThumbnail(const QString& path, const QString& size = "w128h128", const QString& format = "jpeg");
    void download(const QString& path, const QString& rev = "");
    void upload(QFile* file, const QString& remotePath, const QString& mode = "add", const bool& autorename = true, const bool& mute = false);

    // users
    void getAccount(const QString& accountId);
    void getCurrentAccount();
    void getSpaceUsage();

Q_SIGNALS:
    void accessTokenChanged(const QString& accessToken);

    // files signals
    void listFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore);
    void listFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore);
    void folderCreated(QDropboxFile* folder);
    void fileDeleted(QDropboxFile* folder);
    void moved(QDropboxFile* file);
    void renamed(QDropboxFile* file);
    void thumbnailLoaded(const QString& path, const QString& size, QImage* thumbnail);
    void downloadStarted(const QString& path);
    void downloaded(const QString& path, const QString& localPath);
    void downloadProgress(const QString& path, qint64 loaded, qint64 total);
    void uploadStarted(const QString& remotePath);
    void uploaded(QDropboxFile* file);
    void uploadProgress(const QString& remotePath, qint64 loaded, qint64 total);

    // users signals
    void accountLoaded(Account* account);
    void currentAccountLoaded(Account* account);
    void spaceUsageLoaded(QDropboxSpaceUsage* spaceUsage);

private slots:
    void onError(QNetworkReply::NetworkError e);

    // files slots
    void onListFolderLoaded();
    void onListFolderContinueLoaded();
    void onFolderCreated();
    void onFileDeleted();
    void onMoved();
    void onRenamed();
    void onThumbnailLoaded();
    void onDownloaded();
    void onDownloadProgress(qint64 loaded, qint64 total);
    void onUploaded();
    void onUploadProgress(qint64 loaded, qint64 total);
    void read();

    // users slots
    void onAccountLoaded();
    void onCurrentAccountLoaded();
    void onSpaceUsageLoaded();

private:
    static Logger logger;

    QNetworkAccessManager m_network;

    QString m_authUrl;
    QString m_url;
    QString m_contentUrl;
    int m_version;

    QString m_accessToken;
    QString m_appSecret;
    QString m_appKey;
    QString m_redirectUri;

    qint64 m_readBufferSize;
    QList<QNetworkReply*> m_uploadsQueue;
    QList<QNetworkReply*> m_downloadsQueue;
    QString m_downloadsFolder;

    QString m_fullUrl;
    QString m_fullContentUrl;

    void init();
    void generateFullUrl();
    void generateFullContentUrl();

    QString getFilename(const QString& path);

    QNetworkRequest prepareRequest(const QString& apiMethod);
    QNetworkRequest prepareContentRequest(const QString& apiMethod);
    QNetworkReply* getReply();

    QNetworkReply* moveFile(const QString& fromPath, const QString& toPath, const bool& allowSharedFolder = false, const bool& autorename = false, const bool& allowOwnershipTransfer = false);
};

#endif /* QDROPBOX_HPP_ */
