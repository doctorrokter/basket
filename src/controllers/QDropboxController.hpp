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
#include "../Logger.hpp"

class QDropboxController: public QObject {
    Q_OBJECT
public:
    QDropboxController(QDropbox* qdropbox, QObject* parent = 0);
    virtual ~QDropboxController();

    Q_INVOKABLE QString fullPath() const;
    Q_INVOKABLE QString topPath() const;
    Q_INVOKABLE QString popPath();
    Q_INVOKABLE void listFolder(const QString& path = "", const int& limit = 0);
    Q_INVOKABLE void getCurrentAccount();

    Q_SIGNALS:
        void listFolderLoaded(const QString& path, const QVariantList& files, const QString& cursor, const bool& hasMore);
        void currentAccountLoaded(Account* account);

private slots:
    void onListFolderLoaded(const QString& path, QList<QDropboxFile*>& files, const QString& cursor, const bool& hasMore);

private:
    static Logger logger;

    QDropbox* m_pQDropbox;
    QStringList m_pathsList;

    void clear(QList<QDropboxFile*>& files);
};

#endif /* QDROPBOXCONTROLLER_HPP_ */
