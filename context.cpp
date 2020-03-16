#include "model.hpp"
#include "context.hpp"
#include "parser.hpp"
#define HOME_X 250
#define HOME_Y 250
#define HOME_HEADING 900

/*
Adds a variable globally
*/
void VariableSymbolTableStack::addVariable(std::string name, int value)
{
	if (existsVariable(name))
	{
		variable_table::iterator i = v.front()->find(name);
		i->second = value;
	}
	else
	{
		v.front()->insert({ name, value });
	}

}

/*
Adds a variable locally
*/
void VariableSymbolTableStack::addLocalVariable(std::string name, int value)
{
	if (existsLocalVariable(name))
	{
		variable_table::iterator i = v.back()->find(name);
		i->second = value;
	}
	else
	{
		v.back()->insert({ name, value });
	}
}

/*
Adds a new variable table to stack
*/
void VariableSymbolTableStack::pushVariableTable()
{
	variable_table * t = new variable_table();
	v.push_back(t);

}

/*
Removes last added variable table from stack
*/
void VariableSymbolTableStack::popVariableTable()
{
	delete v.back();
	v.pop_back();
}

/*
Checks whether given variable exists (global scope)
*/
bool VariableSymbolTableStack::existsVariable(std::string name)
{
	variable_table :: iterator i = v.front()->find(name);

	if (i == v.front()->end())
	{
		return false;
	}

	return true;
}

/*
Checks whether given variable exists (local scope)
*/
bool VariableSymbolTableStack::existsLocalVariable(std::string name)
{
	variable_table::iterator i = v.back()->find(name);

	if (i == v.back()->end())
	{
		return false;
	}

	return true;
}

/*
Searches for a variable from the top of the call stack
*/
int VariableSymbolTableStack::getVariable(std::string name)
{
	for (std::list<variable_table*>::reverse_iterator i = v.rbegin(); i != v.rend(); i++)
	{
		variable_table::iterator j = (*i)->find(name);
		if (j != (*i)->end()) return j->second;
	}

	throw "Nonexistent variable!\n";
}

/*
Adds a function to the function list
*/
FunctionDefinition * FunctionSymbolTable::addFunction(FunctionDefinition * f, std::string name)
{
	FunctionDefinition * f_prev = tab[name];
	tab[name] = f;
	return f_prev;
}

/*
Searches for a function in the function list
*/
FunctionDefinition * FunctionSymbolTable::getFunction(std::string name)
{
	function_table::iterator i = tab.find(name);
	if (i == tab.end())
	{
		return nullptr;
	}
	return i->second;
}

/*
Checks whether given function exists
*/
bool FunctionSymbolTable::existsFunction(std::string name)
{
	function_table::iterator i = tab.find(name);
	if (i == tab.end())
	{
		return false;
	}
	return true;
}

/*
Restores the given function table to what it currently holds
*/
void FunctionSymbolTable::restoreFunctionSymbolTable(FunctionSymbolTable * fun)
{
    if(this->tab.empty()) return;
    function_table::iterator i = fun->tab.begin();
    while(i != fun->tab.end())
    {
        FunctionDefinition * fundef = this->getFunction(i->first);
        if(fundef == nullptr)
        {
            fun->removeFunction(i->first);
            i = fun->tab.begin();
        }
        else
        {
            fun->addFunction(fundef, i->first);
            i++;
        }
    }
}

/*
Removes the function with a given name from the function table
*/
void FunctionSymbolTable::removeFunction(std::string name)
{
    function_table::iterator i = tab.find(name);
    tab.erase(i);
}

/*
Adds a function to the function list
*/
void ProgramContext::addFunction(FunctionDefinition * f, std::string name)
{
	function_table.addFunction(f, name);
}

/*
Searches for a function in the function list
*/
FunctionDefinition * ProgramContext::getFunction(std::string name)
{
	return function_table.getFunction(name);
}

/*
Restores the function table to what it currently holds
*/
void ProgramContext::restoreFunctionSymbolTable(FunctionSymbolTable * fun)
{
    function_table.restoreFunctionSymbolTable(fun);
}

