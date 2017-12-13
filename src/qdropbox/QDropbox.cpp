/*
 * QDropbox.cpp
 *
 *  Created on: Nov 25, 2017
 *      Author: doctorrokter
 */

#include "QDropbox.hpp"
#include <QUrl>
#include <QDebug>
#include <QList>
#include <QDir>
#include "../qjson/serializer.h"
#include "../qjson/parser.h"
#include "QDropboxFile.hpp"
#include "QDropboxCommon.hpp"

Logger QDropbox::logger = Logger::getLogger("QDropbox");

QDropbox::QDropbox(QObject* parent) : QObject(parent) {
    init();
}

QDropbox::QDropbox(const QString& accessToken, QObject* parent) : QObject(parent) {
    init();
    m_accessToken = accessToken;
}

QDropbox::QDropbox(const QString& appSecret, const QString& appKey, const QString& redirectUri, QObject* parent) : QObject(parent) {
    init();
    m_appSecret = appSecret;
    m_appKey = appKey;
    m_redirectUri = redirectUri;
}

QDropbox::~QDropbox() {}

const QString& QDropbox::getUrl() const { return m_url; }
QDropbox& QDropbox::setUrl(const QString& url) {
    m_url = url;
    generateFullUrl();
    return *this;
}

const int& QDropbox::getVersion() const { return m_version; }
QDropbox& QDropbox::setVersion(const int& version) {
    m_version = version;
    generateFullUrl();
    return *this;
}

const QString& QDropbox::getAppSecret() const { return m_appSecret; }
QDropbox& QDropbox::setAppSecret(const QString& appSecret) {
    m_appSecret = appSecret;
    return *this;
}

const QString& QDropbox::getAppKey() const { return m_appKey; }
QDropbox& QDropbox::setAppKey(const QString& appKey) {
    m_appKey = appKey;
    return *this;
}

const QString& QDropbox::getRedirectUri() const { return m_redirectUri; }
QDropbox& QDropbox::setRedirectUri(const QString& redirectUri) {
    m_redirectUri = redirectUri;
    return *this;
}

const QString& QDropbox::getAccessToken() const { return m_accessToken; }
QDropbox& QDropbox::setAccessToken(const QString& accessToken) {
    if (m_accessToken.compare(accessToken) != 0) {
        m_accessToken = accessToken;
        emit accessTokenChanged(m_accessToken);
    }
    return *this;
}

const QString& QDropbox::getDownloadsFolder() const { return m_downloadsFolder; }
QDropbox& QDropbox::setDownloadsFolder(const QString& downloadsFolder) {
    m_downloadsFolder = downloadsFolder;
    return *this;
}

QDropbox& QDropbox::setReadBufferSize(qint64 readBufferSize) {
    m_readBufferSize = readBufferSize;
    return *this;
}

QString QDropbox::authUrl() const {
    return QString(m_authUrl).append("/authorize?response_type=token&client_id=").append(m_appKey).append("&redirect_uri=").append(m_redirectUri);
}

void QDropbox::listFolder(const QString& path, const bool& includeMediaInfo, const bool& recursive,
                    const bool& includeDeleted, const bool& includeHasExplicitSharedMembers, const bool& includeMountedFolders,
                    const int& limit, SharedLink sharedLink) {

    QNetworkRequest req = prepareRequest("/files/list_folder");
    QVariantMap map;
    if (!sharedLink.isEmpty()) {
        map["shared_link"] = sharedLink.toMap();
    }
    if (limit != 0) {
        map["limit"] = limit;
    }
    map["path"] = path;
    map["include_media_info"] = includeMediaInfo;
    map["recursive"] = recursive;
    map["include_deleted"] = includeDeleted;
    map["include_has_explicit_shared_members"] = includeHasExplicitSharedMembers;
    map["include_mounted_folders"] = includeMountedFolders;

    QJson::Serializer serializer;

    QNetworkReply* reply = m_network.post(req, serializer.serialize(map));
    reply->setProperty("path", path);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onListFolderLoaded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onListFolderLoaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QVariantMap dataMap = data.toMap();
            QString cursor = dataMap.value("cursor").toString();
            bool hasMore = dataMap.value("has_more").toBool();
            QVariantList entries = dataMap.value("entries").toList();
            QList<QDropboxFile*> files;
            foreach(QVariant v, entries) {
                QDropboxFile* pFile = new QDropboxFile(this);
                pFile->fromMap(v.toMap());
                files.append(pFile);
            }
            emit listFolderLoaded(reply->property("path").toString(), files, cursor, hasMore);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::listFolderContinue(const QString& cursor) {
    QNetworkRequest req = prepareRequest("/files/list_folder/continue");
    QVariantMap map;
    map["cursor"] = cursor;

    QJson::Serializer serializer;

    QNetworkReply* reply = m_network.post(req, serializer.serialize(map));
    reply->setProperty("cursor", cursor);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onListFolderContinueLoaded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);

}

