/*
 * QDropboxTempLink.h
 *
 *  Created on: Dec 15, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXTEMPLINK_HPP_
#define QDROPBOXTEMPLINK_HPP_

#include <QObject>
#include <QVariantMap>
#include "QDropboxFile.hpp"

class QDropboxTempLink: public QObject {
    Q_OBJECT
public:
    QDropboxTempLink(QObject* parent = 0);
    virtual ~QDropboxTempLink();

    const QString& getLink() const;
    QDropboxTempLink& setLink(const QString& link);

    const QDropboxFile& getFile() const;
    QDropboxTempLink& setFile(const QDropboxFile& file);

    QVariantMap toMap();
    QDropboxTempLink& fromMap(const QVariantMap& map);

private:
    QString m_link;
    QDropboxFile m_file;
};

#endif /* QDROPBOXTEMPLINK_HPP_ */
