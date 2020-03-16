#ifndef SOURCE_H
#define SOURCE_H

#pragma once
#include <string>

struct position
{
	int byte_number;
	int column_number;
	int row_number;
};


class Source
{
public:
	Source();
	char getNextChar();
	position getPosition();
	void addToSource(std::string s);

private:
	position pos;
	std::string source;
	

};

#endif
