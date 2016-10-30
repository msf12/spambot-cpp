#pragma once
#include <string>

using std::string;

struct StringQueue
{
	struct Node
	{
		void *metadata;
		Node *next = nullptr;
		string data;
	};

	Node *head = nullptr;
	Node *tail = nullptr;
	int size = 0;

	~StringQueue();

	void enqueue(string data);
	void enqueue(string data, void *metadata);
	void *peek_metadata();
	string dequeue();
};