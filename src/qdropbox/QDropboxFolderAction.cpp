/*
 * QDropboxFolderAction.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: doctorrokter
 */

#include "QDropboxFolderAction.hpp"

#define CHANGE_OPTIONS_STR "change_options"
#define DISABLE_VIEWER_INFO_STR "disable_viewer_info"
#define EDIT_CONTENTS_STR "edit_contents"
#define ENABLE_VIEWER_INFO_STR "enable_viewer_info"
#define INVITE_EDITOR_STR "invite_editor"
#define INVITE_VIEWER_STR "invite_viewer"
#define INVITE_VIEWER_NO_COMMENT_STR "invite_viewer_no_comment"
#define RELINQUISH_MEMBERSHIP_STR "relinquish_membership"
#define UNMOUNT_STR "unmount"
#define UNSHARE_STR "unshare"
#define LEAVE_A_COPY_STR "leave_a_copy"
#define SHARE_LINK_STR "share_link"
#define CREATE_LINK_STR "create_link"

QDropboxFolderAction::QDropboxFolderAction(QObject* parent) : QObject(parent) {
    m_folderAction = NONE;

    map[CHANGE_OPTIONS_STR] = CHANGE_OPTIONS;
    map[DISABLE_VIEWER_INFO_STR] = DISABLE_VIEWER_INFO;
    map[EDIT_CONTENTS_STR] = EDIT_CONTENTS;
    map[ENABLE_VIEWER_INFO_STR] = ENABLE_VIEWER_INFO;
    map[INVITE_EDITOR_STR] = INVITE_EDITOR;
    map[INVITE_VIEWER_STR] = INVITE_VIEWER;
    map[INVITE_VIEWER_NO_COMMENT_STR] = INVITE_VIEWER_NO_COMMENT;
    map[RELINQUISH_MEMBERSHIP_STR] = RELINQUISH_MEMBERSHIP;
    map[UNMOUNT_STR] = UNMOUNT;
    map[UNSHARE_STR] = UNSHARE;
    map[LEAVE_A_COPY_STR] = LEAVE_A_COPY;
    map[SHARE_LINK_STR] = SHARE_LINK;
    map[CREATE_LINK_STR] = CREATE_LINK;
}

QDropboxFolderAction::QDropboxFolderAction(const QDropboxFolderAction& folderAction) : QObject(folderAction.parent()) {
    m_folderAction = folderAction.value();
}

QDropboxFolderAction::~QDropboxFolderAction() {}

QString QDropboxFolderAction::name() const {
    foreach(QString k, map.keys()) {
        if (map.value(k) == m_folderAction) {
            return k;
        }
    }
    return "";
}

void QDropboxFolderAction::fromStr(const QString& str) {
    m_folderAction = map.value(str);
}

const QDropboxFolderAction::FolderAction& QDropboxFolderAction::value() const { return m_folderAction; }
QDropboxFolderAction& QDropboxFolderAction::value(const FolderAction& folderAction) {
    m_folderAction = folderAction;
    return *this;
}
