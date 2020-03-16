#include "parser.hpp"

Statement::~Statement(){}
InFunctionStatement::~InFunctionStatement(){}

/*
Returns a pointer to an additive expression or throws an exception
*/
AdditiveExpression * Parser::doAdditiveExpression()
{
	Token unary_operator;
	MultiplicativeExpression * first_operand = nullptr;
	Token binary_operator;
	AdditiveExpression * last_operand = nullptr;
	try
	{
		if (buf.type == T_ADD_OPER)
		{
			unary_operator = buf;
			getNextToken();
		}

		first_operand = doMultiplicativeExpression();

		if (buf.type == T_ADD_OPER)
		{
			binary_operator = buf;
			getNextToken();
		}
		else
		{
			return new AdditiveExpression(unary_operator, first_operand);
		}

		last_operand = doAdditiveExpression();
		return new AdditiveExpression(unary_operator, first_operand, binary_operator, last_operand);
	}
	catch (...)
	{
		delete first_operand;
		delete last_operand;
		throw;
	}
}

/*
Returns a pointer to a multiplicative expression or throws an exception
*/
MultiplicativeExpression * Parser::doMultiplicativeExpression()
{
	Token number;
	bool isNumber = false;
	Variable * variable = nullptr;
	Function * function = nullptr;
	AdditiveExpression * additive_expression_in_parentheses = nullptr;
	Token binary_operator;
	MultiplicativeExpression * last_operand = nullptr;
	try
	{
		if (buf.type == T_NUMBER)
		{
			number = buf;
			isNumber = true;
			getNextToken();

		}
		else if (buf.type == T_PAREN_OPEN)
		{

			getNextToken();
			additive_expression_in_parentheses = doAdditiveExpression();

			if (buf.type == T_PAREN_CLOSE)
			{
				getNextToken();
			}
			else
			{
				throw "A closing parenthesis was expected!\n";
			}
		}
        else
		{
			function = doFunction();

			if (function == nullptr)
			{
				variable = doVariable();
                if(variable == nullptr) throw "A multiplicative expression was expected!\n";
			}
		}

		if (buf.type == T_MULT_OPER)
		{
			binary_operator = buf;
			getNextToken();
		}
		else
		{
			if (isNumber)
			{
				return new MultiplicativeExpression(number);
			}
			else if (additive_expression_in_parentheses != nullptr)
			{
				return new MultiplicativeExpression(additive_expression_in_parentheses);
			}
			else if (variable != nullptr)
			{
				return new MultiplicativeExpression(variable);
			}
			else if (function != nullptr)
			{
				return new MultiplicativeExpression(function);
			}
		}

		last_operand = doMultiplicativeExpression();

		if (isNumber)
		{
			return new MultiplicativeExpression(number, binary_operator, last_operand);
		}
		else if (additive_expression_in_parentheses != nullptr)
		{
			return new MultiplicativeExpression(additive_expression_in_parentheses, binary_operator, last_operand);
		}
		else if (variable != nullptr)
		{
			return new MultiplicativeExpression(variable, binary_operator, last_operand);
		}
		else if (function != nullptr)
		{
			return new MultiplicativeExpression(function, binary_operator, last_operand);
		}

		throw "A multiplicative expression was expected\n";
	}
	catch (...)
	{
		delete additive_expression_in_parentheses;
		delete variable;
		delete function;
		delete last_operand;
		throw;
	}
}

/*
Returns a pointer to a set of logical expressions or throws an exception
*/
LogicalExpressionSet * Parser::doLogicalExpressionSet()
{
	LogicalExpression * first_operand = nullptr;
	Token binary_operator;
	LogicalExpressionSet * last_operand = nullptr;

	try
	{
		first_operand = doLogicalExpression();

		if (buf.type == T_KEYWORD && (buf.integer_value == K_AND || buf.integer_value == K_OR || buf.integer_value == K_XOR))
		{
			binary_operator = buf;
			getNextToken();
			last_operand = doLogicalExpressionSet();

			return new LogicalExpressionSet(first_operand, binary_operator, last_operand);			
		}
		else
		{
			return new LogicalExpressionSet(first_operand);
		}
	}
	catch (...)
	{
		delete first_operand;
		delete last_operand;
		throw;
	}
}

