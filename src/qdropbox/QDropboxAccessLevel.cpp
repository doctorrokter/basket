/*
 * QDropboxAccessLevel.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#include "QDropboxAccessLevel.hpp"

QDropboxAccessLevel::QDropboxAccessLevel(QObject* parent) : QObject(parent) {}

QDropboxAccessLevel::~QDropboxAccessLevel() {}

QDropboxAccessLevel& QDropboxAccessLevel::operator=(const QDropboxAccessLevel& accessLevel) {
    m_accessLevel = accessLevel.value();
    return *this;
}

QString QDropboxAccessLevel::name() const {
    switch(m_accessLevel) {
        case OWNER: return "owner";
        case EDITOR: return "editor";
        case VIEWER: return "viewer";
        case VIEWER_NO_COMMENT: return "viewer_no_comment";
        default: return "";
    }
}

void QDropboxAccessLevel::fromStr(const QString& str) {
    if (str.compare("owner") == 0) {
        m_accessLevel = OWNER;
    } else if (str.compare("editor") == 0) {
        m_accessLevel = EDITOR;
    } else if (str.compare("viewer") == 0) {
        m_accessLevel = VIEWER;
    } else if (str.compare("viewer_no_comment") == 0) {
        m_accessLevel = VIEWER_NO_COMMENT;
    }
}

const QDropboxAccessLevel::AccessLevel& QDropboxAccessLevel::value() const { return m_accessLevel; }
QDropboxAccessLevel& QDropboxAccessLevel::value(const AccessLevel& value) {
    m_accessLevel = value;
    return *this;
}


