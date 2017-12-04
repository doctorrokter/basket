/*
 * Logger.hpp
 *
 *  Created on: Nov 30, 2017
 *      Author: doctorrokter
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <QUrl>
#include <QVariantList>
#include <QNetworkReply>
#include <QDebug>

class Logger: public QObject {
    Q_OBJECT
public:
    Logger(const QString& clazz, QObject* parent = 0);
    Logger(const Logger& logger);
    virtual ~Logger();

    static Logger getLogger(const QString& clazz);

    const QString& getClass() const;

    template <typename T> void info(const T& t) {
        log("INFO", t);
    }

    template <typename T> void error(const T& t) {
        log("ERROR", t);
    }

    template <typename T> void debug(const T& t) {
        log("DEBUG", t);
    }

    template <typename T> void warn(const T& t) {
        log("WARN", t);
    }

private:
    QString m_class;

    QString currDateString();

    template <typename T> void log(const QString& level, const T& t) {
        qDebug() << "[" << level << "]" << "[" << currDateString() << "] -" << m_class << "-" << t << endl;
    }
};

#endif /* LOGGER_HPP_ */