/*
Returns a pointer to a logical expression or throws an exception
*/
LogicalExpression * Parser::doLogicalExpression()
{

	AdditiveExpression * left_expression = nullptr;
	AdditiveExpression * right_expression = nullptr;
	Token binary_operator;
	LogicalExpressionSet * s = nullptr;

	try
	{
		if (buf.type == T_KEYWORD)
		{
			if (buf.integer_value == K_TRUE)
			{
				getNextToken();
				return new LogicalExpression(true);
			}
			else if (buf.integer_value == K_FALSE)
			{
				getNextToken();
				return new LogicalExpression(false);
			}
			else if (buf.integer_value == K_NOT)
			{
				getNextToken();
				s = doLogicalExpressionSet();
				return new LogicalExpression(true, s);
			}
		}

		if (buf.type == T_LOG_OPEN)
		{
			getNextToken();
			s = doLogicalExpressionSet();

			if (buf.type != T_LOG_CLOSE)
			{
				throw "Closing braces for a logical expression expected!\n";
			}

			getNextToken();
			return new LogicalExpression(s);
		}

		left_expression = doAdditiveExpression();

		if (buf.type != T_COMP_OPER)
		{
			throw "A comparative operator was expected!\n";
		}

		binary_operator = buf;
		getNextToken();
		right_expression = doAdditiveExpression();
		
		return new LogicalExpression(left_expression, right_expression, binary_operator);
	}
	catch (...)
	{
		delete left_expression;
		delete right_expression;
		delete s;
		throw;
	}
}

/*
Adds a statement to the list of statements if not a nullptr
*/
void StartingStatement::addStatement(Statement * s)
{
	if(s != nullptr) statementList.push_back(s);
}

StartingStatement::~StartingStatement()
{
    while (!statementList.empty())
    {
        if (FunctionDefinition * def = dynamic_cast<FunctionDefinition*>(statementList.front()))
        {
           statementList.pop_front();
        }
        else
        {
            delete statementList.front();
            statementList.pop_front();
        }

    }
}

/*
Executes all statements in the list unless an exception is thrown
*/
void StartingStatement::execute(ProgramContext * pc)
{
	
	if (statementList.empty())
	{
		return;
	}
	
	for (std::list <Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++)
	{
		try
		{
			(*i)->execute(pc);
		}
        catch (const char * c)
		{
            std::string str(c);
            pc->writeToErrorLog(str);
			return;
		}
	}
}

/*
Returns a pointer to a list containing all the function definitions in a given statement
*/
std::list<Statement*>* StartingStatement::getFunDefs()
{
	std::list<Statement*> * l = new std::list<Statement*>;

	for (std::list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++)
	{
        if (FunctionDefinition * def = dynamic_cast<FunctionDefinition*>(*i))
        {
			l->push_back(*i);
        }
	}
	
	return l;
}

/*
Puts the latest token returned by the lexer into the buffer
*/
void Parser::getNextToken()
{
	buf = lex->getNextToken();
	if (buf.type == T_NONEXISTENT) throw "Nonexistent token read!\n";
}

/*
Checks if given identifier represents a function
*/
bool Parser::isFunction(Token identifier)
{
	return fun.existsFunction(identifier.string_value);
}

/*
Returns the pointer to the starting statement or nullptr if an exception was encountered
*/
StartingStatement* Parser::doStartingStatement()
{
	StartingStatement * s = nullptr;
	Statement * st = nullptr;
	try
	{
		getNextToken();
		s = new StartingStatement();
		
		while (buf.type != T_END_OF_TEXT)
		{

			st = doStatement();
			s->addStatement(st);

		}

		return s;
	}
    catch (const char * c)
	{
        if(s == nullptr)
        {
            std::string str1(c);
            pc->writeToErrorLog(str1);
            return nullptr;
        }

        pc->restoreFunctionSymbolTable(&fun);

        std::list<Statement*> * fun_list = s->getFunDefs();

        delete s;

		while (!fun_list->empty())
		{
			delete fun_list->front();
			fun_list->pop_front();
		}
		delete fun_list;
        std::string str(c);
        pc->writeToErrorLog(str);

		return nullptr;
	}

    return nullptr;
}

/*
Returns a pointer to a statement or a nullptr or throws an exception
*/
Statement * Parser::doStatement()
{
	InFunctionStatement * s = nullptr;
	FunctionDefinition * f = nullptr;
	try
	{
		f = doFunctionDefinition();

		if (f != nullptr) return f;
		else
		{
			s = doInFunctionStatement();
			if (s != nullptr)
			{
				return s;
			}
            else
            {
                while (buf.type != T_END_OF_TEXT)
                {
                    getNextToken();
                }
                throw "No statement was formed!\n";
            }
		}
	}
	catch (...)
	{
		delete f;
		delete s;
		throw;
	}

	return nullptr;
	
}

