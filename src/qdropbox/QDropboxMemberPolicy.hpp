/*
 * QDropboxMemberPolicy.hpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXMEMBERPOLICY_HPP_
#define QDROPBOXMEMBERPOLICY_HPP_

#include <QObject>
#include <QMap>

class QDropboxMemberPolicy: public QObject {
    Q_OBJECT
public:
    QDropboxMemberPolicy(QObject* parent = 0);
    virtual ~QDropboxMemberPolicy();

    QDropboxMemberPolicy& operator=(const QDropboxMemberPolicy& policy);

    enum MemberPolicy {
        TEAM,
        ANYONE,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const MemberPolicy& value() const;
    QDropboxMemberPolicy& value(const MemberPolicy& policy);

private:
    MemberPolicy m_policy;
    QMap<QString, MemberPolicy> map;
};

#endif /* QDROPBOXMEMBERPOLICY_HPP_ */
