/*
 * ThumbnailImageView.cpp
 *
 *  Created on: Jan 6, 2018
 *      Author: doctorrokter
 */

#include "ThumbnailImageView.hpp"
#include <QFile>
#include <QDir>
#include <bb/cascades/Image>
#include <bb/cascades/UIPalette>
#include <bb/cascades/Color>
#include <QByteArray>
#include <QObject>
#include <QtGui/QImage>
#include <QDebug>
#include "../Common.hpp"

QString ThumbnailImageView::m_accessToken;

ThumbnailImageView::ThumbnailImageView(Container* container) : bb::cascades::ImageView(container) {
    m_qdropbox.setAccessToken(m_accessToken);
    m_path = "";
    m_size = "w128h128";

    m_thumbs = QDir::currentPath() + THUMBNAILS_DIR;
    QDir dir(m_thumbs);
    if (!dir.exists()) {
        dir.mkpath(m_thumbs);
    }

    if (dir.exists()) {
        if (dir.count() >= THUMBNAILS_THRESHOLD) {
            QDirIterator it(QDir::currentPath() + THUMBNAILS_DIR, QDir::NoDotAndDotDot | QDir::Files);
            int countForDelete = 100;
            while (countForDelete != 0) {
                QString path = it.next();
                QFile file(path);
                if (file.exists()) {
                    file.remove();
                }
                countForDelete--;
            }
        }
    }

    bool res = QObject::connect(&m_qdropbox, SIGNAL(thumbnailLoaded(const QString&, const QString&, QImage*)), this, SLOT(onThumbnailLoaded(const QString&, const QString&, QImage*)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

ThumbnailImageView::~ThumbnailImageView() {
    bool res = QObject::disconnect(&m_qdropbox, SIGNAL(thumbnailLoaded(const QString&, const QString&, QImage*)), this, SLOT(onThumbnailLoaded(const QString&, const QString&, QImage*)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

const QString& ThumbnailImageView::getPath() const { return m_path; }
void ThumbnailImageView::setPath(const QString& path) {
    m_path = path;
    emit pathChanged(m_path);
    reload();
}

const QString& ThumbnailImageView::getSize() const { return m_size; }
void ThumbnailImageView::setSize(const QString& size) {
    m_size = size;
    emit sizeChanged(m_size);
    reload();
}

void ThumbnailImageView::loadImageFromFile(const QString& localPath) {
    setImageSource(localPath);
}

void ThumbnailImageView::reload() {
    if (!m_path.isEmpty()) {
        QString localPath = m_thumbs + "/" + m_size + "_" + m_path.split("/").last();
        QFile file(localPath);
        if (file.exists()) {
            loadImageFromFile(localPath);
            emit loaded(m_path);
        } else {
            loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_picture.png"));
            m_qdropbox.getThumbnail(m_path, m_size);
        }
    }
}

void ThumbnailImageView::onThumbnailLoaded(const QString& path, const QString& size, QImage* thumbnail) {
    QString localPath = m_thumbs + "/" + size + "_" + path.split("/").last();
    QFile file(localPath);
    if (!file.exists()) {
        file.open(QIODevice::WriteOnly);
        thumbnail->save(&file);
        file.close();
    }

    delete thumbnail;
    thumbnail = 0;

    loadImageFromFile(localPath);
    emit loaded(path);
}

void ThumbnailImageView::setAccessToken(const QString& accessToken) {
    m_accessToken = accessToken;
}
