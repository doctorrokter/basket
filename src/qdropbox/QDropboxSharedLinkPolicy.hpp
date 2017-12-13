/*
 * QDropboxSharedLinkPolicy.hpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXSHAREDLINKPOLICY_HPP_
#define QDROPBOXSHAREDLINKPOLICY_HPP_

#include <QObject>
#include <QMap>

class QDropboxSharedLinkPolicy: public QObject {
    Q_OBJECT
public:
    QDropboxSharedLinkPolicy(QObject* parent = 0);
    virtual ~QDropboxSharedLinkPolicy();

    QDropboxSharedLinkPolicy& operator=(const QDropboxSharedLinkPolicy& policy);

    enum SharedLinkPolicy {
        TEAM,
        ANYONE,
        MEMBERS,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const SharedLinkPolicy& value() const;
    QDropboxSharedLinkPolicy& value(const SharedLinkPolicy& policy);
private:
    SharedLinkPolicy m_policy;
    QMap<QString, SharedLinkPolicy> map;
};

#endif /* QDROPBOXSHAREDLINKPOLICY_HPP_ */
