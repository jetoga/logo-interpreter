#ifndef PARSER_H
#define PARSER_H

#pragma once
#include <iostream>
#include <list> 
#include <typeinfo>
#include <chrono>
#include <thread>
#include "lexer.hpp"
#include "context.hpp"


/*
Struct used to return values from function execution
*/
struct function_result
{
	int integer_value = 0;
	bool returns_a_value = false;
	bool is_output = false;
};



/*
Virtual class to represent all statements
*/
class Statement
{
public:
	Statement() = default;
    virtual ~Statement()=0;
	virtual function_result execute(ProgramContext * pc) = 0;
};

/*
Virtual class to represent all statements which can appear inside of a body of a function
*/
class InFunctionStatement : public Statement
{
public:
	InFunctionStatement() = default;
    virtual ~InFunctionStatement()=0;
	virtual function_result execute(ProgramContext * pc) = 0;
};

class Function;

/*
Class representing a function definition statement
*/
class FunctionDefinition : public Statement
{
public:
	FunctionDefinition(Token i, int n, std::list<Token> * a, std::list<InFunctionStatement*> * s) : identifier(i), number_of_arguments(n), arguments(a), statementList(s) {}
    ~FunctionDefinition() { if(number_of_arguments != 0) delete arguments; while (!statementList->empty()) { delete statementList->front(), statementList->pop_front(); } delete statementList; }
	
	int getNumberOfArgs() { return number_of_arguments; }
	std::list<Token> * getArgList() { return arguments; }
	std::list<InFunctionStatement*> * getStatementList() { return statementList; }

	function_result execute(ProgramContext * pc);
	void update(std::list<InFunctionStatement*> * s) { this->statementList = s; }

private:
	Token identifier;
	int number_of_arguments;
	std::list<Token> * arguments;
	std::list<InFunctionStatement*> * statementList;

};

/*
Class representing a variable
*/
class Variable
{
public:
	Variable(Token i) : identifier(i) {}
	int evaluate(ProgramContext * pc);

private:
	Token identifier;
};

/*
Declaration of a class representing an additive expression
*/
class AdditiveExpression;

/*
Class representing a defined function
*/
class Function : public InFunctionStatement
{
public:
	Function(Token i, std::list<AdditiveExpression *> * a) : identifier(i), argument_list(a) {}
	Function() = default;
	function_result execute(ProgramContext * pc);
	~Function();

private:
	Token identifier;
    std::list<AdditiveExpression *> * argument_list = nullptr;
};

/*
Class representing a multiplicative expression
*/
class MultiplicativeExpression
{
public:
	enum { M_NUMBER, M_VARIABLE, M_FUNCTION, M_PARENTHESIS };
	MultiplicativeExpression() = default;
	MultiplicativeExpression(Token n) : number(n), first_operand_type(M_NUMBER), has_last_operand(false) {}
	MultiplicativeExpression(Token n, Token b, MultiplicativeExpression * l) : number(n), binary_operator(b), last_operand(l), first_operand_type(M_NUMBER), has_last_operand(true) {}
	MultiplicativeExpression(Variable * v) : variable(v), first_operand_type(M_VARIABLE), has_last_operand(false) {}
	MultiplicativeExpression(Variable * v, Token b, MultiplicativeExpression * l) : variable(v), binary_operator(b), last_operand(l), first_operand_type(M_VARIABLE), has_last_operand(true) {}
	MultiplicativeExpression(Function * f) : function(f), first_operand_type(M_FUNCTION), has_last_operand(false) {}
	MultiplicativeExpression(Function * f, Token b, MultiplicativeExpression * l) : function(f), binary_operator(b), last_operand(l), first_operand_type(M_FUNCTION), has_last_operand(true) {}
	MultiplicativeExpression(AdditiveExpression * aeip) : additive_expression_in_parentheses(aeip), first_operand_type(M_PARENTHESIS), has_last_operand(false) {}
	MultiplicativeExpression(AdditiveExpression * aeip, Token b, MultiplicativeExpression * l) : additive_expression_in_parentheses(aeip), binary_operator(b), last_operand(l), first_operand_type(M_PARENTHESIS), has_last_operand(true) {}
	int evaluate(ProgramContext * pc);
	~MultiplicativeExpression();

private:
	Token number;
	Variable * variable;
	Function * function;
	AdditiveExpression * additive_expression_in_parentheses;
	Token binary_operator;
	MultiplicativeExpression * last_operand;
    int first_operand_type;
    bool has_last_operand;
};

