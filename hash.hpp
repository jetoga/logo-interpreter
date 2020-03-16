#ifndef HASH_H
#define HASH_H

#pragma once
#include <string>
#include <map>

typedef std::map<int, std::string> keyword_map;

class KeywordMap
{
public:
	KeywordMap();
	bool checkHash(std::string s);
	

private:
	int p = 71;
	int q = 9887;
	keyword_map k;
	int hash(std::string s);
	

};

#endif
