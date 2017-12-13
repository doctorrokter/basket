/*
 * QDropboxMember.hpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXMEMBER_HPP_
#define QDROPBOXMEMBER_HPP_

#include <QObject>
#include <QVariantMap>
#include "QDropboxAccessLevel.hpp"
#include "QDropboxTag.hpp"

class QDropboxMember: public QObject {
    Q_OBJECT
public:
    QDropboxMember(QObject* parent = 0);
    QDropboxMember(const QDropboxMember& member);
    virtual ~QDropboxMember();

    QDropboxMember& operator=(const QDropboxMember& member);

    const QDropboxTag& getTag() const;

    const QString& getEmail() const;
    QDropboxMember& setEmail(const QString& email);

    const QString& getDropboxId() const;
    QDropboxMember& setDropboxId(const QString& dropboxId);

    const QDropboxAccessLevel& getAccessLevel() const;
    QDropboxMember& setAccessLevel(const QDropboxAccessLevel& accessLevel);

    QVariantMap toMap();
    void fromMap(const QVariantMap& map);

private:
    QString m_email;
    QString m_dropboxId;
    QDropboxTag m_tag;
    QDropboxAccessLevel m_accessLevel;

    void swap(const QDropboxMember& member);
};

#endif /* QDROPBOXMEMBER_HPP_ */
