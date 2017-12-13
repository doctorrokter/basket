/*
 * QDropboxFolderAction.hpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#ifndef QDROPBOXFOLDERACTION_HPP_
#define QDROPBOXFOLDERACTION_HPP_

#include <QObject>
#include <QMap>

class QDropboxFolderAction: public QObject {
    Q_OBJECT
public:
    QDropboxFolderAction(QObject* parent = 0);
    QDropboxFolderAction(const QDropboxFolderAction& folderAction);
    virtual ~QDropboxFolderAction();

    QDropboxFolderAction& operator=(const QDropboxFolderAction& folderAction);

    enum FolderAction {
        CHANGE_OPTIONS,
        DISABLE_VIEWER_INFO,
        EDIT_CONTENTS,
        ENABLE_VIEWER_INFO,
        INVITE_EDITOR,
        INVITE_VIEWER,
        INVITE_VIEWER_NO_COMMENT,
        RELINQUISH_MEMBERSHIP,
        UNMOUNT,
        UNSHARE,
        LEAVE_A_COPY,
        SHARE_LINK,
        CREATE_LINK,
        NONE
    };

    QString name() const;
    void fromStr(const QString& str);

    const FolderAction& value() const;
    QDropboxFolderAction& value(const FolderAction& folderAction);

private:
    FolderAction m_folderAction;
    QMap<QString, FolderAction> map;
};

#endif /* QDROPBOXFOLDERACTION_HPP_ */
