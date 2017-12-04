/*
 * Account.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: doctorrokter
 */

#include "Account.hpp"

Account::Account(QObject* parent) : QObject(parent), m_accountId(""), m_pName(0), m_email(""), m_emailVerified(false), m_profilePhotoUrl(""),
m_disabled(false), m_country(""), m_locale(""), m_referralLink(""), m_isPaired(false), m_isTeammate(false), m_pAccountType(0), m_pRootInfo(0), m_teamMemberId(""), m_pTeam(0) {}

Account::~Account() {
    if (m_pName != 0) {
        m_pName->deleteLater();
    }

    if (m_pAccountType != 0) {
        m_pAccountType->deleteLater();
    }

    if (m_pRootInfo != 0) {
        m_pRootInfo->deleteLater();
    }

    if (m_pTeam != 0) {
        m_pTeam->deleteLater();
    }
}

const QString& Account::getAccountId() const { return m_accountId; }
Account& Account::setAccountId(const QString& accountId) {
    m_accountId = accountId;
    return *this;
}

Name* Account::getName() const { return m_pName; }
Account& Account::setName(Name* name) {
    if (m_pName != 0) {
        delete m_pName;
    }
    m_pName = name;
    return *this;
}

const QString& Account::getEmail() const { return m_email; }
Account& Account::setEmail(const QString& email) {
    m_email = email;
    return *this;
}

const bool& Account::isEmailVerified() const { return m_emailVerified; }
Account& Account::setEmailVerified(const bool& emailVerified) {
    m_emailVerified = emailVerified;
    return *this;
}

const QString& Account::getProfilePhotoUrl() const { return m_profilePhotoUrl; }
Account& Account::setProfilePhotoUrl(const QString& profilePhotoUrl) {
    m_profilePhotoUrl = profilePhotoUrl;
    return *this;
}

const bool& Account::isDisabled() const { return m_disabled; }
Account& Account::setDisabled(const bool& disabled) {
    m_disabled = disabled;
    return *this;
}

const QString& Account::getCountry() const { return m_country; }
Account& Account::setCountry(const QString& country) {
    m_country = country;
    return *this;
}

const QString& Account::getLocale() const { return m_locale; }
Account& Account::setLocale(const QString& locale) {
    m_locale = locale;
    return *this;
}

const QString& Account::getReferralLink() const { return m_referralLink; }
Account& Account::setReferralLink(const QString& referralLink) {
    m_referralLink = referralLink;
    return *this;
}

const bool& Account::isPaired() const { return m_isPaired; }
Account& Account::setPaired(const bool& paired) {
    m_isPaired = paired;
    return *this;
}

const bool& Account::isTeammate() const { return m_isTeammate; }
Account& Account::setTeammate(const bool& teammate) {
    m_isTeammate = teammate;
    return *this;
}

AccountType* Account::getAccountType() const { return m_pAccountType; }
Account& Account::setAccountType(AccountType* accountType) {
    if (m_pAccountType != 0) {
        delete m_pAccountType;
    }
    m_pAccountType = accountType;
    return *this;
}

RootInfo* Account::getRootInfo() const { return m_pRootInfo; }
Account& Account::setRootInfo(RootInfo* rootInfo) {
    if (m_pRootInfo != 0) {
        delete m_pRootInfo;
    }
    m_pRootInfo = rootInfo;
    return *this;
}

const QString& Account::getTeamMemberId() const { return m_teamMemberId; }
Account& Account::setTeamMemberId(const QString& teamMemberId) {
    m_teamMemberId = teamMemberId;
    return *this;
}

Team* Account::getTeam() const { return m_pTeam; }
Account& Account::setTeam(Team* team) {
    if (m_pTeam != 0) {
        delete m_pTeam;
    }
    m_pTeam = team;
    return *this;
}

QVariantMap Account::toMap() {
    QVariantMap map;
    map["account_id"] = m_accountId;
    map["email"] = m_email;
    map["email_verified"] = m_emailVerified;
    map["profile_photo_url"] = m_profilePhotoUrl;
    map["disabled"] = m_disabled;
    map["country"] = m_country;
    map["locale"] = m_locale;
    map["referral_link"] = m_referralLink;
    map["is_paired"] = m_isPaired;
    map["is_teammate"] = m_isTeammate;
    map["team_member_id"] = m_teamMemberId;

    if (m_pName != 0) {
        map["name"] = m_pName->toMap();
    }

    if (m_pAccountType != 0) {
        map["account_type"] = m_pAccountType->toMap();
    }

    if (m_pRootInfo != 0) {
        map["root_info"] = m_pRootInfo->toMap();
    }

    if (m_pTeam != 0) {
        map["team"] = m_pTeam->toMap();
    }

    return map;
}

void Account::fromMap(const QVariantMap& map) {
    m_accountId = map.value("account_id", "").toString();
    m_email = map.value("email", "").toString();
    m_emailVerified = map.value("email_verified", false).toBool();
    m_profilePhotoUrl = map.value("profile_photo_url", "").toString();
    m_disabled = map.value("disabled", false).toBool();
    m_country = map.value("country", "").toString();
    m_locale = map.value("locale", "").toString();
    m_referralLink = map.value("referral_link", "").toString();
    m_isPaired = map.value("is_paired", false).toBool();
    m_isTeammate = map.value("is_teammate", false).toBool();
    m_teamMemberId = map.value("team_member_id", "").toString();

    if (map.contains("name")) {
        Name* pName = new Name(this);
        pName->fromMap(map.value("name").toMap());
        setName(pName);
    }

    if (map.contains("account_type")) {
        AccountType* pAccType = new AccountType(this);
        pAccType->fromMap(map.value("account_type").toMap());
        setAccountType(pAccType);
    }

    if (map.contains("root_info")) {
        RootInfo* pRoot = new RootInfo(this);
        pRoot->fromMap(map.value("root_info").toMap());
        setRootInfo(pRoot);
    }

    if (map.contains("team")) {
        Team* pTeam = new Team(this);
        pTeam->fromMap(map.value("team").toMap());
        setTeam(pTeam);
    }

}

