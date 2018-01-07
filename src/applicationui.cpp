/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeQueryTargetsRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/system/SystemUiPosition.hpp>
#include <bb/cascades/ThemeSupport>
#include <QSettings>
#include <QDir>
#include "Common.hpp"
#include <QTime>
#include <QUrl>
#include <bb/system/Clipboard>
#include <QFile>
#include "components/ThumbnailImageView.hpp"

Logger ApplicationUI::logger = Logger::getLogger("ApplicationUI");

ApplicationUI::ApplicationUI() :
        QObject(),
        m_translator(new QTranslator(this)),
        m_localeHandler(new LocaleHandler(this)),
        m_invokeManager(new InvokeManager(this)),
        m_pAccount(0),
        m_pFileUtil(new FileUtil(this)),
        m_watchCamera(false) {

    QCoreApplication::setOrganizationName("mikhail.chachkouski");
    QCoreApplication::setApplicationName("Basket");

    QString theme = prop("theme", "BRIGHT").toString();
    if (theme.compare("DARK") == 0) {
        Application::instance()->themeSupport()->setVisualStyle(VisualStyle::Dark);
    } else {
        Application::instance()->themeSupport()->setVisualStyle(VisualStyle::Bright);
    }

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    m_palette << "#323232" << "#0092CC" << "#FF3333" << "#DCD427" << "#779933" << "#282828" << "#087099" << "#CC3333" << "#B7B327" << "#5C7829";
    m_downloadsFolder = QDir::currentPath() + "/shared/downloads/basket";

    QString df = m_settings.value("date_format", "").toString();
    m_pDateUtil = new DateUtil(df, this);

//    QString token = m_settings.value(ACCESS_TOKEN_KEY, "").toString();
    QString token = "u_XewBWc388AAAAAAAAGvByN0abEdptmv4bv09iheXnQlZZvcgCwJwJ30xBnrCqh";

    QmlDocument* qml = 0;
    if (token.compare("") == 0) {
        m_pQdropbox = new QDropbox(CLIENT_SECRET, CLIENT_ID, REDIRECT_URL, this);
        qml = QmlDocument::create("asset:///pages/AuthPage.qml").parent(this);
    } else {
        m_pQdropbox = new QDropbox(token, this);
        ThumbnailImageView::m_accessToken = token;
        qml = QmlDocument::create("asset:///main.qml").parent(this);
    }
    m_pQdropbox->setDownloadsFolder(m_downloadsFolder);
    m_pQdropboxController = new QDropboxController(m_pQdropbox, m_pFileUtil, this);

    setAutoloadEnabled(prop("autoload.camera.files", false).toBool());

    initSignals();
    onSystemLanguageChanged();
    configureQml();
    startHeadless();

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);
}

ApplicationUI::~ApplicationUI() {
    m_invokeManager->deleteLater();
    m_localeHandler->deleteLater();
    m_translator->deleteLater();
    m_pQdropbox->deleteLater();
    m_pQdropboxController->deleteLater();
    if (m_pAccount != 0) {
        m_pAccount->deleteLater();
    }
    foreach(SharedLink* l, m_sharedLinksMap.values()) {
        l->deleteLater();
    }
}

void ApplicationUI::onSystemLanguageChanged() {
    QCoreApplication::instance()->removeTranslator(m_translator);
    QString locale_string = QLocale().name();
    QString file_name = QString("Basket_%1").arg(locale_string);
    if (m_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_translator);
    }
}

QVariant ApplicationUI::prop(const QString& key, const QVariant& defaultValue) {
    return m_settings.value(key, defaultValue);
}

void ApplicationUI::setProp(const QString& key, const QVariant& val) {
    logger.debug("Settings changed: " + key);
    if (key.compare(AUTOLOAD_SETTINGS) == 0) {
        bool enabled = val.toBool();
        setAutoloadEnabled(enabled);
    }

    m_settings.setValue(key, val);
    m_settings.sync();
    emit propChanged(key, val);
}

