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

bool DBManager::addFlashcard(int deckId, const QString &frontName, const QString &backName) {
    QString insertQuery = "INSERT INTO flashcards (deck_id, frontSide, backSide) VALUES (:deck_id, :question, :answer)";
    QSqlQuery query;
    query.prepare(insertQuery);
    query.bindValue(":deck_id", deckId);
    query.bindValue(":question", frontName);
    query.bindValue(":answer", backName);

    if (!query.exec()) {
        qDebug() << "Failed to insert flashcard:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery DBManager::fetchFlashcards(int deckId)
{
    // Fetch all flashcards for the given deckId from the database
    QString selectQuery = "SELECT frontSide, backSide FROM flashcards WHERE deck_id = :deck_id";
    QSqlQuery query;
    query.prepare(selectQuery);
    query.bindValue(":deck_id", deckId);

    if (!query.exec()) {
        qDebug() << "Failed to retrieve flashcards:" << query.lastError().text();
        return query;
    }
    return query;
}


