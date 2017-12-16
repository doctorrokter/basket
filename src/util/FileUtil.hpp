/*
 * FileUtil.hpp
 *
 *  Created on: Apr 29, 2017
 *      Author: misha
 */

#ifndef FILEUTIL_HPP_
#define FILEUTIL_HPP_

#include <QObject>
#include <QStringList>
#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeManager>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "../qdropbox/QDropboxTempLink.hpp"

using namespace bb::system;

class FileUtil: public QObject {
    Q_OBJECT
public:
    FileUtil(QObject* parent = 0);
    virtual ~FileUtil();

    Q_INVOKABLE bool isImage(const QString& ext);
    Q_INVOKABLE bool isVideo(const QString& ext);
    Q_INVOKABLE bool isAudio(const QString& ext);
    Q_INVOKABLE bool isDoc(const QString& ext);
    Q_INVOKABLE bool isSpreadSheet(const QString& ext);
    Q_INVOKABLE bool isPresentation(const QString& ext);
    Q_INVOKABLE bool isPdf(const QString& ext);
    Q_INVOKABLE QString filename(const QString& filepath);
    Q_INVOKABLE bool removeDir(const QString& dirName);
    Q_INVOKABLE QString extension(const QString& nameOrPath);

    Q_INVOKABLE void open(const QVariantMap& linkMap);

private slots:
    void onCoreInvoked();
    void onTempLinkLoaded();

private:
    QStringList m_imagesList;
    QStringList m_videoList;
    QStringList m_audioList;
    QStringList m_docList;
    QStringList m_xlsList;
    QStringList m_pptList;

    QNetworkAccessManager m_network;
    InvokeManager m_invokeManager;
    InvokeTargetReply* m_invokeReply;

    QDropboxTempLink* m_pTempLink;

    void invokeCore(InvokeRequest& request);
    void openLocalFile(const QString& path, const QString& ext);
};

#endif /* FILEUTIL_HPP_ */
