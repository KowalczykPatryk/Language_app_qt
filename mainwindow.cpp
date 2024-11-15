#include "mainwindow.h"
#include <QPushButton>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , colCount(0)
    , rowCount(0)
{
    // Create a central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    gridLayout = new QGridLayout();
    centralWidget->setLayout(gridLayout);

    // Create toolbar
    QToolBar *toolBar = addToolBar("Flashcard Controls");

    // Example buttons for adding and removing flashcards
    QPushButton *addButton = new QPushButton("Add Flashcard");
    QPushButton *removeButton = new QPushButton("Remove Flashcard");

    // Add buttons to the toolbar
    toolBar->addWidget(addButton);
    toolBar->addWidget(removeButton);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addFlashcard);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeFlashcard);
}

MainWindow::~MainWindow()
{

}

void MainWindow::addFlashcard()
{
    QLabel *flashcard = new QLabel("Flashcard");
    flashcard->setAlignment(Qt::AlignCenter);
    flashcard->setFrameStyle(QFrame::Panel | QFrame::Raised);
    flashcard->setStyleSheet("background-color: lightgray;");

    // Example grid positioning logic
    gridLayout->addWidget(flashcard, rowCount, colCount);
    colCount++;
    if (colCount >= 3)
    // example of 3 columns per row
    {
        colCount = 0;
        rowCount++;
    }
}

void MainWindow::removeFlashcard()
{
    if (gridLayout->count() > 0)
    {
        QWidget *widget = gridLayout->itemAt(gridLayout->count() - 1)->widget();
        gridLayout->removeWidget(widget);
        delete widget; colCount--;
        if (colCount < 0)
        {
            colCount = 2; // example of 3 columns per row
            rowCount--;
        }
    }
}
