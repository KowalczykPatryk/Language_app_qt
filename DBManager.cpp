#include "DBManager.h"

DBManager::DBManager(const QString& host, const QString& dbName, const QString& user, int port)
    : dbHost(host), dbName(dbName), dbUser(user), dbPort(port)  {}
bool DBManager::connect()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(dbHost);
    db.setDatabaseName(dbName);
    db.setUserName(dbUser);
    db.setPassword("secure_password");
    db.setPort(dbPort);
    if (!db.open())
    {
        qDebug() << "Error: Could not connect to the database.";
        qDebug() << db.lastError().text();
        return false;
    }
    qDebug() << "Database connected successfully!";
    return true;
}
QSqlQuery DBManager::executeQuery(const QString& query, const QVariantList& values)
{
    QSqlQuery q;
    q.prepare(query); // Prepare the query
    // Bind values if any
    for (int i = 0; i < values.size(); ++i)
    {
        q.bindValue(i, values[i]);
    }
    if (!q.exec())
    {
        qDebug() << "Query execution error: " << q.lastError().text();
    }
    return q;
}
QSqlQuery DBManager::executeQuery(const QString& query)
{
    QSqlQuery q;

    // Print the query string for debugging
    qDebug() << "Executing Query:" << query;

    // Execute the query directly without preparing
    if (!q.exec(query))
    {
        qDebug() << "Query execution error: " << q.lastError().text();
    } else {
        qDebug() << "Query executed successfully.";
    }

    return q;
}