/*
Definition of a class representing an additive expression
*/
class AdditiveExpression
{
public:
	AdditiveExpression() = default;
	AdditiveExpression(Token u, MultiplicativeExpression * f) : unary_operator(u), first_operand(f), has_last_operand(false) {}
	AdditiveExpression(Token u, MultiplicativeExpression * f, Token b, AdditiveExpression * l) : unary_operator(u), first_operand(f), binary_operator(b), last_operand(l), has_last_operand(true) {}
	int evaluate(ProgramContext * pc);
    ~AdditiveExpression() { delete first_operand; if(has_last_operand) delete last_operand; }
private:
	Token unary_operator;
	MultiplicativeExpression * first_operand;
	Token binary_operator;
	AdditiveExpression * last_operand;
    bool has_last_operand;
};

class LogicalExpressionSet;

/*
Class representing a logical expression
*/
class LogicalExpression
{
public:
	enum { L_BASE_LOGICAL_VALUE, L_COMPARISON, L_UNARY, L_BRACES };
	LogicalExpression(AdditiveExpression * f, AdditiveExpression * l, Token b) : has_unary_in_front(false), first_operand(f), last_operand(l), binary_operator(b), logical_type(L_COMPARISON) {}
	LogicalExpression(bool h, LogicalExpressionSet * s) : has_unary_in_front(h), logical_expression_set(s), logical_type(L_UNARY) {}
	LogicalExpression(LogicalExpressionSet * s) : has_unary_in_front(false), logical_expression_set(s), logical_type(L_BRACES) {}
	LogicalExpression(bool l) : has_unary_in_front(false), logical_value(l), logical_type(L_BASE_LOGICAL_VALUE) {}
	bool evaluate(ProgramContext * pc);
	~LogicalExpression();

private:
    bool has_unary_in_front;
	AdditiveExpression * first_operand;
	AdditiveExpression * last_operand;
	Token binary_operator;
	LogicalExpressionSet * logical_expression_set;
	bool logical_value;
    int logical_type;
};

/*
Class representing a set of logical expressions
*/
class LogicalExpressionSet
{
public:
	LogicalExpressionSet(LogicalExpression * f) : first_operand(f), has_last_operand(false) {}
	LogicalExpressionSet(LogicalExpression * f, Token b, LogicalExpressionSet * l) : first_operand(f), binary_operator(b), last_operand(l), has_last_operand(true) {}
	bool evaluate(ProgramContext * pc);
    ~LogicalExpressionSet() { delete first_operand; if(has_last_operand) delete last_operand; }

private:
	LogicalExpression * first_operand;
	Token binary_operator;
	LogicalExpressionSet * last_operand;
    bool has_last_operand;
};

/*
Class to represent a forward statement
*/
class Forward : public InFunctionStatement
{
public:
	Forward(AdditiveExpression * a) : move_by_value(a) {}
	function_result execute(ProgramContext * pc);
	~Forward() { delete move_by_value; }

private:
	AdditiveExpression * move_by_value;
};

/*
Class to represent a backward statement
*/
class Backward : public InFunctionStatement
{
public:
	Backward(AdditiveExpression * a) : move_by_value(a) {}
	function_result execute(ProgramContext * pc);
	~Backward() { delete move_by_value; }

private:
	AdditiveExpression * move_by_value;
};

/*
Class to represent a right turn statement
*/
class RightTurn : public InFunctionStatement
{
public:
	RightTurn(AdditiveExpression * a) : move_by_value(a) {}
	function_result execute(ProgramContext * pc);
	~RightTurn() { delete move_by_value; }

private:
	AdditiveExpression * move_by_value;
};

