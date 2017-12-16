/*
 * QDropboxMember.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#include "QDropboxMember.hpp"

QDropboxMember::QDropboxMember(QObject* parent) : QObject(parent), m_email(""), m_dropboxId("") {}

QDropboxMember::QDropboxMember(const QDropboxMember& member) : QObject(member.parent()) {
    swap(member);
}

QDropboxMember::~QDropboxMember() {}

QDropboxMember& QDropboxMember::operator=(const QDropboxMember& member) {
    swap(member);
    return *this;
}

const QDropboxTag& QDropboxMember::getTag() const { return m_tag; }

const QString& QDropboxMember::getEmail() const { return m_email; }
QDropboxMember& QDropboxMember::setEmail(const QString& email) {
    m_email = email;
    if (m_tag.value() != QDropboxTag::DROPBOX_ID) {
        m_tag.fromStr("email");
    }
    return *this;
}

const QString& QDropboxMember::getDropboxId() const { return m_dropboxId; }
QDropboxMember& QDropboxMember::setDropboxId(const QString& dropboxId) {
    m_dropboxId = dropboxId;
    m_tag.fromStr("dropbox_id");
    return *this;
}

const QDropboxAccessLevel& QDropboxMember::getAccessLevel() const { return m_accessLevel; }
QDropboxMember& QDropboxMember::setAccessLevel(const QDropboxAccessLevel& accessLevel) {
    m_accessLevel = accessLevel;
    return *this;
}

QVariantMap QDropboxMember::toMap() {
    QVariantMap map;
    QVariantMap member;
    member[".tag"] = m_tag.name();
    if (!m_dropboxId.isEmpty()) {
        member["dropbox_id"] = m_dropboxId;
    } else {
        member["email"] = m_email;
    }

    map["member"] = member;
    map["access_level"] = m_accessLevel.name();
    return map;
}

void QDropboxMember::fromMap(const QVariantMap& map) {
    QVariantMap member = map.value("member").toMap();
    m_tag.fromStr(member.value(".tag").toString());
    m_email = member.value("email", "").toString();
    m_dropboxId = member.value("dropbox_id", "").toString();
    m_accessLevel.fromStr(map.value("access_level").toString());
}

void QDropboxMember::swap(const QDropboxMember& member) {
    m_tag = member.getTag();
    m_email = member.getEmail();
    m_dropboxId = member.getDropboxId();
    m_accessLevel = member.getAccessLevel();
}

