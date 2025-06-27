#include "clientapi.h"
#include <QDebug>

ClientAPI* ClientAPI::instance = nullptr;

ClientAPI::ClientAPI(QObject* parent) : QObject(parent)
{
    mTcpSocket = new QTcpSocket(this);
}

ClientAPI::~ClientAPI()
{
    if (mTcpSocket && mTcpSocket->isOpen()) {
        mTcpSocket->disconnectFromHost();
        mTcpSocket->close();
    }
}

ClientAPI* ClientAPI::getInstance()
{
    if (!instance)
        instance = new ClientAPI();
    return instance;
}

bool ClientAPI::connectToServer(const QString& host, quint16 port)
{
    mTcpSocket->connectToHost(host, port);
    return mTcpSocket->waitForConnected(3000);
}

QByteArray ClientAPI::query_to_server(const QString& query)
{
    if (!mTcpSocket || !mTcpSocket->isOpen())
        return "❌ Ошибка: нет подключения к серверу\n";

    mTcpSocket->write(query.toUtf8() + '\n');

    if (!mTcpSocket->waitForReadyRead(3000))
        return "⏳ Ошибка: сервер не ответил вовремя\n";

    QByteArray response;
    while (mTcpSocket->waitForReadyRead(100))
        response.append(mTcpSocket->readAll());

    qDebug() << "[Ответ сервера]" << response;
    return response;
}
