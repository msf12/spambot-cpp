#include "StringQueue.h"

void StringQueue::enqueue(string data)
{
	if (size)
	{
		tail->next = new Node;
		tail = tail->next;
		tail->data = data;
	}
	else
	{
		head = new Node;
		tail = head;
		head->data = data;
	}
	++size;
}

string StringQueue::dequeue()
{
	if (size)
	{
		Node *temp = head;
		string result = temp->data;
		head = head->next;
		delete temp;
		--size;
		return result;
	}
	else
	{
		return "";
	}
}

StringQueue::~StringQueue()
{
	while (size)
	{
		Node *temp = head;
		head = head->next;
		delete temp;
		--size;
	}
	delete head;
	delete tail;
}