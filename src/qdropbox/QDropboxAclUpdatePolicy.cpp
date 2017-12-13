/*
 * QDropboxAclUpdatePolicy.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#include "QDropboxAclUpdatePolicy.hpp"

#define OWNER_STR "owner"
#define EDITORS_STR "editors"

QDropboxAclUpdatePolicy::QDropboxAclUpdatePolicy(QObject* parent) : QObject(parent) {
    m_policy = NONE;

    map[OWNER_STR] = OWNER;
    map[EDITORS_STR] = EDITORS;
}

QDropboxAclUpdatePolicy::~QDropboxAclUpdatePolicy() {}

QDropboxAclUpdatePolicy& QDropboxAclUpdatePolicy::operator=(const QDropboxAclUpdatePolicy& policy) {
    m_policy = policy.value();
    return *this;
}

QString QDropboxAclUpdatePolicy::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_policy) {
            return k;
        }
    }
    return "";
}

void QDropboxAclUpdatePolicy::fromStr(const QString& str) {
    m_policy = map.value(str);
}

const QDropboxAclUpdatePolicy::AclUpdatePolicy& QDropboxAclUpdatePolicy::value() const { return m_policy; }
QDropboxAclUpdatePolicy& QDropboxAclUpdatePolicy::value(const AclUpdatePolicy& policy) {
    m_policy = policy;
    return *this;
}
