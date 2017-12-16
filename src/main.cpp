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

#include <QLocale>
#include <QTranslator>

#include <Qt/qdeclarativedebug.h>

#include "controllers/QDropboxController.hpp"
#include "vendor/WebImageView.h"
#include <QTimer>

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv) {
    qRegisterMetaType<QDropboxController*>("QDropboxController*");
    qmlRegisterType<WebImageView>("WebImageView", 1, 0, "WebImageView");
    qmlRegisterType<QTimer>("basket.helpers", 1, 0, "Timer");

    QTextCodec *codec1 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec1);
    QTextCodec::setCodecForTr(codec1);
    QTextCodec::setCodecForCStrings(codec1);

    Application app(argc, argv);
    ApplicationUI appui;
    return Application::exec();
}
