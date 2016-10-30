#pragma once

#include <string>
#include <cmath>
#include <map>
using std::string; using std::map;

// TODO: rename variables?
class StringTrie
{
	struct Node
	{
		map<char, Node> children;
		bool isValidString;
	};

	Node Root;
	size_t Size;

	void toString(string *result, string *parentString, Node *node);

public:
	StringTrie();
	~StringTrie();
	
	bool add(string data);
	size_t size() { return Size; }

	string toString();
};