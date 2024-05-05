#pragma once
#include <iostream>

using namespace std;

namespace CircularLinkedList
{
	template <class T>
	class CircularLinkedList
	{
	private:
		struct Node
		{
			T Data;
			Node* Next;
			Node* Prev;
			template<class U>
			Node(U&& data, Node* next, Node* prev) : Data(forward<U>(data)), Next(next), Prev(prev) {}
			template<class U>
			Node(U&& data, Node* next) : Node(data, next, nullptr) {}
			template<class U>
			Node(U&& data) : Node(data, nullptr, nullptr) {}
		};
		Node* _head;
		Node* _tail;
		int _count;
		template<class U>
		void FirstAdd(U&& value)
		{
			_head = new Node(forward<U>(value));
			_tail = _head;

			_head->Next = _tail;
			_head->Prev = _tail;
			_tail->Next = _head;
			_tail->Prev = _head;

			_count++;
		}
		Node* GetNodeByIndex(int index) const
		{
			if (abs(index) >= _count)
				index %= _count;
			index = (index >= 0) ? index : _count + index;

			Node* node;
			if (_count / 2 > index)
			{
				node = _head;
				for (int i = 0; i < index; i++)
					node = node->Next;
			}
			else
			{
				node = _tail;
				for (int i = _count - 1; i > index; i--)
					node = node->Prev;
			}
			return node;
		}
	public:
		class Iterator
		{
		private:
			Node* _current;
		public:
			Iterator() : _current(nullptr) {}
			Iterator(Node* current) : _current(current) {}
			Iterator(const Iterator& other) {
				*this = other;
			}
			Iterator& operator=(const Iterator& other) {
				_current = other._current;
				return *this;
			}
			~Iterator() {}

			T& operator *() const {
				return _current->Data;
			}
			Iterator& operator=(const T& data) {
				_current->Data = data;
				return *this;
			}
			Iterator& operator+=(const int n) {
				for (int i = 0; i < n && _current != nullptr; i++)
					_current = _current->Next;
				return *this;
			}
			Iterator& operator-=(const int n) {
				for (int i = 0; i < n && _current != nullptr; i++)
					_current = _current->Prev;
				return *this;
			}
			Iterator& operator++()
			{
				_current = _current->Next;
				return *this;
			}
			Iterator& operator--()
			{
				_current = _current->Prev;
				return *this;
			}

			friend Iterator& operator+ (const Iterator& other, const int n)
			{
				if (n < 0)
					return other - (-n);

				Iterator* iterator = new Iterator(other);
				for (int i = 0; i < n && iterator->_current != nullptr; i++)
					++(*iterator);
				return *iterator;
			}
			friend Iterator& operator- (const Iterator& other, const int n)
			{
				if (n < 0)
					return other + (-n);

				Iterator* iterator = new Iterator(other);
				for (int i = 0; i < n && iterator->_current != nullptr; i++)
					--(*iterator);
				return *iterator;
			}

			bool operator==(const Iterator& other) const {
				return _current == other._current;
			}
			bool operator!=(const Iterator& other) const {
				return _current != other._current;
			}
		};
		CircularLinkedList() : _count(0), _head(nullptr), _tail(nullptr) {}
		CircularLinkedList(initializer_list<T> il) : CircularLinkedList() {
			for (auto i : il)
				PushBack(i);
		}
		CircularLinkedList(const CircularLinkedList& other)
		{
			_count = 0;
			_head = nullptr;
			_tail = _head;

			Node* temp = other._head;
			for (int i = 0; i < other._count; ++i)
			{
				PushBack(temp->Data);
				temp = temp->Next;
			}
		}
		template <class U>
		void PushBack(U&& value)
		{
			if (Empty())
				return FirstAdd(value);

			Node* node = new Node(forward<U>(value), _head, _tail);
			_tail->Next = node;
			_tail = node;
			_head->Prev = node;

			_count++;
		}
		template <class U>
		void PushFront(U&& value)
		{
			if (Empty())
				return FirstAdd(value);

			Node* node = new Node(forward<U>(value), _head, _tail);
			_head->Prev = node;
			_head = node;
			_tail->Next = node;

			_count++;
		}
		template <class U>
		void Insert(U&& value, int index)
		{
			index = (index < _count) ? index : index % _count;
			index = (index >= 0) ? index : _count + index;

			if (index == 0)
				return PushFront(value);
			if (index == _count)
				return PushBack(value);

			Node* nodeByIndex = GetNodeByIndex(index);

			Node* node = new Node(forward<U>(value), nodeByIndex, nodeByIndex->Prev);
			nodeByIndex->Prev->Next = node;
			nodeByIndex->Prev = node;

			_count++;
		}
		bool PopBack()
		{
			if (Empty())
				return false;

			if (_count == 1)
			{
				Clear();
				return true;
			}

			_tail = _tail->Prev;
			delete _tail->Next;
			_tail->Next = _head;

			_count--;
			return true;
		}
		bool PopFront()
		{
			if (Empty())
				return false;

			if (_count == 1)
			{
				Clear();
				return true;
			}

			_head = _head->Next;
			delete _tail->Next;
			_tail->Next = _head;

			_count--;
			return true;
		}
		template <class U>
		bool Remove(U&& target)
		{
			if (Empty())
				return false;

			Node* deleted = _head;
			int index = 0;
			for (; index < _count && deleted->Data != target; ++index, deleted = deleted->Next);

			if (index == _count)
				return false;

			if (_count == 1)
			{
				Clear();
				return true;
			}

			deleted->Prev->Next = deleted->Next;
			deleted->Next->Prev = deleted->Prev;
			delete deleted;

			_count--;
			return true;
		}
		bool RemoveAt(int index)
		{
			if (Empty())
				return false;

			if (_count == 1)
			{
				Clear();
				return true;
			}

			index %= _count;

			if (index == 0)
				return PopFront();
			if (index == _count - 1)
				return PopBack();

			Node* deleted = GetNodeByIndex(index);
			deleted->Prev->Next = deleted->Next;
			deleted->Next->Prev = deleted->Prev;
			delete deleted;

			_count--;
			return true;
		}

		template <class U>
		int Find(U&& target)const
		{
			Node* current = _head;
			for (int i = 0; i < _count; ++i, current = current->Next)
				if (current->Data == target)
					return i;
			return -1;
		}
		T& operator[](int index) const
		{
			return GetNodeByIndex(index)->Data;
		}

		Iterator& Begin() const {
			Iterator* iter = new Iterator(_head);
			return *iter;
		}
		Iterator& End() const {
			Iterator* iter = new Iterator(_tail);
			return *iter;
		}
		void Clear()
		{
			Node* iterator = _head;
			Node* deleted;
			while (iterator != nullptr)
			{
				deleted = iterator;
				iterator = iterator->Next;
				delete deleted;
			}
			_count = 0;
			_head = nullptr;
			_tail = nullptr;
		}
		bool Empty() const {
			return _head == nullptr;
		}
		int Count() const {
			return _count;
		}
	};
}

