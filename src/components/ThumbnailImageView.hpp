/*
 * ThumbnailImageView.hpp
 *
 *  Created on: Jan 6, 2018
 *      Author: doctorrokter
 */

#ifndef THUMBNAILIMAGEVIEW_HPP_
#define THUMBNAILIMAGEVIEW_HPP_

#include <bb/cascades/ImageView>
#include <bb/cascades/Container>
#include <QUrl>
#include <qdropbox/QDropbox.hpp>
#include "../util/FileUtil.hpp"

using namespace bb::cascades;

class ThumbnailImageView: public bb::cascades::ImageView {
    Q_OBJECT
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString size READ getSize WRITE setSize NOTIFY sizeChanged)
public:
    ThumbnailImageView(Container* container = 0);
    virtual ~ThumbnailImageView();

    const QString& getPath() const;
    void setPath(const QString& path);

    const QString& getSize() const;
    void setSize(const QString& size);

    static QString m_accessToken;

    Q_SIGNALS:
        void pathChanged(const QString& path);
        void sizeChanged(const QString& size);
        void loaded(const QString& remotePath);

private slots:
    void onThumbnailLoaded(const QString& path, const QString& size, QImage* thumbnail);

private:
    static FileUtil m_file;

    QString m_path;
    QString m_size;
    QString m_thumbs;
    QDropbox m_qdropbox;

    void reload();
    void loadImageFromFile(const QString& localPath);
};

#endif /* THUMBNAILIMAGEVIEW_HPP_ */