/*
Returns a pointer to a function definition or a nullptr or throws an exception
*/
FunctionDefinition * Parser::doFunctionDefinition()
{
	Token id;
	std::list<InFunctionStatement*> * statement_list = nullptr;
	std::list<Token> * arg_list = nullptr;
	FunctionDefinition * f = nullptr;
    bool wasAdded = false;
	if (buf.type != T_KEYWORD || buf.integer_value != K_TO)
	{
		return nullptr;
	}

	try
	{
		getNextToken();

		if (buf.type != T_IDENTIFIER)
		{
			throw "An identifier was expected!\n";
		}

		id = buf;
		getNextToken();

		int i = 0;
		statement_list = new std::list<InFunctionStatement*>;
		arg_list = new std::list<Token>;

		while (buf.type == T_IDENTIFIER && !isFunction(buf) && buf.string_value != id.string_value)
		{
			Token arg = buf;
			arg_list->push_back(arg);
			i++;
			getNextToken();
		}

		f = new FunctionDefinition(id, i, arg_list, statement_list);
        fun.addFunction(f, id.string_value);
        wasAdded = true;

		while (!(buf.type == T_KEYWORD && buf.integer_value == K_END))
		{
			if (buf.type == T_END_OF_TEXT)
			{
				throw "A statement was expected, reached end of text!\n";
			}
			if (buf.type == T_KEYWORD && buf.integer_value == K_TO)
			{
				throw "A statement was expected, found a beginning of a function definition instead!\n";
			}

			statement_list->push_back(doInFunctionStatement());
		}
	
		getNextToken();

		f->update(statement_list);
		
		return f;
	}
	catch (...)
	{

		if (statement_list != nullptr)
		{
			while (!statement_list->empty())
			{
				delete statement_list->front();
				statement_list->pop_front();
			}
		}

        if(!wasAdded) delete arg_list;
		delete f;

		throw;
	}
}

/*
Returns a pointer to a statement that is not a function definition or a nullptr or throws an exception
*/
InFunctionStatement * Parser::doInFunctionStatement()
{
	InFunctionStatement * s = nullptr;
	try
	{
		s = doFunction();
		if (s != nullptr) return s;

		s = doForward();
		if (s != nullptr) return s;

		s = doBackward();
		if (s != nullptr) return s;

		s = doRightTurn();
		if (s != nullptr) return s;

		s = doLeftTurn();
		if (s != nullptr) return s;

		s = doMoveByVector();
		if (s != nullptr) return s;

		s = doMoveToPosition();
		if (s != nullptr) return s;

		s = doSetHeading();
		if (s != nullptr) return s;

		s = doTurtleGoHome();
		if (s != nullptr) return s;

		s = doCleanScreen();
		if (s != nullptr) return s;

		s = doPenUp();
		if (s != nullptr) return s;

		s = doPenDown();
		if (s != nullptr) return s;

		s = doSetColor();
		if (s != nullptr) return s;

		s = doOutput();
		if (s != nullptr) return s;

		s = doPrint();
		if (s != nullptr) return s;

		s = doScan();
		if (s != nullptr) return s;

		s = doLocalMakeScan();
		if (s != nullptr) return s;

		s = doMake();
		if (s != nullptr) return s;

		s = doIfStatement();
		if (s != nullptr) return s;

		s = doRepeatStatement();
		if (s != nullptr) return s;

		s = doTurtleSleep();
		return s;
	}
	catch (...)
	{
		delete s;
		throw;
	}
}

/*
Returns a pointer to a forward statement or a nullptr or throws an exception
*/
Forward * Parser::doForward()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_FD) return nullptr;

	getNextToken();
	AdditiveExpression * a = doAdditiveExpression();
	return new Forward(a);
}

/*
Returns a pointer to a backward statement or a nullptr or throws an exception
*/
Backward * Parser::doBackward()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_BK) return nullptr;

	getNextToken();
	AdditiveExpression * a = doAdditiveExpression();
	return new Backward(a);
}

/*
Returns a pointer to a right turn statement or a nullptr or throws an exception
*/
RightTurn * Parser::doRightTurn()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_RT) return nullptr;
	
	getNextToken();
	AdditiveExpression * a = doAdditiveExpression();
	return new RightTurn(a);
}

/*
Returns a pointer to a left turn statement or a nullptr or throws an exception
*/
LeftTurn * Parser::doLeftTurn()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_LT) return nullptr;
	
	getNextToken();
	AdditiveExpression * a = doAdditiveExpression();
	return new LeftTurn(a);

}

