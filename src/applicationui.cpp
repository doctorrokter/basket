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
#include <QSettings>
#include <QDir>
#include "Common.hpp"
#include <QTime>
#include <bb/system/Clipboard>

#define ACCESS_TOKEN_KEY "dropbox.access_token"

ApplicationUI::ApplicationUI() :
        QObject(),
        m_translator(new QTranslator(this)),
        m_localeHandler(new LocaleHandler(this)),
        m_invokeManager(new InvokeManager(this)),
        m_pAccount(0),
        m_pFileUtil(new FileUtil(this)) {

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    m_palette << "#323232" << "#0092CC" << "#FF3333" << "#DCD427" << "#779933" << "#282828" << "#087099" << "#CC3333" << "#B7B327" << "#5C7829";

    m_downloadsFolder = QDir::currentPath() + "/shared/downloads/basket";

    if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
            this, SLOT(onSystemLanguageChanged()))) {
        qWarning() << "Recovering from a failed connect()";
    }

    onSystemLanguageChanged();

    QString df = m_settings.value("date_format", "").toString();
    m_pDateUtil = new DateUtil(df, this);

//    QString token = settings.value(ACCESS_TOKEN_KEY, "").toString();
    QString token = "u_XewBWc388AAAAAAAAF9Xc0lW_rhLW1dbzA_XoRYeGEi_6iazRrv5LMmxbJGZ0W";
    QmlDocument* qml = 0;
    if (token.compare("") == 0) {
        m_pQdropbox = new QDropbox(CLIENT_SECRET, CLIENT_ID, "basket://auth", this);
        qml = QmlDocument::create("asset:///pages/AuthPage.qml").parent(this);
    } else {
        m_pQdropbox = new QDropbox(token, this);
        qml = QmlDocument::create("asset:///main.qml").parent(this);
    }
    m_pQdropbox->setDownloadsFolder(m_downloadsFolder);
    m_pQdropboxController = new QDropboxController(m_pQdropbox, m_pFileUtil, this);
    bool res = QObject::connect(m_pQdropboxController, SIGNAL(currentAccountLoaded(Account*)), this, SLOT(onCurrentAccountLoaded(Account*)));
    Q_ASSERT(res);
    Q_UNUSED(res);

    configureQml();
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
    m_settings.setValue(key, val);
    emit propChanged(key, val);
}

void ApplicationUI::resendNotification() {
    InvokeRequest request;
    request.setTarget("com.example.BasketService");
    request.setAction("com.example.BasketService.RESET");
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
    QSettings settings;
    settings.remove(ACCESS_TOKEN_KEY);
    m_pQdropbox->setAccessToken("");
    m_pQdropbox = new QDropbox("wqynh6pf0cu5506", "q2ficti4tr8zql8", "basket://auth", this);
    m_pQdropbox->setDownloadsFolder(m_downloadsFolder);

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

void ApplicationUI::onAccessTokenObtained(const QString& accessToken) {
    if (!accessToken.isEmpty()) {
        m_settings.setValue(ACCESS_TOKEN_KEY, accessToken);
        m_pQdropbox->setAccessToken(accessToken);

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
