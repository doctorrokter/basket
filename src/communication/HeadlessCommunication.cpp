/*
 * HeadlessCommunication.cpp
 *
 *  Created on: Dec 16, 2017
 *      Author: doctorrokter
 */

#include "HeadlessCommunication.hpp"
#include <QHostAddress>
#include <QTcpSocket>

#define PORT 10002

Logger HeadlessCommunication::logger = Logger::getLogger("HeadlessCommunication::UI");

HeadlessCommunication::HeadlessCommunication(QObject* parent) : QObject(parent),
        m_pServer(new QTcpServer(this)) {
    m_pSocket = NULL;
    m_pServer->listen(QHostAddress::LocalHost, PORT);
    bool result = QObject::connect(m_pServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    Q_ASSERT(result);
    Q_UNUSED(result);
}

HeadlessCommunication::~HeadlessCommunication() {
    if (m_pSocket != NULL) {
        bool res = QObject::disconnect(m_pSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        Q_ASSERT(res);
        Q_UNUSED(res);
        m_pSocket->close();
        m_pSocket->deleteLater();
    }
    m_pServer->close();
    m_pServer->deleteLater();
}

void HeadlessCommunication::send(const QString& command) {
    logger.info("Trying to send command: " + command);
    if (m_pSocket != NULL) {
        m_pSocket->write(command.toUtf8());
        m_pSocket->flush();
    } else {
        logger.info("No connections available.");
    }
}

void HeadlessCommunication::newConnection() {
    m_pSocket = m_pServer->nextPendingConnection();
    if (m_pSocket->state() == QTcpSocket::ConnectedState) {
        logger.info("Socket connected");
        bool res = QObject::connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        Q_ASSERT(res);
        Q_UNUSED(res);
    } else {
        logger.info("Socket not connected: " + m_pSocket->errorString());
    }
}

void HeadlessCommunication::readyRead() {
    if (m_pSocket->bytesAvailable()) {
        QString command = m_pSocket->readAll();
        emit commandReceived(command);
    }
}

