/*
 * QDropboxFolderMember.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: doctorrokter
 */

#include "QDropboxFolderMember.hpp"

QDropboxFolderMember::QDropboxFolderMember(QObject* parent) : QObject(parent), m_pAccessType(0), m_pUser(0), m_isInherited(false) {}

QDropboxFolderMember::QDropboxFolderMember(const QDropboxFolderMember& member) : QObject(member.parent()) {
    swap(member);
}

QDropboxFolderMember::~QDropboxFolderMember() {
    if (m_pAccessType != 0) {
        m_pAccessType->deleteLater();
    }

    if (m_pUser != 0) {
        m_pUser->deleteLater();
    }
}

QDropboxFolderMember& QDropboxFolderMember::operator=(const QDropboxFolderMember& member) {
    if (this != &member) {
        swap(member);
    }
    return *this;
}

AccessType* QDropboxFolderMember::getAccessType() const { return m_pAccessType; }
QDropboxFolderMember& QDropboxFolderMember::setAccessType(AccessType* accessType) {
    if (m_pAccessType != 0) {
        delete m_pAccessType;
    }
    m_pAccessType = accessType;
    return *this;
}

User* QDropboxFolderMember::getUser() const { return m_pUser; }
QDropboxFolderMember& QDropboxFolderMember::setUser(User* user) {
    if (m_pUser != 0) {
        delete m_pUser;
    }
    m_pUser = user;
    return *this;
}

const bool& QDropboxFolderMember::isInherited() const { return m_isInherited; }
QDropboxFolderMember& QDropboxFolderMember::setInherited(const bool& isInherited) {
    m_isInherited = isInherited;
    return *this;
}

QVariantMap QDropboxFolderMember::toMap() {
    QVariantMap map;
    map["is_inherited"] = m_isInherited;
    if (m_pAccessType != 0) {
        map["access_type"] = m_pAccessType->toMap();
    }
    if (m_pUser != 0) {
        map["user"] = m_pUser->toMap();
    }
    return map;
}

void QDropboxFolderMember::fromMap(const QVariantMap& map) {
    m_isInherited = map.value("is_inherited").toBool();
    m_pAccessType = new AccessType(this);
    m_pAccessType->fromMap(map.value("access_type").toMap());

    m_pUser = new User(this);
    m_pUser->fromMap(map.value("user").toMap());
}

void QDropboxFolderMember::swap(const QDropboxFolderMember& member) {
    m_isInherited = member.isInherited();
    AccessType* at = new AccessType(this);
    at->fromMap(member.getAccessType()->toMap());
    setAccessType(at);

    User* u = new User(this);
    u->fromMap(member.getUser()->toMap());
    setUser(u);
}
