#include "StringTrie.h"

// TODO: All of this was directly ported from D
// and needs to be cleaned up significantly

StringTrie::StringTrie()
{
	Size = 0;
	Root = Node{ map<char,Node>(), false };
}

StringTrie::~StringTrie()
{
}

bool StringTrie::add(string data)
{
	//pointer to the node currently being modified
	Node* CurrentNode = &Root;

	//the current character in data being added
	char current_char = data[0];

	//iterate backwards from the length of data until there is one character left
	for(auto i = data.length(); i > 0; --i,
		//set CurrentNode to point to the appropriate child
		CurrentNode = &((*CurrentNode).children[current_char]),
		//set current_char to the next character in data
		current_char = data[data.length() - i])
	{
		//if the final character of data is already in the trie
		if(CurrentNode->children.find(current_char) != CurrentNode->children.end() && i == 1)
		{
			//return true if the child was not a valid string already
			if(!((*CurrentNode).children[current_char].isValidString))
			{	
				++Size;
				return (*CurrentNode).children[current_char].isValidString = true;
			}
			//return false if it was a valid string
			return false;
		}
		else
		{
			//if the current character from data is not in the trie
			if(!(CurrentNode->children.find(current_char) != CurrentNode->children.end()))
			{
				//add a new Node to the trie
				(*CurrentNode).children[current_char] = Node{map<char,Node>(),false};
			}
			//if add is at the last character add the end of the new string and return
			if(i == 1)
			{
				++Size;
				return (*CurrentNode).children[current_char].isValidString = true;
			}
		}
	}
	return false;
}

void StringTrie::toString(string *result, string *parentString, Node *node)
{
	for(auto &pair: node->children)
	{
		string childString = *parentString + pair.first;
		if(pair.second.isValidString)
		{
			*result += "\"" + childString + "\", ";
		}
		toString(result, &childString, &pair.second);
	}
}

string StringTrie::toString()
{
	string result = "{ ";
	string path = "";
	toString(&result, &path, &Root);
	result = result.substr(0, result.length()-2) + " }";
	return result;
}