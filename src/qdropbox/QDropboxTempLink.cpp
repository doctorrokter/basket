/*
 * QDropboxTempLink.cpp
 *
 *  Created on: Dec 15, 2017
 *      Author: doctorrokter
 */

#include "QDropboxTempLink.hpp"

QDropboxTempLink::QDropboxTempLink(QObject* parent) : QObject(parent) {}

QDropboxTempLink::~QDropboxTempLink() {}

const QString& QDropboxTempLink::getLink() const { return m_link; }
QDropboxTempLink& QDropboxTempLink::setLink(const QString& link) {
    m_link = link;
    return *this;
}

const QDropboxFile& QDropboxTempLink::getFile() const { return m_file; }
QDropboxTempLink& QDropboxTempLink::setFile(const QDropboxFile& file) {
    m_file = file;
    return *this;
}

QVariantMap QDropboxTempLink::toMap() {
    QVariantMap map;
    map["link"] = m_link;
    map["metadata"] = m_file.toMap();
    return map;
}

QDropboxTempLink& QDropboxTempLink::fromMap(const QVariantMap& map) {
    m_link = map.value("link").toString();
    m_file.fromMap(map.value("metadata").toMap());
    return *this;
}

