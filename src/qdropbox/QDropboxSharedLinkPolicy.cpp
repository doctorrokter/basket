/*
 * QDropboxSharedLinkPolicy.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#include "QDropboxSharedLinkPolicy.hpp"

QDropboxSharedLinkPolicy::QDropboxSharedLinkPolicy(QObject* parent) : QObject(parent) {
    m_policy = NONE;

    map["team"] = TEAM;
    map["anyone"] = ANYONE;
    map["members"] = MEMBERS;
}

QDropboxSharedLinkPolicy::~QDropboxSharedLinkPolicy() {}

QDropboxSharedLinkPolicy& QDropboxSharedLinkPolicy::operator=(const QDropboxSharedLinkPolicy& policy) {
    m_policy = policy.value();
    return *this;
}

QString QDropboxSharedLinkPolicy::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_policy) {
            return k;
        }
    }
    return "";
}

void QDropboxSharedLinkPolicy::fromStr(const QString& str) {
    m_policy = map.value(str);
}

const QDropboxSharedLinkPolicy::SharedLinkPolicy& QDropboxSharedLinkPolicy::value() const { return m_policy; }
QDropboxSharedLinkPolicy& QDropboxSharedLinkPolicy::value(const SharedLinkPolicy& policy) {
    m_policy = policy;
    return *this;
}
