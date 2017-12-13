/*
 * QDropboxAclUpdatePolicy.hpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXACLUPDATEPOLICY_HPP_
#define QDROPBOXACLUPDATEPOLICY_HPP_

#include <QObject>
#include <QMap>

class QDropboxAclUpdatePolicy: public QObject {
    Q_OBJECT
public:
    QDropboxAclUpdatePolicy(QObject* parent = 0);
    virtual ~QDropboxAclUpdatePolicy();

    QDropboxAclUpdatePolicy& operator=(const QDropboxAclUpdatePolicy& policy);

    enum AclUpdatePolicy {
        OWNER,
        EDITORS,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const AclUpdatePolicy& value() const;
    QDropboxAclUpdatePolicy& value(const AclUpdatePolicy& policy);

private:
    AclUpdatePolicy m_policy;
    QMap<QString, AclUpdatePolicy> map;
};

#endif /* QDROPBOXACLUPDATEPOLICY_HPP_ */
