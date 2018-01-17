/*
 * DB.cpp
 *
 *  Created on: Jan 13, 2018
 *      Author: doctorrokter
 */

#include "DB.hpp"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QSettings>

#define DB_NAME "basket.db"
#define MIGRATIONS_PATH "app/native/assets/migrations"

Logger DB::logger = Logger::getLogger("DB");
SqlDataAccess* DB::m_pSda = 0;

DB::DB(QObject* parent) : QObject(parent) {
    QSettings qsettings;
    m_dbCreated = qsettings.value("db_created", false).toBool();

    QString dbDirPath = QDir::currentPath() + "/data/cache";
    QDir dbDir(dbDirPath);
    if (!m_dbCreated || !dbDir.exists()) {
        dbDir.mkpath(dbDirPath);
    }

    QString dbpath = dbDirPath + "/" + QString(DB_NAME);
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbpath);
    m_database.open();

    m_pSda = new SqlDataAccess(dbpath, this);
    m_pSda->execute("PRAGMA encoding = \"UTF-8\"");
    m_pSda->execute("PRAGMA foreign_keys = ON");

    if (!m_dbCreated) {
        logger.debug("Create DB from scratch");
        migrate();
        qsettings.setValue("db_created", true);
    } else {
        logger.debug("DB already exists. Use one.");
        if (!qsettings.value("db_migrated", false).toBool()) {
            migrate();
            qsettings.setValue("db_migrated", true);
        }

        if (maxMigrationVersion() > getVersion()) {
            migrate();
        } else {
            logger.debug("DB versions matches!");
        }
        logger.debug("Current DB version is: " + QString::number(getVersion()));
    }
}

DB::~DB() {
    m_database.close();
    m_pSda->deleteLater();
}

bool DB::isNewDb() const { return !m_dbCreated; }

QVariant DB::execute(const QString& query) {
//    logger.debug(query);
    return m_pSda->execute(query);
}

QVariant DB::execute(const QString& query, const QVariantMap& values) {
//    logger.debug(query);
//    logger.debug(values);
    return m_pSda->execute(query, values);
}

void DB::runMigration(const QString& path) {
    logger.debug("Process migration: " + path);
    int version = getMigrationVersion(path);

    QFile migration(QString::fromLatin1(MIGRATIONS_PATH) + "/" + path);
    migration.open(QIODevice::ReadOnly);
    QString data = migration.readAll();

    QStringList statements = data.split(";");
    foreach(QString stmt, statements) {
        if (!stmt.trimmed().isEmpty()) {
            logger.debug(stmt);
            m_pSda->execute(stmt);
            if (m_pSda->hasError()) {
                logger.error("Sql Exception: " + m_pSda->error().errorMessage()) ;
            }
        }
    }

    setVersion(version);
}

bool DB::hasVersion(const int version) {
    return m_pSda->execute(QString::fromLatin1("SELECT EXISTS (SELECT 1 FROM schema_version WHERE version = %1 LIMIT 1) AS exists").arg(version))
            .toList().first().toMap().value("exists").toInt() != 0;
}

void DB::setVersion(const int version) {
    m_pSda->execute(QString::fromLatin1("INSERT INTO schema_version (version) VALUES (%1)").arg(version));
}

int DB::getVersion() {
    QVariantList res = m_pSda->execute("SELECT version FROM schema_version ORDER BY version DESC LIMIT 1").toList();
    return res.empty() ? 0 : res.first().toMap().value("version").toInt();
}

bool DB::hasSchemaVersionTable() {
    return m_pSda->execute("SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'schema_version'").toList().size() != 0;
}

int DB::getMigrationVersion(const QString& path) {
    return path.split("_")[0].split("/").last().toInt();
}

void DB::migrate() {
    QDir dir(MIGRATIONS_PATH);
    int currVersion = 0;

    if (hasSchemaVersionTable()) {
        currVersion = getVersion();
    }

    dir.setSorting(QDir::Name);
    QStringList paths = dir.entryList();
    foreach(QString path, paths) {
        logger.debug(path);
        if (path.endsWith(".sql")) {
            if (getMigrationVersion(path) > currVersion) {
                logger.debug("Found new migration");
                runMigration(path);
            }
        }
    }
}

int DB::maxMigrationVersion() {
    QDirIterator iter(MIGRATIONS_PATH, QDirIterator::NoIteratorFlags);
    int startVersion = 0;
    while (iter.hasNext()) {
        QString path = iter.next();
        if (path.endsWith(".sql")) {
            int v = getMigrationVersion(path);
            if (v > startVersion) {
                startVersion = v;
            }
        }
    }
    return startVersion;
}

