#include "lexer.hpp"

/*
Constructor of the Lexer class, needs the source
*/
Lexer::Lexer(Source * s, KeywordMap & k)
{
	this->s = s;
	this->k = k;
	getPosition();
	getChar();
}

/*
Returns next token
*/
Token Lexer::getNextToken()
{
	
	Token t;

	skipWhitespace();
	getPosition();

	/*T_BRACKET_OPEN*/
	if (current_character == '[')
	{
		t = Token(T_BRACKET_OPEN, current_position, current_character);
		getChar();
		return t;
	}

	/*T_BRACKET_CLOSE*/
	if (current_character == ']')
	{
		t = Token(T_BRACKET_CLOSE, current_position, current_character);
		getChar();
		return t;
	}

	/*T_PAREN_OPEN*/
	if (current_character == '(')
	{
		t = Token(T_PAREN_OPEN, current_position, current_character);
		getChar();
		return t;
	}

	/*T_PAREN_CLOSE*/
	if (current_character == ')')
	{
		t = Token(T_PAREN_CLOSE, current_position, current_character);
		getChar();
		return t;
	}

	/*T_ADD_OPER*/
	if (current_character == '+' || current_character == '-')
	{
		t = Token(T_ADD_OPER, current_position, (char)current_character);
		getChar();
		return t;
	}

	/*T_MULT_OPER*/
	if (current_character == '*' || current_character == '/')
	{
		t = Token(T_MULT_OPER, current_position, current_character);
		getChar();
		return t;
	}

	/*T_LOG_OPEN*/
	if (current_character == '{')
	{
		t = Token(T_LOG_OPEN, current_position, current_character);
		getChar();
		return t;
	}
	/*T_LOG_CLOSE*/
	if (current_character == '}')
	{
		t = Token(T_LOG_CLOSE, current_position, current_character);
		getChar();
		return t;
	}

	/*T_COMP_OPER*/
	if (current_character == '<' || current_character == '>' || current_character == '=' )
	{	
		t = Token(T_COMP_OPER, current_position, current_character);
		getChar();
		return t;
	}
	if (current_character == '!' )
	{
		getChar();
		if (current_character == '=')
		{
			getChar();
			return Token(T_COMP_OPER, current_position, '!');
		}
		else
		{
			return  Token(T_NONEXISTENT, current_position);
		}
	}

	/*T_NUMBER*/
	if (isdigit(current_character))
		return buildNumber(t);

	/*T_STRING*/
	if (current_character == '"')
        return buildString();

	/*T_IDENTIFIER, T_KEYWORD*/
	if (isalpha(current_character))
        return buildIdent();

	/*T_END_OF_TEXT*/
	if (isEOF(current_character))
	{
		getPosition();
		return Token(T_END_OF_TEXT, current_position);
	}

	t = Token(T_NONEXISTENT, current_position);
	skipAfterNonexistent();
	return t;
}

void Lexer::updateLexer()
{
	getChar();
	getPosition();
}

/*
Sets current_character, which represents the next byte read from the source
*/
void Lexer::getChar()
{
	current_character = s->getNextChar();
}

/*
Sets current_position, which represents the position of the next byte to be read from the source
*/
void Lexer::getPosition()
{
	current_position = s->getPosition();
}

/*
Returns the value from keyword_type enumerator matching the given keyword, -1 if not a matching keyword
*/
int Lexer::getKeywordEnum(std::string s)
{
	if (s == "or")
		return K_OR;
	if (s == "xor")
		return K_XOR;
	if (s == "and")
		return K_AND;
	if (s == "not")
		return K_NOT;
	if (s == "fd")
		return K_FD;
	if (s == "bk")
		return K_BK;
	if (s == "rt")
		return K_RT;
	if (s == "lt")
		return K_LT;
	if (s == "move")
		return K_MOVE;
	if (s == "setxy")
		return K_SETXY;
	if (s == "head")
		return K_HEAD;
	if (s == "home")
		return K_HOME;
	if (s == "getx")
		return K_GETX;
	if (s == "gety")
		return K_GETY;
	if (s == "getheading")
		return K_GETHEADING;
	if (s == "cs")
		return K_CS;
	if (s == "pu")
		return K_PU;
	if (s == "pd")
		return K_PD;
	if (s == "setcolor")
		return K_SETCOLOR;
	if (s == "output")
		return K_OUTPUT;
	if (s == "print")
		return K_PRINT;
	if (s == "scan")
		return K_SCAN;
	if (s == "make")
		return K_MAKE;
	if (s == "local")
		return K_LOCAL;
	if (s == "if")
		return K_IF;
	if (s == "repeat")
		return K_REPEAT;
	if (s == "to")
		return K_TO;
	if (s == "end")
		return K_END;
	if (s == "true")
		return K_TRUE;
	if (s == "false")
		return K_FALSE;
	if (s == "sleep")
		return K_SLEEP;
	return -1;
}

/*
Reads from the source and skips all whitespace characters
*/
void Lexer::skipWhitespace()
{
	while (isspace(current_character))
	{
		getChar();
	}
}

/*
Skips the characters that caused a number token to not be accepted
*/
void Lexer::skipAfterNonexistent()
{
	while (!isSeparator(current_character))
	{
		getChar();
	}
}

/*
Checks if given character is the End Of File character
*/
bool Lexer::isEOF(char c)
{
	if (c == EOF) return true;
	return false;
}

