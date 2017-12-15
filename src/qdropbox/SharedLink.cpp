/*
 * SharedLink.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: doctorrokter
 */

#include "SharedLink.hpp"

SharedLink::SharedLink(QObject* parent) : QObject(parent), m_url(""), m_password(""), m_path(""), m_pVisibility(0) {}

SharedLink::SharedLink(const QString& url, const QString& password, QObject* parent) : QObject(parent) {
    m_url = url;
    m_password = password;
    m_path = "";
    m_pVisibility = 0;
}

SharedLink::~SharedLink() {
    if (m_pVisibility != 0) {
        m_pVisibility->deleteLater();
    }
}

const QString& SharedLink::getUrl() const { return m_url; }
SharedLink& SharedLink::setUrl(const QString& url) {
    m_url = url;
    return *this;
}

const QString& SharedLink::getPassword() const { return m_password; }
SharedLink& SharedLink::setPassword(const QString& password) {
    m_password = password;
    return *this;
}

const QString& SharedLink::getPath() const { return m_path; }
SharedLink& SharedLink::setPath(const QString& path) {
    m_path = path;
    return *this;
}

Visibility* SharedLink::getVisibility() const { return m_pVisibility; }
SharedLink& SharedLink::setVisibility(Visibility* visiblity) {
    if (m_pVisibility != 0) {
        delete m_pVisibility;
    }
    m_pVisibility = visiblity;
    return *this;
}

bool SharedLink::isEmpty() {
    return m_url.isEmpty();
}

QVariantMap SharedLink::toMap() {
    QVariantMap map;
    map["url"] = m_url;
    map["password"] = m_password;
    map["path"] = m_path;
    if (m_pVisibility != 0) {
        map["visibility"] = m_pVisibility->toMap();
    }
    return map;
}

void SharedLink::fromMap(const QVariantMap& map) {
    m_url = map.value("url").toString();
    m_path = map.value("path").toString();
    m_password = map.value("password", "").toString();
    if (map.contains("visibility")) {
        m_pVisibility = new Visibility(this);
        m_pVisibility->fromMap(map.value("visibility").toMap());
    }
}