void ApplicationUI::resendNotification() {
    InvokeRequest request;
    request.setTarget("chachkouski.BasketService");
    request.setAction("chachkouski.BasketService.RESET");
    m_invokeManager->invoke(request);
    Application::instance()->minimize();
}

void ApplicationUI::authorize() {
    InvokeRequest req;
    req.setTarget("sys.browser");
    req.setAction("bb.action.OPEN");
    req.setUri(m_pQdropbox->authUrl());
    m_invokeManager->invoke(req);
}

void ApplicationUI::logout() {
    m_pQdropbox->authTokenRevoke();
    setProp("theme", "BRIGHT");

    m_settings.remove(ACCESS_TOKEN_KEY);
    m_settings.sync();

    m_pQdropbox->setAccessToken("");
    bool res = QObject::disconnect(m_pQdropbox, SIGNAL(sharedLinksLoaded(const QList<SharedLink*>&)), this, SLOT(onSharedLinksLoaded(const QList<SharedLink*>&)));
    Q_ASSERT(res);
    res = QObject::disconnect(m_pQdropbox, SIGNAL(sharedLinkCreated(SharedLink*)), this, SLOT(onSharedLinkCreated(SharedLink*)));
    Q_ASSERT(res);

    delete m_pQdropbox;

    m_pQdropbox = new QDropbox(CLIENT_SECRET, CLIENT_ID, REDIRECT_URL, this);
    m_pQdropbox->setDownloadsFolder(m_downloadsFolder);
    res = QObject::connect(m_pQdropbox, SIGNAL(sharedLinksLoaded(const QList<SharedLink*>&)), this, SLOT(onSharedLinksLoaded(const QList<SharedLink*>&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQdropbox, SIGNAL(sharedLinkCreated(SharedLink*)), this, SLOT(onSharedLinkCreated(SharedLink*)));
    Q_ASSERT(res);
    Q_UNUSED(res);

    QmlDocument* qml = QmlDocument::create("asset:///pages/AuthPage.qml").parent(this);
    configureQml();
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);
}

void ApplicationUI::toast(const QString& message) {
    m_toast.setBody(message);
    m_toast.setPosition(SystemUiPosition::MiddleCenter);
    m_toast.show();
}

void ApplicationUI::auth(const QString& token) {
    onAccessTokenObtained(token);
}

void ApplicationUI::onAccessTokenObtained(const QString& accessToken) {
    if (!accessToken.isEmpty()) {
        m_settings.setValue(ACCESS_TOKEN_KEY, accessToken);
        m_settings.sync();

        m_pQdropbox->setAccessToken(accessToken);
        ThumbnailImageView::m_accessToken = accessToken;

        QmlDocument* qml = QmlDocument::create("asset:///main.qml").parent(this);
        configureQml();
        AbstractPane *root = qml->createRootObject<AbstractPane>();
        Application::instance()->setScene(root);
    }
}

void ApplicationUI::onCurrentAccountLoaded(Account* account) {
    if (m_pAccount != 0) {
        delete m_pAccount;
    }
    account->setParent(this);
    m_pAccount = account;
    emit currentAccountLoaded(m_pAccount->toMap());
}

void ApplicationUI::configureQml() {
    QDeclarativeEngine* engine = QmlDocument::defaultDeclarativeEngine();
    QDeclarativeContext* rootContext = engine->rootContext();
    rootContext->setContextProperty("_app", this);
    rootContext->setContextProperty("_qdropbox", m_pQdropboxController);
    rootContext->setContextProperty("_file", m_pFileUtil);
    rootContext->setContextProperty("_date", m_pDateUtil);
}

QString ApplicationUI::getRandomColor() const {
    int i = qrand() % m_palette.size();
    return m_palette.at(i);
}

QString ApplicationUI::getAccountId() const {
    return m_pAccount->getAccountId();
}

bool ApplicationUI::copyToClipboard(const QString& str) {
    Clipboard clipboard;
    clipboard.clear();
    QByteArray data = str.toLatin1();
    toast(tr("Shared link copied to clipboard"));
    return clipboard.insert("text/plain", data);
}

void ApplicationUI::shareText(const QString& str) {
    InvokeQueryTargetsRequest request;
    request.setAction("bb.action.OPEN");
    request.setMimeType("text/plain");
    request.setUri(str);
    InvokeQueryTargetsReply* reply = m_invokeManager->queryTargets(request);
    bool res = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onShared()));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void ApplicationUI::onShared() {
    InvokeQueryTargetsReply* reply = qobject_cast<InvokeQueryTargetsReply*>(QObject::sender());
    delete reply;
    reply = 0;
}

