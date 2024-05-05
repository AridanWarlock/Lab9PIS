#pragma once
#include <iostream>
#include <exception>

using namespace std;

namespace List
{
	template <class T>
	class List
	{
	private:
		int _count;
		struct Node
		{
		public:
			T Data;
			Node* Next;
			Node* Prev;
			template<class U>
			Node(U&& data, Node* next, Node* prev) : Data(forward<U>(data)), Next(next), Prev(prev) {}
			template<class U>
			Node(U&& data, Node* next) : Node(data, next, nullptr) {}
			template<class U>
			Node(U&& data) : Node(data, nullptr, nullptr) {}
		private:
			Node(const Node&) = delete;
			Node& operator=(const Node&) = delete;
		};
		Node* _head;
		Node* _tail;
		Node* _end;
		Node* GetNodeByIndex(int index) const
		{
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
		List() : _head(nullptr), _tail(nullptr), _count(0) {
			_end = new Node(T());
		}
		List(initializer_list<T> il) : List()
		{
			for (const auto& i : il)
				PushBack(i);
		}
		List(const List& other) : List()
		{
			for (Iterator iter = other.Begin(); iter != other.End(); ++iter)
				PushBack(*iter);
		}
		~List() {
			Clear();
			delete _end;
		}
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
				if (this != &other)
					_current = other._current;
				return *this;
			}
			~Iterator() {}

			T& operator *() const {
				return _current->Data;
			}

			Iterator& operator+=(const int n) {
				for (int i = 0; i < n && _current != nullptr; i++)
					_current = _current->Next;
				return*this;
			}
			Iterator& operator-=(const int n) {
				for (int i = 0; i < n && _current != nullptr; i++)
					_current = _current->Prev;
				return*this;
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
				Iterator* iterator = new Iterator(other);
				for (int i = 0; i < n && iterator->_current != nullptr; i++)
					++(*iterator);
				return *iterator;
			}
			friend Iterator& operator- (const Iterator& other, const int n)
			{
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
		template <class U>
		void PushBack(U&& value)
		{
			if (!_head) {
				_head = new Node(forward<U>(value), _end, nullptr);
				_tail = _head;
				_end->Prev = _tail;
			}
			else
			{
				_tail = new Node(forward<U>(value), _end, _tail);
				_end->Prev = _tail;
				_tail->Prev->Next = _tail;
			}
			_count++;
		}
		template <class U>
		void PushFront(U&& value)
		{
			if (!_head) {
				_head = new Node(forward<U>(value), _end, nullptr);
				_tail = _head;
				_end->Prev = _tail;
			}
			else
			{
				_head = new Node(forward<U>(value), _head, nullptr);
				_head->Next->Prev = _head;
			}
			_count++;
		}
		template <class U>
		void Insert(U&& value, int index)
		{
			if (index < 0 || index > _count)
				throw out_of_range("Индекс за пределами коллекции!");

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
		template <class U>
		int Find(U&& target) const
		{
			Node* pointer = _head;
			for (int i = 0; i < _count; i++, pointer = pointer->Next)
				if (pointer->Data == target)
					return i;

			return -1;
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
			_tail->Next = _end;
			_end->Prev = _tail;

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
			delete _head->Prev;
			_head->Prev = nullptr;

			_count--;
			return true;
		}
		template <class U>
		bool Remove(U&& target)
		{
			if (Empty())
				return false;
			if (_head->Data == target)
				return PopFront();
			if (_count == 1)
				return false;
			if (_tail->Data == target)
				return PopBack();

			int i = 0;
			Node* node = _head->Next;
			for (; i < _count - 1 && node->Data != target; i++)
				node = node->Next;

			if (i == _count - 1)
				return false;

			node->Prev->Next = node->Next;
			node->Next->Prev = node->Prev;
			delete node;

			_count--;
			return true;
		}
		bool RemoveAt(int index)
		{
			if (index < 0 || index > _count - 1)
				return false;
			if (index == 0)
				return PopFront();
			if (index == _count - 1)
				return PopBack();

			Node* node = GetNodeByIndex(index);
			node->Prev->Next = node->Next;
			node->Next->Prev = node->Prev;
			delete node;

			_count--;
			return true;
		}

		template<class U>
		void Assign(int newCount, U&& value)
		{
			if (newCount < 0)
				throw invalid_argument("Count > 0!");

			if (newCount == 0)
				return Clear();

			Node* changingNode = _head;
			for (int i = 0; i < newCount && changingNode != _end; i++)
			{
				changingNode->Data = value;
				changingNode = changingNode->Next;
			}

			while (_count < newCount)
				PushBack(value);

			while (_count > newCount)
				PopBack();
		}
		template< class U>
		void Resize(int newCount, U&& value)
		{
			if (newCount < 0)
				throw invalid_argument("Count > 0!");

			if (newCount == 0)
				return Clear();

			while (_count > newCount)
				PopBack();

			while (_count < newCount)
				PushBack(value);
		}
		void Resize(int newCount) {
			Resize(newCount, T());
		}
		T& operator[](int index) const {
			return GetNodeByIndex(index)->Data;
		}
		T& At(int index) const
		{
			if (index < 0 || index > _count - 1)
				throw out_of_range("Индекс за пределами списка!");
			return (*this)[index];
		}
		T& Front() const {
			return _head->Data;
		}
		T& Back() const {
			return _tail->Data;
		}
		Iterator Begin() const {
			Iterator iter = _head;
			return iter;
		}
		Iterator End() const {
			Iterator iter = _end;
			return iter;
		}

		void Clear()
		{
			Node* iterator = _end->Prev;
			Node* deleted;
			while (iterator != nullptr)
			{
				deleted = iterator;
				iterator = iterator->Prev;
				delete deleted;
			}
			_head = nullptr;
			_tail = nullptr;
			_end->Prev = nullptr;
			_count = 0;
		}
		bool Empty() const {
			return !_head;
		}
		int Count() const {
			return _count;
		}
	};
}