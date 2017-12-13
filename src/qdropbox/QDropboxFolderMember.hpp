/*
 * QDropboxFolderMember.hpp
 *
 *  Created on: Dec 13, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXFOLDERMEMBER_HPP_
#define QDROPBOXFOLDERMEMBER_HPP_

#include <QObject>
#include <QVariantMap>

struct AccessType : public QObject {

    AccessType(QObject* parent = 0) : QObject(parent), tag("") {}

    QString tag;

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag").toString();
    }
};

struct User : public QObject {

    User(QObject* parent = 0) : QObject(parent), accountId(""), sameTeam(false) {}

    QString accountId;
    bool sameTeam;

    QVariantMap toMap() {
        QVariantMap map;
        map["account_id"] = accountId;
        map["same_team"] = sameTeam;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        accountId = map.value("account_id").toString();
        sameTeam = map.value("same_team").toBool();
    }
};

class QDropboxFolderMember: public QObject {
    Q_OBJECT
public:
    QDropboxFolderMember(QObject* parent = 0);
    QDropboxFolderMember(const QDropboxFolderMember& member);
    virtual ~QDropboxFolderMember();

    QDropboxFolderMember& operator=(const QDropboxFolderMember& member);

    AccessType* getAccessType() const;
    QDropboxFolderMember& setAccessType(AccessType* accessType);

    User* getUser() const;
    QDropboxFolderMember& setUser(User* user);

    const bool& isInherited() const;
    QDropboxFolderMember& setInherited(const bool& isInherited);

    QVariantMap toMap();
    void fromMap(const QVariantMap& map);

private:
    AccessType* m_pAccessType;
    User* m_pUser;
    bool m_isInherited;

    void swap(const QDropboxFolderMember& member);
};

#endif /* QDROPBOXFOLDERMEMBER_HPP_ */