void ApplicationUI::onSharedLinksLoaded(const QList<SharedLink*>& links) {
    qDebug() << "shared links loaded: " << links.size() << endl;
    foreach(SharedLink* l, links) {
        m_sharedLinksMap[l->getPath()] = l;
    }
    emit sharedLinksLoaded();
}

void ApplicationUI::onSharedLinkCreated(SharedLink* link) {
    m_sharedLinksMap[link->getPath()] = link;
    qDebug() << "shared link added: " << m_sharedLinksMap.size() << endl;
}

QVariantMap ApplicationUI::getSharedLink(const QString& path) {
    if (m_sharedLinksMap.contains(path)) {
        return m_sharedLinksMap.value(path)->toMap();
    }
    return QVariantMap();
}

void ApplicationUI::onSharedLinkRevoked(const QString& sharedLinkUrl) {
    SharedLink* link = 0;
    for (int index = 0; index < m_sharedLinksMap.values().size(); index++) {
        SharedLink* l = m_sharedLinksMap.values().at(index);
        if (l->getUrl().compare(sharedLinkUrl) == 0) {
            link = l;
            m_sharedLinksMap.keys().removeAt(index);
            return;
        }
    }
    if (link != 0) {
        link->deleteLater();
    }
}

void ApplicationUI::onInvoke(const bb::system::InvokeRequest& req) {
    qDebug() << req.action() << endl;
    qDebug() << req.target() << endl;
    qDebug() << req.mimeType() << endl;
    qDebug() << req.uri() << endl;
}

void ApplicationUI::startHeadless() {
    logger.info("Start Headless");
    InvokeRequest request;
    request.setTarget("chachkouski.BasketService");
    request.setAction("chachkouski.BasketService.START");
    InvokeTargetReply* reply = m_invokeManager->invoke(request);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(headlessInvoked()));
}

void ApplicationUI::headlessInvoked() {
    InvokeTargetReply* reply = qobject_cast<InvokeTargetReply*>(QObject::sender());
    logger.info(QString("Invoked headless success: ").append(reply->target()));
    reply->deleteLater();
}

void ApplicationUI::initSignals() {
    if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
                this, SLOT(onSystemLanguageChanged()))) {
            logger.warn("Recovering from a failed connect()");
    }

    bool res = QObject::connect(m_pQdropboxController, SIGNAL(currentAccountLoaded(Account*)), this, SLOT(onCurrentAccountLoaded(Account*)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQdropbox, SIGNAL(sharedLinksLoaded(const QList<SharedLink*>&)), this, SLOT(onSharedLinksLoaded(const QList<SharedLink*>&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQdropbox, SIGNAL(sharedLinkCreated(SharedLink*)), this, SLOT(onSharedLinkCreated(SharedLink*)));
    Q_ASSERT(res);
    res = QObject::connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoke(const bb::system::InvokeRequest&)));
    Q_ASSERT(res);
    res = QObject::connect(m_pQdropbox, SIGNAL(sharedLinkRevoked(const QString&)), this, SLOT(onSharedLinkRevoked(const QString&)));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

const bool& ApplicationUI::isAutoloadEnabled() const { return m_watchCamera; }
void ApplicationUI::setAutoloadEnabled(const bool& autoload) {
    if (m_watchCamera != autoload) {
        m_watchCamera = autoload;
        emit autoloadChanged(m_watchCamera);
    }
}
