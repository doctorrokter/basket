/*
 * QDropboxPendingUpload.hpp
 *
 *  Created on: Dec 15, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXPENDINGUPLOAD_HPP_
#define QDROPBOXPENDINGUPLOAD_HPP_

#include <QObject>
#include <QMap>

class QDropboxPendingUpload: public QObject {
    Q_OBJECT
public:
    QDropboxPendingUpload(QObject* parent = 0);
    virtual ~QDropboxPendingUpload();

    enum PendingUpload {
        FILE,
        FOLDER,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const PendingUpload& value() const;
    QDropboxPendingUpload& value(const PendingUpload& value);

private:
    PendingUpload m_pending;
    QMap<QString, PendingUpload> map;
};

#endif /* QDROPBOXPENDINGUPLOAD_HPP_ */
