/*
 * QDropboxFile.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXFILE_HPP_
#define QDROPBOXFILE_HPP_

#include <QObject>
#include <QVariantMap>

struct SharingInfo : public QObject {

    SharingInfo(QObject* parent = 0) : QObject(parent), readOnly(false), sharedFolderId(""), parentSharedFolderId(""), traverseOnly(false), noAccess(false) {}

    bool readOnly;
    QString sharedFolderId;
    QString parentSharedFolderId;
    QString modifiedBy;
    bool traverseOnly;
    bool noAccess;

    QVariantMap toMap() {
        QVariantMap map;
        map["read_only"] = readOnly;
        map["shared_folder_id"] = sharedFolderId;
        map["parent_shared_folder_id"] = parentSharedFolderId;
        map["modified_by"] = modifiedBy;
        map["traverse_only"] = traverseOnly;
        map["no_access"] = noAccess;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        readOnly = map.value("read_only", false).toBool();
        sharedFolderId = map.value("shared_folder_id", "").toString();
        parentSharedFolderId = map.value("parent_shared_folder_id", "").toString();
        modifiedBy = map.value("modified_by", "").toString();
        traverseOnly = map.value("traverse_only", false).toBool();
        noAccess = map.value("no_access", false).toBool();
    }
};

struct Dimensions : public QObject {

    Dimensions(QObject* parent = 0) : QObject(parent), height(0), width(0) {}

    int height;
    int width;

    QVariantMap toMap() {
        QVariantMap map;
        map["height"] = height;
        map["width"] = width;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        height = map.value("height").toInt();
        width = map.value("width").toInt();
    }
};

struct Location : public QObject {
    Location(QObject* parent = 0) : QObject(parent), latitude(0), longitude(0) {}

    double latitude;
    double longitude;

    QVariantMap toMap() {
        QVariantMap map;
        map["latitude"] = latitude;
        map["longitude"] = longitude;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        latitude = map.value("latitude").toDouble();
        longitude = map.value("longitude").toDouble();
    }
};

struct Metadata : public QObject {

    Metadata(QObject* parent = 0) : QObject(parent), tag(""), timeTaken(""), dimensions(0), location(0) {}
    ~Metadata() {
        if (dimensions != 0) {
            delete dimensions;
            dimensions = 0;
        }
        if (location != 0) {
            delete location;
            location = 0;
        }
    }

    QString tag;
    QString timeTaken;
    Dimensions* dimensions;
    Location* location;

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        map["time_taken"] = timeTaken;
        if (dimensions != 0) {
            map["dimensions"] = dimensions->toMap();
        }
        if (location != 0) {
            map["location"] = location->toMap();
        }
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag").toString();
        timeTaken = map.value("time_taken", "").toString();
        if (map.contains("dimensions")) {
            if (dimensions != 0) {
                delete dimensions;
            }
            dimensions = new Dimensions(this);
            dimensions->fromMap(map.value("dimensions").toMap());
        }

        if (map.contains("location")) {
            if (location != 0) {
                delete location;
            }
            location = new Location(this);
            location->fromMap(map.value("location").toMap());
        }
    }
};

struct PhotoMetadata : public Metadata {
    PhotoMetadata(QObject* parent = 0) : Metadata(parent) {}
};

struct VideoMetadata : public Metadata {
    VideoMetadata(QObject* parent) : Metadata(parent), duration(0) {}

    int duration;

    QVariantMap toMap() {
        QVariantMap map = Metadata::toMap();
        map["duration"] = duration;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        Metadata::fromMap(map);
        duration = map.value("duration", 0).toInt();
    }
};

struct MediaInfo : public QObject {

    MediaInfo(QObject* parent = 0) : QObject(parent), tag(""), metadata(0) {}
    ~MediaInfo() {
        if (metadata != 0) {
            delete metadata;
            metadata = 0;
        }
    }

    QString tag;
    Metadata* metadata;

    bool isPhoto() {
        return tag.compare("photo") == 0;
    }

    bool isVideo() {
        return tag.compare("video") == 0;
    }

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        if (metadata != 0) {
            map["metadata"] = metadata->toMap();
        }
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag").toString();
        if (map.contains("metadata")) {
            QVariantMap data = map.value("metadata").toMap();
            QString tag = data.value(".tag").toString();
            if (tag.compare("video") == 0) {
                metadata = new VideoMetadata(this);
            } else {
                metadata = new PhotoMetadata(this);
            }
            metadata->fromMap(data);
        }
    }
};

class QDropboxFile: public QObject {
    Q_OBJECT
public:
    QDropboxFile(QObject* parent = 0);
    QDropboxFile(const QDropboxFile& file);
    virtual ~QDropboxFile();

    QDropboxFile& operator=(const QDropboxFile& file);

    bool isDir() const;
    bool isFile() const;

    const QString& getTag() const;
    QDropboxFile& setTag(const QString& tag);

    const QString& getName() const;
    QDropboxFile& setName(const QString& name);

    const QString& getPathLower() const;
    QDropboxFile& setPathLower(const QString& pathLower);

    const QString& getPathDisplay() const;
    QDropboxFile& setPathDisplay(const QString& pathDisplay);

    const QString& getId() const;
    QDropboxFile& setId(const QString& id);

    const QString& getSharedFolderId() const;
    QDropboxFile& setSharedFolderId(const QString& sharedFolderId);

    SharingInfo* getSharingInfo() const;
    QDropboxFile& setSharingInfo(SharingInfo* sharingInfo);

    const QString& getClientModified() const;
    QDropboxFile& setClientModified(const QString& clientModified);

    const QString& getServerModified() const;
    QDropboxFile& setServerModified(const QString& serverModified);

    const QString& getRev() const;
    QDropboxFile& setRev(const QString& rev);

    const int& getSize() const;
    QDropboxFile& setSize(const int& size);

    const QString& getContentHash() const;
    QDropboxFile& setContentHash(const QString& contentHash);

    MediaInfo* getMediaInfo() const;
    QDropboxFile& setMediaInfo(MediaInfo* mediaInfo);

    void fromMap(const QVariantMap& map);
    QVariantMap toMap();

private:
    QString m_tag;
    QString m_name;
    QString m_pathLower;
    QString m_pathDisplay;
    QString m_id;
    QString m_sharedFolderId;
    SharingInfo* m_sharingInfo;

    QString m_clientModified;
    QString m_serverModified;
    QString m_rev;
    int m_size;
    QString m_contentHash;

    MediaInfo* m_mediaInfo;

    void swap(const QDropboxFile& file);
};

#endif /* QDROPBOXFILE_HPP_ */
