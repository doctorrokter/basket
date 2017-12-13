/*
 * QDropboxTag.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#include "QDropboxTag.hpp"

#define EMAIL_STR "email"
#define DROPBOX_ID_STR "dropbox_id"

QDropboxTag::QDropboxTag(QObject* parent) : QObject(parent) {
    m_tag = NONE;

    map[EMAIL_STR] = EMAIL;
    map[DROPBOX_ID_STR] = DROPBOX_ID;
}

QDropboxTag::~QDropboxTag() {}

QDropboxTag& QDropboxTag::operator=(const QDropboxTag& tag) {
    m_tag = tag.value();
    return *this;
}

QString QDropboxTag::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_tag) {
            return k;
        }
    }
    return "";
}

void QDropboxTag::fromStr(const QString& str) {
    m_tag = map.value(str);
}

const QDropboxTag::Tag& QDropboxTag::value() const { return m_tag; }
QDropboxTag& QDropboxTag::value(const Tag& value) {
    m_tag = value;
    return *this;
}
