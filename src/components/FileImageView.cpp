/*
 * FileImageView.cpp
 *
 *  Created on: Jan 6, 2018
 *      Author: doctorrokter
 */

#include <QFile>
#include <bb/cascades/Image>
#include <QByteArray>
#include <bb/cascades/UIPalette>
#include <bb/cascades/Color>
#include "FileImageView.hpp"
#include "../Common.hpp"
#include <QDebug>

FileUtil* FileImageView::m_pFile = 0;

FileImageView::FileImageView(Container* container) : bb::cascades::ImageView(container) {
    m_path = "";
    m_localPath = "";
}

FileImageView::~FileImageView() {}

const QString& FileImageView::getPath() const { return m_path; }
void FileImageView::setPath(const QString& path) {
    m_path = path;
    QString ext = m_pFile->extension(m_path);
    if (m_pFile->isImage(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_picture.png"));
    } else if (m_pFile->isVideo(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_video.png"));
    } else if (m_pFile->isAudio(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_music.png"));
    } else if (m_pFile->isPdf(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_pdf.png"));
    } else if (m_pFile->isDoc(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_doc.png"));
    } else if (m_pFile->isSpreadSheet(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_xls.png"));
    } else if (m_pFile->isPresentation(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_ppt.png"));
    } else {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_generic.png"));
    }
    setFilterColor(ui()->palette()->textOnPlain());
    emit pathChanged(m_path);
}

void FileImageView::loadImageFromFile(const QString& localPath) {
    if (m_localPath.compare(localPath) != 0) {
        m_localPath = localPath;
        QFile imageFile(m_localPath);
        if (imageFile.open(QIODevice::ReadOnly)) {
            QByteArray imageData = imageFile.readAll();
            setImage(Image(imageData));
            imageFile.close();
        }
    }
}

void FileImageView::setFileUtil(FileUtil* fileUtil) {
    m_pFile = fileUtil;
}
