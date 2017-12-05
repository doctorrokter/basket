/*
 * QDropboxSpaceUsage.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: doctorrokter
 */

#include "QDropboxSpaceUsage.hpp"

QDropboxSpaceUsage::QDropboxSpaceUsage(QObject* parent) : QObject(parent), m_used(0), m_pAllocation(0) {}

QDropboxSpaceUsage::~QDropboxSpaceUsage() {
    if (m_pAllocation != 0) {
        m_pAllocation->deleteLater();
    }
}

const long long& QDropboxSpaceUsage::getUsed() const { return m_used; }
QDropboxSpaceUsage& QDropboxSpaceUsage::setUsed(const long long& used) {
    m_used = used;
    return *this;
}

SpaceAllocation* QDropboxSpaceUsage::getSpaceAllocation() const { return m_pAllocation; }
QDropboxSpaceUsage& QDropboxSpaceUsage::setSpaceAllocation(SpaceAllocation* allocation) {
    if (m_pAllocation != 0) {
        delete m_pAllocation;
    }
    allocation->setParent(this);
    m_pAllocation = allocation;
    return *this;
}

QVariantMap QDropboxSpaceUsage::toMap() const {
    QVariantMap map;
    map["used"] = m_used;
    if (m_pAllocation != 0) {
        map["allocation"] = m_pAllocation->toMap();
    }
    return map;
}
void QDropboxSpaceUsage::fromMap(const QVariantMap& map) {
    m_used = map.value("used").toULongLong();
    QVariantMap allocation = map.value("allocation").toMap();
    QString tag = allocation.value(".tag").toString();
    if (tag.compare("individual") == 0) {
        m_pAllocation = new IndividualAllocation(this);
    } else {
        m_pAllocation = new TeamAllocation(this);
    }
    m_pAllocation->fromMap(allocation);
}

