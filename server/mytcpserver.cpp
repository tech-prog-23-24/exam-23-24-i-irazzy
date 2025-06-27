#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject* parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &MyTcpServer::onNewConnection);

    if (!server->listen(QHostAddress::Any, 33333)) {
        qDebug() << "❌ Не удалось запустить сервер";
    } else {
        qDebug() << "✅ Сервер запущен на порту 33333";
    }
}

MyTcpServer::~MyTcpServer()
{
    for (auto client : buffers.keys()) {
        client->disconnectFromHost();
        client->deleteLater();
    }
    server->close();
}

void MyTcpServer::onNewConnection()
{
    QTcpSocket* client = server->nextPendingConnection();
    buffers[client] = QByteArray();

    qDebug() << "🔌 Новый клиент подключился:" << client;

    client->write("Welcome!\n");
    client->write(QString("Участников: %1\n").arg(buffers.size()).toUtf8());

    connect(client, &QTcpSocket::readyRead, this, &MyTcpServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &MyTcpServer::onClientDisconnected);
}

void MyTcpServer::onClientDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    qDebug() << "❎ Клиент отключился:" << client;

    buffers.remove(client);
    finishedClients.remove(client);
    client->deleteLater();
}

void MyTcpServer::onReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    qDebug() << "📥 Данные от клиента:" << client;

    QByteArray& buffer = buffers[client];
    buffer.append(client->readAll());

    while (buffer.contains('\n')) {
        int idx = buffer.indexOf('\n');
        QByteArray line = buffer.left(idx).trimmed();
        buffer.remove(0, idx + 1);

        QString command = QString::fromUtf8(line);
        qDebug() << "📦 Команда получена:" << command;

        processCommand(client, command);
    }
}

void MyTcpServer::processCommand(QTcpSocket* client, const QString& command)
{
    if (command.startsWith("/add ")) {
        QString item = command.mid(5).trimmed();
        sharedList.append(item);
        client->write("Добавлено: " + item.toUtf8() + "\n");

    } else if (command.startsWith("/remove ")) {
        QString item = command.mid(8).trimmed();
        if (sharedList.removeAll(item))
            client->write("Удалено: " + item.toUtf8() + "\n");
        else
            client->write("Элемент не найден\n");

    } else if (command == "/finish") {
        finishedClients.insert(client);
        client->write("Ожидание других пользователей...\n");

        if (finishedClients.size() == buffers.size()) {
            QString result = "Финальный список:\n";
            for (const QString& item : sharedList)
                result += "- " + item + "\n";

            broadcast(result.toUtf8());
            sharedList.clear();
            finishedClients.clear();
        }

    } else {
        client->write("❓ Неизвестная команда. Используй /add, /remove или /finish\n");
    }
}

void MyTcpServer::broadcast(const QByteArray& message)
{
    for (QTcpSocket* client : buffers.keys()) {
        client->write(message);
    }
}
