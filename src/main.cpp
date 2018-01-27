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
#include "components/ThumbnailImageView.hpp"
#include "components/FileImageView.hpp"
#include "vendor/WebImageView.h"
#include <QTimer>
#include "vendor/Console.hpp"
#include "cache/QDropboxPoller.hpp"

using namespace bb::cascades;

void myMessageOutput(QtMsgType type, const char* msg) {  // <-- ADD THIS
    Q_UNUSED(type);
    fprintf(stdout, "%s\n", msg);
    fflush(stdout);

    QSettings settings;
    if (settings.value("sendToConsoleDebug", true).toBool()) {
        Console* console = new Console();
        console->sendMessage("ConsoleThis$$" + QString(msg));
        console->deleteLater();
    }
}

Q_DECL_EXPORT int main(int argc, char **argv) {
    qInstallMsgHandler(myMessageOutput);

    qRegisterMetaType<QDropboxController*>("QDropboxController*");
    qRegisterMetaType<QDropboxPoller*>("QDropboxPoller*");
    qmlRegisterType<WebImageView>("WebImageView", 1, 0, "WebImageView");
    qmlRegisterType<ThumbnailImageView>("components", 1, 0, "ThumbnailImageView");
    qmlRegisterType<FileImageView>("components", 1, 0, "FileImageView");
    qmlRegisterType<QTimer>("basket.helpers", 1, 0, "Timer");

    QTextCodec *codec1 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec1);
    QTextCodec::setCodecForTr(codec1);
    QTextCodec::setCodecForCStrings(codec1);

    Application app(argc, argv);
    ApplicationUI appui;
    return Application::exec();
}
