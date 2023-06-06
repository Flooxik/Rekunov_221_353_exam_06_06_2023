#include "mytcpserver.h"
#include <QDebug>
#include <QTextStream>
#include <QFile>

// Конструктор класса MyTcpServer
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent) {
    // Создаем объект mTcpServer как новый QTcpServer
    mTcpServer = new QTcpServer(this);

    // Подключаем сигнал о новом подключении к слоту для обработки этого подключения
    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    // Если сервер не может прослушивать на порту 33333, выводим сообщение об ошибке
    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "server is not started";
    } else {
        // Иначе, сервер успешно запущен
        qDebug() << "server is started";
    }

}

// Деструктор класса MyTcpServer, который просто закрывает mTcpServer
MyTcpServer::~MyTcpServer() {
    mTcpServer->close();

}

// Слот для обработки нового подключения
void MyTcpServer::slotNewConnection() {
    // Создаем сокет для нового клиента
    QTcpSocket* clientSocket = mTcpServer->nextPendingConnection();

    // Подключаем сигналы готовности чтения и отключения к соответствующим слотам
    connect(clientSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

    // Создаем нового игрока и добавляем его в игру
    Player newPlayer = {clientSocket, 0, ""};
    if (mPlayers.first.socket == nullptr) {
        mPlayers.first = newPlayer;
    } else if (mPlayers.second.socket == nullptr) {
        mPlayers.second = newPlayer;
    } else {
        // Если игра уже полная, отключаемся
        clientSocket->disconnectFromHost();
    }

    // Добавляем соответствие между сокетом и игроком в map
    mSocketToPlayerMap[clientSocket] = &newPlayer;
}

// Слот для чтения данных от клиента
void MyTcpServer::slotServerRead() {
    // Определяем, от какого клиента пришли данные
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    Player* currentPlayer = mSocketToPlayerMap[clientSocket];

    // Читаем все данные от клиента
    QTextStream stream(clientSocket);
    QString strReceived = stream.readAll();

    // Разбираем полученную строку на команды
    QStringList cmd = strReceived.split('&');

    // Обрабатываем команды от клиента
    if (cmd[0] == "start_game") {
        // Устанавливаем имя игрока
        currentPlayer->name = cmd[1];
    } else if (cmd[0] == "move") {
        // Обновляем счет игрока
        currentPlayer->score = gameFunction(cmd[1].toInt(), cmd[2].toInt());
    } else if (cmd[0] == "stop_game") {
        // Игрок сдался, сообщаем всем об этом
        sendMessageToAll(currentPlayer->name + " surrended.");
    } else if (cmd[0] == "stats") {
        // Выводим статистику всех игроков
        sendMessageToAll("Players statistics:");
        sendMessageToAll(mPlayers.first.name + ": " + QString::number(mPlayers.first.score));
        sendMessageToAll(mPlayers.second.name + ": " + QString::number(mPlayers.second.score));
    }
}

// Слот для обработки отключения клиента
void MyTcpServer::slotClientDisconnected() {
    // Определяем, какой клиент отключился, и сообщаем всем об этом
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    sendMessageToAll(mSocketToPlayerMap[clientSocket]->name + " отключился.");
    // Удаляем сокет из map
    mSocketToPlayerMap.remove(clientSocket);
}

// Функция для отправки сообщения всем подключенным клиентам
void MyTcpServer::sendMessageToAll(const QString& message) {
    for (auto socket : mSocketToPlayerMap.keys()) {
        QTextStream stream(socket);
        stream << message;
    }
}

// Функция игры, просто возвращает сумму двух аргументов
int MyTcpServer::gameFunction(int arg1, int arg2) {
    return arg1 + arg2;
}
