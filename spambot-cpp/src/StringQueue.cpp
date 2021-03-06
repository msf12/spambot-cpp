#include "StringQueue.h"

void StringQueue::enqueue(string data)
{
	if(size)
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

void StringQueue::enqueue(string data, void *metadata)
{
	StringQueue::enqueue(data);
	tail->metadata = metadata;
}

string StringQueue::dequeue()
{
	if(size)
	{
		if(head == nullptr)
		{
			head->data = "This just broke";
		}
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

void * StringQueue::peek_metadata()
{
	return head->metadata;
}

StringQueue::~StringQueue()
{
	while(size)
	{
		Node *temp = head;
		head = head->next;
		delete temp;
		--size;
	}
	delete head;
	delete tail;
}