/*
Returns a pointer to a move by vector statement or a nullptr or throws an exception
*/
MoveByVector * Parser::doMoveByVector()
{
	AdditiveExpression * x = nullptr;
	AdditiveExpression * y = nullptr;
	if (buf.type != T_KEYWORD || buf.integer_value != K_MOVE) return nullptr;
	try
	{
		getNextToken();
		x = doAdditiveExpression();
		y = doAdditiveExpression();
		return new MoveByVector(x, y);
	}
	catch (...)
	{
		delete x;
		delete y;
		throw;
	}
}

/*
Returns a pointer to a move to position statement or a nullptr or throws an exception
*/
MoveToPosition * Parser::doMoveToPosition()
{
	AdditiveExpression * x = nullptr;
	AdditiveExpression * y = nullptr;
	if (buf.type != T_KEYWORD || buf.integer_value != K_SETXY) return nullptr;
	
	try
	{
		getNextToken();
		x = doAdditiveExpression();
		y = doAdditiveExpression();
		return new MoveToPosition(x, y);
	}
	catch (...)
	{
		delete x;
		delete y;
		throw;
	}

}

/*
Returns a pointer to a set heading statement or a nullptr or throws an exception
*/
SetHeading * Parser::doSetHeading()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_HEAD) return nullptr;
	
	getNextToken();
	AdditiveExpression * h = doAdditiveExpression();
	return new SetHeading(h);
}

/*
Returns a pointer to a trutle go home statement or a nullptr or throws an exception
*/
TurtleGoHome * Parser::doTurtleGoHome()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_HOME) return nullptr;

	getNextToken();
	return new TurtleGoHome();
}

/*
Returns a pointer to a get x statement or a nullptr or throws an exception
*/
GetX * Parser::doGetX()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_GETX) return nullptr;
	
	getNextToken();
	return new GetX();
}

/*
Returns a pointer to a get y statement or a nullptr or throws an exception
*/
GetY * Parser::doGetY()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_GETY) return nullptr;
	
	getNextToken();
	return new GetY();
}

/*
Returns a pointer to a get heading statement or a nullptr or throws an exception
*/
GetHeading * Parser::doGetHeading()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_GETHEADING) return nullptr;
	
	getNextToken();
	return new GetHeading();
}

/*
Returns a pointer to a clear screen statement or a nullptr or throws an exception
*/
CleanScreen * Parser::doCleanScreen()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_CS) return nullptr;
	
	getNextToken();
	return new CleanScreen();
}

/*
Returns a pointer to a pen up statement or a nullptr or throws an exception
*/
PenUp * Parser::doPenUp()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_PU) return nullptr;
	
	getNextToken();
	return new PenUp();
}

/*
Returns a pointer to a pen down statement or a nullptr or throws an exception
*/
PenDown * Parser::doPenDown()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_PD) return nullptr;
	
	getNextToken();
	return new PenDown();
}

/*
Returns a pointer to a set color statement or a nullptr or throws an exception
*/
SetColor * Parser::doSetColor()
{
	AdditiveExpression * red = nullptr;
	AdditiveExpression * green = nullptr;
	AdditiveExpression * blue = nullptr;
	if (buf.type != T_KEYWORD || buf.integer_value != K_SETCOLOR) return nullptr;
	try {
		getNextToken();
		red = doAdditiveExpression();
		green = doAdditiveExpression();
		blue = doAdditiveExpression();
		return new SetColor(red, green, blue);
	}
	catch (...)
	{
		delete red;
		delete green;
		delete blue; 
		throw;
	}
}

/*
Returns a pointer to an output statement or a nullptr or throws an exception
*/
Output * Parser::doOutput()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_OUTPUT) return nullptr;

	getNextToken();
	AdditiveExpression * a = doAdditiveExpression();
	return new Output(a);

}

/*
Returns a pointer to a print statement or a nullptr or throws an exception
*/
Print * Parser::doPrint()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_PRINT) return nullptr;

	getNextToken();


	if (buf.type == T_STRING)
	{
		Print * p = new Print(buf.string_value);
		getNextToken();
		return p;
	}

	AdditiveExpression * a = doAdditiveExpression();
	return new Print(a);

}

/*
Returns a pointer to a scan statement or a nullptr or throws an exception
*/
Scan * Parser::doScan()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_SCAN) return nullptr;
	
	getNextToken();
	if (buf.type == T_IDENTIFIER)
	{
		if (isFunction(buf))
		{
			throw "A variable name was expected!\n";
		}

		Token id = buf;
		getNextToken();
		return new Scan(id);
	}

	throw "An identifier was expected!\n";
}

