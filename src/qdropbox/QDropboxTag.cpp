/*
 * QDropboxTag.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#include "QDropboxTag.hpp"

QDropboxTag::QDropboxTag(QObject* parent) : QObject(parent) {}

QDropboxTag::~QDropboxTag() {}

QDropboxTag& QDropboxTag::operator=(const QDropboxTag& tag) {
    m_tag = tag.value();
    return *this;
}

QString QDropboxTag::name() const {
    switch(m_tag) {
        case EMAIL: return "email";
        case DROPBOX_ID: return "dropbox_id";
        default: return "";
    }
}

void QDropboxTag::fromStr(const QString& str) {
    if (str.compare("email") == 0) {
        m_tag = EMAIL;
    } else if (str.compare("dropbox_id") == 0) {
        m_tag = DROPBOX_ID;
    }
}

const QDropboxTag::Tag& QDropboxTag::value() const { return m_tag; }
QDropboxTag& QDropboxTag::value(const Tag& value) {
    m_tag = value;
    return *this;
}
