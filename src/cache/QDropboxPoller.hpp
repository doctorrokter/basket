/*
 * QDropboxPoller.hpp
 *
 *  Created on: Jan 20, 2018
 *      Author: doctorrokter
 */

#ifndef QDROPBOXPOLLER_HPP_
#define QDROPBOXPOLLER_HPP_

#include <qdropbox/QDropbox.hpp>
#include "QDropboxCache.hpp"
#include <QTimer>
#include "../Logger.hpp"
#include <QQueue>

class QDropboxPoller: public QObject {
    Q_OBJECT
public:
    QDropboxPoller(QDropbox* qdropbox, QDropboxCache* cache, QObject* parent = 0);
    virtual ~QDropboxPoller();

    void stop();

    Q_SIGNALS:
        void pathChanged(const QString& path);

public slots:
    void start();

private slots:
    void onLongPoll(const QString& cursor, const bool& changes);
    void poll();
    void onListFolderContinueLoaded(QList<QDropboxFile*>& files, const QString& prevCursor, const QString& cursor, const bool& hasMore);

private:
    static Logger logger;

    QTimer m_timer;
    QDropbox* m_pQDropbox;
    QDropboxCache* m_pCache;

    QQueue<QString> m_queue;
    bool m_busy;

    void processQueue();
};

#endif /* QDROPBOXPOLLER_HPP_ */