/*
Returns a pointer to a make statement or a nullptr or throws an exception
*/
Make * Parser::doMake()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_MAKE) return nullptr;
	
	getNextToken();
	if (buf.type == T_IDENTIFIER)
	{
		if (isFunction(buf))
		{
			throw "A variable name was expected!\n";
		}

		Token id = buf;
		getNextToken();
		AdditiveExpression * assigned_value = doAdditiveExpression();
		return new Make(id, assigned_value);

	}

	throw "An identifier was expected!\n";
}

/*
Returns a pointer to a local make scan statement or a nullptr or throws an exception
*/
LocalMakeScan * Parser::doLocalMakeScan()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_LOCAL) return nullptr;

	getNextToken();

	if (buf.type == T_KEYWORD && buf.integer_value == K_MAKE)
	{
		getNextToken();
		if (buf.type == T_IDENTIFIER)
		{
			if (isFunction(buf))
			{
				throw "A variable name was expected!\n";
			}

			Token id = buf;
			getNextToken();
			AdditiveExpression * assigned_value = doAdditiveExpression();
			return new LocalMakeScan(id, assigned_value);

		}

		throw "An identifier was expected!\n";
	}
	else if (buf.type == T_KEYWORD && buf.integer_value == K_SCAN)
	{
		getNextToken();
		if (buf.type == T_IDENTIFIER)
		{
			if (isFunction(buf))
			{
				throw "A variable name was expected!\n";
			}

			Token id = buf;
			getNextToken();
			return new LocalMakeScan(id);
		}

		throw "An identifier was expected!\n";
	}
	else
	{
		throw "Make or scan was expected!\n";
	}
}

/*
Returns a pointer to an if statement or a nullptr or throws an exception
*/
IfStatement * Parser::doIfStatement()
{
	LogicalExpressionSet * l = nullptr;
	std::list<Statement*> * s = nullptr;
	if (buf.type != T_KEYWORD || buf.integer_value != K_IF) return nullptr;
	try
	{
		getNextToken();
		l = doLogicalExpressionSet();

		if (buf.type == T_BRACKET_OPEN)
		{
			getNextToken();
			s = new std::list<Statement*>;

			while (buf.type != T_BRACKET_CLOSE)
			{
				if (buf.type == T_END_OF_TEXT)
				{
					throw "A closing bracket was expected!\n";
				}
				s->push_back(doInFunctionStatement());
			}
			getNextToken();
			return new IfStatement(l, s);
		}
		else
		{
			throw "An opening bracket was expected!\n";
		}
	}
	catch(...)
	{
		delete l;
		if (s != nullptr)
		{
			while (!s->empty())
			{
				delete s->front();
				s->pop_front();
			}
		}
		throw;
	}
}

/*
Returns a pointer to a repeat statement or a nullptr or throws an exception
*/
RepeatStatement * Parser::doRepeatStatement()
{
	AdditiveExpression * a = nullptr;
	std::list<Statement*> * s = nullptr;
	if (buf.type != T_KEYWORD || buf.integer_value != K_REPEAT) return nullptr;
	
	try 
	{
		getNextToken();
		a = doAdditiveExpression();

		if (buf.type == T_BRACKET_OPEN)
		{
			getNextToken();
			s = new std::list<Statement*>;

			while (buf.type != T_BRACKET_CLOSE)
			{
				s->push_back(doInFunctionStatement());
				if (buf.type == T_END_OF_TEXT)
				{
					throw "A closing bracket was expected!\n";
				}
			}

			getNextToken();
			return new RepeatStatement(a, s);

		}
		else
		{
			throw "An opening bracket was expected!\n";
		}
	}
	catch (...)
	{
		delete a;

		while (!s->empty())
		{
			delete s->front(), s->pop_front();
		}

		delete s;
		throw;
	}
}

/*
Returns a pointer to a turtle sleep statement or a nullptr or throws an exception
*/
TurtleSleep * Parser::doTurtleSleep()
{
	if (buf.type != T_KEYWORD || buf.integer_value != K_SLEEP) return nullptr;

	getNextToken();
	AdditiveExpression * time = doAdditiveExpression();
	return new TurtleSleep(time);
}

/*
Returns a pointer to a variable statement or throws an exception
*/
Variable * Parser::doVariable()
{
    if (buf.type != T_IDENTIFIER)
    {
        return nullptr;
    }
	Token id = buf;
	getNextToken();
	return new Variable(id);
}

