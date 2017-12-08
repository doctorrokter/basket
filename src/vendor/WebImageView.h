#ifndef WEBIMAGEVIEW_H_
#define WEBIMAGEVIEW_H_

// Sets to -1 to disabled deleting cached images
#define MAX_NUMBER_OF_IMAGES_SAVED 500

#include <bb/cascades/ImageView>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QUrl>

using namespace bb::cascades;

class WebImageView: public bb::cascades::ImageView {
	Q_OBJECT
	Q_PROPERTY (QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY (float loading READ loading NOTIFY loadingChanged)

public:
	WebImageView();
    virtual ~WebImageView() {}

	const QUrl& url() const;
	double loading() const;

    Q_INVOKABLE void clearCache();

    Q_SIGNALS:
        void urlChanged();
        void loadingChanged();

public Q_SLOTS:
	void setUrl(QUrl url);

private Q_SLOTS:
	void imageLoaded();
	void dowloadProgressed(qint64,qint64);

private:
	static QNetworkAccessManager* mNetManager;
	static QNetworkDiskCache* mNetworkDiskCache;

    QUrl mUrl;
	float mLoading;

	bool isARedirectedUrl(QNetworkReply *reply);
	void setURLToRedirectedUrl(QNetworkReply *reply);
	void loadFromFile(QString filePath);
	void releaseSomeCache(const int& maxNumberOfImagesSaved);
};

#endif /* WEBIMAGEVIEW_H_ */
