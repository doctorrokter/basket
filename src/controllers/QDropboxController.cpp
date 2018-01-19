/*
 * QDropboxController.cpp
 *
 *  Created on: Nov 30, 2017
 *      Author: doctorrokter
 */

#include <QFile>
#include <QDir>
#include <QDirIterator>
#include "QDropboxController.hpp"
#include <qdropbox/QDropboxAccessLevel.hpp>
#include <bb/system/InvokeQueryTargetsRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/data/JsonDataAccess>

#define THUMBNAILS_DIR "/data/thumbnails"
#define THUMBNAILS_THRESHOLD 500
#define THUMBNAILS_QUEUE_SIZE 5

using namespace bb::data;


Logger QDropboxController::logger = Logger::getLogger("QDropboxController");

QDropboxController::QDropboxController(QDropbox* qdropbox, FileUtil* fileUtil, QDropboxCache* cache, QObject* parent) : QObject(parent), m_invokeManager(new InvokeManager(this)) {
    m_pQDropbox = qdropbox;
    m_pFileUtil = fileUtil;
    m_pCache = cache;
    m_orderBy = "name";
    m_order = "asc";

    bool res = QObject::connect(m_pQDropbox, SIGNAL(listFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)), this, SLOT(onListFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(listFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)), this, SLOT(onListFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderCreated(QDropboxFile*)), this, SLOT(onFolderCreated(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(fileDeleted(QDropboxFile*)), this, SLOT(onFileDeleted(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(moved(QDropboxFile*, const QString&, const QString&)), this, SLOT(onMoved(QDropboxFile*, const QString&, const QString&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(renamed(QDropboxFile*)), this, SLOT(onRenamed(QDropboxFile*)));
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
    res = QObject::connect(m_pQDropbox, SIGNAL(deletedBatch(const QStringList&)), this, SLOT(onDeletedBatch(const QStringList&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(urlSaved()), this, SIGNAL(urlSaved()));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(movedBatch(const QList<MoveEntry>&)), this, SLOT(onMovedBatch(const QList<MoveEntry>&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(metadataReceived(QDropboxFile*)), this, SLOT(onMetadataReceived(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(folderUnshared(const UnshareJobStatus&)), this, SLOT(onFolderUnshared(const UnshareJobStatus&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(jobStatusChecked(const UnshareJobStatus&)), this, SLOT(onJobStatusChecked(const UnshareJobStatus&)));
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
    res = QObject::disconnect(m_pQDropbox, SIGNAL(moved(QDropboxFile*, const QString&, const QString&)), this, SLOT(onMoved(QDropboxFile*, const QString&, const QString&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(renamed(QDropboxFile*)), this, SLOT(onRenamed(QDropboxFile*)));
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
    res = QObject::disconnect(m_pQDropbox, SIGNAL(deletedBatch(const QStringList&)), this, SLOT(onDeletedBatch(const QStringList&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(urlSaved()), this, SIGNAL(urlSaved()));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(movedBatch(const QList<MoveEntry>&)), this, SLOT(onMovedBatch(const QList<MoveEntry>&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(metadataReceived(QDropboxFile*)), this, SLOT(onMetadataReceived(QDropboxFile*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(folderUnshared(const UnshareJobStatus&)), this, SLOT(onFolderUnshared(const UnshareJobStatus&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(jobStatusChecked(const UnshareJobStatus&)), this, SLOT(onJobStatusChecked(const UnshareJobStatus&)));
    Q_ASSERT(res);
    Q_UNUSED(res);
    m_invokeManager->deleteLater();
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

void QDropboxController::listFolder(const QString& path, const int& limit, const QString& orderBy, const QString& order) {
    m_pathsList.append(path);
    m_orderBy = orderBy;
    m_order = order;
    Cache cache = m_pCache->findForPath(path, m_orderBy, m_order);
    if (!cache.isEmpty()) {
        logger.debug("Path " + path + " loaded from cache");
        listFolderLoaded(path, cache.files, cache.cursor, false);
    } else {
        m_pQDropbox->listFolder(path, true, false, false, false, false, limit);
    }
}

void QDropboxController::onListFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore) {
    if (m_pathsList.last().compare(path) == 0) {
//        QVariantList list;
//        foreach(QDropboxFile* f, files) {
//            list.append(f->toMap());
//        }
//        m_pCache->updateByPath(path, files, cursor);
//        emit listFolderLoaded(path, list, cursor, hasMore);
        m_pCache->updateByPath(path, files, cursor);
        clear(files);
        if (hasMore) {
            listFolderContinue(cursor);
        } else {
            Cache cache = m_pCache->findForPath(path, m_orderBy, m_order);
            emit listFolderLoaded(path, cache.files, cache.cursor, hasMore);
        }
    }
//    clear(files);
}

void QDropboxController::listFolderContinue(const QString& cursor) {
    m_pQDropbox->listFolderContinue(cursor);
}

void QDropboxController::onListFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore) {
//    QVariantList list;
//    foreach(QDropboxFile* f, files) {
//        list.append(f->toMap());
//    }
//    m_pCache->updateByCursor(prevCursor, files, cursor);
//    emit listFolderContinueLoaded(list, prevCursor, cursor, hasMore);
//    clear(files);
    m_pCache->updateByCursor(prevCursor, files, cursor);
    clear(files);
    if (hasMore) {
        listFolderContinue(cursor);
    } else {
        Cache cache = m_pCache->findForCursor(cursor, m_orderBy, m_order);
        emit listFolderLoaded(cache.path, cache.files, cursor, false);
    }
//    clear(files);
}

void QDropboxController::createFolder(const QString& path) {
    m_pQDropbox->createFolder(path, false);
}

void QDropboxController::onFolderCreated(QDropboxFile* folder) {
    m_pCache->add(folder);
    emit folderCreated(folder->toMap());
    folder->deleteLater();
}

void QDropboxController::deleteFile(const QString& path) {
    m_pQDropbox->deleteFile(path);
}

void QDropboxController::onFileDeleted(QDropboxFile* file) {
    m_pCache->remove(file);
    emit fileDeleted(file->toMap());
    file->deleteLater();
}

void QDropboxController::move(const QString& fromPath, const QString& toPath) {
    m_pQDropbox->move(fromPath, toPath);
}

void QDropboxController::rename(const QString& fromPath, const QString& toPath) {
    m_pQDropbox->rename(fromPath, toPath);
}

void QDropboxController::onMoved(QDropboxFile* file, const QString& fromPath, const QString& toPath) {
    m_pCache->update(file);
    emit moved(file->toMap());
    m_selected.clear();
    emit selectedChanged(m_selected);
    file->deleteLater();
    Q_UNUSED(fromPath);
    Q_UNUSED(toPath);
}

void QDropboxController::onRenamed(QDropboxFile* file) {
    m_pCache->update(file);
    emit renamed(file->toMap());
    file->deleteLater();
}

void QDropboxController::onDeletedBatch(const QStringList& paths) {
    m_pCache->deleteByPaths(paths);
    emit deletedBatch();
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

const QVariantList& QDropboxController::getDownloads() const {
    return m_downloads;
}

const QVariantList& QDropboxController::getUploads() const {
    return m_uploads;
}

void QDropboxController::download(const QString& path) {
    m_pQDropbox->download(path);
}

void QDropboxController::downloadZip(const QString& path) {
    m_pQDropbox->downloadZip(path);
}

void QDropboxController::upload(const QString& localPath, const QString& remotePath) {
    logger.debug("Upload: " + localPath);
    QString filename = m_pFileUtil->filename(localPath);
    QFile* file = new QFile(localPath, this);
    m_pQDropbox->upload(file, remotePath + "/" + filename);
}

void QDropboxController::onUploaded(QDropboxFile* file) {
    m_pCache->add(file);
    QString path = file->getPathDisplay();
    m_uploads.removeAll(path);
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
    m_pQDropbox->getMetadata(path);
    emit sharedFolder(path, sharedFolderId);
}

void QDropboxController::unshareFolder(const QString& path, const QString& sharedFolderId) {
    m_sharedFolderIds[sharedFolderId] = path;
    m_pQDropbox->unshareFolder(sharedFolderId);
}

void QDropboxController::onFolderUnshared(const UnshareJobStatus& status) {
    if (status.status == UnshareJobStatus::Complete) {
        m_pQDropbox->getMetadata(m_sharedFolderIds.value(status.sharedFolderId));
        m_sharedFolderIds.remove(status.sharedFolderId);
    } else {
        InvokeRequest request;
        request.setTarget("chachkouski.BasketService");
        request.setAction("chachkouski.BasketService.CHECK_JOB_STATUS");
        request.setMimeType("text/plain");

        QVariantMap map;
        map["path"] = m_sharedFolderIds[status.sharedFolderId];
        map["status"] = status.toMap();

        QByteArray data;
        QDataStream os(&data, QIODevice::WriteOnly);
        os << map;
        request.setData(data);

        InvokeTargetReply* reply = m_invokeManager->invoke(request);
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(headlessInvoked()));
        m_sharedFolderIds.remove(status.sharedFolderId);
//        m_jobStatuses[status.asyncJobId] = status;
//        m_pQDropbox->checkJobStatus(status.asyncJobId);
    }

    emit unsharedFolder(status.sharedFolderId);
}

void QDropboxController::headlessInvoked() {
    InvokeTargetReply* reply = qobject_cast<InvokeTargetReply*>(QObject::sender());
    logger.info("Invoked headless success: ");
    logger.info(reply->error());
    reply->deleteLater();
}
void QDropboxController::onJobStatusChecked(const UnshareJobStatus& status) {
    if (status.status == UnshareJobStatus::Complete) {
        UnshareJobStatus oldStatus = m_jobStatuses.value(status.asyncJobId);
        m_pQDropbox->getMetadata(m_sharedFolderIds.value(oldStatus.sharedFolderId));
        m_sharedFolderIds.remove(oldStatus.sharedFolderId);
        m_jobStatuses.remove(status.asyncJobId);
    } else {
        m_pQDropbox->checkJobStatus(status.asyncJobId);
    }
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

void QDropboxController::deleteBatch(const QVariantList& paths) {
    QStringList list;
    foreach(QVariant path, paths) {
        list << path.toString();
    }
    m_pQDropbox->deleteBatch(list);
}

void QDropboxController::moveBatch(const QString& toPath) {
    QList<MoveEntry> entries;
    foreach(QVariant v, m_selected) {
        QVariantMap file = v.toMap();
        QString newPath = toPath + "/" + file.value("name").toString();
        MoveEntry e(file.value("path_display").toString(), newPath);
        entries.append(e);

        file["path_display"] = newPath;
        file["path_lower"] = newPath.toLower();
        emit moved(file);
    }
    unselectAll();
    m_pQDropbox->moveBatch(entries);
}

void QDropboxController::onMovedBatch(const QList<MoveEntry>& moveEntries) {
    m_pCache->move(moveEntries);
    emit movedBatch();
}

void QDropboxController::saveUrl(const QString& path, const QString& url) {
    m_pQDropbox->saveUrl(path, url);
}

void QDropboxController::onMetadataReceived(QDropboxFile* file) {
    logger.debug(file->toMap());
    m_pCache->update(file);
    file->deleteLater();
}