/*
Returns a pointer to a function statement or a nullptr or throws an exception
*/
Function * Parser::doFunction()
{
	Function * s = nullptr;
	std::list<AdditiveExpression *> * argument_list = nullptr;
	try
	{
		s = doGetX();
		if (s != nullptr) return s;

		s = doGetY();
		if (s != nullptr) return s;

		s = doGetHeading();
		if (s != nullptr) return s;

		if (buf.type != T_IDENTIFIER)
		{
			return nullptr;
		}

		FunctionDefinition * f = fun.getFunction(buf.string_value);
		if (f == nullptr)
		{
			return nullptr;
		}

		Token id = buf;
		getNextToken();
		int i = f->getNumberOfArgs();

		argument_list = new std::list<AdditiveExpression *>;

		for (int k = 0; k < i; k++)
		{
			AdditiveExpression * a = doAdditiveExpression();
			argument_list->push_back(a);
		}

		return new Function(id, argument_list);
	}
	catch (...)
	{
		while (!argument_list->empty())
		{
			delete argument_list->front(), argument_list->pop_front();
		}
		delete argument_list;
		delete s;
        throw;
	}

}

/*
Executes the function
*/
function_result Function::execute(ProgramContext * pc)
{
    Function * fun = this;
    if(GetX* gx = dynamic_cast<GetX*>(fun))
    {
        return gx->execute(pc);
    }

    if(GetY* gy = dynamic_cast<GetY*>(fun))
    {
        return gy->execute(pc);
    }

    if(GetHeading* gh = dynamic_cast<GetHeading*>(fun))
    {
        return gh->execute(pc);
    }

	FunctionDefinition * f = pc->getFunction(identifier.string_value);
	std::list<Token> * arguments = f->getArgList();
	std::list<Token>::iterator a = arguments->begin();
	std::list<InFunctionStatement*> * statementList = f->getStatementList();
	std::list<int> l;
	function_result r;
	

	for(std::list<AdditiveExpression *>::iterator i = argument_list->begin(); i != argument_list->end() ; i++)
	{
		 l.push_back((*i)->evaluate(pc));
	}

	pc->pushContext();
	for (std::list<int>::iterator b = l.begin(); b != l.end(); b++)
	{
		pc->addLocalVariable(a->string_value, *b);
		a++;
	}

	for (std::list<InFunctionStatement*>::iterator i = statementList->begin(); i != statementList->end(); i++)
	{
		try
		{
			r = (*i)->execute(pc);
		}
		catch (...)
		{
			pc->popContext();
			throw;
		}

		if (r.is_output)
		{
			pc->popContext();
			return r;
		}
	}
	
	pc->popContext();
	return r;
}

/*
Destructor of Function
*/
Function::~Function()
{

    if(argument_list == nullptr) return;

	while (!argument_list->empty()) 
	{ 
		delete argument_list->front(), argument_list->pop_front(); 
	}

	delete argument_list;
}

/*
Evaluates the value of the variable
*/
int Variable::evaluate(ProgramContext * pc)
{
	int i = pc->getVariable(identifier.string_value);
	return i;
}

/*
Evaluates the value of the multiplicative expression
*/
int MultiplicativeExpression::evaluate(ProgramContext * pc)
{
	function_result r;
	if (!has_last_operand)
	{
		switch (first_operand_type)
		{
		case this->M_NUMBER:
			return number.integer_value;
		case this->M_VARIABLE:
			return variable->evaluate(pc);
		case this->M_FUNCTION:
			r = function->execute(pc);
			if (r.returns_a_value) return r.integer_value;
			else throw "Expected a function to return a value!\n";
		case this->M_PARENTHESIS:
			return additive_expression_in_parentheses->evaluate(pc);
		}
	}
	else
	{
        int first_operand = 0;
		switch (first_operand_type)
		{
			case this->M_NUMBER:
				first_operand = number.integer_value;
				break;

			case this->M_VARIABLE:
				first_operand = variable->evaluate(pc);
				break;

			case this->M_FUNCTION:
				r = function->execute(pc);
				if (r.returns_a_value) first_operand = r.integer_value;
				else throw "Expected a function to return a value!\n";
				break;

			case this->M_PARENTHESIS:
				first_operand = additive_expression_in_parentheses->evaluate(pc);
		}

		if (binary_operator.string_value[0] == '*')
		{
			return first_operand * last_operand->evaluate(pc);
		}
		else
		{
			if (last_operand->evaluate(pc) == 0)
			{
				throw "Division by zero!\n";
			}

			return first_operand / last_operand->evaluate(pc);
		}
	}
	return 0;
}

/*
Destructor for MultiplicativeExpression
*/
MultiplicativeExpression::~MultiplicativeExpression()
{
    if(first_operand_type == M_VARIABLE) delete variable;
    if(first_operand_type == M_FUNCTION) delete function;
    if(first_operand_type == M_PARENTHESIS) delete additive_expression_in_parentheses;
    if(has_last_operand) delete last_operand;
}

