/*
 * QDropboxTag.hpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXTAG_HPP_
#define QDROPBOXTAG_HPP_

#include <QObject>
#include <QMap>

class QDropboxTag: public QObject {
    Q_OBJECT
public:
    QDropboxTag(QObject* parent = 0);
    virtual ~QDropboxTag();

    QDropboxTag& operator=(const QDropboxTag& tag);

    enum Tag {
        EMAIL,
        DROPBOX_ID,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const Tag& value() const;
    QDropboxTag& value(const Tag& value);

private:
    Tag m_tag;
    QMap<QString, Tag> map;
};

#endif /* QDROPBOXTAG_HPP_ */
