#include "parser.hpp"
#include <windows.h>

int main()
{
	Source * s = new Source();
	KeywordMap k = KeywordMap();
	std::string str;
	std::getline(std::cin, str);
	if (str == "exit")
	{
		delete s;
		return 0;
	}

	s->addToSource(str);

	Lexer * l = new Lexer(s, k);
	ProgramContext * pc = new ProgramContext();
	pc->pushContext();
	Parser p = Parser(l);
	StartingStatement * x = nullptr;
	std::list<Statement*> * fun_list = nullptr;
	std::list<Statement*> aggregated;

	while (str != "exit")
	{
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

		std::getline(std::cin, str);
		s->addToSource(str);
		l->updateLexer();
	}

	while (!aggregated.empty())
	{
		delete aggregated.front();
		aggregated.pop_front();
	}

	delete s;
	delete pc;

	return 0;
}