/*
Class to represent a left turn statement
*/
class LeftTurn : public InFunctionStatement
{
public:
	LeftTurn(AdditiveExpression * a) : move_by_value(a) {}
	function_result execute(ProgramContext * pc);
	~LeftTurn() { delete move_by_value; }

private:

	AdditiveExpression * move_by_value;
};

/*
Class to represent a move by vector statement
*/
class MoveByVector : public InFunctionStatement
{
public:
	MoveByVector(AdditiveExpression * x, AdditiveExpression * y) : x_value(x), y_value(y) {}
	function_result execute(ProgramContext * pc);
	~MoveByVector() { delete x_value; delete y_value; }

private:
	AdditiveExpression * x_value;
	AdditiveExpression * y_value;
};

/*
Class to represent a move to position statement
*/
class MoveToPosition : public InFunctionStatement
{
public:
	MoveToPosition(AdditiveExpression * x, AdditiveExpression * y) : x_value(x), y_value(y) {}
	function_result execute(ProgramContext * pc);
	~MoveToPosition() { delete x_value; delete y_value; }

private:
	AdditiveExpression * x_value;
	AdditiveExpression * y_value;
};

/*
Class to represent a set heading statement
*/
class SetHeading : public InFunctionStatement
{
public:
	SetHeading(AdditiveExpression * h) : heading_value(h) {}
	function_result execute(ProgramContext * pc);
	~SetHeading() { delete heading_value; }

private:
	AdditiveExpression * heading_value;
};

