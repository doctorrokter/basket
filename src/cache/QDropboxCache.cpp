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
    QString dirPath = QDir::currentPath() + CACHE_DIR;
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(dirPath);
    }

    QFile cursors(dirPath + "/cursors.json");
    if (cursors.exists()) {
        bool res = false;
        QVariant data = QJson::Parser().parse(&cursors, &res);
        if (res) {
            m_pathsCursors = data.toMap();
        } else {
            logger.error("Cannot read cursors.json");
            logger.error(data);
        }
    }
}

QDropboxCache::~QDropboxCache() {
    persistCursors();
}

void QDropboxCache::updateByPath(const QString& path, QList<QDropboxFile*>& files, const QString& cursor) {
    logger.debug("Update by path: " + path + ", cursor: " + cursor);
    QVariantMap data;
    data["path"] = path;
    DB::execute("DELETE FROM files WHERE path = :path", data);
    insert(path, files, cursor);
    persistCursors();
}

void QDropboxCache::updateByCursor(const QString& prevCursor, QList<QDropboxFile*>& files, const QString& cursor) {
    logger.debug("Update by cursor: prev - " + prevCursor + ", new - " + cursor);
    foreach(QString path, m_pathsCursors.keys()) {
        if (m_pathsCursors.value(path).toString().compare(prevCursor) == 0) {
            m_pathsCursors[path] = cursor;
            insert(path, files, cursor);
            return;
        }
    }
    persistCursors();
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

void QDropboxCache::add(QDropboxFile* file) {
    QString path = pathFromPathDisplay(file->getPathDisplay(), file->getName());
    insert(path, file);
}

void QDropboxCache::remove(QDropboxFile* file) {
    QVariantMap data;
    data["id"] = file->getId();
    DB::execute("DELETE FROM files WHERE id = :id", data);
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
}

void QDropboxCache::insert(const QString& path, QDropboxFile* file) {
    QVariantMap data;
    data["id"] = file->getId();
    data["content_hash"] = file->getContentHash();
    data["name"] = file->getName();
    data["type"] = file->getTag();
    data["content"] = QString(QJson::Serializer().serialize(file->toMap()));
    data["date"] = QDateTime::fromString(file->getClientModified()).toTime_t();
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
    return m_pathsCursors.value(path).toString();
}

QString QDropboxCache::findPath(const QString& cursor) {
    foreach(QString path, m_pathsCursors.keys()) {
        if (m_pathsCursors.value(path).toString().compare(cursor) == 0) {
            return path;
        }
    }
    return "";
}

void QDropboxCache::persistCursors() {
    QFile cursors(QDir::currentPath() + CACHE_DIR + "/cursors.json");
    if (cursors.exists()) {
        cursors.remove();
    }

    cursors.open(QIODevice::WriteOnly);
    cursors.write(QJson::Serializer().serialize(m_pathsCursors));
    cursors.close();
}

void QDropboxCache::deleteById(const QString& id) {
    QVariantMap data;
    data["id"] = id;
    DB::execute("DELETE FROM files WHERE id = :id", data);
}

QString QDropboxCache::pathFromPathDisplay(QString pathDisplay, QString name) {
    return pathDisplay.replace("/" + name, "");
}
