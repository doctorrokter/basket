/*
 * QDropboxController.cpp
 *
 *  Created on: Nov 30, 2017
 *      Author: doctorrokter
 */

#include "QDropboxController.hpp"

Logger QDropboxController::logger = Logger::getLogger("QDropboxController");

QDropboxController::QDropboxController(QDropbox* qdropbox, QObject* parent) : QObject(parent) {
    m_pQDropbox = qdropbox;

    bool res = QObject::connect(m_pQDropbox, SIGNAL(listFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)), this, SLOT(onListFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(currentAccountLoaded(Account*)), this, SIGNAL(currentAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQDropbox, SIGNAL(spaceUsageLoaded(QDropboxSpaceUsage*)), this, SLOT(onSpaceUsageLoaded(QDropboxSpaceUsage*)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

QDropboxController::~QDropboxController() {
    bool res = QObject::disconnect(m_pQDropbox, SIGNAL(listFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)), this, SLOT(onListFolderLoaded(const QString&, QList<QDropboxFile*>&, const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(currentAccountLoaded(Account*)), this, SIGNAL(currentAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQDropbox, SIGNAL(spaceUsageLoaded(QDropboxSpaceUsage*)), this, SLOT(onSpaceUsageLoaded(QDropboxSpaceUsage*)));
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

void QDropboxController::getCurrentAccount() {
    m_pQDropbox->getCurrentAccount();
}

void QDropboxController::getSpaceUsage() {
    m_pQDropbox->getSpaceUsage();
}

void QDropboxController::onSpaceUsageLoaded(QDropboxSpaceUsage* spaceUsage) {
    emit spaceUsageLoaded(spaceUsage->toMap());
    spaceUsage->deleteLater();
}

void QDropboxController::clear(QList<QDropboxFile*>& files) {
    foreach(QDropboxFile* f, files) {
        f->deleteLater();
    }
    files.clear();
}

