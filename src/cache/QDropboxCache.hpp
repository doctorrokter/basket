/*
 * QDropboxCache.hpp
 *
 *  Created on: Jan 9, 2018
 *      Author: doctorrokter
 */

#ifndef QDROPBOXCACHE_HPP_
#define QDROPBOXCACHE_HPP_

#include <QObject>
#include <QList>
#include <QFile>
#include <QVariantList>
#include "../../include/qdropbox/QDropboxFile.hpp"
#include <QVariantMap>
#include "../../include/qdropbox/Logger.hpp"
#include <qdropbox/QDropbox.hpp>

struct Cache {
    QString path;
    QString cursor;
    QVariantList files;

    bool isEmpty() {
        return files.empty();
    }
};

class QDropboxCache: public QObject {
    Q_OBJECT
public:
    QDropboxCache(QObject* parent = 0);
    virtual ~QDropboxCache();

    void updateByPath(const QString& path, QList<QDropboxFile*>& files, const QString& cursor);
    void updateByCursor(const QString& prevCursor, QList<QDropboxFile*>& files, const QString& cursor);
    Cache findForPath(const QString& path, const QString& orderBy = "name", const QString& order = "asc");
    Cache findForCursor(const QString& cursor, const QString& orderBy = "name", const QString& order = "asc");

    void add(QDropboxFile* file);
    void remove(QDropboxFile* file);
    void update(QDropboxFile* file);
    void deleteByPaths(const QStringList& paths);
    void move(const QList<MoveEntry>& moveEntries);

    void flush();

private:
    static Logger logger;

    QVariantMap m_pathsCursors;

    void insert(const QString& path, QList<QDropboxFile*>& files, const QString& cursor);
    void insert(const QString& path, QDropboxFile* file);
    QVariantList select(const QString& path, const QString& type, const QString& orderBy, const QString& order);
    QString findCursor(const QString& path);
    QString findPath(const QString& cursor);
    void persistCursors();
    void deleteById(const QString& id);
    QString pathFromPathDisplay(QString pathDisplay, QString name);
};

#endif /* QDROPBOXCACHE_HPP_ */
