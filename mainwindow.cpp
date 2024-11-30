#include "mainwindow.h"
#include <QPushButton>
#include <QToolBar>
#include <QScrollArea>
#include <QInputDialog>
#include <QPointer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , colCount(0)
    , rowCount(0)
    , dbManager("localhost", "flashcards_db", "flashcards_user", 5432)
{

    initializeDeckTable();
    // Set the size of the main window
    resize(600,800);

    // Create a scroll area and set its properties
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

    // Create a container widget for the scroll area
    QWidget *containerWidget = new QWidget();
    scrollArea->setWidget(containerWidget);

    // Create a grid layout for the container widget
    gridLayout = new QGridLayout();
    containerWidget->setLayout(gridLayout);

    // Create toolbar
    QToolBar *toolBar = addToolBar("Flashcard Decks Controls");

    // Example buttons for adding and removing decks of flashcards
    QPushButton *addDeckButton = new QPushButton("Add Deck");
    QPushButton *removeDeckButton = new QPushButton("Remove Deck");
    comboBox = new QComboBox(); // ComboBox for selecting decks

    // Add buttons to the toolbar
    toolBar->addWidget(addDeckButton);
    toolBar->addWidget(removeDeckButton);
    toolBar->addWidget(comboBox);

    connect(addDeckButton, &QPushButton::clicked, this, &MainWindow::addDeck);
    connect(removeDeckButton, &QPushButton::clicked, this, &MainWindow::removeDeck);

    // Call loadDecks to load and display decks from the database
    loadDecks();

    // Set the scroll area as the central widget
    setCentralWidget(scrollArea);
}

void MainWindow::initializeDeckTable()
{
    // Connect to the database
    if (!dbManager.connect())
    {
        qDebug() << "Failed to connect to the database.";
        return;
    }
    // Check if the decks table exists, and create it if it does not
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS decks ( id SERIAL PRIMARY KEY, name VARCHAR(255) NOT NULL )";
    QSqlQuery query = dbManager.executeQuery(createTableQuery);
    if (query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << "Failed to create decks table:" << query.lastError().text();
    }
    else
    {
        qDebug() << "Decks table is ready.";
    }
}

void MainWindow::loadDecks()
{
    // Retrieve the decks from the database
    QString selectQuery = "SELECT name, id FROM decks";
    QSqlQuery query = dbManager.executeQuery(selectQuery);
    while (query.next())
    {
        int deckId = query.value("id").toInt();
        QString deckName = query.value("name").toString();
        // Add the deck to the UI
        QLabel *newDeck = new QLabel(deckName);
        newDeck->setAlignment(Qt::AlignCenter);
        newDeck->setFrameStyle(QFrame::Panel | QFrame::Raised);
        newDeck->setStyleSheet("background-color: lightgray;");
        // Set fixed size for the deck
        newDeck->setFixedSize(180, 120);
        // Example grid positioning logic
        gridLayout->addWidget(newDeck, rowCount, colCount);
        colCount++;
        if (colCount >= 3)
        {
            colCount = 0;
            rowCount++;
        }
        // Store the widget in the map using the newDeckId
        deckWidgets[deckId] = newDeck;
        // Add deck name to combo box
        comboBox->addItem(deckName, deckId);
    }
}


void MainWindow::addDeck()
{
    // Prompt the user for the deck name
    bool ok;
    QString deckName = QInputDialog::getText(this, tr("Add Deck"), tr("Deck Name:"), QLineEdit::Normal, QString(), &ok);

    if (ok && !deckName.isEmpty())
    {

        // Prepare the query and values
        QString insertQuery = "INSERT INTO decks (name) VALUES (?)";
        QVariantList values;
        values << deckName;
        // Execute the query securely
        QSqlQuery result = dbManager.executeQuery(insertQuery, values);

        // Check if the insertion was successful
        if (result.lastError().type() != QSqlError::NoError)
        {
            qDebug() << "Failed to insert deck into the database:" << result.lastError().text();
            return;
        }

        // Get the ID of the last inserted deck
        int newDeckId = result.lastInsertId().toInt();

        QLabel *newDeck = new QLabel(deckName);
        newDeck->setAlignment(Qt::AlignCenter);
        newDeck->setFrameStyle(QFrame::Panel | QFrame::Raised);
        newDeck->setStyleSheet("background-color: lightgray;");
        // Set fixed size for the deck
        newDeck->setFixedSize(180, 120);
        // Example grid positioning logic
        gridLayout->addWidget(newDeck, rowCount, colCount);
        colCount++;
        if (colCount >= 3)
        {
            colCount = 0;
            rowCount++;
        }

        // Store the widget in the map using the newDeckId
        deckWidgets[newDeckId] = newDeck;

        // Add deck name to combo box
        comboBox->addItem(deckName, newDeckId);
    }
}

void MainWindow::removeDeck()
{
    int index = comboBox->currentIndex();
    if (index != -1)
    {
        QVariant data = comboBox->itemData(index);
        int deckId = data.toInt();

        QString query = "DELETE FROM decks WHERE id = ?";
        QVariantList values;
        values << deckId;

        QSqlQuery result = dbManager.executeQuery(query, values);

        if (result.lastError().type() != QSqlError::NoError)
        {
            qDebug() << "Failed to delete deck from the database" << result.lastError().text();
            return;
        }
        // Remove the widget from the map and the layout
        if (deckWidgets.contains(deckId))
        {
            QWidget *itemWidget = deckWidgets.value(deckId);
            if (itemWidget)
            {
                qDebug() << "Removing widget with deckId:" << deckId;
                gridLayout->removeWidget(itemWidget);
                delete itemWidget; deckWidgets.remove(deckId);
            }
            else
            {
                qDebug() << "Item widget for deckId" << deckId << "is null.";
            }
        }
        else
        {
            qDebug() << "DeckWidgets does not contain deckId:" << deckId;
        }
        comboBox->removeItem(index);
    }

}
