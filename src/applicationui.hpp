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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QSettings>
#include <QVariantMap>
#include <bb/cascades/QmlDocument>
#include "qdropbox/QDropbox.hpp"
#include "qdropbox/Account.hpp"
#include "controllers/QDropboxController.hpp"
#include "util/DateUtil.hpp"
#include "util/FileUtil.hpp"

namespace bb {
    namespace cascades {
        class LocaleHandler;
    }
    namespace system {
        class InvokeManager;
    }
}

using namespace bb::cascades;

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI: public QObject {
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI();

    Q_INVOKABLE QVariant prop(const QString& key, const QVariant& defaultValue = "");
    Q_INVOKABLE void setProp(const QString& key, const QVariant& val);
    Q_INVOKABLE void resendNotification();
    Q_INVOKABLE void authorize();
    Q_INVOKABLE void logout();

    Q_SIGNALS:
        void currentAccountLoaded(const QVariantMap& accountMap);
        void propChanged(const QString& key, const QVariant& val);

private slots:
    void onSystemLanguageChanged();
    void onAccessTokenObtained(const QString& accessToken);
    void onCurrentAccountLoaded(Account* account);
private:
    QSettings m_settings;

    QTranslator* m_translator;
    bb::cascades::LocaleHandler* m_localeHandler;
    bb::system::InvokeManager* m_invokeManager;

    QDropbox* m_pQdropbox;
    Account* m_pAccount;

    QDropboxController* m_pQdropboxController;
    FileUtil* m_pFileUtil;
    DateUtil* m_pDateUtil;

    QString m_downloadsFolder;

    void configureQml();
};

#endif /* ApplicationUI_HPP_ */
