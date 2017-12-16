/*
 * FileUtil.cpp
 *
 *  Created on: Apr 29, 2017
 *      Author: misha
 */

#include <src/util/FileUtil.hpp>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#define TEMP_DIR "/data/temp"

FileUtil::FileUtil(QObject* parent) : QObject(parent), m_invokeReply(0), m_pTempLink(0) {
    m_imagesList << "jpg" << "jpeg" << "gif" << "png";
    m_videoList << "mp4" << "avi" << "mov" << "mkv" << "3gp" << "3g2" << "asf" << "f4v" << "ismv" << "m4v" << "mpeg" << "wmv";
    m_audioList << "mp3" << "wav" << "aac" << "amr" << "flac" << "m4a" << "wma";
    m_docList << "doc" << "dot" << "txt" << "docx" << "dotx" << "docm" << "dotm" << "json" << "xml" << "js";
    m_xlsList << "xls" << "xlt" << "xlsx" << "xltx" << "xlsm" << "xltm" << "csv";
    m_pptList << "ppt" << "pot" << "pps" << "pptx" << "potx" << "ppsx" << "pptm" << "potm" << "ppsm";
}

FileUtil::~FileUtil() {
    if (m_invokeReply != 0) {
        delete m_invokeReply;
        m_invokeReply = 0;
    }

    if (m_pTempLink != 0) {
        delete m_pTempLink;
        m_pTempLink = 0;
    }
}

bool FileUtil::isImage(const QString& ext) {
    foreach(QString e, m_imagesList) {
        if (e.toLower().compare(ext.toLower()) == 0) {
            return true;
        }
    }
    return false;
}

bool FileUtil::isVideo(const QString& ext) {
    foreach(QString e, m_videoList) {
        if (e.toLower().compare(ext.toLower()) == 0) {
            return true;
        }
    }
    return false;
}

bool FileUtil::isAudio(const QString& ext) {
    foreach(QString e, m_audioList) {
        if (e.toLower().compare(ext.toLower()) == 0) {
            return true;
        }
    }
    return false;
}

bool FileUtil::isDoc(const QString& ext) {
    foreach(QString e, m_docList) {
        if (e.toLower().compare(ext.toLower()) == 0) {
            return true;
        }
    }
    return false;
}

bool FileUtil::isSpreadSheet(const QString& ext) {
    foreach(QString e, m_xlsList) {
        if (e.toLower().compare(ext.toLower()) == 0) {
            return true;
        }
    }
    return false;
}

bool FileUtil::isPresentation(const QString& ext) {
    foreach(QString e, m_pptList) {
        if (e.toLower().compare(ext.toLower()) == 0) {
            return true;
        }
    }
    return false;
}

bool FileUtil::isPdf(const QString& ext) {
    return ext.toLower().compare("pdf") == 0;
}

QString FileUtil::filename(const QString& filepath) {
    QStringList parts = filepath.split("/");
    return parts[parts.size() - 1];
}

bool FileUtil::removeDir(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        Q_FOREACH(QFileInfo info, list) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

QString FileUtil::extension(const QString& nameOrPath) {
    return nameOrPath.split(".").last();
}

void FileUtil::open(const QVariantMap& linkMap) {
    if (m_pTempLink != 0) {
        delete m_pTempLink;
    }

    QString dirPath = QDir::currentPath() + TEMP_DIR;
    QDir dir(dirPath);
    if (!dir.exists()) {
        QDir dir(dirPath);
        dir.mkpath(dirPath);
    }

    m_pTempLink = new QDropboxTempLink(this);
    m_pTempLink->fromMap(linkMap);

    QString ext = extension(m_pTempLink->getFile().getName());

    if (isAudio(ext) || isVideo(ext)) {
        QUrl url(m_pTempLink->getLink());

        InvokeRequest request;
        request.setUri(url);
        request.setTarget("sys.mediaplayer.previewer");
        invokeCore(request);
    } else {
        QNetworkRequest req;
        QUrl url(m_pTempLink->getLink());
        req.setUrl(url);

        QString name = filename(m_pTempLink->getFile().getName());
        QString localPath = QDir::currentPath() + TEMP_DIR + "/" + name;
        if (!QFile::exists(localPath)) {
            QNetworkReply* reply = m_network.get(req);
            reply->setProperty("ext", ext);
            reply->setProperty("name", name);
            bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onTempLinkLoaded()));
            Q_ASSERT(res);
            Q_UNUSED(res);
        } else {
            openLocalFile(localPath, ext);
        }
    }
}

void FileUtil::onTempLinkLoaded() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());

    if (reply->error() == QNetworkReply::NoError) {
        QString name = reply->property("name").toString();
        QString ext = reply->property("ext").toString();
        QString localPath = QDir::currentPath() + TEMP_DIR + "/" + name;
        QFile file(localPath);
        file.open(QIODevice::WriteOnly);
        file.write(reply->readAll());
        file.close();
        openLocalFile(localPath, ext);
    }

    if (m_pTempLink != 0) {
        delete m_pTempLink;
        m_pTempLink = 0;
    }

    reply->deleteLater();
}

void FileUtil::openLocalFile(const QString& path, const QString& ext) {
    InvokeRequest request;

    QUrl url("file://" + path);
    request.setUri(url);

    if (isPdf(ext)) {
        request.setTarget("com.rim.bb.app.adobeReader.viewer");
    } else if (isImage(ext)) {
        request.setTarget("sys.pictures.card.previewer");
    } else if (isDoc(ext)) {
        request.setTarget("sys.wordtogo.previewer");
    } else if (isSpreadSheet(ext)) {
        request.setTarget("sys.sheettogo.previewer");
    } else if (isPresentation(ext)) {
        request.setTarget("sys.slideshowtogo.previewer");
    }

    invokeCore(request);
}

void FileUtil::invokeCore(InvokeRequest& request) {
    if (!request.target().isEmpty()) {
        request.setAction("bb.action.VIEW");
        m_invokeReply = m_invokeManager.invoke(request);
        bool res = QObject::connect(m_invokeReply, SIGNAL(finished()), this, SLOT(onCoreInvoked()));
        Q_ASSERT(res);
        Q_UNUSED(res);
    }
}

void FileUtil::onCoreInvoked() {
    bool res = QObject::disconnect(m_invokeReply, SIGNAL(finished()), this, SLOT(onCoreInvoked()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    qDebug() << m_invokeReply->error() << endl;

    if (m_pTempLink != 0) {
        delete m_pTempLink;
        m_pTempLink = 0;
    }

    delete m_invokeReply;
    m_invokeReply = 0;
}