/*
Checks if given character could be a separator after a number or an identifier/keyword
*/
bool Lexer::isSeparator(char c)
{
    if ( c== '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}'
        || c == '+' || c == '-'	|| c == '*' || c == '/'	|| c == '<' || c == '>'
        || c == '=' || c == '!'	|| c == '"' || isspace(c) || isEOF(c))
		return true;

	return false;
}

/*
Builds a token representing a number returning either it or a nonexsistent token if not a number
*/
Token Lexer::buildNumber(Token t)
{
	if (current_character == '0')
	{
		getChar();
		if (isSeparator(current_character))
		{

			return Token(T_NUMBER, current_position, 0);
		}
		else
		{
			t = Token(T_NONEXISTENT, current_position);
			skipAfterNonexistent();
			return t;
		}
	}
	
 	unsigned int i = current_character - '0';
	getChar();
	while (isdigit(current_character))
	{
		i *= 10;
		i += current_character - '0';
		if (i > INT_MAX)
		{
			t = Token(T_NONEXISTENT, current_position);
			skipAfterNonexistent();
			return t;
		}
		getChar();
	}

	if (isSeparator(current_character))
	{
		return Token(T_NUMBER, current_position, int(i));
	}
	else
	{
		t = Token(T_NONEXISTENT, current_position);
		skipAfterNonexistent();
		return t;
	}
	
}

/*
Builds a token representing a string and returns it unless source runs out of characters before the string's end
*/
Token Lexer::buildString()
{
	getChar();
	std::string str = "";
	if (current_character == '"') return Token(T_STRING, current_position, str);
	while (current_character != '"' && !isEOF(current_character))
	{
		if (current_character == '\\')
		{
			getChar();
			if (current_character == '"')
			{
				str += current_character;
				getChar();
			}
			else
			{
				str += '\\';
			}
		}
		
		else
		{
			str += current_character;
			getChar();
		}

	}

	if (isEOF(current_character))
	{
		getChar();
		return Token(T_NONEXISTENT, current_position);
	}
	else
	{
		getChar();
		return Token(T_STRING, current_position, str);
	}
}

/*
Builds a token representing an identifier or a keyword, depending on the results from KeywordMap and returns it if possible; if not, returns a nonexistent token
*/
Token Lexer::buildIdent()
{
	std::string str = "";
	str += current_character;
	getChar();
	while (current_character == '_' || isalpha(current_character) || isdigit(current_character))
	{
		str += current_character;
		getChar();
	}

	if (k.checkHash(str))
	{
		int i = getKeywordEnum(str);
		if(i >= 0)
			return Token(T_KEYWORD, current_position, i, str);
		else
			return Token(T_NONEXISTENT, current_position);
	}
	else
	{
		return Token(T_IDENTIFIER, current_position, str);
	}
	
}

Token::Token(const Token & t)
{
	this->type = t.type;
	this->value = t.value;
	this->pos.byte_number = t.pos.byte_number;
	this->pos.column_number = t.pos.column_number;
	this->pos.row_number = t.pos.row_number;
	this->integer_value = t.integer_value;
	this->string_value = std::string(t.string_value);
}

Token & Token::operator=(Token t)
{
	this->type = t.type;
	this->value = t.value;
	this->pos.byte_number = t.pos.byte_number;
	this->pos.column_number = t.pos.column_number;
	this->pos.row_number = t.pos.row_number;
	this->integer_value = t.integer_value;
	this->string_value = std::string(t.string_value);
	return *this;
}

/*
Debug related function which shows the contents of a member of the Token class
*/
void Token::showToken()
{
	std::cout << "Token type: " << getTokenType() << std::endl;;
	
	std::cout << "Token position: " << pos.byte_number << ", Row: " << pos.row_number << ", Column: " << pos.column_number << std::endl;

	if (value == T_INT)
	{
		std::cout << "Token value is int: " << integer_value << std::endl;
	}
	else if (value == T_STR)
	{
		std::cout << "Token value is string: " << string_value << std::endl;
	}
	else
	{
		if (type == T_NONEXISTENT)
		{
			std::cout << "Error! Token does not exist!" << std::endl;
		}
		else
		{
			std::cout << "End of character stream reached" << std::endl;
		}
	}

	std::cout << std::endl;
}

/*
Returns the string that represents the type in the enumerator
*/
std::string Token::getTokenType()
{
	switch (type)
	{
	case T_NUMBER:
		return "T_NUMBER";
	case T_ADD_OPER:
		return "T_ADD_OPER";
	case T_MULT_OPER:
		return "T_MULT_OPER";
	case T_PAREN_OPEN:
		return "T_PAREN_OPEN";
	case T_PAREN_CLOSE:
		return "T_PAREN_CLOSE";
	case T_COMP_OPER:
		return "T_COMP_OPER";
	case T_LOG_OPEN:
		return "T_LOG_OPEN";
	case T_LOG_CLOSE:
		return "T_LOG_CLOSE";
	case T_STRING:
		return "T_STRING";
	case T_IDENTIFIER:
		return "T_IDENTIFIER";
	case T_KEYWORD:
		return "T_KEYWORD";
	case T_BRACKET_OPEN:
		return "T_BRACKET_OPEN";
	case T_BRACKET_CLOSE:
		return "T_BRACKET_CLOSE";
	case T_NONEXISTENT:
		return "T_NONEXISTENT";
	case T_END_OF_TEXT:
		return "T_END_OF_TEXT";
    case T_EMPTY:
        return "T_EMPTY";
	}
	return "";
}
