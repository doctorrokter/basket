/*
 * SharedLink.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: doctorrokter
 */

#include "SharedLink.hpp"

SharedLink::SharedLink(QObject* parent) : QObject(parent), m_url(""), m_password("") {}

SharedLink::SharedLink(const QString& url, const QString& password, QObject* parent) : QObject(parent) {
    m_url = url;
    m_password = password;
}

SharedLink::~SharedLink() {}

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

bool SharedLink::isEmpty() {
    return m_url.isEmpty();
}

QVariantMap SharedLink::toMap() {
    QVariantMap map;
    map["url"] = m_url;
    map["password"] = m_password;
    return map;
}

