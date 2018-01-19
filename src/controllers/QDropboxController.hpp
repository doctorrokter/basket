/*
 * QDropboxController.hpp
 *
 *  Created on: Nov 30, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXCONTROLLER_HPP_
#define QDROPBOXCONTROLLER_HPP_

#include <QObject>
#include <QStringList>
#include <QList>
#include <QVariantList>
#include <QMap>
#include <QtGui/QImage>
#include <qdropbox/QDropbox.hpp>
#include <qdropbox/QDropboxFile.hpp>
#include <qdropbox/QDropboxMember.hpp>
#include <qdropbox/QDropboxFolderMember.hpp>
#include <qdropbox/QDropboxTempLink.hpp>
#include <qdropbox/Account.hpp>
#include <qdropbox/QDropboxSpaceUsage.hpp>
#include <qdropbox/SharedLink.hpp>
#include "../Logger.hpp"
#include <bb/system/SystemToast>
#include "../util/FileUtil.hpp"
#include <QNetworkReply>
#include "../cache/QDropboxCache.hpp"
#include <bb/system/InvokeManager>

using namespace bb::system;

class QDropboxController: public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList selected READ getSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(QVariantList downloads READ getDownloads NOTIFY downloadsChanged)
    Q_PROPERTY(QVariantList uploads READ getUploads NOTIFY uploadsChanged)
public:
    QDropboxController(QDropbox* qdropbox, FileUtil* fileUtil, QDropboxCache* cache, QObject* parent = 0);
    virtual ~QDropboxController();

    Q_INVOKABLE QString fullPath() const;
    Q_INVOKABLE QString topPath() const;
    Q_INVOKABLE QString popPath();
    Q_INVOKABLE void listFolder(const QString& path = "", const int& limit = 0, const QString& orderBy = "name", const QString& order = "asc");
    Q_INVOKABLE void listFolderContinue(const QString& cursor);
    Q_INVOKABLE void createFolder(const QString& path);
    Q_INVOKABLE void deleteFile(const QString& path);
    Q_INVOKABLE void move(const QString& fromPath, const QString& toPath);
    Q_INVOKABLE void rename(const QString& fromPath, const QString& toPath);
    Q_INVOKABLE void getCurrentAccount();
    Q_INVOKABLE void getAccount(const QString& accountId);
    Q_INVOKABLE void getAccountBatch(const QVariantList& accountIds);
    Q_INVOKABLE void getSpaceUsage();
    Q_INVOKABLE void download(const QString& path);
    Q_INVOKABLE void downloadZip(const QString& path);
    Q_INVOKABLE void upload(const QString& localPath, const QString& remotePath);
    Q_INVOKABLE void shareFolder(const QString& path);
    Q_INVOKABLE void unshareFolder(const QString& path, const QString& sharedFolderId);
    Q_INVOKABLE void addFolderMember(const QString& sharedFolderId, const QVariantList& members, const int& accessLevel);
    Q_INVOKABLE void removeFolderMember(const QString& sharedFolderId, const QVariantMap& accountMap);
    Q_INVOKABLE void updateFolderMember(const QString& sharedFolderId, const QVariantMap& accountMap, const int& accessLevel);
    Q_INVOKABLE void listFolderMembers(const QString& sharedFolderId, const int& limit = 0);
    Q_INVOKABLE void createSharedLink(const QString& path);
    Q_INVOKABLE void getSharedLinks();
    Q_INVOKABLE void getTemporaryLink(const QString& path);
    Q_INVOKABLE void revokeSharedLink(const QString& sharedLinkUrl);
    Q_INVOKABLE void deleteBatch(const QVariantList& paths);
    Q_INVOKABLE void moveBatch(const QString& toPath);
    Q_INVOKABLE void saveUrl(const QString& path, const QString& url);

    Q_INVOKABLE const QVariantList& getSelected() const;
    Q_INVOKABLE void setSelected(const QVariantList& selected);
    Q_INVOKABLE void select(const QVariantMap& file);
    Q_INVOKABLE void unselectAll();

    Q_INVOKABLE const QVariantList& getDownloads() const;
    Q_INVOKABLE const QVariantList& getUploads() const;

    Q_SIGNALS:
        void listFolderLoaded(const QString& path, const QVariantList& files, const QString& cursor, const bool& hasMore);
        void listFolderContinueLoaded(const QVariantList& files, const QString& prevCursor, const QString& cursor, const bool& hasMore);
        void folderCreated(const QVariantMap& folder);
        void fileDeleted(const QVariantMap& file);
        void moved(const QVariantMap& file);
        void renamed(const QVariantMap& file);
        void currentAccountLoaded(Account* account);
        void accountLoaded(const QVariantMap& account);
        void accountBatchLoaded(const QVariantList& accounts);
        void spaceUsageLoaded(const QVariantMap& spaceUsage);
        void downloadsChanged(const QVariantList& downloads);
        void downloaded(const QString& path, const QString& localPath);
        void downloadProgress(const QString& path, qint64 loaded, qint64 total);
        void downloadStarted(const QString& path);
        void uploadsChanged(const QVariantList& uploads);
        void uploadStarted(const QString& remotePath);
        void uploaded(const QVariantMap& file);
        void uploadProgress(const QString& remotePath, qint64 loaded, qint64 total);
        void sharedFolder(const QString& path, const QString& sharedFolderId);
        void unsharedFolder(const QString& sharedFolderId);
        void folderMemberAdded(const QString& sharedFolderId);
        void listFolderMembersLoaded(const QString& sharedFolderId, const QVariantList& members, const QString& cursor);
        void sharedLinkCreated(const QVariantMap& link);
        void temporaryLinkLoaded(const QVariantMap& link);
        void folderMemberRemoved(const QString& sharedFolderId, const QVariantMap& member);
        void folderMemberUpdated(const QString& sharedFolderId, const QVariantMap& member);
        void sharedLinkRevoked(const QString& sharedLinkUrl);
        void deletedBatch();
        void movedBatch();
        void urlSaved();

        void selectedChanged(const QVariantList& selected);
        void error(const QString& error);

private slots:
    void onListFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore);
    void onListFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore);
    void onFolderCreated(QDropboxFile* folder);
    void onFileDeleted(QDropboxFile* file);
    void onMoved(QDropboxFile* file, const QString& fromPath, const QString& toPath);
    void onMovedBatch(const QList<MoveEntry>& moveEntries);
    void onRenamed(QDropboxFile* file);
    void onDeletedBatch(const QStringList& paths);
    void onSpaceUsageLoaded(QDropboxSpaceUsage* spaceUsage);
    void onDownloaded(const QString& path, const QString& localPath);
    void onDownloadStarted(const QString& path);
    void onUploaded(QDropboxFile* file);
    void onUploadStarted(const QString& remotePath);
    void onFolderShared(const QString& path, const QString& sharedFolderId);
    void onFolderUnshared(const UnshareJobStatus& status);
    void onListFolderMembers(const QString& sharedFolderId, const QList<QDropboxFolderMember*>& members, const QString& cursor = "");
    void onAccountBatchLoaded(const QList<Account*>& accounts);
    void onAccountLoaded(Account* account);
    void onSharedLinkCreated(SharedLink* link);
    void onTemporaryLinkLoaded(QDropboxTempLink* link);
    void onFolderMemberRemoved(const QString& sharedFolderId, QDropboxMember* member);
    void onFolderMemberUpdated(const QString& sharedFolderId, QDropboxMember* member);
    void onMetadataReceived(QDropboxFile* file);
    void onJobStatusChecked(const UnshareJobStatus& status);

    void onError(QNetworkReply::NetworkError e, const QString& errorString);
    void headlessInvoked();

private:
    static Logger logger;

    QDropbox* m_pQDropbox;
    FileUtil* m_pFileUtil;
    QDropboxCache* m_pCache;
    InvokeManager* m_invokeManager;

    QString m_orderBy;
    QString m_order;

    QStringList m_pathsList;
    QVariantList m_selected;
    QVariantList m_downloads;
    QVariantList m_uploads;

    SystemToast m_toast;
    QMap<QString, QString> m_sharedFolderIds;
    QMap<QString, UnshareJobStatus> m_jobStatuses;

    void clear(QList<QDropboxFile*>& files);
};

#endif /* QDROPBOXCONTROLLER_HPP_ */
