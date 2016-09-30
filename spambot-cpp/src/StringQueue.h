#pragma once
#include <string>

using std::string;

struct StringQueue
{
	struct Node
	{
		string data;
		Node *next = nullptr;
	};

	Node *head = nullptr;
	Node *tail = nullptr;
	int size = 0;

	~StringQueue();

	void enqueue(string data);
	string dequeue();
};