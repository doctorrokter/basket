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
#include <QVariantList>
#include "../qdropbox/QDropbox.hpp"
#include "../qdropbox/QDropboxFile.hpp"
#include "../qdropbox/Account.hpp"
#include "../qdropbox/QDropboxSpaceUsage.hpp"
#include "../Logger.hpp"

class QDropboxController: public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList selected READ getSelected WRITE setSelected NOTIFY selectedChanged)
public:
    QDropboxController(QDropbox* qdropbox, QObject* parent = 0);
    virtual ~QDropboxController();

    Q_INVOKABLE QString fullPath() const;
    Q_INVOKABLE QString topPath() const;
    Q_INVOKABLE QString popPath();
    Q_INVOKABLE void listFolder(const QString& path = "", const int& limit = 0);
    Q_INVOKABLE void listFolderContinue(const QString& cursor);
    Q_INVOKABLE void createFolder(const QString& path);
    Q_INVOKABLE void deleteFile(const QString& path);
    Q_INVOKABLE void move(const QString& fromPath, const QString& toPath);
    Q_INVOKABLE void rename(const QString& fromPath, const QString& toPath);
    Q_INVOKABLE void getCurrentAccount();
    Q_INVOKABLE void getSpaceUsage();

    Q_INVOKABLE const QVariantList& getSelected() const;
    Q_INVOKABLE void setSelected(const QVariantList& selected);
    Q_INVOKABLE void select(const QVariantMap& file);
    Q_INVOKABLE void unselectAll();

    Q_SIGNALS:
        void listFolderLoaded(const QString& path, const QVariantList& files, const QString& cursor, const bool& hasMore);
        void listFolderContinueLoaded(const QVariantList& files, const QString& prevCursor, const QString& cursor, const bool& hasMore);
        void folderCreated(const QVariantMap& folder);
        void fileDeleted(const QVariantMap& file);
        void moved(const QVariantMap& file);
        void renamed(const QVariantMap& file);
        void currentAccountLoaded(Account* account);
        void spaceUsageLoaded(const QVariantMap& spaceUsage);

        void selectedChanged(const QVariantList& selected);

private slots:
    void onListFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore);
    void onListFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore);
    void onFolderCreated(QDropboxFile* folder);
    void onFileDeleted(QDropboxFile* file);
    void onMoved(QDropboxFile* file);
    void onRenamed(QDropboxFile* file);
    void onSpaceUsageLoaded(QDropboxSpaceUsage* spaceUsage);

private:
    static Logger logger;

    QDropbox* m_pQDropbox;
    QStringList m_pathsList;
    QVariantList m_selected;

    void clear(QList<QDropboxFile*>& files);
};

#endif /* QDROPBOXCONTROLLER_HPP_ */
