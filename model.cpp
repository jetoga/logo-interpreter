#include "model.hpp"
#include "mainwindow.hpp"
#include <QInputDialog>

/*
Constructor
*/
Model::Model(MainWindow * m)
{
    s = new Source();
    k = KeywordMap();
    l = new Lexer(s, k);
    mw = m;
    pc = new ProgramContext();
    pc->model = this;
    pc->turtleInit();
    pc->pushContext();
    p = Parser(l, pc);
}

/*
Destructor
*/
Model::~Model()
{

    while (!aggregated.empty())
    {
        delete aggregated.front();
        aggregated.pop_front();
    }

    pc->popContext();

    delete s;
    delete l;
    delete pc;
}

/*
Processes the statements read from the user
*/
OutputLog *Model::processStatements(std::string str)
{
    s->addToSource(str);
    l->updateLexer();
    x = nullptr;
    x = p.doStartingStatement();

    if (x != nullptr)
    {
        fun_list = nullptr;
        x->execute(pc);
        fun_list = x->getFunDefs();
        for (std::list<Statement*>::iterator fun_iter = fun_list->begin(); fun_iter != fun_list->end(); fun_iter++)
        {
            aggregated.push_back(*fun_iter);
        }

        fun_list->clear();
        delete fun_list;
        delete x;
    }
    std::string log = pc->readFromLog();
    std::string err_log = pc->readFromErrorLog();

    return new OutputLog(log, err_log);
}

/*
Gets a value from user by displaying an input dialog
*/
int Model::getValueFromUser(std::string s)
{
    QString title = QString("Input");
    std::string str = "Please input the value of the variable: "  + s;
    QString label = QString::fromStdString(str);
    int value = QInputDialog::getInt(0, title, label, 0);
    return value;
}

/*
Tells the view to draw a line from two points
*/
void Model::drawLine2Point(int x1, int y1, int x2, int y2)
{
    mw->drawLine2Point(x1, y1, x2, y2);
}

/*
Tells the view to draw a line from a point, a length and an angle
*/
void Model::drawLinePointAngleLength(int x1, int y1, int length, int angle)
{
    mw->drawLinePointAngleLength(x1, y1, length, angle);
}

/*
Moves the turtle to a given point
*/
void Model::moveTurtle2Point(int x2, int y2)
{
    mw->moveTurtle2Point(x2, y2);
}

/*
Moves the turtle from a given point at an angle by length
*/
void Model::moveTurtlePointAngleLength(int x1, int y1, int length, int angle)
{
    mw->moveTurtlePointAngleLength(x1, y1, length, angle);
}

/*
Clears the screen
*/
void Model::clearScreen()
{
    mw->clearScreen();
}

/*
Updates the color of lines to be drawn
*/
void Model::updateColor(int r, int g, int b)
{
    mw->setColor(r, g, b);
}
