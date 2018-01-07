/*
 * FileImageView.hpp
 *
 *  Created on: Jan 6, 2018
 *      Author: doctorrokter
 */

#ifndef FILEIMAGEVIEW_HPP_
#define FILEIMAGEVIEW_HPP_

#include <QObject>
#include <bb/cascades/ImageView>
#include <bb/cascades/Container>
#include "../util/FileUtil.hpp"

using namespace bb::cascades;

class FileImageView: public bb::cascades::ImageView {
    Q_OBJECT
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
public:
    FileImageView(Container* container = 0);
    virtual ~FileImageView();

    const QString& getPath() const;
    void setPath(const QString& path);

    static void setFileUtil(FileUtil* fileUtil);

    Q_SIGNALS:
        void pathChanged(const QString& path);
private:
    QString m_path;
    QString m_localPath;

    static FileUtil* m_pFile;

    void loadImageFromFile(const QString& localPath);
};

#endif /* FILEIMAGEVIEW_HPP_ */
