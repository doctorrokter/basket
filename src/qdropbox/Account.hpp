/*
 * Account.hpp
 *
 *  Created on: Dec 3, 2017
 *      Author: doctorrokter
 */

#ifndef ACCOUNT_HPP_
#define ACCOUNT_HPP_

#include <QObject>
#include <QVariantMap>

struct Policy : public QObject {
    Policy(QObject* parent = 0) : QObject(parent) {}

    QString tag;

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag", "").toString();
    }
};

struct SharingPolicies : public QObject {
    SharingPolicies(QObject* parent = 0) : QObject(parent), sharedFolderMemberPolicy(0), sharedFolderJoinPolicy(0), sharedLinkCreatePolicy(0) {}
    ~SharingPolicies() {
        if (sharedFolderMemberPolicy != 0) {
            sharedFolderMemberPolicy->deleteLater();
        }

        if (sharedFolderJoinPolicy != 0) {
            sharedFolderJoinPolicy->deleteLater();
        }

        if (sharedLinkCreatePolicy != 0) {
            sharedFolderJoinPolicy->deleteLater();
        }
    }

    Policy* sharedFolderMemberPolicy;
    Policy* sharedFolderJoinPolicy;
    Policy* sharedLinkCreatePolicy;

    QVariantMap toMap() {
        QVariantMap map;

        if (sharedFolderMemberPolicy != 0) {
            map["shared_folder_memebr_policy"] = sharedFolderMemberPolicy->toMap();
        }

        if (sharedFolderJoinPolicy != 0) {
            map["shared_folder_join_policy"] = sharedFolderJoinPolicy->toMap();
        }

        if (sharedLinkCreatePolicy != 0) {
            map["shared_link_create_policy"] = sharedLinkCreatePolicy->toMap();
        }

        return map;
    }

    void fromMap(const QVariantMap& map) {
        if (map.contains("shared_folder_memebr_policy")) {
            if (sharedFolderMemberPolicy != 0) {
                delete sharedFolderMemberPolicy;
            }
            sharedFolderMemberPolicy = new Policy(this);
            sharedFolderMemberPolicy->fromMap(map.value("shared_folder_memebr_policy").toMap());
        }

        if (map.contains("shared_folder_join_policy")) {
            if (sharedFolderJoinPolicy != 0) {
                delete sharedFolderJoinPolicy;
            }
            sharedFolderJoinPolicy = new Policy(this);
            sharedFolderJoinPolicy->fromMap(map.value("shared_folder_join_policy").toMap());
        }

        if (map.contains("shared_link_create_policy")) {
            if (sharedFolderJoinPolicy != 0) {
                delete sharedLinkCreatePolicy;
            }
            sharedLinkCreatePolicy = new Policy(this);
            sharedLinkCreatePolicy->fromMap(map.value("shared_link_create_policy").toMap());
        }
    }
};

struct Team : public QObject {
    Team(QObject* parent = 0) : QObject(parent), officeAddinPolicy(0), sharingPolicies(0) {}
    ~Team() {
        if (officeAddinPolicy != 0) {
            officeAddinPolicy->deleteLater();
        }

        if (sharingPolicies != 0) {
            sharingPolicies->deleteLater();
        }
    }

    QString id;
    QString name;
    Policy* officeAddinPolicy;
    SharingPolicies* sharingPolicies;

    QVariantMap toMap() {
        QVariantMap map;
        map["id"] = id;
        map["name"] = name;

        if (officeAddinPolicy != 0) {
            map["office_addin_policy"] = officeAddinPolicy->toMap();
        }

        if (sharingPolicies != 0) {
            map["sharing_policies"] = sharingPolicies->toMap();
        }

        return map;
    }

