#ifndef CLICKABLEWIDGET_H
#define CLICKABLEWIDGET_H

#include <QLabel>
#include <QMouseEvent>
#include <QObject>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QLabel *parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
    }
};


#endif // CLICKABLEWIDGET_H
