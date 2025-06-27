#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QStringList>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject* parent = nullptr);
    ~MyTcpServer();

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();

private:
    QTcpServer* server;
    QMap<QTcpSocket*, QByteArray> buffers;
    QSet<QTcpSocket*> finishedClients;
    QStringList sharedList;

    void processCommand(QTcpSocket* client, const QString& command);
    void broadcast(const QByteArray& message);
};

#endif // MYTCPSERVER_H
