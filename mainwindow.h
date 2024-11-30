#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include "DBManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void addDeck();
    void removeDeck();
private:
    void initializeDeckTable();
    DBManager dbManager;
    void loadDecks();
    QMap<int, QWidget*> deckWidgets;
    QGridLayout *gridLayout;
    QComboBox *comboBox;
    int rowCount;
    int colCount;
};
#endif // MAINWINDOW_H