    void fromMap(const QVariantMap& map) {
        id = map.value("id").toString();
        name = map.value("name").toString();
        if (map.contains("office_addin_policy")) {
            if (officeAddinPolicy != 0) {
                delete officeAddinPolicy;
            }
            officeAddinPolicy = new Policy(this);
            officeAddinPolicy->fromMap(map.value("office_addin_policy").toMap());
        }

        if (map.contains("sharing_policies")) {
            if (sharingPolicies != 0) {
                delete sharingPolicies;
            }
            sharingPolicies = new SharingPolicies(this);
            sharingPolicies->fromMap(map.value("sharing_policies").toMap());
        }
    }
};

struct Name : public QObject {
    Name(QObject* parent = 0) : QObject(parent) {}

    QString givenName;
    QString surname;
    QString familiarName;
    QString displayName;
    QString abbreviatedName;

    QVariantMap toMap() {
        QVariantMap map;
        map["given_name"] = givenName;
        map["surname"] = surname;
        map["familiar_name"] = familiarName;
        map["display_name"] = displayName;
        map["abbreviated_name"] = abbreviatedName;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        givenName = map.value("given_name", "").toString();
        surname = map.value("surname", "").toString();
        familiarName = map.value("familiar_name", "").toString();
        displayName = map.value("display_name", "").toString();
        abbreviatedName = map.value("abbreviated_name", "").toString();
    }
};

struct AccountType : public QObject {
    AccountType(QObject* parent = 0) : QObject(parent) {}

    QString tag;

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag", "").toString();
    }
};

struct RootInfo : public QObject {
    RootInfo(QObject* parent = 0) : QObject(parent) {}

    QString tag;
    QString rootNamespaceId;
    QString homeNamespaceId;

    QVariantMap toMap() {
        QVariantMap map;
        map[".tag"] = tag;
        map["root_namespace_id"] = rootNamespaceId;
        map["home_namespace_id"] = homeNamespaceId;
        return map;
    }

    void fromMap(const QVariantMap& map) {
        tag = map.value(".tag", "").toString();
        rootNamespaceId = map.value("root_namespace_id", "").toString();
        homeNamespaceId = map.value("home_namespace_id", "").toString();
    }
};

class Account: public QObject {
    Q_OBJECT
public:
    Account(QObject* parent = 0);
    virtual ~Account();

    const QString& getAccountId() const;
    Account& setAccountId(const QString& accountId);

    Name* getName() const;
    Account& setName(Name* name);

    const QString& getEmail() const;
    Account& setEmail(const QString& email);

    const bool& isEmailVerified() const;
    Account& setEmailVerified(const bool& emailVerified);

    const QString& getProfilePhotoUrl() const;
    Account& setProfilePhotoUrl(const QString& profilePhotoUrl);

    const bool& isDisabled() const;
    Account& setDisabled(const bool& disabled);

    const QString& getCountry() const;
    Account& setCountry(const QString& country);

    const QString& getLocale() const;
    Account& setLocale(const QString& locale);

    const QString& getReferralLink() const;
    Account& setReferralLink(const QString& referralLink);

    const bool& isPaired() const;
    Account& setPaired(const bool& paired);

    const bool& isTeammate() const;
    Account& setTeammate(const bool& teammate);

    AccountType* getAccountType() const;
    Account& setAccountType(AccountType* accountType);

    RootInfo* getRootInfo() const;
    Account& setRootInfo(RootInfo* rootInfo);

    const QString& getTeamMemberId() const;
    Account& setTeamMemberId(const QString& teamMemberId);

    Team* getTeam() const;
    Account& setTeam(Team* team);

    QVariantMap toMap();
    void fromMap(const QVariantMap& map);

private:
    QString m_accountId;
    Name* m_pName;
    QString m_email;
    bool m_emailVerified;
    QString m_profilePhotoUrl;
    bool m_disabled;
    QString m_country;
    QString m_locale;
    QString m_referralLink;
    bool m_isPaired;
    bool m_isTeammate;
    AccountType* m_pAccountType;
    RootInfo* m_pRootInfo;
    QString m_teamMemberId;
    Team* m_pTeam;
};

#endif /* ACCOUNT_HPP_ */