/*
Searches for a variable from the top of the call stack
*/
int ProgramContext::getVariable(std::string name)
{
	int i = variable_table_stack.getVariable(name);
	return i;
}

/*
Adds a variable globally
*/
void ProgramContext::addVariable(std::string name, int value)
{
	variable_table_stack.addVariable(name, value);
}

/*
Adds a variable locally
*/
void ProgramContext::addLocalVariable(std::string name, int value)
{
	variable_table_stack.addLocalVariable(name, value);
}

/*
Adds a new context for a function call
*/
void ProgramContext::pushContext()
{
	variable_table_stack.pushVariableTable();
}

/*
Removes last added context for a function call
*/
void ProgramContext::popContext()
{
	variable_table_stack.popVariableTable();
}

/*
Initializes the turtle-describing variables
*/
void ProgramContext::turtleInit()
{
    x = HOME_X;
    y = HOME_Y;
    heading = HOME_HEADING;
    pen_is_up = false;
    red = 0;
    green = 0;
    blue = 0;
}

/*
Moves the turtle forward by lenght (or backward, if negative)
*/
void ProgramContext::move_forward(int length)
{

    if(pen_is_up)
    {
        model->moveTurtlePointAngleLength(this->x, this->y, length, heading);
    }
    else
    {
        model->drawLinePointAngleLength(this->x, this->y, length, heading);
    }
}

/*
Turns the turtle by angle tenths of a degree
*/
void ProgramContext::turn_by(int angle)
{
    heading += angle;
    heading %= 3600;
}

/*
Sets turtle's heading
*/
void ProgramContext::set_heading(int angle)
{
    heading = angle;
}

/*
Moves the turtle to the specified point
*/
void ProgramContext::move_to(int x, int y)
{
    if(pen_is_up)
    {
        model->moveTurtle2Point(x, y);
    }
    else
    {
        model->drawLine2Point(this->x, this->y, x, y);
    }
}

/*
Moves the turtle by a specified vector
*/
void ProgramContext::move_by(int x, int y)
{
    if(pen_is_up)
    {
        model->moveTurtle2Point(this->x + x, this->y + y);
    }
    else
    {
        model->drawLine2Point(this->x, this->y, this->x + x, this->y + y);
    }
}

/*
Lifts the pen
*/
void ProgramContext::pen_up()
{
    pen_is_up = true;
}

/*
Lowers the pen
*/
void ProgramContext::pen_down()
{
    pen_is_up = false;
}

/*
Clears the screen
*/
void ProgramContext::clear_screen()
{
    model->clearScreen();
}

/*
Moves the turtle back to the middle of the screen
*/
void ProgramContext::go_home()
{
    move_to(HOME_X, HOME_Y);
    heading = HOME_HEADING;
}

/*
Sets the color of the pen
*/
void ProgramContext::set_color(int r, int g, int b)
{
    red = r;
    green = g;
    blue = b;
    model->updateColor(r, g, b);
}

/*
Returns current x
*/
int ProgramContext::getx()
{
    return x;
}

/*
Returns current y
*/
int ProgramContext::gety()
{
    return y;
}

/*
Returns current heading
*/
int ProgramContext::geth()
{
    return heading;
}

/*
Writes to log
*/
void ProgramContext::writeToLog(std::string s)
{
    output_log += s;
}

/*
Reads from log and clears it
*/
std::string ProgramContext::readFromLog()
{
    std::string s = std::string(output_log);
    output_log = "";
    return s;
}

/*
Writes to error log
*/
void ProgramContext::writeToErrorLog(std::string s)
{
    error_log += s;
}

/*
Reads from error log and clears it
*/
std::string ProgramContext::readFromErrorLog()
{
    std::string s = std::string(error_log);
    error_log = "";
    return s;
}

/*
Gets a value from user
*/
int ProgramContext::getValueFromUser(std::string s)
{
    return model->getValueFromUser(s);
}



