/*
 * QDropboxSpaceUsage.hpp
 *
 *  Created on: Dec 5, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXSPACEUSAGE_HPP_
#define QDROPBOXSPACEUSAGE_HPP_

#include <QObject>
#include <QVariantMap>

struct SpaceAllocation : public QObject {
    SpaceAllocation(QObject* parent = 0) : QObject(parent), tag(""), allocated(0) {}

    QString tag;
    long long allocated;

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        map["allocated"] = allocated;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag").toString();
        allocated = map.value("allocated").toLongLong();
    }
};

struct IndividualAllocation : public SpaceAllocation {
    IndividualAllocation(QObject* parent = 0) : SpaceAllocation(parent) {}
};

struct TeamAllocation : public SpaceAllocation {
    TeamAllocation(QObject* parent = 0) : SpaceAllocation(parent), used(0) {}

    long long used;

    QVariantMap toMap() {
        QVariantMap map = SpaceAllocation::toMap();
        map["used"] = used;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        SpaceAllocation::fromMap(map);
        used = map.value("used").toLongLong();
    }
};

class QDropboxSpaceUsage: public QObject {
    Q_OBJECT
public:
    QDropboxSpaceUsage(QObject* parent = 0);
    virtual ~QDropboxSpaceUsage();

    const long long& getUsed() const;
    QDropboxSpaceUsage& setUsed(const long long& used);

    SpaceAllocation* getSpaceAllocation() const;
    QDropboxSpaceUsage& setSpaceAllocation(SpaceAllocation* allocation);

    QVariantMap toMap() const;
    void fromMap(const QVariantMap& map);

private:
    long long m_used;
    SpaceAllocation* m_pAllocation;
};

#endif /* QDROPBOXSPACEUSAGE_HPP_ */
