/*
 * QDropboxAccessLevel.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#include "QDropboxAccessLevel.hpp"

#define OWNER_STR "owner"
#define EDITOR_STR "editor"
#define VIEWER_STR "viewer"
#define VIEWER_NO_COMMENT_STR "viewer_no_comment"

QDropboxAccessLevel::QDropboxAccessLevel(QObject* parent) : QObject(parent) {
    m_accessLevel = NONE;

    map[OWNER_STR] = OWNER;
    map[EDITOR_STR] = EDITOR;
    map[VIEWER_STR] = VIEWER;
    map[VIEWER_NO_COMMENT_STR] = VIEWER_NO_COMMENT;
}

QDropboxAccessLevel::~QDropboxAccessLevel() {}

QDropboxAccessLevel& QDropboxAccessLevel::operator=(const QDropboxAccessLevel& accessLevel) {
    m_accessLevel = accessLevel.value();
    return *this;
}

QString QDropboxAccessLevel::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_accessLevel) {
            return k;
        }
    }
    return "";
}

void QDropboxAccessLevel::fromStr(const QString& str) {
    m_accessLevel = map.value(str);
}

const QDropboxAccessLevel::AccessLevel& QDropboxAccessLevel::value() const { return m_accessLevel; }
QDropboxAccessLevel& QDropboxAccessLevel::value(const AccessLevel& value) {
    m_accessLevel = value;
    return *this;
}

QDropboxAccessLevel& QDropboxAccessLevel::valueFromInt(const int& val) {
    switch (val) {
        case 0: value(QDropboxAccessLevel::OWNER); break;
        case 1: value(QDropboxAccessLevel::EDITOR); break;
        case 2: value(QDropboxAccessLevel::VIEWER); break;
        case 3: value(QDropboxAccessLevel::VIEWER_NO_COMMENT); break;
    }
    return *this;
}


