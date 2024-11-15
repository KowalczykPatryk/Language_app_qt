#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addFlashcard();
    void removeFlashcard();
private:
    Ui::MainWindow *ui;
    QGridLayout *gridLayout;
    int rowCount;
    int colCount;
};
#endif // MAINWINDOW_H
