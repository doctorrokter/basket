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

struct Visibility : public QObject {
    Visibility(QObject* parent = 0) : QObject(parent) {}

    bool operator==(const Visibility& visibility) {
        return tag.compare(visibility.tag) == 0;
    }

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

class SharedLink: public QObject {
    Q_OBJECT
public:
    SharedLink(QObject* parent = 0);
    SharedLink(const QString& url, const QString& password, QObject* parent = 0);
    virtual ~SharedLink();

    bool operator==(const SharedLink& link);

    const QString& getUrl() const;
    SharedLink& setUrl(const QString& url);

    const QString& getPassword() const;
    SharedLink& setPassword(const QString& password);

    const QString& getPath() const;
    SharedLink& setPath(const QString& path);

    Visibility* getVisibility() const;
    SharedLink& setVisibility(Visibility* visiblity);

    bool isEmpty();
    QVariantMap toMap();
    void fromMap(const QVariantMap& map);

private:
    QString m_url;
    QString m_password;
    QString m_path;
    Visibility* m_pVisibility;
};

#endif /* SHAREDLINK_HPP_ */
