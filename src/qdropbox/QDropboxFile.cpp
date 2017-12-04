/*
 * QDropboxFile.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: doctorrokter
 */

#include "QDropboxFile.hpp"

#define FOLDER_TAG "folder"
#define FILE_TAG "file"

QDropboxFile::QDropboxFile(QObject* parent) : QObject(parent), m_tag(""), m_name(""), m_pathLower(""), m_pathDisplay(""), m_id(""), m_sharedFolderId(""),
m_sharingInfo(0), m_clientModified(""), m_serverModified(""), m_rev(""), m_size(0), m_contentHash(""), m_mediaInfo(0) {}

QDropboxFile::QDropboxFile(const QDropboxFile& file) : QObject(file.parent()) {
    if (this != &file) {
        swap(file);
    }
}

QDropboxFile::~QDropboxFile() {
    if (m_sharingInfo != 0) {
        delete m_sharingInfo;
        m_sharingInfo = 0;
    }

    if (m_mediaInfo != 0) {
        delete m_mediaInfo;
        m_mediaInfo = 0;
    }
}

QDropboxFile& QDropboxFile::operator=(const QDropboxFile& file) {
    if (this != &file) {
        swap(file);
    }
    return *this;
}

bool QDropboxFile::isDir() const {
    return m_tag.compare(FOLDER_TAG) == 0;
}

bool QDropboxFile::isFile() const {
    return m_tag.compare(FILE_TAG) == 0;
}

const QString& QDropboxFile::getTag() const { return m_tag; }
QDropboxFile& QDropboxFile::setTag(const QString& tag) {
    m_tag = tag;
    return *this;
}

const QString& QDropboxFile::getName() const { return m_name; }
QDropboxFile& QDropboxFile::setName(const QString& name) {
    m_name = name;
    return *this;
}

const QString& QDropboxFile::getPathLower() const { return m_pathLower; }
QDropboxFile& QDropboxFile::setPathLower(const QString& pathLower) {
    m_pathLower = pathLower;
    return *this;
}

const QString& QDropboxFile::getPathDisplay() const { return m_pathDisplay; }
QDropboxFile& QDropboxFile::setPathDisplay(const QString& pathDisplay) {
    m_pathDisplay = pathDisplay;
    return *this;
}

const QString& QDropboxFile::getId() const { return m_id; }
QDropboxFile& QDropboxFile::setId(const QString& id) {
    m_id = id;
    return *this;
}

const QString& QDropboxFile::getSharedFolderId() const { return m_sharedFolderId; }
QDropboxFile& QDropboxFile::setSharedFolderId(const QString& sharedFolderId) {
    m_sharedFolderId = sharedFolderId;
    return *this;
}

SharingInfo* QDropboxFile::getSharingInfo() const { return m_sharingInfo; }
QDropboxFile& QDropboxFile::setSharingInfo(SharingInfo* sharingInfo) {
    if (m_sharingInfo != 0) {
        delete m_sharingInfo;
    }
    sharingInfo->setParent(this);
    m_sharingInfo = sharingInfo;
    return *this;
}

const QString& QDropboxFile::getClientModified() const { return m_clientModified; }
QDropboxFile& QDropboxFile::setClientModified(const QString& clientModified) {
    m_clientModified = clientModified;
    return *this;
}

const QString& QDropboxFile::getServerModified() const { return m_serverModified; }
QDropboxFile& QDropboxFile::setServerModified(const QString& serverModified) {
    m_serverModified = serverModified;
    return *this;
}

const QString& QDropboxFile::getRev() const { return m_rev; }
QDropboxFile& QDropboxFile::setRev(const QString& rev) {
    m_rev = rev;
    return *this;
}

const int& QDropboxFile::getSize() const { return m_size; }
QDropboxFile& QDropboxFile::setSize(const int& size) {
    m_size = size;
    return *this;
}

const QString& QDropboxFile::getContentHash() const { return m_contentHash; }
QDropboxFile& QDropboxFile::setContentHash(const QString& contentHash) {
    m_contentHash = contentHash;
    return *this;
}

MediaInfo* QDropboxFile::getMediaInfo() const { return m_mediaInfo; }
QDropboxFile& QDropboxFile::setMediaInfo(MediaInfo* mediaInfo) {
    if (mediaInfo != 0) {
        delete m_mediaInfo;
    }
    mediaInfo->setParent(this);
    m_mediaInfo = mediaInfo;
    return *this;
}

void QDropboxFile::fromMap(const QVariantMap& map) {
    m_tag = map.value(".tag").toString();
    m_name = map.value("name").toString();
    m_pathLower = map.value("path_lower").toString();
    m_pathDisplay = map.value("path_display").toString();
    m_id = map.value("id").toString();
    m_sharedFolderId = map.value("shared_folder_id", "").toString();

    if (map.contains("sharing_info")) {
        SharingInfo* sharingInfo = new SharingInfo(this);
        sharingInfo->fromMap(map.value("sharing_info").toMap());
        setSharingInfo(sharingInfo);
    }

    if (m_tag.compare(FILE_TAG) == 0) {
        m_size = map.value("size").toInt();
        m_rev = map.value("rev").toString();
        m_contentHash = map.value("content_hash").toString();
        m_clientModified = map.value("client_modified").toString();
        m_serverModified = map.value("server_modified").toString();

        if (map.contains("media_info")) {
            MediaInfo* mediaInfo = new MediaInfo(this);
            mediaInfo->fromMap(map.value("media_info").toMap());
            setMediaInfo(mediaInfo);
        }
    }
}

QVariantMap QDropboxFile::toMap() {
    QVariantMap map;
    map[".tag"] = m_tag;
    map["name"] = m_name;
    map["path_lower"] = m_pathLower;
    map["path_display"] = m_pathDisplay;
    map["id"] = m_id;
    map["shared_folder_id"] = m_sharedFolderId;

    if (m_sharingInfo != NULL) {
        map["sharing_info"] = m_sharingInfo->toMap();
    }

    if (m_tag.compare(FILE_TAG) == 0) {
        map["size"] = m_size;
        map["rev"] = m_rev;
        map["content_hash"] = m_contentHash;
        map["client_modified"] = m_clientModified;
        map["server_modified"] = m_serverModified;

        if (m_mediaInfo != NULL) {
            map["media_info"] = m_mediaInfo->toMap();
        }
    }
    return map;
}

void QDropboxFile::swap(const QDropboxFile& file) {
    m_tag = file.getTag();
    m_name = file.getName();
    m_pathLower = file.getPathLower();
    m_pathDisplay = file.getPathDisplay();
    m_id = file.getId();
    m_sharedFolderId = file.getSharedFolderId();

    if (file.getSharingInfo() != 0) {
        SharingInfo* sharingInfo = new SharingInfo(this);
        sharingInfo->fromMap(file.getSharingInfo()->toMap());
        setSharingInfo(sharingInfo);
    }

    if (file.getTag().compare(FILE_TAG) == 0) {
        m_size = file.getSize();
        m_rev = file.getRev();
        m_contentHash = file.getContentHash();
        m_clientModified = file.getClientModified();
        m_serverModified = file.getServerModified();

        if (file.getMediaInfo() != 0) {
            MediaInfo* mediaInfo = new MediaInfo(this);
            mediaInfo->fromMap(file.getMediaInfo()->toMap());
            setMediaInfo(mediaInfo);
        }
    }
}

