/*
 * QDropboxViewerInfoPolicy.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#include "QDropboxViewerInfoPolicy.hpp"

QDropboxViewerInfoPolicy::QDropboxViewerInfoPolicy(QObject* parent) : QObject(parent) {
    m_policy = NONE;

    map["enabled"] = ENABLED;
    map["disabled"] = DISABLED;
}

QDropboxViewerInfoPolicy::~QDropboxViewerInfoPolicy() {}

QDropboxViewerInfoPolicy& QDropboxViewerInfoPolicy::operator=(const QDropboxViewerInfoPolicy& policy) {
    m_policy = policy.value();
    return *this;
}

QString QDropboxViewerInfoPolicy::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_policy) {
            return k;
        }
    }
    return "";
}

void QDropboxViewerInfoPolicy::fromStr(const QString& str) {
    m_policy = map.value(str);
}

const QDropboxViewerInfoPolicy::ViewerInfoPolicy& QDropboxViewerInfoPolicy::value() const { return m_policy; }
QDropboxViewerInfoPolicy& QDropboxViewerInfoPolicy::value(const ViewerInfoPolicy& policy) {
    m_policy = policy;
    return *this;
}
