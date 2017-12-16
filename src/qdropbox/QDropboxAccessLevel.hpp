/*
 * QDropboxAccessLevel.hpp
 *
 *  Created on: Dec 11, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXACCESSLEVEL_HPP_
#define QDROPBOXACCESSLEVEL_HPP_

#include <QObject>
#include <QMap>

class QDropboxAccessLevel: public QObject {
    Q_OBJECT
public:
    QDropboxAccessLevel(QObject* parent = 0);
    virtual ~QDropboxAccessLevel();

    QDropboxAccessLevel& operator=(const QDropboxAccessLevel& accessLevel);

    enum AccessLevel {
        OWNER,
        EDITOR,
        VIEWER,
        VIEWER_NO_COMMENT,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const AccessLevel& value() const;
    QDropboxAccessLevel& value(const AccessLevel& value);
    QDropboxAccessLevel& valueFromInt(const int& val);

private:
    AccessLevel m_accessLevel;
    QMap<QString, AccessLevel> map;
};

#endif /* QDROPBOXACCESSLEVEL_HPP_ */
