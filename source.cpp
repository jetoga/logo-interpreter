#include "source.hpp"

/*
Default constructor of the source, with default values
*/
Source::Source()
{
	source = "";
	pos.byte_number = 0;
	pos.column_number = 0;
	pos.row_number = 0;
}

/*
Returns next non-processed character from the source
*/
char Source::getNextChar()
{
	//TO DO change source to be flexible
	if (pos.byte_number >= int(source.length())) return EOF;

	char c = source[size_t(pos.byte_number)];

	if (c == '\n')
	{
		pos.column_number = 0;
		pos.row_number++;
	}
	else
	{
		pos.column_number++;
	}

	pos.byte_number++;

	return c;
}

/*
Returns current position of character that will be read
*/
position Source::getPosition()
{
	return pos;
}

void Source::addToSource(std::string s)
{
	source = std::string(s);
	pos.byte_number = 0;
	pos.column_number = 0;
	pos.row_number = 0;
}
