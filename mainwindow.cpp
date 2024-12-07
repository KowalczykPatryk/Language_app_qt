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
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcess>
#include <QThread>
#include <QDir>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , colCount(0)
    , rowCount(0)
    , dbManager("localhost", "flashcards_db", "flashcards_user", 5432)
{

    if (!isServerRunning())
    {
        runServer();
    }

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

    connect(this, &MainWindow::responseReady, this, &MainWindow::handleResponse);

    connect(QApplication::instance(), &QApplication::aboutToQuit, this, &MainWindow::shutDownServer);

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
void MainWindow::initializeFlashcardsTable()
{
    // Connect to the database
    if (!dbManager.connect())
    {
        qDebug() << "Failed to connect to the database.";
        return;
    }
    // Check if the decks table exists, and create it if it does not
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS flashcards ( id SERIAL PRIMARY KEY, frontSide TEXT NOT NULL, backSide TEXT NOT NULL, deck_id INTEGER NOT NULL, FOREIGN KEY (deck_id) REFERENCES decks(id) ON DELETE CASCADE )";
    QSqlQuery query = dbManager.executeQuery(createTableQuery);
    if (query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << "Failed to create flashcards table:" << query.lastError().text();
    }
    else
    {
        qDebug() << "Flashcards table is ready.";
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
        ClickableLabel *newDeck = new ClickableLabel();
        newDeck->setText(deckName);
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

        // Connect the ClickableLabel's clicked signal to showOptions slot
        connect(newDeck, &ClickableLabel::clicked, this, &MainWindow::showOptions);
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

        ClickableLabel *newDeck = new ClickableLabel();
        newDeck->setText(deckName);
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
        connect(newDeck, &ClickableLabel::clicked, this, &MainWindow::showOptions);
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

void MainWindow::showOptions() {
    // Find which ClickableLabel triggered this slot
    ClickableLabel *clickedLabel = qobject_cast<ClickableLabel*>(sender());
    if (!clickedLabel) {
        return; // Safety check, in case the sender is not a ClickableLabel
    }

    // Find the corresponding deckId
    int deckId = -1;
    for (auto it = deckWidgets.begin(); it != deckWidgets.end(); ++it) {
        if (it.value() == clickedLabel) {
            deckId = it.key();
            break;
        }
    }

    if (deckId == -1) {
        return; // Safety check, in case the deckId wasn't found
    }

    // Create a dialog to show options
    QDialog optionsDialog(this);
    optionsDialog.setWindowTitle("Options");

    // Create a horizontal layout
    QVBoxLayout *layout = new QVBoxLayout(&optionsDialog);

    // Add buttons to the layout
    QPushButton *addFlashcardButton = new QPushButton("Add Flashcard", &optionsDialog);
    QPushButton *openFlashcardsButton = new QPushButton("Open Flashcards", &optionsDialog);
    QPushButton *openCustomExercisesButton = new QPushButton("Custom Exercises", &optionsDialog);
    QPushButton *cancelButton = new QPushButton("Cancel", &optionsDialog);

    layout->addWidget(addFlashcardButton);
    layout->addWidget(openFlashcardsButton);
    layout->addWidget(openCustomExercisesButton);
    layout->addWidget(cancelButton);

    // Connect buttons to their respective slots
    connect(addFlashcardButton, &QPushButton::clicked, &optionsDialog, [deckId, this, &optionsDialog](){addFlashcard(deckId); optionsDialog.accept();});
    connect(openFlashcardsButton, &QPushButton::clicked, &optionsDialog, [deckId, this, &optionsDialog](){showFlashcards(deckId); optionsDialog.accept();});
    connect(openCustomExercisesButton, &QPushButton::clicked, &optionsDialog, [deckId, this, &optionsDialog](){showCustomExercise(deckId); optionsDialog.accept();});
    connect(cancelButton, &QPushButton::clicked, &optionsDialog, &QDialog::reject);

    // Execute the dialog
    optionsDialog.exec();
}

void MainWindow::addFlashcard(int deckId)
{
    initializeFlashcardsTable();
    // Prompt the user for the flashcard info
    bool ok;
    QString frontSide = QInputDialog::getText(this, tr("Add Flashcard"), tr("Front (Question):"), QLineEdit::Normal, QString(), &ok);
    if (!ok || frontSide.isEmpty())
        return;
    QString backSide = QInputDialog::getText(this, tr("Add Flashcard"), tr("Back (Answer):"), QLineEdit::Normal, QString(), &ok);
    if (!ok || backSide.isEmpty())
        return;
    if (dbManager.addFlashcard(deckId, frontSide, backSide))
    {
        qDebug() << "Flashcard was added successfully";
    } else {
        qDebug() << "Failed to add flashcard";
    }
}

void MainWindow::showFlashcards(int deckId)
{
    // Clear the grid layout
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    QSqlQuery query = dbManager.fetchFlashcards(deckId);

    QVector<QPair<QString, QString>> flashcards;
    while (query.next()) {
        QString question = query.value("frontSide").toString();
        QString answer = query.value("backSide").toString();
        flashcards.append(qMakePair(question, answer));
    }

    if (flashcards.isEmpty()) {
        qDebug() << "No flashcards found for deck ID:" << deckId;
        return;
    }

    // Create a widget to display the flashcards
    QWidget *flashcardWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(flashcardWidget);

    // Create labels for question and answer
    QLabel *questionLabel = new QLabel(flashcardWidget);
    QLabel *answerLabel = new QLabel(flashcardWidget);
    answerLabel->setVisible(false); // Initially hide the answer

    // Center the text in the labels
    questionLabel->setAlignment(Qt::AlignCenter);
    answerLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(questionLabel);
    layout->addWidget(answerLabel);

    // Create a button to show the next flashcard
    QPushButton *nextButton = new QPushButton("Next Flashcard", flashcardWidget);
    layout->addWidget(nextButton);

    // Add the flashcard widget to the grid layout
    gridLayout->addWidget(flashcardWidget);

    // Function to show a random flashcard
    auto showRandomFlashcard = [flashcards, questionLabel, answerLabel, this]() {
        if (flashcards.isEmpty())
        {
            showOptions();
            qDebug() << "No flashcards available to show.";
            return;
        }
        int randomIndex = QRandomGenerator::global()->bounded(flashcards.size());
        if (randomIndex < 0 || randomIndex >= flashcards.size())
        {
            showOptions();
            qDebug() << "Generated index out of range:" << randomIndex;
            return;
        }
        const auto &flashcard = flashcards[randomIndex];
        questionLabel->setText(flashcard.first);
        answerLabel->setText(flashcard.second);
        answerLabel->setVisible(false); // Hide the answer initially

        qDebug() << "Showing flashcard at index:" << randomIndex;
    };
    // Show the first random flashcard
    showRandomFlashcard();

    // Connect the next button to show the next random flashcard
    connect(nextButton, &QPushButton::clicked, showRandomFlashcard);

    // Add a button to show the answer
    QPushButton *showAnswerButton = new QPushButton("Show Answer", flashcardWidget);
    layout->addWidget(showAnswerButton);
    connect(showAnswerButton, &QPushButton::clicked, this, [=]() { answerLabel->setVisible(true); });
}

void MainWindow::promptOllama(const QString &frontSide, const QString &backSide)
{

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url("http://localhost:8000/prompt/");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["front_side"] = frontSide;
    json["back_side"] = backSide;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response_data = reply->readAll();
            QJsonDocument response_doc = QJsonDocument::fromJson(response_data);
            QJsonObject response_obj = response_doc.object();
            QString response_text = response_obj["response"].toString();
            ollamaResponse = response_text;
            qDebug() << "Ollama's response:" << response_text;
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
        emit responseReady();
        reply->deleteLater();
    });
}

void MainWindow::handleResponse()
{
    // Exit the event loop once the response is received
    eventLoop.quit();
}

void MainWindow::showCustomExercise(int deckId)
{
    QSqlQuery query = dbManager.fetchFlashcards(deckId);
    QVector<QPair<QString, QString>> flashcards;
    while (query.next()) {
        QString question = query.value("frontSide").toString();
        QString answer = query.value("backSide").toString();
        flashcards.append(qMakePair(question, answer));
    }
    if (flashcards.isEmpty()) {
        qDebug() << "No flashcards found for deck ID:" << deckId;
        return;
    }
    int randomIndex = QRandomGenerator::global()->bounded(flashcards.size());
    const auto &flashcard = flashcards[randomIndex];
    QString frontSide = flashcard.first;
    QString backSide = flashcard.second;
    promptOllama(frontSide, backSide);

    // Wait for the response to be handled
    eventLoop.exec();

    // Now ollamaResponse contains the response and can be used
    qDebug() << "Response to be used in showCustomExercise: " << ollamaResponse;

    // Set up the UI to display the sentence with a gap and prompt user input
    QLabel *sentenceLabel = new QLabel(ollamaResponse, this);
    QLineEdit *inputEdit = new QLineEdit(this);
    QPushButton *submitButton = new QPushButton("Submit", this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(sentenceLabel);
    layout->addWidget(inputEdit);
    layout->addWidget(submitButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Connect the submit button to the slot for checking the answer
    connect(submitButton, &QPushButton::clicked, this, [=]() {
        QString userInput = inputEdit->text().trimmed();
        if (userInput.compare(frontSide, Qt::CaseInsensitive) == 0) {
            QMessageBox::information(this, "Correct!", "Well done, that's the right word!");
        } else {
            QMessageBox::warning(this, "Incorrect", "Oops! Try again.");
        }
    });

}

bool MainWindow::isServerRunning()
{
    // Step 1: Check if port 8000 is in use
    QProcess process;
    process.start("sh", QStringList() << "-c" << "lsof -i :8000 | grep LISTEN");
    process.waitForFinished();
    QString output = process.readAllStandardOutput().trimmed();

    if (output.isEmpty()) {
        // No process is listening on port 8000
        return false;
    }

    // Step 2: Check if the server is responding
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://localhost:8000/prompt/"));
    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool serverIsRunning = reply->error() == QNetworkReply::NoError;
    reply->deleteLater();

    return serverIsRunning;
}


void MainWindow::runServer()
{
    QProcess *serverProcess = new QProcess(this);

    // Get the user's home directory
    QString homeDir = QDir::homePath();
    // Construct the full path to server.py
    QString scriptPath = QDir(homeDir).filePath("Language_app_qt/server.py");
    QString pythonExecutable = "/usr/bin/python3"; // Adjust this path to your Python executable

    serverProcess->start(pythonExecutable, QStringList() << scriptPath);

    if (!serverProcess->waitForStarted()) {
        qDebug() << "Failed to start server.py";
        qDebug() << serverProcess->errorString(); // Log process error
    } else {
        qDebug() << "server.py started successfully";
        connect(serverProcess, &QProcess::readyReadStandardOutput, this, [=]() {
            qDebug() << serverProcess->readAllStandardOutput();
        });
        connect(serverProcess, &QProcess::readyReadStandardError, this, [=]() {
            qDebug() << serverProcess->readAllStandardError();
        });
    }
}

void MainWindow::shutDownServer()
{
    QProcess findProcess;
    findProcess.start("sh", QStringList() << "-c" << "lsof -ti :8000");
    findProcess.waitForFinished();
    QString pid = findProcess.readAllStandardOutput().trimmed();
    if (!pid.isEmpty())
    {
        QProcess killProcess;
        killProcess.start("sh", QStringList() << "-c" << QString("kill -9 %1").arg(pid));
        killProcess.waitForFinished(); qDebug() << "Process on port 8000 killed successfully";
    }
    else
    {
        qDebug() << "No process found on port 8000";
    }
}






