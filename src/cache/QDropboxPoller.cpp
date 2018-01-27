/*
 * QDropboxPoller.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: doctorrokter
 */

#include "QDropboxPoller.hpp"
#include <QMap>

Logger QDropboxPoller::logger = Logger::getLogger("QDropboxPoller");

QDropboxPoller::QDropboxPoller(QDropbox* qdropbox, QDropboxCache* cache, QObject* parent) : QObject(parent), m_pQDropbox(qdropbox), m_pCache(cache) {
    bool res = QObject::connect(m_pQDropbox, SIGNAL(listFolderLongPollFinished(const QString&, const bool&)), this, SLOT(onLongPoll(const QString&, const bool&)));
    Q_ASSERT(res);
    m_busy = false;

    m_timer.setInterval(60000);
    res = QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(poll()));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

QDropboxPoller::~QDropboxPoller() {
    m_timer.stop();
    bool res = QObject::disconnect(m_pQDropbox, SIGNAL(listFolderLongPollFinished(const QString&, const bool&)), this, SLOT(onLongPoll(const QString&, const bool&)));
    Q_ASSERT(res);
    res = QObject::disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(poll()));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void QDropboxPoller::stop() {
    m_timer.stop();
}

void QDropboxPoller::start() {
    poll();
    m_timer.start();
}

void QDropboxPoller::onLongPoll(const QString& cursor, const bool& changes) {
    m_busy = false;

    if (changes) {
        bool res = QObject::connect(m_pQDropbox, SIGNAL(listFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)), this, SLOT(onListFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)));
        Q_ASSERT(res);
        Q_UNUSED(res);
        m_pQDropbox->listFolderContinue(cursor);
    } else {
        m_queue.dequeue();
        processQueue();
    }
}

void QDropboxPoller::poll() {
    QMap<QString, QString> pathsCursors = m_pCache->getPathsCursors();
    foreach(QString path, pathsCursors.keys()) {
        if (!m_queue.contains(pathsCursors.value(path))) {
            m_queue.enqueue(pathsCursors.value(path));
            processQueue();
        }
    }
}

void QDropboxPoller::processQueue() {
    if (!m_busy && m_queue.size()) {
        m_busy = true;
        m_pQDropbox->listFolderLongPoll(m_queue.head());
    }
}

void QDropboxPoller::onListFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore) {
    Q_UNUSED(hasMore);
    if (prevCursor.compare(m_queue.head()) == 0) {
        bool res = QObject::disconnect(m_pQDropbox, SIGNAL(listFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)), this, SLOT(onListFolderContinueLoaded(QList<QDropboxFile*>&, const QString&, const QString&, const bool&)));
        Q_ASSERT(res);
        Q_UNUSED(res);

        QStringList toDelete;
        foreach(QDropboxFile* file, files) {
            logger.debug(file->toMap());
            if (file->getTag().compare("deleted") == 0) {
                toDelete.append(file->getPathDisplay());
            } else {
                m_pCache->add(file);
            }

            file->deleteLater();
        }
        files.clear();

        if (toDelete.size()) {
            m_pCache->deleteByPaths(toDelete);
        }

        QString path = m_pCache->findPath(prevCursor);
        m_pCache->updatePathsCursors(path, cursor);
        emit pathChanged(path);

        m_queue.dequeue();
        processQueue();
    }

}
