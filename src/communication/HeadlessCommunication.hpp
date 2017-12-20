/*
 * HeadlessCommunication.hpp
 *
 *  Created on: Dec 16, 2017
 *      Author: doctorrokter
 */

#ifndef HEADLESSCOMMUNICATION_HPP_
#define HEADLESSCOMMUNICATION_HPP_

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "../Logger.hpp"

class HeadlessCommunication: public QObject {
    Q_OBJECT
public:
    HeadlessCommunication(QObject* parent = 0);
    virtual ~HeadlessCommunication();

    Q_INVOKABLE void send(const QString& command);

    Q_SIGNALS:
        void commandReceived(const QString& command);

private slots:
    void newConnection();
    void readyRead();

private:
    static Logger logger;
    QTcpServer* m_pServer;
    QTcpSocket* m_pSocket;
};

#endif /* HEADLESSCOMMUNICATION_HPP_ */
