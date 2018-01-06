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

FileUtil FileImageView::m_file;

FileImageView::FileImageView(Container* container) : bb::cascades::ImageView(container) {}

FileImageView::~FileImageView() {}

const QString& FileImageView::getPath() const { return m_path; }
void FileImageView::setPath(const QString& path) {
    m_path = path;
    QString ext = m_file.extension(m_path);
    if (m_file.isImage(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_picture.png"));
    } else if (m_file.isVideo(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_video.png"));
    } else if (m_file.isAudio(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_music.png"));
    } else if (m_file.isPdf(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_pdf.png"));
    } else if (m_file.isDoc(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_doc.png"));
    } else if (m_file.isSpreadSheet(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_xls.png"));
    } else if (m_file.isPresentation(ext)) {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_ppt.png"));
    } else {
        loadImageFromFile(QString(IMAGES_ASSETS_DIR).append("/ic_doctype_generic.png"));
    }
    setFilterColor(ui()->palette()->textOnPlain());
    emit pathChanged(m_path);
}

void FileImageView::loadImageFromFile(const QString& localPath) {
    QFile imageFile(localPath);
    if (imageFile.open(QIODevice::ReadOnly)) {
        QByteArray imageData = imageFile.readAll();
        setImage(Image(imageData));
        imageFile.close();
    }
}