/*
Evaluates the value of the additive expression
*/
int AdditiveExpression::evaluate(ProgramContext * pc)
{
	int first = first_operand->evaluate(pc);

	if ((!(unary_operator.type == T_EMPTY)) && unary_operator.string_value[0] == '-')
	{
		first *= -1;
	}
	if (!has_last_operand)
	{
		return first;
	}
	else
	{

		if (binary_operator.string_value[0] == '+')
		{
			return first += last_operand->evaluate(pc);
		}
		else
		{
			return first -= last_operand->evaluate(pc);
		}
	}

}

/*
Evaluates the value of the logical expression
*/
bool LogicalExpression::evaluate(ProgramContext * pc)
{
	if (logical_type == L_BASE_LOGICAL_VALUE)
	{
		return logical_value;
	}

	if (logical_type == L_COMPARISON)
	{
		int i = first_operand->evaluate(pc);
		int j = last_operand->evaluate(pc);
		if (binary_operator.string_value[0] == '<')
			return i < j;
		if (binary_operator.string_value[0] == '>')
			return i > j;
		if (binary_operator.string_value[0] == '=')
			return i == j;
		if (binary_operator.string_value[0] == '!')
			return i != j;
	}

	if (logical_type == L_UNARY)
	{
		return !(logical_expression_set->evaluate(pc));
	}

	if (logical_type == L_BRACES)
	{
		return logical_expression_set->evaluate(pc);
	}

	return false;

}

/*
Destructor of LogicalExpression
*/
LogicalExpression::~LogicalExpression()
{
    if(logical_type == L_COMPARISON)
    {
        delete first_operand;
        delete last_operand;
    }
    if(logical_type == L_UNARY || logical_type == L_BRACES) delete logical_expression_set;
}

/*
Evaluates the value of the logical expression set
*/
bool LogicalExpressionSet::evaluate(ProgramContext * pc)
{
	if (!has_last_operand)
	{
		return first_operand->evaluate(pc);
	}

	if (binary_operator.integer_value == K_AND)
	{
		return first_operand->evaluate(pc) && last_operand->evaluate(pc);
	}

	if (binary_operator.integer_value == K_OR)
	{
		return first_operand->evaluate(pc) || last_operand->evaluate(pc);
	}

	if (binary_operator.integer_value == K_XOR)
	{
		return first_operand->evaluate(pc) != last_operand->evaluate(pc);
	}

	return false;
}

/*
Executes a function definition
*/
function_result FunctionDefinition::execute(ProgramContext * pc)
{
	pc->addFunction(this, this->identifier.string_value);
	return function_result();
}

/*
Executes a forward statement
*/
function_result Forward::execute(ProgramContext * pc)
{
	int l = move_by_value->evaluate(pc);
	pc->move_forward(l);
	function_result f;
	return f;
}

/*
Executes a backward statement
*/
function_result Backward::execute(ProgramContext * pc)
{
	int l = move_by_value->evaluate(pc);
	pc->move_forward(-l);
	function_result f;
	return f;
}

/*
Executes a right turn statement
*/
function_result RightTurn::execute(ProgramContext * pc)
{
	int a = move_by_value->evaluate(pc);
    pc->turn_by(-a);
	function_result f;
	return f;
}

/*
Executes a left turn statement
*/
function_result LeftTurn::execute(ProgramContext * pc)
{
	int a = move_by_value->evaluate(pc);
    pc->turn_by(a);
	function_result f;
	return f;
}

/*
Executes a move by vector statement
*/
function_result MoveByVector::execute(ProgramContext * pc)
{
	int x = x_value->evaluate(pc);
	int y = y_value->evaluate(pc);
	pc->move_by(x, y);
	function_result f;
	return f;
}

/*
Executes a move to position statement
*/
function_result MoveToPosition::execute(ProgramContext * pc)
{
	int x = x_value->evaluate(pc);
	int y = y_value->evaluate(pc);
	pc->move_to(x, y);
	function_result f;
	return f;
}

/*
Executes a set heading statement
*/
function_result SetHeading::execute(ProgramContext * pc)
{
	int i = heading_value->evaluate(pc);
	while (i > 3600)
	{
		i -= 3600;
	}

	while (i < 0)
	{
		i += 3600;
	}

	pc->set_heading(i);
	function_result f;
	return f;
}

/*
Executes a turtle go home statement
*/
function_result TurtleGoHome::execute(ProgramContext * pc)
{
	pc->go_home();
	function_result f;
	return f;
}

