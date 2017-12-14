#include "WebImageView.h"

#include <QtGui/QDesktopServices>
#include <QNetworkReply>
#include <bb/cascades/Image>

#include <QFile>
#include <QDir>
#include <QFileInfo>

using namespace bb::cascades;

QNetworkAccessManager* WebImageView::mNetManager = new QNetworkAccessManager();
QNetworkDiskCache* WebImageView::mNetworkDiskCache = new QNetworkDiskCache();

WebImageView::WebImageView() {
    // Creates the folder if it doesn't exist
    QFileInfo imageDir(QDir::currentPath() + "/data/images/");
    if (!imageDir.exists()) {
        QDir().mkpath(imageDir.path());
    }

    // Initialize network cache
    mNetworkDiskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));

    // Set cache in manager
    mNetManager->setCache(mNetworkDiskCache);

    // Set defaults
    mLoading = 0;
}

const QUrl& WebImageView::url() const {
    return mUrl;
}

void WebImageView::setUrl(QUrl url) {
    // Variables
    mUrl = url;
    mLoading = 0;

    // Reset the image
    resetImage();

    QString fileName = url.toString().split("/").last();
    QFileInfo imageFile(QDir::currentPath() + "/data/images/" + fileName);

    // If image doesn' exists, download it, otherwise reuse the image saved
    if (!imageFile.exists()) {
        // Create request
        QNetworkRequest request;
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        request.setUrl(url);

        // Create reply
        QNetworkReply * reply = mNetManager->get(request);

        // Connect to signals
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
        QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(dowloadProgressed(qint64,qint64)));
    }
    else{
        loadFromFile(imageFile.filePath());
    }

    emit urlChanged();
}

void WebImageView::loadFromFile(QString filePath) {
    QFile imageFile(filePath);
    if (imageFile.open(QIODevice::ReadOnly)) {
        QByteArray imageData = imageFile.readAll();
        setImage(Image(imageData));
        mLoading = 1;
        emit loadingChanged();
    }
}

double WebImageView::loading() const {
    return mLoading;
}

void WebImageView::imageLoaded() {
    // Get reply
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        if (isARedirectedUrl(reply)) {
            setURLToRedirectedUrl(reply);
            return;
        } else {
            QString fileName = reply->url().toString().split("/").last();
            QByteArray imageData = reply->readAll();
            QFile imageFile(QDir::currentPath() + "/data/images/" + fileName);
            if (imageFile.open(QIODevice::WriteOnly)) {
                imageFile.write(imageData);
                imageFile.close();
                setImage(Image(imageData));

                releaseSomeCache(MAX_NUMBER_OF_IMAGES_SAVED);
            }
        }
    } else {
        qDebug() << reply->error() << endl;
        qDebug() << reply->errorString() << endl;
    }

    // Memory management
    reply->deleteLater();
}

bool WebImageView::isARedirectedUrl(QNetworkReply *reply) {
    QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    return !redirection.isEmpty();
}

void WebImageView::setURLToRedirectedUrl(QNetworkReply *reply) {
    QUrl redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    QUrl baseUrl = reply->url();
    QUrl resolvedUrl = baseUrl.resolved(redirectionUrl);

    setUrl(resolvedUrl.toString());
}

void WebImageView::clearCache() {
    mNetworkDiskCache->clear();

    QDir imageDir(QDir::currentPath() + "/data/images");
    imageDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(const QString& file, imageDir.entryList()){
        imageDir.remove(file);
    }
}

void WebImageView::releaseSomeCache(const int& maxNumberOfImagesSaved) {
    if (maxNumberOfImagesSaved < 0)
        return;

    QDir imageDir(QDir::currentPath() + "/data/images");
    imageDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    imageDir.setSorting(QDir::Time | QDir::Reversed);

    QStringList entryList = imageDir.entryList();
    for (int i = maxNumberOfImagesSaved; i < entryList.size(); i++) {
        imageDir.remove(entryList[i]);
    }
}

void WebImageView::dowloadProgressed(qint64 bytes, qint64 total) {
    mLoading = double(bytes) / double(total);

    emit loadingChanged();
}
