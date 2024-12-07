#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QDebug>

class DBManager
{
public:
    DBManager(const QString& host, const QString& dbName, const QString& user, int port);
    bool connect();
    QSqlQuery executeQuery(const QString& query);
    QSqlQuery executeQuery(const QString& query, const QVariantList& values);
    bool addFlashcard(int deckId, const QString &frontName, const QString &backName);
    QSqlQuery fetchFlashcards(int deckId);
private:
    QSqlDatabase db;
    QString dbHost;
    QString dbName;
    QString dbUser;
    int dbPort;
};

#endif // DBMANAGER_H
