#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    model = new Model(this);
    pen = new QPen(QColor(0,0,0,255));
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(QRectF(0, 0, 500, 500));
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete pen;
    delete scene;
}

void MainWindow::on_pushButton_clicked()
{
    QString input = ui->plainTextEdit->toPlainText();
    std::string input_std_string = input.toStdString();
    OutputLog * output_string_pair = model->processStatements(input_std_string);
    QString log = QString::fromStdString(output_string_pair->log);
    QString err_log = QString::fromStdString(output_string_pair->err_log);

    ui->label->setText(err_log);
    ui->plainTextEdit_2->insertPlainText(log);

    delete output_string_pair;
}

void MainWindow::setColor(int r, int g, int b)
{
   pen->setColor(QColor(r, g, b, 255));
}

void MainWindow::clearScreen()
{
    scene->clear();
    qApp->processEvents();
}

void MainWindow::drawLine2Point(int x1, int y1, int x2, int y2)
{
    scene->addLine(x1, y1, x2, y2, *pen);
    model->set_xy(x2, y2);
    qApp->processEvents();
}

void MainWindow::drawLinePointAngleLength(int x1, int y1, int length, int angle)
{
    double a = angle;
    a /= 10;
    if(a > 180) a = a - 360;
    QLineF line = QLineF(x1, y1, 0, 0);
    line.setAngle(a);
    line.setLength(length);
    QPointF p = line.p2();
    QPoint newp = p.toPoint();
    int newx1 = newp.rx();
    int newy1 = newp.ry();
    model->set_xy(newx1, newy1);
    scene->addLine(line, *pen);
    qApp->processEvents();
}

void MainWindow::moveTurtle2Point(int x2, int y2)
{
    model->set_xy(x2, y2);
}

void MainWindow::moveTurtlePointAngleLength(int x1, int y1, int length, int angle)
{
    double a = angle;
    a /= 10;
    if(a > 180) a = a - 360;
    QLineF line = QLineF(x1, y1, 0, 0);
    line.setAngle(a);
    line.setLength(length);
    QPointF p = line.p2();
    QPoint newp = p.toPoint();
    int newx1 = newp.rx();
    int newy1 = newp.ry();
    model->set_xy(newx1, newy1);
}
