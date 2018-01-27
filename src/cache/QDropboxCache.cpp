/*
 * QDropboxCache.cpp
 *
 *  Created on: Jan 9, 2018
 *      Author: doctorrokter
 */

#include "QDropboxCache.hpp"
#include <QDir>
#include "../qjson/serializer.h"
#include "../qjson/parser.h"
#include <QDebug>
#include <QTextStream>
#include "DB.hpp"
#include <QDateTime>
#include "../Common.hpp"

Logger QDropboxCache::logger = Logger::getLogger("QDropboxCache");

QDropboxCache::QDropboxCache(QObject* parent) : QObject(parent) {
    QVariantList list = DB::execute("SELECT * FROM paths_cursors").toList();
    foreach(QVariant v, list) {
        QVariantMap m = v.toMap();
        m_pathsCursors[m.value("path").toString()] = m.value("cursor").toString();
    }
    logger.debug(m_pathsCursors);
}

QDropboxCache::~QDropboxCache() {}

void QDropboxCache::updateByPath(const QString& path, QList<QDropboxFile*>& files, const QString& cursor) {
    logger.debug("Update by path: " + path + ", cursor: " + cursor);
    QVariantMap data;
    data["path"] = path;
    DB::execute("DELETE FROM files WHERE path = :path", data);
    insert(path, files, cursor);

    updatePathsCursors(path, cursor);
}

void QDropboxCache::updateByCursor(const QString& prevCursor, QList<QDropboxFile*>& files, const QString& cursor) {
    logger.debug("Update by cursor: prev - " + prevCursor + ", new - " + cursor);
    foreach(QString path, m_pathsCursors.keys()) {
        if (m_pathsCursors.value(path).compare(prevCursor) == 0) {
            insert(path, files, cursor);
            updatePathsCursors(path, cursor);
            return;
        }
    }
}

Cache QDropboxCache::findForPath(const QString& path, const QString& orderBy, const QString& order) {
    Cache cache;
    cache.path = path;
    cache.cursor = "";
    QVariantList files;

    files.append(select(path, "folder", orderBy, order));
    files.append(select(path, "file", orderBy, order));

    if (files.size()) {
        cache.files = files;
        cache.cursor = findCursor(path);
    }


    return cache;
}

Cache QDropboxCache::findForCursor(const QString& cursor, const QString& orderBy, const QString& order) {
    return findForPath(findPath(cursor), orderBy, order);
}

QMap<QString, QString> QDropboxCache::getPathsCursors() const {
    return m_pathsCursors;
}

void QDropboxCache::add(QDropboxFile* file) {
    if (isExists(file)) {
        update(file);
    } else {
        QString path = pathFromPathDisplay(file->getPathDisplay(), file->getName());
        insert(path, file);
    }
}

void QDropboxCache::remove(QDropboxFile* file) {
    deleteById(file->getId());
}

void QDropboxCache::insert(const QString& path, QList<QDropboxFile*>& files, const QString& cursor) {
    m_pathsCursors[path] = cursor;
    foreach(QDropboxFile* f, files) {
        insert(path, f);
    }
}

void QDropboxCache::update(QDropboxFile* file) {
    deleteById(file->getId());
    add(file);
}

void QDropboxCache::deleteByPaths(const QStringList& paths) {
    QString query = "DELETE FROM files WHERE path_display IN ('" + paths.join("','") + "')";
    DB::execute(query);
}

void QDropboxCache::move(const QList<MoveEntry>& moveEntries) {
    foreach(MoveEntry e, moveEntries) {
        QVariantMap data;
        data["path_display"] = e.fromPath;
        QVariantMap row = DB::execute("SELECT * FROM files WHERE path_display = :path_display", data).toList().at(0).toMap();

        bool res = false;
        QVariantMap fileMap = QJson::Parser().parse(row.value("content").toByteArray(), &res).toMap();
        QDropboxFile file;
        file.fromMap(fileMap);
        file.setPathDisplay(e.toPath);
        file.setPathLower(e.toPath.toLower());

        update(&file);
    }
}

void QDropboxCache::flush() {
    DB::execute("DELETE FROM files");
    DB::execute("DELETE FROM paths_cursors");
    m_pathsCursors.clear();
}

void QDropboxCache::insert(const QString& path, QDropboxFile* file) {
    QDateTime time = QDateTime::fromString(file->getClientModified(), Qt::ISODate);
    time.setTimeSpec(Qt::UTC);
    uint timestamp = time.toTime_t();

    QVariantMap data;
    data["id"] = file->getId();
    data["content_hash"] = file->getContentHash();
    data["name"] = file->getName();
    data["type"] = file->getTag();
    data["content"] = QString(QJson::Serializer().serialize(file->toMap()));
    data["date"] = timestamp;
    data["path_display"] = file->getPathDisplay();

    QString sql;
    if (path.isEmpty()) {
        sql = "INSERT INTO files (id, content_hash, name, type, date, content, path_display) VALUES (:id, :content_hash, :name, :type, :date, :content, :path_display)";
    } else {
        data["path"] = path;
        sql = "INSERT INTO files (id, content_hash, path, name, type, date, content, path_display) VALUES (:id, :content_hash, :path, :name, :type, :date, :content, :path_display)";
    }

    DB::execute(sql, data);
}

QVariantList QDropboxCache::select(const QString& path, const QString& type, const QString& orderBy, const QString& order) {
    QVariantMap data;
    data["type"] = type;

    QString query;
    if (path.isEmpty()) {
        query = "SELECT * FROM files WHERE (path IS NULL OR path = '') AND type = :type ORDER BY " + orderBy + " " + order;
    } else {
        data["path"] = path;
        query = "SELECT * FROM files WHERE path = :path AND type = :type ORDER BY " + orderBy + " " + order;
    }
    QVariantList list = DB::execute(query, data).toList();
    QVariantList files;
    foreach(QVariant v, list) {
        QVariantMap map = v.toMap();
        bool res = false;
        files.append(QJson::Parser().parse(map.value("content").toString().toUtf8(), &res).toMap());
    }

    return files;
}

QString QDropboxCache::findCursor(const QString& path) {
    return m_pathsCursors.value(path);
}

QString QDropboxCache::findPath(const QString& cursor) {
    foreach(QString path, m_pathsCursors.keys()) {
        if (m_pathsCursors.value(path).compare(cursor) == 0) {
            return path;
        }
    }
    return "";
}

void QDropboxCache::deleteById(const QString& id) {
    QVariantMap data;
    data["id"] = id;
    DB::execute("DELETE FROM files WHERE id = :id", data);
}

QString QDropboxCache::pathFromPathDisplay(QString pathDisplay, QString name) {
    return pathDisplay.replace("/" + name, "");
}

bool QDropboxCache::isExists(QDropboxFile* file) {
    QVariantMap map;
    map["id"] = file->getId();
    map["content_hash"] = file->getContentHash();
    return DB::execute("SELECT EXISTS (SELECT 1 FROM files WHERE id = :id AND content_hash = :content_hash) AS present", map).toList().at(0).toMap().value("present").toBool();
}

void QDropboxCache::updatePathsCursors(const QString& path, const QString& cursor) {
    DB::execute(QString("DELETE FROM paths_cursors WHERE path = '%1'").arg(path));
    DB::execute(QString("INSERT INTO paths_cursors (path, cursor) VALUES ('%1', '%2')").arg(path).arg(cursor));
    m_pathsCursors[path] = cursor;
}
