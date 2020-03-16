#include "hash.hpp"

/*
Constructor of the Keyword class
*/
KeywordMap::KeywordMap()
{
	k.insert(keyword_map::value_type(hash("or"), "or"));
	k.insert(keyword_map::value_type(hash("xor"), "xor"));
	k.insert(keyword_map::value_type(hash("and"), "and"));
	k.insert(keyword_map::value_type(hash("not"), "not"));
	k.insert(keyword_map::value_type(hash("fd"), "fd"));
	k.insert(keyword_map::value_type(hash("bk"), "bk"));
	k.insert(keyword_map::value_type(hash("rt"), "rt"));
	k.insert(keyword_map::value_type(hash("lt"), "lt"));
	k.insert(keyword_map::value_type(hash("move"), "move"));
	k.insert(keyword_map::value_type(hash("setxy"), "setxy"));
	k.insert(keyword_map::value_type(hash("head"), "head"));
	k.insert(keyword_map::value_type(hash("home"), "home"));
	k.insert(keyword_map::value_type(hash("getx"), "getx"));
	k.insert(keyword_map::value_type(hash("gety"), "gety"));
	k.insert(keyword_map::value_type(hash("getheading"), "getheading"));
	k.insert(keyword_map::value_type(hash("cs"), "cs"));
	k.insert(keyword_map::value_type(hash("pu"), "pu"));
	k.insert(keyword_map::value_type(hash("pd"), "pd"));
	k.insert(keyword_map::value_type(hash("setcolor"), "setcolor"));
	k.insert(keyword_map::value_type(hash("output"), "output"));
	k.insert(keyword_map::value_type(hash("print"), "print"));
	k.insert(keyword_map::value_type(hash("scan"), "scan"));
	k.insert(keyword_map::value_type(hash("make"), "make"));
	k.insert(keyword_map::value_type(hash("local"), "local"));
	k.insert(keyword_map::value_type(hash("if"), "if"));
	k.insert(keyword_map::value_type(hash("repeat"), "repeat"));
	k.insert(keyword_map::value_type(hash("to"), "to"));
	k.insert(keyword_map::value_type(hash("end"), "end"));
	k.insert(keyword_map::value_type(hash("true"), "true"));
	k.insert(keyword_map::value_type(hash("false"), "false"));
	k.insert(keyword_map::value_type(hash("sleep"), "sleep"));
}

/*
Returns true if a given string exists in the keyword map
*/
bool KeywordMap::checkHash(std::string s)
{
	std::map <int, std::string> :: const_iterator i = k.find(hash(s));
	if (i == k.end())
	{
		return false;
	}

	if (i->second == s)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

/*
Returns the calculated hash of the input string
*/
int KeywordMap::hash(std::string s)
{
	int h = 0;
	for (int i = int(s.length() - 1); i >= 0; i--)
	{
		h = (p * h + int(s[i])) % q;
	}
	return h;
}