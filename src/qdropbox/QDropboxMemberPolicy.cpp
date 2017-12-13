/*
 * QDropboxMemberPolicy.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#include "QDropboxMemberPolicy.hpp"

#define TEAM_STR "team"
#define ANYONE_STR "anyone"

QDropboxMemberPolicy::QDropboxMemberPolicy(QObject* parent) : QObject(parent) {
    m_policy = NONE;

    map[TEAM_STR] = TEAM;
    map[ANYONE_STR] = ANYONE;
}

QDropboxMemberPolicy::~QDropboxMemberPolicy() {}

QDropboxMemberPolicy& QDropboxMemberPolicy::operator=(const QDropboxMemberPolicy& policy) {
    m_policy = policy.value();
    return *this;
}

QString QDropboxMemberPolicy::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_policy) {
            return k;
        }
    }
    return "";
}

void QDropboxMemberPolicy::fromStr(const QString& str) {
    m_policy = map.value(str);
}

const QDropboxMemberPolicy::MemberPolicy& QDropboxMemberPolicy::value() const { return m_policy; }
QDropboxMemberPolicy& QDropboxMemberPolicy::value(const MemberPolicy& policy) {
    m_policy = policy;
    return *this;
}
