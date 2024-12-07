#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include "DBManager.h"
#include "ClickableLabel.h"
#include <QEventLoop>
#include <QProcess>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

signals:
    void responseReady();

private slots:
    void addDeck();
    void removeDeck();
    void handleResponse();
private:
    void initializeDeckTable();
    void initializeFlashcardsTable();
    void showOptions();
    void addFlashcard(int deckId);
    void showFlashcards(int deckId);
    void showCustomExercise(int deckId);
    void promptOllama(const QString &frontSide, const QString &backSide);
    void runServer();
    bool isServerRunning();
    void shutDownServer();
    DBManager dbManager;
    void loadDecks();
    QMap<int, ClickableLabel*> deckWidgets;
    QGridLayout *gridLayout;
    QComboBox *comboBox;
    int rowCount;
    int colCount;
    QString ollamaResponse;
    QEventLoop eventLoop;
};
#endif // MAINWINDOW_H