/*
Class to represent a turtle go home statement
*/
class TurtleGoHome : public InFunctionStatement
{
public:
	TurtleGoHome() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a get x statement
*/
class GetX : public Function
{
public:
	GetX() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a get y statement
*/
class GetY  : public Function
{
public:
	GetY() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a get heading statement
*/
class GetHeading : public Function
{
public:
	GetHeading() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a clear screen statement
*/
class CleanScreen : public InFunctionStatement
{
public:
	CleanScreen() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a pen up statement
*/
class PenUp : public InFunctionStatement
{
public:
	PenUp() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a pen down statement
*/
class PenDown : public InFunctionStatement
{
public:
	PenDown() {}
	function_result execute(ProgramContext * pc);
};

/*
Class to represent a set color statement
*/
class SetColor : public InFunctionStatement
{
public:
	SetColor(AdditiveExpression * r, AdditiveExpression * g, AdditiveExpression * b) : red(r), green(g), blue(b) {}
	function_result execute(ProgramContext * pc);
	~SetColor() { delete red; delete green; delete blue; }

private:
	AdditiveExpression * red;
	AdditiveExpression * green;
	AdditiveExpression * blue;
};

/*
Class to represent an output statement
*/
class Output : public InFunctionStatement
{
public:
	Output(AdditiveExpression * a) : additive_exp(a) {}
	function_result execute(ProgramContext * pc);
	int evaluate(ProgramContext * pc);
	~Output() { delete additive_exp; }

private:
	AdditiveExpression * additive_exp;
};

/*
Class to represent a print statement
*/
class Print : public InFunctionStatement
{
public:
	Print(AdditiveExpression * a) : additive_exp(a), is_string(false) {}
	Print(std::string s) : string_exp(s), is_string(true) {}
	function_result execute(ProgramContext * pc);
    ~Print() { if(!is_string) delete additive_exp; }

private:
	AdditiveExpression * additive_exp;
	std::string string_exp;
	bool is_string;
};

/*
Class to represent a scan statement
*/
class Scan : public InFunctionStatement
{
public:
	Scan(Token i) : identifier(i) {}
	function_result execute(ProgramContext * pc);

private:
	Token identifier;
};

/*
Class to represent a make statement
*/
class Make : public InFunctionStatement
{
public:
	Make(Token i, AdditiveExpression * a) : identifier(i), assigned_value(a) {}
	function_result execute(ProgramContext * pc);
	~Make() { delete assigned_value; }

private:
	Token identifier;
	AdditiveExpression * assigned_value;
};

/*
Class to represent a make statement
*/
class LocalMakeScan : public InFunctionStatement
{
public:
    LocalMakeScan(Token i, AdditiveExpression * a) : isScan(false), identifier(i), assigned_value(a) {}
    LocalMakeScan(Token i) : isScan(true), identifier(i) {}
	function_result execute(ProgramContext * pc);
    ~LocalMakeScan() { if(!isScan) delete assigned_value; }

private:
	bool isScan;
	Token identifier;
	AdditiveExpression * assigned_value;
};

/*
Class to represent an if statement
*/
class IfStatement : public InFunctionStatement
{
public:
	IfStatement(LogicalExpressionSet * c, std::list<Statement*> * s) : condition(c), statementList(s) {}
	function_result execute(ProgramContext * pc);
	~IfStatement() { delete condition; while (!statementList->empty()) { delete statementList->front(), statementList->pop_front(); } delete statementList; }

private:
	LogicalExpressionSet * condition;
	std::list<Statement*> * statementList;
};

/*
Class to represent a repeat statement
*/
class RepeatStatement : public InFunctionStatement
{
public:
	RepeatStatement(AdditiveExpression * n, std::list<Statement*> * s) : number_of_repetitions(n), statementList(s) {}
	function_result execute(ProgramContext * pc);
	~RepeatStatement() { delete number_of_repetitions; while (!statementList->empty()) { delete statementList->front(), statementList->pop_front(); } delete statementList; }
private:
	AdditiveExpression * number_of_repetitions;
	std::list<Statement*> * statementList;
};

/*
Class to represent a turtle sleep statement
*/
class TurtleSleep : public InFunctionStatement
{
public:
    TurtleSleep(AdditiveExpression * t) : time_to_sleep(t) {}
	function_result execute(ProgramContext * pc);
    ~TurtleSleep() { delete time_to_sleep; }

private:
    AdditiveExpression * time_to_sleep;
};

/*
Class to represent the starting statement
*/
class StartingStatement
{
public:
	StartingStatement() = default;
	void addStatement(Statement * s);
	void execute(ProgramContext * pc);
    ~StartingStatement();
	std::list<Statement*> * getFunDefs();

private:
	std::list<Statement*> statementList;
	
};

/*
Class to represent the parser
*/
class Parser
{
public:
    Parser(Lexer * l, ProgramContext * p) : lex(l), pc(p) {}
    Parser() = default;
	StartingStatement* doStartingStatement();

private:
	Lexer * lex;
    ProgramContext * pc;
	Token buf;
    FunctionSymbolTable fun;

	void getNextToken();
	bool isFunction(Token identifier);
	AdditiveExpression * doAdditiveExpression();
	MultiplicativeExpression * doMultiplicativeExpression();
	LogicalExpressionSet * doLogicalExpressionSet();
	LogicalExpression * doLogicalExpression();
	Statement * doStatement();
	FunctionDefinition * doFunctionDefinition();
	InFunctionStatement * doInFunctionStatement();
	Forward * doForward();
	Backward * doBackward();
	RightTurn * doRightTurn();
	LeftTurn * doLeftTurn();
	MoveByVector * doMoveByVector();
	MoveToPosition * doMoveToPosition();
	SetHeading * doSetHeading();
	TurtleGoHome * doTurtleGoHome();
	GetX * doGetX();
	GetY * doGetY();
	GetHeading * doGetHeading();
	CleanScreen * doCleanScreen();
	PenUp * doPenUp();
	PenDown * doPenDown();
	SetColor * doSetColor();
	Output * doOutput();
	Print * doPrint();
	Scan * doScan();
	Make * doMake();
	LocalMakeScan * doLocalMakeScan();
	IfStatement * doIfStatement();
	RepeatStatement * doRepeatStatement();
	TurtleSleep * doTurtleSleep();
	Variable * doVariable();
	Function * doFunction();
};

#endif
