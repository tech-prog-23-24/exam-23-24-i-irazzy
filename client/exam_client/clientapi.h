#ifndef CLIENTAPI_H
#define CLIENTAPI_H

#include <QObject>
#include <QTcpSocket>

class ClientAPI : public QObject
{
    Q_OBJECT

private:
    static ClientAPI* instance;
    QTcpSocket* mTcpSocket;

    explicit ClientAPI(QObject* parent = nullptr); // Конструктор
    Q_DISABLE_COPY(ClientAPI)

public:
    static ClientAPI* getInstance();
    ~ClientAPI() override; // Деструктор

    bool connectToServer(const QString& host, quint16 port);
    QByteArray query_to_server(const QString& query);
};

#endif // CLIENTAPI_H
