#ifndef CONTEXT_H
#define CONTEXT_H

#pragma once
#include <string>
#include <map>
#include <list>
#include <iostream>

typedef std::map<std::string, int> variable_table;


class VariableSymbolTableStack
{
public:
	VariableSymbolTableStack() = default;
	void addVariable(std::string name, int value);
	void addLocalVariable(std::string name, int value);
	void pushVariableTable();
	void popVariableTable();
	bool existsVariable(std::string name);
	bool existsLocalVariable(std::string name);
	int getVariable(std::string name);

private:
	std::list<variable_table*> v;


};

class Statement;

class FunctionDefinition;

typedef std::map<std::string, FunctionDefinition *> function_table;

class FunctionSymbolTable
{
public:
	FunctionSymbolTable() = default;
	FunctionDefinition * addFunction(FunctionDefinition * f, std::string name);
	FunctionDefinition * getFunction(std::string name);
	bool existsFunction(std::string name);
    void restoreFunctionSymbolTable(FunctionSymbolTable * fun);
    void removeFunction(std::string name);

private:
    function_table tab;

};

class Model;

class ProgramContext
{
public:
    ProgramContext() = default;
	~ProgramContext() = default;

	void addFunction(FunctionDefinition * f, std::string name);
	FunctionDefinition * getFunction(std::string name);
    void restoreFunctionSymbolTable(FunctionSymbolTable * fun);


	int getVariable(std::string name);
	void addVariable(std::string name, int value);
	void addLocalVariable(std::string name, int value);
	void pushContext();
	void popContext();

    void turtleInit();
    void move_forward(int length);
	void turn_by(int angle);
	void set_heading(int angle);
	void move_to(int x, int y);
	void move_by(int x, int y);
	void pen_up();
	void pen_down();
	void clear_screen();
	void go_home();
	void set_color(int r, int g, int b);

	int getx();
	int gety();
	int geth();

    void set_xy(int x, int y) { this->x = x; this->y = y; }

    Model * model;

    void writeToLog(std::string s);
    std::string readFromLog();
    void writeToErrorLog(std::string s);
    std::string readFromErrorLog();
    int getValueFromUser(std::string s);

private:
	FunctionSymbolTable function_table;
	VariableSymbolTableStack variable_table_stack;

    int x;
    int y;

    int heading;
    bool pen_is_up;

    int red;
    int green;
    int blue;

    std::string output_log = "";
    std::string error_log = "";

};

#endif
