/*
 * QDropboxController.cpp
 *
 *  Created on: Nov 30, 2017
 *      Author: doctorrokter
 */

#include <QFile>
#include <QDir>
#include "QDropboxController.hpp"
#include "../qdropbox/QDropboxAccessLevel.hpp"

Logger QDropboxController::logger = Logger::getLogger("QDropboxController");

QDropboxController::QDropboxController(QDropbox* qdropbox, FileUtil* fileUtil, QObject* parent) : QObject(parent) {
    m_pQDropbox = qdropbox;
    m_pFileUtil = fileUtil;

    bool res = QObject::connect(m_pQDropbox, SIGNAL(listFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)), this, SLOT(onListFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(listFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)), this, SLOT(onListFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderCreated(QDropboxFile*)), this, SLOT(onFolderCreated(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(fileDeleted(QDropboxFile*)), this, SLOT(onFileDeleted(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(moved(QDropboxFile*)), this, SLOT(onMoved(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(renamed(QDropboxFile*)), this, SLOT(onRenamed(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(thumbnailLoaded(const QString&, const QString&, QImage*)), this, SLOT(onThumbnailLoaded(const QString&, const QString&, QImage*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(downloaded(const QString&, const QString&)), this, SLOT(onDownloaded(const QString&, const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(downloadProgress(const QString&, qint64, qint64)), this, SIGNAL(downloadProgress(const QString&, qint64, qint64)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(downloadStarted(const QString&)), this, SLOT(onDownloadStarted(const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(uploaded(QDropboxFile*)), this, SLOT(onUploaded(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(uploadProgress(const QString&, qint64, qint64)), this, SIGNAL(uploadProgress(const QString&, qint64, qint64)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(uploadStarted(const QString&)), this, SLOT(onUploadStarted(const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(currentAccountLoaded(Account*)), this, SIGNAL(currentAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(spaceUsageLoaded(QDropboxSpaceUsage*)), this, SLOT(onSpaceUsageLoaded(QDropboxSpaceUsage*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(error(QNetworkReply::NetworkError, const QString&)), this, SLOT(onError(QNetworkReply::NetworkError, const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderShared(const QString&, const QString&)), this, SLOT(onFolderShared(const QString&, const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderMemberAdded(const QString&)), this, SIGNAL(folderMemberAdded(const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(listFolderMembersLoaded(const QString&, const QList<QDropboxFolderMember*>&, const QString&)), this, SLOT(onListFolderMembers(const QString&, const QList<QDropboxFolderMember*>&, const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(accountBatchLoaded(const QList<Account*>)), this, SLOT(onAccountBatchLoaded(const QList<Account*>)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(accountLoaded(Account*)), this, SLOT(onAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderUnshared(const QString&)), this, SIGNAL(unsharedFolder(const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(sharedLinkCreated(SharedLink*)), this, SLOT(onSharedLinkCreated(SharedLink*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(temporaryLinkLoaded(QDropboxTempLink*)), this, SLOT(onTemporaryLinkLoaded(QDropboxTempLink*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderMemberRemoved(const QString&, QDropboxMember*)), this, SLOT(onFolderMemberRemoved(const QString&, QDropboxMember*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderMemberUpdated(const QString&, QDropboxMember*)), this, SLOT(onFolderMemberUpdated(const QString&, QDropboxMember*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(sharedLinkRevoked(const QString&)), this, SIGNAL(sharedLinkRevoked(const QString&)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

QDropboxController::~QDropboxController() {
    bool res = QObject::disconnect(m_pQDropbox, SIGNAL(listFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)), this, SLOT(onListFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(listFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)), this, SLOT(onListFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderCreated(QDropboxFile*)), this, SLOT(onFolderCreated(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(fileDeleted(QDropboxFile*)), this, SLOT(onFileDeleted(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(moved(QDropboxFile*)), this, SLOT(onMoved(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(renamed(QDropboxFile*)), this, SLOT(onRenamed(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(thumbnailLoaded(const QString&, const QString&, QImage*)), this, SLOT(onThumbnailLoaded(const QString&, const QString&, QImage*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(downloaded(const QString&, const QString&)), this, SLOT(onDownloaded(const QString&, const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(downloadProgress(const QString&, qint64, qint64)), this, SIGNAL(downloadProgress(const QString&, qint64, qint64)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(downloadStarted(const QString&)), this, SLOT(onDownloadStarted(const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(uploaded(QDropboxFile*)), this, SLOT(onUploaded(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(uploadProgress(const QString&, qint64, qint64)), this, SIGNAL(uploadProgress(const QString&, qint64, qint64)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(uploadStarted(const QString&)), this, SLOT(onUploadStarted(const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(currentAccountLoaded(Account*)), this, SIGNAL(currentAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(spaceUsageLoaded(QDropboxSpaceUsage*)), this, SLOT(onSpaceUsageLoaded(QDropboxSpaceUsage*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(error(QNetworkReply::NetworkError, const QString&)), this, SLOT(onError(QNetworkReply::NetworkError, const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderShared(const QString&, const QString&)), this, SLOT(onFolderShared(const QString&, const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderMemberAdded(const QString&)), this, SIGNAL(folderMemberAdded(const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(listFolderMembersLoaded(const QString&, const QList<QDropboxFolderMember>&, const QString&)), this, SLOT(onListFolderMembers(const QString&, const QList<QDropboxFolderMember>&, const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(accountBatchLoaded(const QList<Account*>)), this, SLOT(onAccountBatchLoaded(const QList<Account*>)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(accountLoaded(Account*)), this, SLOT(onAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderUnshared(const QString&)), this, SIGNAL(unsharedFolder(const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(sharedLinkCreated(SharedLink*)), this, SLOT(onSharedLinkCreated(SharedLink*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(temporaryLinkLoaded(QDropboxTempLink*)), this, SLOT(onTemporaryLinkLoaded(QDropboxTempLink*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderMemberRemoved(const QString&, QDropboxMember*)), this, SLOT(onFolderMemberRemoved(const QString&, QDropboxMember*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderMemberUpdated(const QString&, QDropboxMember*)), this, SLOT(onFolderMemberUpdated(const QString&, QDropboxMember*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(sharedLinkRevoked(const QString&)), this, SIGNAL(sharedLinkRevoked(const QString&)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

QString QDropboxController::fullPath() const {
    return m_pathsList.join("/");
}

QString QDropboxController::topPath() const {
    return m_pathsList.last();
}

QString QDropboxController::popPath() {
    QString last = m_pathsList.last();
    m_pathsList.removeLast();
    return last;
}

void QDropboxController::listFolder(const QString& path, const int& limit) {
    m_pathsList.append(path);
    m_pQDropbox->listFolder(path, true, false, false, false, false, limit);
}

void QDropboxController::onListFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore) {
    if (m_pathsList.last().compare(path) == 0) {
        logger.debug("Loaded for path: " + path);
        logger.debug("Files: " + QString::number(files.size()));
        QVariantList list;
        foreach(QDropboxFile* f, files) {
            list.append(f->toMap());
        }
        emit listFolderLoaded(path, list, cursor, hasMore);
    }
    clear(files);
}

void QDropboxController::listFolderContinue(const QString& cursor) {
    m_pQDropbox->listFolderContinue(cursor);
}

void QDropboxController::onListFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore) {
    QVariantList list;
    foreach(QDropboxFile* f, files) {
        list.append(f->toMap());
    }
    emit listFolderContinueLoaded(list, prevCursor, cursor, hasMore);
    clear(files);
}

void QDropboxController::createFolder(const QString& path) {
    m_pQDropbox->createFolder(path, false);
}

void QDropboxController::onFolderCreated(QDropboxFile* folder) {
    emit folderCreated(folder->toMap());
    folder->deleteLater();
}

void QDropboxController::deleteFile(const QString& path) {
    m_pQDropbox->deleteFile(path);
}

void QDropboxController::onFileDeleted(QDropboxFile* file) {
    emit fileDeleted(file->toMap());
    file->deleteLater();
}

void QDropboxController::move(const QString& fromPath, const QString& toPath) {
    m_pQDropbox->move(fromPath, toPath);
}

void QDropboxController::rename(const QString& fromPath, const QString& toPath) {
    m_pQDropbox->rename(fromPath, toPath);
}

void QDropboxController::onMoved(QDropboxFile* file) {
    emit moved(file->toMap());
    m_selected.clear();
    emit selectedChanged(m_selected);
    file->deleteLater();
}

void QDropboxController::onRenamed(QDropboxFile* file) {
    emit renamed(file->toMap());
    file->deleteLater();
}

void QDropboxController::getCurrentAccount() {
    m_pQDropbox->getCurrentAccount();
}

void QDropboxController::getAccountBatch(const QVariantList& accountIds) {
    QStringList ids;
    foreach(QVariant v, accountIds) {
        ids.append(v.toString());
    }
    m_pQDropbox->getAccountBatch(ids);
}

void QDropboxController::onAccountBatchLoaded(const QList<Account*>& accounts) {
    QVariantList list;
    foreach(Account* account, accounts) {
        list.append(account->toMap());
        account->deleteLater();
    }
    emit accountBatchLoaded(list);
}

void QDropboxController::getSpaceUsage() {
    m_pQDropbox->getSpaceUsage();
}

void QDropboxController::getThumbnail(const QString& path, const QString& size) {
    QString thumbs = QDir::currentPath() + "/data/thumbnails";
    QDir dir(thumbs);
    if (!dir.exists()) {
        m_pQDropbox->getThumbnail(path, "w128h128");
    } else {
        QString localPath = thumbs + "/" + size + "_" + path.split("/").last();
        if (QFile::exists(localPath)) {
            emit thumbnailLoaded(path, localPath);
        } else {
            m_pQDropbox->getThumbnail(path, size);
        }
    }
}

void QDropboxController::onThumbnailLoaded(const QString& path, const QString& size, QImage* thumbnail) {

    QString thumbs = QDir::currentPath() + "/data/thumbnails";
    QDir dir(thumbs);
    if (!dir.exists()) {
        dir.mkpath(thumbs);
    }

    QString localPath = thumbs + "/" + size + "_" + path.split("/").last();
    QFile file(localPath);
    file.open(QIODevice::WriteOnly);

    thumbnail->save(&file);

    delete thumbnail;
    thumbnail = 0;

    emit thumbnailLoaded(path, localPath);
}

const QVariantList& QDropboxController::getDownloads() const {
    return m_downloads;
}

const QVariantList& QDropboxController::getUploads() const {
    return m_uploads;
}

void QDropboxController::download(const QString& path) {
    m_pQDropbox->download(path);
}

void QDropboxController::upload(const QString& localPath, const QString& remotePath) {
    logger.debug("Upload: " + localPath);
    QString filename = m_pFileUtil->filename(localPath);
    QFile* file = new QFile(localPath, this);
    m_pQDropbox->upload(file, remotePath + "/" + filename);
}

void QDropboxController::onUploaded(QDropboxFile* file) {
    QString path = file->getPathDisplay();
    m_downloads.removeAll(path);
    emit uploadsChanged(m_uploads);
    emit uploaded(file->toMap());
    m_toast.setBody(tr("Upload finished: ") + path);
    m_toast.show();
    file->deleteLater();
}

void QDropboxController::onUploadStarted(const QString& remotePath) {
    m_uploads.append(remotePath);
    emit uploadsChanged(m_uploads);
    emit uploadStarted(remotePath);
}

void QDropboxController::shareFolder(const QString& path) {
    m_pQDropbox->shareFolder(path);
}

void QDropboxController::onFolderShared(const QString& path, const QString& sharedFolderId) {
    emit sharedFolder(path, sharedFolderId);
}

void QDropboxController::unshareFolder(const QString& sharedFolderId) {
    m_pQDropbox->unshareFolder(sharedFolderId);
}

void QDropboxController::addFolderMember(const QString& sharedFolderId, const QVariantList& members, const int& accessLevel) {
    QList<QDropboxMember> membersList;
    foreach(QVariant v, members) {
        QDropboxAccessLevel level;
        level.valueFromInt(accessLevel);

        QDropboxMember member;
        member
            .setEmail(v.toString())
            .setAccessLevel(level);
        membersList.append(member);
    }
    m_pQDropbox->addFolderMember(sharedFolderId, membersList);
}

void QDropboxController::listFolderMembers(const QString& sharedFolderId, const int& limit) {
    m_pQDropbox->listFolderMembers(sharedFolderId, limit);
}

void QDropboxController::onListFolderMembers(const QString& sharedFolderId, const QList<QDropboxFolderMember*>& members, const QString& cursor) {
    QVariantList membersList;
    foreach(QDropboxFolderMember* m, members) {
        membersList.append(m->toMap());
        m->deleteLater();
    }
    emit listFolderMembersLoaded(sharedFolderId, membersList, cursor);
}

void QDropboxController::createSharedLink(const QString& path) {
    m_pQDropbox->createSharedLink(path);
}

void QDropboxController::onSharedLinkCreated(SharedLink* link) {
    emit sharedLinkCreated(link->toMap());
}

void QDropboxController::getSharedLinks() {
    m_pQDropbox->getSharedLinks();
}

void QDropboxController::onSpaceUsageLoaded(QDropboxSpaceUsage* spaceUsage) {
    emit spaceUsageLoaded(spaceUsage->toMap());
    spaceUsage->deleteLater();
}

const QVariantList& QDropboxController::getSelected() const {
    return m_selected;
}

void QDropboxController::setSelected(const QVariantList& selected) {
    m_selected = selected;
    emit selectedChanged(m_selected);
}

void QDropboxController::select(const QVariantMap& file) {
    m_selected.append(file);
    emit selectedChanged(m_selected);
}

void QDropboxController::unselectAll() {
    m_selected.clear();
    emit selectedChanged(m_selected);
}

void QDropboxController::clear(QList<QDropboxFile*>& files) {
    foreach(QDropboxFile* f, files) {
        f->deleteLater();
    }
    files.clear();
}

void QDropboxController::onDownloaded(const QString& path, const QString& localPath) {
    m_downloads.removeAll(path);
    emit downloadsChanged(m_downloads);
    emit downloaded(path, localPath);
    m_toast.setBody(tr("Download finished: ") + "/downloads/basket/" + m_pFileUtil->filename(localPath));
    m_toast.show();
}

void QDropboxController::onDownloadStarted(const QString& path) {
    m_downloads.append(path);
    emit downloadsChanged(m_downloads);
    emit downloadStarted(path);
}

void QDropboxController::onError(QNetworkReply::NetworkError e, const QString& errorString) {
    Q_UNUSED(e);
    m_toast.setBody(errorString);
    m_toast.show();
    emit error(errorString);
}

void QDropboxController::getAccount(const QString& accountId) {
    m_pQDropbox->getAccount(accountId);
}

void QDropboxController::onAccountLoaded(Account* account) {
    emit accountLoaded(account->toMap());
    account->deleteLater();
}

void QDropboxController::getTemporaryLink(const QString& path) {
    m_pQDropbox->getTemporaryLink(path);
}

void QDropboxController::onTemporaryLinkLoaded(QDropboxTempLink* link) {
    emit temporaryLinkLoaded(link->toMap());
    link->deleteLater();
}

void QDropboxController::removeFolderMember(const QString& sharedFolderId, const QVariantMap& accountMap) {
    QDropboxMember m;
    m.setEmail(accountMap.value("email", "").toString());
    m.setDropboxId(accountMap.value("account_id", "").toString());
    m_pQDropbox->removeFolderMember(sharedFolderId, m);
}

void QDropboxController::onFolderMemberRemoved(const QString& sharedFolderId, QDropboxMember* member) {
    emit folderMemberRemoved(sharedFolderId, member->toMap());
    member->deleteLater();
}

void QDropboxController::updateFolderMember(const QString& sharedFolderId, const QVariantMap& accountMap, const int& accessLevel) {
    QDropboxMember m;
    m.setEmail(accountMap.value("email", "").toString());
    m.setDropboxId(accountMap.value("account_id", "").toString());

    QDropboxAccessLevel level;
    level.valueFromInt(accessLevel);
    m.setAccessLevel(level);

    m_pQDropbox->updateFolderMember(sharedFolderId, m);
}

void QDropboxController::onFolderMemberUpdated(const QString& sharedFolderId, QDropboxMember* member) {
    emit folderMemberUpdated(sharedFolderId, member->toMap());
    member->deleteLater();
}

void QDropboxController::revokeSharedLink(const QString& sharedLinkUrl) {
    m_pQDropbox->revokeSharedLink(sharedLinkUrl);
}