void QDropbox::onListFolderContinueLoaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QVariantMap dataMap = data.toMap();
            QString cursor = dataMap.value("cursor").toString();
            bool hasMore = dataMap.value("has_more").toBool();
            QVariantList entries = dataMap.value("entries").toList();
            QList<QDropboxFile*> files;
            foreach(QVariant v, entries) {
                QDropboxFile* pFile = new QDropboxFile(this);
                pFile->fromMap(v.toMap());
                files.append(pFile);
            }
            emit listFolderContinueLoaded(files, reply->property("cursor").toString(), cursor, hasMore);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::createFolder(const QString& path, const bool& autorename) {
    QNetworkRequest req = prepareRequest("/files/create_folder_v2");
    QVariantMap map;
    map["path"] = path;
    map["autorename"] = autorename;

    QJson::Serializer serializer;
    QNetworkReply* reply = m_network.post(req, serializer.serialize(map));
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onFolderCreated()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onFolderCreated() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QDropboxFile* pFolder = new QDropboxFile(this);
            pFolder->fromMap(data.toMap().value("metadata").toMap());
            pFolder->setTag("folder");
            emit folderCreated(pFolder);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::deleteFile(const QString& path) {
    QNetworkRequest req = prepareRequest("/files/delete_v2");
    QVariantMap map;
    map["path"] = path;

    QJson::Serializer serializer;
    QNetworkReply* reply = m_network.post(req, serializer.serialize(map));
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onFileDeleted()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onFileDeleted() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QDropboxFile* pFile = new QDropboxFile(this);
            pFile->fromMap(data.toMap().value("metadata").toMap());
            emit fileDeleted(pFile);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::move(const QString& fromPath, const QString& toPath, const bool& allowSharedFolder, const bool& autorename, const bool& allowOwnershipTransfer) {
    QNetworkReply* reply = moveFile(fromPath, toPath, allowSharedFolder, autorename, allowOwnershipTransfer);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onMoved()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onMoved() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QDropboxFile* pFile = new QDropboxFile(this);
            pFile->fromMap(data.toMap().value("metadata").toMap());
            emit moved(pFile);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::rename(const QString& fromPath, const QString& toPath, const bool& allowSharedFolder, const bool& autorename, const bool& allowOwnershipTransfer) {
    QNetworkReply* reply = moveFile(fromPath, toPath, allowSharedFolder, autorename, allowOwnershipTransfer);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onRenamed()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onRenamed() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QDropboxFile* pFile = new QDropboxFile(this);
            pFile->fromMap(data.toMap().value("metadata").toMap());
            emit renamed(pFile);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::getThumbnail(const QString& path, const QString& size, const QString& format) {
    if (!path.trimmed().isEmpty()) {
        QNetworkRequest req = prepareContentRequest("/files/get_thumbnail");

        QVariantMap map;

        QVariantMap sizeMap;
        sizeMap[".tag"] = size;
        map["size"] = sizeMap;
        map["path"] = path;
        map["format"] = format;

        QJson::Serializer serializer;
        QByteArray data = serializer.serialize(map);
        req.setRawHeader("Dropbox-API-Arg", data);

        logger.debug("Dropbox-API-Arg: " + data);

        QNetworkReply* reply = m_network.post(req, "");
        reply->setProperty("path", path);
        reply->setProperty("size", size);
        bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onThumbnailLoaded()));
        Q_ASSERT(res);
        res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        Q_ASSERT(res);
        Q_UNUSED(res);
    }

}

void QDropbox::onThumbnailLoaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QImage* thumbnail = new QImage();
        thumbnail->loadFromData(reply->readAll());
        emit thumbnailLoaded(reply->property("path").toString(), reply->property("size").toString(), thumbnail);
    }

    reply->deleteLater();
}

