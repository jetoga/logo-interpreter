#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "model.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setColor(int r, int g, int b);
    void clearScreen();
    void drawLine2Point(int x1, int y1, int x2, int y2);
    void drawLinePointAngleLength(int x1, int y1, int length, int angle);
    void moveTurtle2Point(int x2, int y2);
    void moveTurtlePointAngleLength(int x1, int y1, int length, int angle);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    Model * model;
    QPen *pen;
    QColor *color;
};

#endif // MAINWINDOW_H
