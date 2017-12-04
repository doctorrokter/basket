/*
 * DateUtil.cpp
 *
 *  Created on: May 7, 2017
 *      Author: misha
 */

#include <src/util/DateUtil.hpp>
#include "../Common.hpp"
#include <QDebug>
#include <time.h>

DateUtil::DateUtil(const QString& dateFormat, QObject* parent) : QObject(parent), m_dateFormat(dateFormat) {}

DateUtil::~DateUtil() {}

void DateUtil::setDateFormat(const QString& dateFormat) {
    m_dateFormat = dateFormat;
}

QString DateUtil::str(const QDateTime& dateTime) {
    QDateTime dt = dateTime;
    dt = dt.addSecs(-timezone);

    if (m_dateFormat.isEmpty() || m_dateFormat.compare(DATE_FORMAT) == 0) {
        return dt.toString(DATE_FORMAT);
    }
    return dt.toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
}