void QDropbox::download(const QString& path, const QString& rev) {
    QNetworkRequest req = prepareContentRequest("/files/download");

    QVariantMap map;
    map["path"] = path;
    if (!rev.isEmpty()) {
        map["rev"] = rev;
    }

    req.setRawHeader("Dropbox-API-Arg", QJson::Serializer().serialize(map));

    QNetworkReply* reply = m_network.post(req, "");
    reply->setReadBufferSize(m_readBufferSize);
    reply->setProperty("path", path);
    m_downloadsQueue.append(reply);

    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onDownloaded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(read()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
    emit downloadStarted(path);
}

void QDropbox::read() {
    QNetworkReply* reply = getReply();
    QString path = reply->property("path").toString();
    QFile file(m_downloadsFolder + "/" + getFilename(path));
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.flush();
    file.close();
}

void QDropbox::onDownloadProgress(qint64 loaded, qint64 total) {
    QNetworkReply* reply = getReply();
    emit downloadProgress(reply->property("path").toString(), loaded, total);
}

void QDropbox::onDownloaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QString path = reply->property("path").toString();
        QString filename = getFilename(path);
        QString localPath = m_downloadsFolder + "/" + filename;
        logger.debug("File downloaded: " + localPath);
        emit downloaded(path, localPath);
    }

    m_downloadsQueue.removeAll(reply);
    reply->deleteLater();
}

void QDropbox::upload(QFile* file, const QString& remotePath, const QString& mode, const bool& autorename, const bool& mute) {
    if (file->exists()) {
        QNetworkRequest req = prepareContentRequest("/files/upload");

        QVariantMap map;
        map["path"] = remotePath;
        map["mode"] = mode;
        map["autorename"] = autorename;
        map["mute"] = mute;

        QByteArray data = QJson::Serializer().serialize(map);
        logger.debug(data);
        req.setRawHeader("Dropbox-API-Arg", data);

        file->open(QIODevice::ReadOnly);
        QNetworkReply* reply = m_network.post(req, file);
        reply->setProperty("path", remotePath);
        file->setParent(reply);
        m_uploadsQueue.append(reply);

        bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onUploaded()));
        Q_ASSERT(res);
        res = QObject::connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
        Q_ASSERT(res);
        res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        Q_ASSERT(res);
        Q_UNUSED(res);
        emit uploadStarted(remotePath);
    } else {
        logger.error("File " + file->fileName() + " does not exists");
    }
}

void QDropbox::onUploaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QVariantMap map = data.toMap();
            map[".tag"] = FILE_TAG;
            QDropboxFile* file = new QDropboxFile(this);
            file->fromMap(map);
            logger.debug("File uploaded: " + file->getPathDisplay());
            emit uploaded(file);
        }
        delete res;
    }

    m_uploadsQueue.removeAll(reply);
    reply->deleteLater();
}

void QDropbox::onUploadProgress(qint64 loaded, qint64 total) {
    QNetworkReply* reply = getReply();
    emit uploadProgress(reply->property("path").toString(), loaded, total);
}

