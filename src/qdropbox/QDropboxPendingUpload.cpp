/*
 * QDropboxPendingUpload.cpp
 *
 *  Created on: Dec 15, 2017
 *      Author: doctorrokter
 */

#include "QDropboxPendingUpload.hpp"

QDropboxPendingUpload::QDropboxPendingUpload(QObject* parent) : QObject(parent), m_pending(NONE) {
    map["file"] = FILE;
    map["folder"] = FOLDER;
}

QDropboxPendingUpload::~QDropboxPendingUpload() {}

QString QDropboxPendingUpload::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_pending) {
            return k;
        }
    }
    return "";
};

void QDropboxPendingUpload::fromStr(const QString& str) {
    m_pending = map.value(str);
}

const QDropboxPendingUpload::PendingUpload& QDropboxPendingUpload::value() const { return m_pending; }
QDropboxPendingUpload& QDropboxPendingUpload::value(const QDropboxPendingUpload::PendingUpload& value) {
    m_pending = value;
    return *this;
}