/*
Executes a get x statement
*/
function_result GetX::execute(ProgramContext * pc)
{
	function_result f;
	f.returns_a_value = true;
	f.integer_value = pc->getx();
	return f;
}

/*
Executes a get y statement
*/
function_result GetY::execute(ProgramContext * pc)
{
	function_result f;
	f.returns_a_value = true;
	f.integer_value = pc->gety();
	return f;
}

/*
Executes a get heading statement
*/
function_result GetHeading::execute(ProgramContext * pc)
{
	function_result f;
	f.returns_a_value = true;
	f.integer_value = pc->geth();
	return f;
}

/*
Executes a clear screen statement
*/
function_result CleanScreen::execute(ProgramContext * pc)
{
	pc->clear_screen();
	function_result f;
	return f;
}

/*
Executes a pen up statement
*/
function_result PenUp::execute(ProgramContext * pc)
{
	pc->pen_up();
	function_result f;
	return f;
}

/*
Executes a pen down statement
*/
function_result PenDown::execute(ProgramContext * pc)
{
	pc->pen_down();
	function_result f;
	return f;
}

/*
Executes a set color statement
*/
function_result SetColor::execute(ProgramContext * pc)
{
    int r = red->evaluate(pc);
    int g = green->evaluate(pc);
    int b = blue->evaluate(pc);
	r %= 256;
	g %= 256;
	b %= 256;
	pc->set_color(r, g, b);
	function_result f;
	return f;
}

/*
Executes an output statement
*/
function_result Output::execute(ProgramContext * pc)
{

	function_result f;
	f.integer_value = evaluate(pc);
	f.returns_a_value = true;
	f.is_output = true;
	return f;
}

/*
Evaluates an output statement
*/
int Output::evaluate(ProgramContext * pc)
{
	return additive_exp->evaluate(pc);
}

/*
Executes print statement
*/
function_result Print::execute(ProgramContext * pc)
{
    if (is_string) pc->writeToLog(string_exp);
    else pc->writeToLog(std::to_string(additive_exp->evaluate(pc)));
    pc->writeToLog("\n");
	function_result f;
	return f;
}

/*
Executes scan statement
*/
function_result Scan::execute(ProgramContext * pc)
{
	int input;
    input = pc->getValueFromUser(identifier.string_value);
	pc->addVariable(identifier.string_value, input);
	function_result f;
	return f;
}

/*
Executes make statement
*/
function_result Make::execute(ProgramContext * pc)
{
	int input = assigned_value->evaluate(pc);
	pc->addVariable(identifier.string_value, input);
	function_result f;
	return f;
}

/*
Executes local make / scan statement
*/
function_result LocalMakeScan::execute(ProgramContext * pc)
{
	if (isScan)
	{
		int input;
		std::cout << "Please input the value of the local variable " << identifier.string_value << "." << std::endl;
		std::cin >> input;
		pc->addLocalVariable(identifier.string_value, input);
		function_result f;
		return f;
	}
	else
	{
		int input = assigned_value->evaluate(pc);
		pc->addLocalVariable(identifier.string_value, input);
		function_result f;
		return f;
	}
}

/*
Executes an if statement
*/
function_result IfStatement::execute(ProgramContext * pc)
{
	function_result f;

	if (condition->evaluate(pc))
	{

		if (statementList->empty())
		{
			return f;
		}

		for (std::list <Statement*>::iterator i = statementList->begin(); i != statementList->end(); i++)
		{
			if (*i != nullptr)
				f = (*i)->execute(pc);
			if (f.is_output) return f;
		}
	}

	return f;
}

/*
Executes a repeat statement
*/
function_result RepeatStatement::execute(ProgramContext * pc)
{
	function_result f;
	if (statementList->empty())
	{
		return f;
	}

	int rep_count = 0;
	while (number_of_repetitions->evaluate(pc) - rep_count)
	{
		for (std::list <Statement*>::iterator i = statementList->begin(); i != statementList->end(); i++)
		{
			if (*i != nullptr)
				f = (*i)->execute(pc);
			if (f.is_output) return f;
		}
		rep_count++;
	}

	return f;
}

/*
Executes a turtle sleep statement
*/
function_result TurtleSleep::execute(ProgramContext * pc)
{
	function_result f;
    int t = time_to_sleep->evaluate(pc);
	if (t < 0)
	{
		throw "Cannot sleep for a negative amount of miliseconds!\n";
	}
    //
    std::chrono::milliseconds militime(t);
    std::this_thread::sleep_for(militime);
	
	return f;
}


