#ifndef MODEL_H
#define MODEL_H
#include "parser.hpp"

class MainWindow;

/*
Used to return the log and the error log from processing a starting statement
*/
class OutputLog
{
public:
    OutputLog(std::string l, std::string e) : log(l), err_log(e) {}
    std::string log;
    std::string err_log;
};

/*
Model of the turtle
*/
class Model
{
public:
    Model(MainWindow * m);
    ~Model();
    OutputLog * processStatements(std::string str);
    int getValueFromUser(std::string s);
    void drawLine2Point(int x1, int y1, int x2, int y2);
    void drawLinePointAngleLength(int x1, int y1, int length, int angle);
    void moveTurtle2Point(int x2, int y2);
    void moveTurtlePointAngleLength(int x1, int y1, int length, int angle);
    void clearScreen();
    void updateColor(int r, int g, int b);

    void set_xy(int x, int y) { pc->set_xy(x, y); }

private:
    Source * s;
    KeywordMap k;
    Lexer * l;
    ProgramContext * pc;
    Parser p;
    MainWindow * mw;
    std::list<Statement*> aggregated;
    std::list<Statement*> * fun_list = nullptr;
    StartingStatement * x = nullptr;

};

#endif // MODEL_H
