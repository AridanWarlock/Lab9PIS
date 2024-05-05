#pragma once
#include <iostream>
#include <exception>

using namespace std;

namespace Stack
{
	template<class T>
	class Stack
	{
	private:
		struct Node
		{
			T Data;
			Node* Next;
			template<class U>
			Node(U&& data, Node* next) : Data(forward<U>(data)), Next(next) {}
			template<class U>
			Node(U&& data) : Node(data, nullptr) {}
		};
		Node* _head;
		int _count;
	public:
		Stack() : _head(nullptr), _count(0) {}
		Stack(initializer_list<T> il) : Stack() {
			typename initializer_list<T>::iterator iter = il.end() - 1;
			for (; iter != il.begin(); --iter)
				Push(*iter);
			Push(*il.begin());

		}
		Stack(const Stack& other)
		{
			*this = other;
		}
		~Stack()
		{
			while (_head)
			{
				Node* temp = _head;
				_head = _head->Next;
				delete temp;
			}
		}
		Stack& operator=(const Stack& other)
		{
			if (this != &other)
			{
				if (!Empty())
					Clear();

				for (Node* copy = other._head; copy != nullptr; copy = copy->Next)
					Push(copy->Data);
			}
			return *this;
		}
		template <class U>
		void Push(U&& value)
		{
			_head = new Node(forward<U>(value), _head);
			_count++;
		}

		bool Pop()
		{
			if (Empty())
				return false;

			Node* temp = _head;
			_head = _head->Next;
			delete temp;
			_count--;

			return true;
		}
		void Clear()
		{
			while (_head)
			{
				Node* temp = _head;
				_head = _head->Next;
				delete temp;
			}
			_count = 0;
		}

		T& Top() const {
			if (Empty())
				throw invalid_argument("Коллекция пуста!");
			return _head->Data;
		}
		bool Empty() const {
			return !_head;
		}
		int Count() const {
			return _count;
		}

		void Swap(Stack& other)
		{
			Stack temp = *this;
			*this = other;
			other = temp;
		}
	};
}
