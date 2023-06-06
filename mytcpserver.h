#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QPair>

// Структура Player, представляющая игрока
struct Player {
    QTcpSocket* socket; // Сокет клиента
    int score; // Счет игрока
    QString name; // Имя игрока
};

class MyTcpServer : public QObject {
    Q_OBJECT
public:
    explicit MyTcpServer(QObject* parent = nullptr);
    ~MyTcpServer();

private slots:
    // Слоты для обработки событий
    void slotNewConnection();
    void slotServerRead();
    void slotClientDisconnected();

private:
    QTcpServer* mTcpServer; // Объект сервера
    QPair<Player, Player> mPlayers; // Пара игроков (первый и второй)
    QHash<QTcpSocket*, Player*> mSocketToPlayerMap; // Хэш-таблица для соответствия сокета игроку

    // Вспомогательные функции
    void sendMessageToAll(const QString& message); // Отправка сообщения всем подключенным клиентам
    int gameFunction(int arg1, int arg2); // Функция игры для вычисления результата

};

#endif  // MYTCPSERVER_H






