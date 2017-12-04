/*
 * SharedLink.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: doctorrokter
 */

#ifndef SHAREDLINK_HPP_
#define SHAREDLINK_HPP_

#include <QObject>
#include <QVariantMap>

class SharedLink: public QObject {
    Q_OBJECT
public:
    SharedLink(QObject* parent = 0);
    SharedLink(const QString& url, const QString& password, QObject* parent = 0);
    virtual ~SharedLink();

    const QString& getUrl() const;
    SharedLink& setUrl(const QString& url);

    const QString& getPassword() const;
    SharedLink& setPassword(const QString& password);

    bool isEmpty();
    QVariantMap toMap();

private:
    QString m_url;
    QString m_password;
};

#endif /* SHAREDLINK_HPP_ */
