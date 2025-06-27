#include <QCoreApplication>
#include <QTextStream>
#include <QSocketNotifier>
#include "clientapi.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream input(stdin), output(stdout);
    auto api = ClientAPI::getInstance();

    if (!api->connectToServer("127.0.0.1", 33333)) {
        output << "❌ Не удалось подключиться к серверу\n";
        return 1;
    }

    output << "✅ Подключено. Используй команды: /add <item>, /remove <item>, /finish, exit\n";
    output << "> " << Qt::flush;

    QSocketNotifier notifier(fileno(stdin), QSocketNotifier::Read, &app);
    QObject::connect(&notifier, &QSocketNotifier::activated, [&](int) {
        QString line = input.readLine().trimmed();
        if (line.compare("exit", Qt::CaseInsensitive) == 0) {
            output << "👋 Завершаем.\n";
            app.quit();
            return;
        }

        QByteArray response = api->query_to_server(line);
        output << QString::fromUtf8(response);
        output << "> " << Qt::flush;
    });

    return app.exec();
}
