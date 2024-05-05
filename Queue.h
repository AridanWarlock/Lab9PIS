#pragma once
#include <iostream>
#include <exception>

using namespace std;

namespace Queue
{
	template<class T>
	class Queue
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
		Node* _tail;
		int _count;
	public:
		Queue() : _head(nullptr), _tail(nullptr), _count(0) {}
		Queue(initializer_list<T> il) {
			for (auto i : il)
				Push(i);
		}
		Queue(const Queue& other) {
			*this = other;
		}
		~Queue() {
			while (_head)
			{
				Node* temp = _head;
				_head = _head->Next;
				delete temp;
			}
		}
		Queue& operator=(const Queue& other)
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
			if (!_head) {
				_head = new Node(forward<U>(value));
				_tail = _head;
			}
			else {
				_tail->Next = new Node(forward<U>(value));
				_tail = _tail->Next;
			}

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
		}
		void Clear()
		{
			while (_head)
			{
				Node* temp = _head;
				_head = _head->Next;
				delete temp;
			}
			_tail = nullptr;
			_count = 0;
		}

		T& Front() const {
			if (Empty())
				throw invalid_argument("Коллекция пуста!");
			return _head->Data;
		}
		T& Back() const {
			if (Empty())
				throw invalid_argument("Коллекция пуста!");
			return _tail->Data;
		}

		bool Empty() const {
			return _count == 0;
		}
		int Count() const {
			return _count;
		}

		template <class U>
		int Find(U&& target) const
		{
			Node* pointer = _head;
			for (int i = 0; i < _count; i++, pointer = pointer->Next)
				if (pointer->Data == target)
					return i;

			return -1;
		}

		void Swap(Queue& other)
		{
			Queue temp = *this;
			*this = other;
			other = temp;
		}
	};
}