void QDropbox::addFolderMember(const QString& sharedFolderId, const QList<QDropboxMember>& members, const bool& quiet, const QString& customMessage) {
    QNetworkRequest req = prepareRequest("/sharing/add_folder_member");
    QVariantMap map;
    map["shared_folder_id"] = sharedFolderId;
    map["quiet"] = quiet;
    if (!customMessage.isEmpty()) {
        map["custom_message"] = customMessage;
    }
    QVariantList membersList;
    foreach(QDropboxMember m, members) {
        membersList.append(m.toMap());
    }
    map["members"] = membersList;

    QByteArray data = QJson::Serializer().serialize(map);
    logger.debug(data);

    QNetworkReply* reply = m_network.post(req, data);
    reply->setProperty("shared_folder_id", sharedFolderId);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onFolderMemberAdded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onFolderMemberAdded() {
    QNetworkReply* reply = getReply();
    emit folderMemberAdded(reply->property("shared_folder_id").toString());
    reply->deleteLater();
}

void QDropbox::listFolderMembers(const QString& sharedFolderId, const int& limit) {
    QNetworkRequest req = prepareRequest("/sharing/list_folder_members");
    QVariantMap map;
    map["shared_folder_id"] = sharedFolderId;
    if (limit != 0) {
        map["limit"] = limit;
    }

    QByteArray data = QJson::Serializer().serialize(map);
    logger.debug(data);

    QNetworkReply* reply = m_network.post(req, data);
    reply->setProperty("shared_folder_id", sharedFolderId);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onListFolderMembers()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onListFolderMembers() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        bool* res = new bool(false);
        QVariant data = QJson::Parser().parse(reply->readAll(), res);
        if (*res) {
            QVariantMap map = data.toMap();
            QList<QDropboxFolderMember*> members;
            QVariantList users = map.value("users").toList();
            foreach(QVariant v, users) {
                QDropboxFolderMember* m = new QDropboxFolderMember(this);
                m->fromMap(v.toMap());
                members.append(m);
            }
            emit listFolderMembersLoaded(reply->property("shared_folder_id").toString(), members, map.value("cursor", "").toString());
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::shareFolder(const QString& path, const bool& forceAsync, const QDropboxAclUpdatePolicy& aclUpdatePolicy,  const QDropboxMemberPolicy& memberPolicy,
            const QDropboxSharedLinkPolicy& sharedLinkPolicy, const QDropboxViewerInfoPolicy& viewerInfoPolicy,
            const QList<QDropboxFolderAction>& folderActions) {

    QNetworkRequest req = prepareRequest("/sharing/share_folder");
    QVariantMap map;
    map["path"] = path;
    map["force_async"] = forceAsync;
    if (aclUpdatePolicy.value() != QDropboxAclUpdatePolicy::NONE) {
        map["acl_update_policy"] = aclUpdatePolicy.name();
    }

    if (memberPolicy.value() != QDropboxMemberPolicy::NONE) {
        map["member_policy"] = memberPolicy.name();
    }

    if (sharedLinkPolicy.value() != QDropboxSharedLinkPolicy::NONE) {
        map["shared_link_policy"] = sharedLinkPolicy.name();
    }

    if (viewerInfoPolicy.value() != QDropboxViewerInfoPolicy::NONE) {
        map["viewer_info_policy"] = viewerInfoPolicy.name();
    }

    if (folderActions.size() > 0) {
        QVariantList actions;
        foreach(QDropboxFolderAction a, folderActions) {
            if (a.value() != QDropboxFolderAction::NONE) {
                QVariantMap aMap;
                aMap[".tag"] = a.name();
                actions.append(aMap);
            }
        }
        if (actions.size() > 0) {
            map["actions"] = actions;
        }
    }

    QByteArray data = QJson::Serializer().serialize(map);
    logger.debug(data);

    QNetworkReply* reply = m_network.post(req, data);
    reply->setProperty("path", path);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onFolderShared()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onFolderShared() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {

            // TODO: process full data in the future

            QVariantMap map = data.toMap();
            emit folderShared(reply->property("path").toString(), map.value("shared_folder_id").toString());
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::getAccount(const QString& accountId) {
    QNetworkRequest req = prepareRequest("/users/get_account");
    QVariantMap map;
    map["account_id"] = accountId;

    QJson::Serializer serializer;

    QNetworkReply* reply = m_network.post(req, serializer.serialize(map));
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onAccountLoaded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onAccountLoaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            Account* account = new Account(this);
            account->fromMap(data.toMap());
            emit accountLoaded(account);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::getCurrentAccount() {
    QNetworkRequest req = prepareRequest("/users/get_current_account");
    QNetworkReply* reply = m_network.post(req, "null");
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onCurrentAccountLoaded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onCurrentAccountLoaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            Account* account = new Account(this);
            account->fromMap(data.toMap());
            emit currentAccountLoaded(account);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::getSpaceUsage() {
    QNetworkRequest req = prepareRequest("/users/get_space_usage");
    QNetworkReply* reply = m_network.post(req, "null");
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onSpaceUsageLoaded()));
    Q_ASSERT(res);
    res = QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropbox::onSpaceUsageLoaded() {
    QNetworkReply* reply = getReply();

    if (reply->error() == QNetworkReply::NoError) {
        QJson::Parser parser;
        bool* res = new bool(false);
        QVariant data = parser.parse(reply->readAll(), res);
        if (*res) {
            QDropboxSpaceUsage* spaceUsage = new QDropboxSpaceUsage(this);
            spaceUsage->fromMap(data.toMap());
            emit spaceUsageLoaded(spaceUsage);
        }
        delete res;
    }

    reply->deleteLater();
}

void QDropbox::onError(QNetworkReply::NetworkError e) {
    QNetworkReply* reply = getReply();
    QString errorString = reply->errorString();
    logger.error(errorString);
    logger.error(e);
    if (reply->bytesAvailable()) {
        logger.error(reply->readAll());
    }
    emit error(e, errorString);
}

void QDropbox::init() {
    m_authUrl = "https://dropbox.com/oauth2";
    m_url = "https://api.dropboxapi.com";
    m_contentUrl = "https://content.dropboxapi.com";
    m_version = 2;
    m_redirectUri = "";
    m_appKey = "";
    m_appSecret = "";
    m_accessToken = "";
    m_downloadsFolder = QDir::currentPath() + "/downloads";
    m_readBufferSize = 5242880; // 5MB
    generateFullUrl();
    generateFullContentUrl();
}

void QDropbox::generateFullUrl() {
    m_fullUrl = QString(m_url).append("/").append(QString::number(m_version));
}

void QDropbox::generateFullContentUrl() {
    m_fullContentUrl = QString(m_contentUrl).append("/").append(QString::number(m_version));
}

QString QDropbox::getFilename(const QString& path) {
    return path.split("/").last();
}

QNetworkRequest QDropbox::prepareRequest(const QString& apiMethod) {
    QUrl url(m_fullUrl + apiMethod);

    QNetworkRequest req;
    req.setUrl(url);
    req.setRawHeader("Authorization", QString("Bearer ").append(m_accessToken).toUtf8());
    req.setRawHeader("Content-Type", "application/json");

    logger.debug(url);

    return req;
}

QNetworkRequest QDropbox::prepareContentRequest(const QString& apiMethod) {
    QUrl url(m_fullContentUrl + apiMethod);

    QNetworkRequest req;
    req.setUrl(url);
    req.setRawHeader("Authorization", QString("Bearer ").append(m_accessToken).toUtf8());
    req.setRawHeader("Content-Type", "application/octet-stream");

    logger.debug(url);

    return req;
}

QNetworkReply* QDropbox::getReply() {
    return qobject_cast<QNetworkReply*>(QObject::sender());
}

QNetworkReply* QDropbox::moveFile(const QString& fromPath, const QString& toPath, const bool& allowSharedFolder, const bool& autorename, const bool& allowOwnershipTransfer) {
    QNetworkRequest req = prepareRequest("/files/move_v2");
    QVariantMap map;
    map["from_path"] = fromPath;
    map["to_path"] = toPath;
    map["allow_shared_folder"] = allowSharedFolder;
    map["autorename"] = autorename;
    map["allow_ownership_transfer"] = allowOwnershipTransfer;

    QJson::Serializer serializer;
    return m_network.post(req, serializer.serialize(map));
}
