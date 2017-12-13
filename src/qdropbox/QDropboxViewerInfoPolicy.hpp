/*
 * QDropboxViewerInfoPolicy.hpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXVIEWERINFOPOLICY_HPP_
#define QDROPBOXVIEWERINFOPOLICY_HPP_

#include <QObject>
#include <QMap>

class QDropboxViewerInfoPolicy: public QObject {
    Q_OBJECT
public:
    QDropboxViewerInfoPolicy(QObject* parent = 0);
    virtual ~QDropboxViewerInfoPolicy();

    QDropboxViewerInfoPolicy& operator=(const QDropboxViewerInfoPolicy& policy);

    enum ViewerInfoPolicy {
        ENABLED,
        DISABLED,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const ViewerInfoPolicy& value() const;
    QDropboxViewerInfoPolicy& value(const ViewerInfoPolicy& policy);

private:
    ViewerInfoPolicy m_policy;
    QMap<QString, ViewerInfoPolicy> map;
};

#endif /* QDROPBOXVIEWERINFOPOLICY_HPP_ */
