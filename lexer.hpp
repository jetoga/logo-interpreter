#ifndef LEXER_H
#define LEXER_H

#pragma once
#include <string>
#include <iostream>
#include <climits>
#include "source.hpp"
#include "hash.hpp"

enum t_token { T_NUMBER, T_ADD_OPER, T_MULT_OPER, T_PAREN_OPEN, T_PAREN_CLOSE, T_COMP_OPER, T_LOG_OPEN, T_LOG_CLOSE, T_STRING, T_IDENTIFIER, T_KEYWORD, T_BRACKET_OPEN, T_BRACKET_CLOSE, T_NONEXISTENT, T_END_OF_TEXT, T_EMPTY };

enum t_value { T_INT, T_STR, T_NONE };
	
enum keyword_type {K_OR, K_XOR, K_AND, K_NOT, K_FD, K_BK, K_RT, K_LT, K_MOVE, K_SETXY, K_HEAD, K_HOME, K_GETX, K_GETY, K_GETHEADING, K_CS, K_PU, K_PD, K_SETCOLOR, K_OUTPUT, K_PRINT, K_SCAN, K_MAKE, K_LOCAL, K_IF, K_REPEAT, K_TO, K_END, K_TRUE, K_FALSE, K_SLEEP };

class Token
{
public:
	Token(t_token t, position p, int i) : type(t), value(T_INT), pos(p), integer_value(i) { }
	Token(t_token t, position p, std::string s) : type(t), value(T_STR), pos(p), string_value(s) { }
	Token(t_token t, position p, int i, std::string s) : type(t), value(T_STR), pos(p), integer_value(i), string_value(s) { }
	Token(t_token t, position p, char c) : type(t), value(T_STR), pos(p), string_value(1, c) { }
	Token(t_token t, position p) : type(t), value(T_NONE), pos(p) { }
	Token() : type(T_EMPTY) { }
	Token(const Token &t);
	Token& operator= (Token t);

	void showToken();
	std::string getTokenType();

	t_token type;
	t_value value;
	position pos;
	int integer_value;
	std::string string_value;
};


class Lexer
{
public:
	Lexer(Source * s, KeywordMap &k);
	Token getNextToken();
	void updateLexer();

private:
	void getChar();
	void getPosition();
	void skipWhitespace();
	void skipAfterNonexistent();
	bool isEOF(char c);
	bool isSeparator(char c);
	int getKeywordEnum(std::string);
	Token buildNumber(Token t);
    Token buildString();
    Token buildIdent();

	Source * s;
	KeywordMap k;
	position current_position;
	char current_character;
};

#endif
