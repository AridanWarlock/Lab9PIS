#pragma once
#include <iostream>
#include <exception>

using namespace std;

namespace ForwardList
{
	template <class T>
	class ForwardList
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

		Node* GetNodeBeforeIndex(int index) const
		{
			if (index < 1 || index > _count)
				return nullptr;

			Node* node = _head;
			for (int i = 0; i < index - 1; i++)
				node = node->Next;
			return node;
		}
		template <class U>
		void FirstAdd(U&& value)
		{
			_head = new Node(forward<U>(value));
			_tail = _head;
			_count++;
		}
	public:
		class Iterator
		{
		private:
			Node* _current;
		public:
			Iterator() : _current(nullptr) {}
			Iterator(Node* current) : _current(current) {}
			T& operator *() {
				return _current->Data;
			}
			Iterator& operator=(const Iterator& other) {
				if (this != &other)
					_current = other._current;
				return *this;
			}
			Iterator& operator=(const T& data) {
				_current->Data = data;
				return *this;
			}
			Iterator& operator+=(int n) {
				for (int i = 0; i < n && _current != nullptr; i++)
					_current = _current->Next;
				return*this;
			}
			Iterator& operator++()
			{
				_current = _current->Next;
				return *this;
			}
			bool operator ==(const Iterator& other) const {
				return _current == other._current;
			}
			bool operator !=(const Iterator& other) const {
				return _current != other._current;
			}
		};
		ForwardList() : _head(nullptr), _tail(nullptr), _count(0) {}
		ForwardList(initializer_list<T> il) : ForwardList()
		{
			for (const auto& i : il)
				PushBack(i);
		}
		ForwardList(const ForwardList& other)
		{
			_head = nullptr;
			_tail = _head;
			_count = 0;

			for (Iterator iter = other.Begin(); iter != other.End(); ++iter)
				PushBack(*iter);
		}
		~ForwardList() {
			Clear();
		}
		template<class U>
		void PushBack(U&& value)
		{
			if (!_head)
				return FirstAdd(forward<U>(value));

			_tail->Next = new Node(forward<U>(value));
			_tail = _tail->Next;

			_count++;
		}
		template<class U>
		void PushFront(U&& value)
		{
			if (!_head)
				return FirstAdd(forward<U>(value));

			_head = new Node(forward<U>(value), _head);

			_count++;
		}
		template<class U>
		void Assign(int newCount, U&& value)
		{
			if (newCount == 0)
				return Clear();

			Node* changingNode = _head;
			for (int i = 0; i < newCount; i++)
			{
				changingNode->Data = value;
				changingNode = changingNode->Next;
			}

			while (changingNode != nullptr)
			{
				Node* deleted = changingNode;
				changingNode = changingNode->Next;
				delete deleted;
			}

			_count = newCount;
		}
		template<class U>
		void Insert(U&& value, int index)
		{
			if (index < 0 || index > _count)
				throw out_of_range("Индекс за пределами коллекции!");

			if (index == 0)
				return PushFront(forward<U>(value));
			if (index == _count)
				return PushBack(forward<U>(value));

			Node* nodeByIndex = _head;
			for (int i = 0; i < index - 1; i++)
				nodeByIndex = nodeByIndex->Next;
			Node* temp = nodeByIndex->Next;

			nodeByIndex->Next = new Node(forward<U>(value), temp);

			_count++;
		}
		template<class U>
		int Find(U&& target) const
		{
			Node* searchNode = _head;
			for (int i = 0; i < _count; i++)
				if (searchNode->Data == target)
					return i;
				else
					searchNode = searchNode->Next;
			return -1;
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
			Node* node = _head;
			_head = _head->Next;
			delete node;

			_count--;
			return true;
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

			Node* node = _head;
			for (int i = 0; i < _count - 2; i++)
				node = node->Next;

			delete node->Next;
			node->Next = nullptr;
			_tail = node;

			_count--;
			return true;
		}
		template<class U>
		bool Remove(U&& target)
		{
			if (Empty())
				return false;

			if (_head->Data == target)
				return PopFront();

			Node* searchNode = _head;
			int i = 1;
			while (i < _count && searchNode->Next->Data != target)
			{
				searchNode = searchNode->Next;
				i++;
			}

			if (i == _count)
				return false;

			Node* temp = searchNode->Next;
			if (temp != _tail)
				searchNode->Next = temp->Next;
			else
				_tail = searchNode;
			delete temp;

			_count--;
			return true;
		}
		bool RemoveAt(int index)
		{
			if (index < 0 || index > _count)
				return false;

			if (index == _count - 1)
				return PopBack();

			Node* nodeByIndex = _head;
			for (int i = 0; i < index - 1; i++)
				nodeByIndex = nodeByIndex->Next;

			Node* temp = nodeByIndex->Next;
			nodeByIndex->Next = nodeByIndex->Next->Next;
			delete temp;

			_count--;
			return true;
		}
		template<class U>
		void Resize(int newCount, U&& value)
		{
			if (newCount == 0)
				return Clear();

			if (_count > newCount)
			{
				Node* node = GetNodeBeforeIndex(newCount);
				_tail = node;
				Node* temp = node->Next;
				while (temp != nullptr)
				{
					node = temp;
					temp = temp->Next;
					delete node;
				}
			}
			else if (_count < newCount)
				for (int i = _count; i < newCount; i++)
				{
					_tail->Next = new Node(forward<U>(value));
					_tail = _tail->Next;
				}

			_count = newCount;
		}
		void Resize(int newCount) {
			Resize(newCount, T());
		}

		Iterator& Begin() const {
			Iterator* iter = new Iterator(_head);
			return *iter;
		}
		Iterator& End() const {
			Iterator* iter = new Iterator(nullptr);
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
		T& operator[](int index) const
		{
			if (index == _count - 1)
				return _tail->Data;
			Node* nodeByIndex = _head;
			for (int i = 0; i < index; i++)
				nodeByIndex = nodeByIndex->Next;
			return nodeByIndex->Data;
		}
		T& At(int index) const
		{
			if (index < 0 || index > _count - 1)
				throw out_of_range("Индекс за пределами списка!");
			return (*this)[index];
		}

		T& Front() const {
			if (Empty())
				throw out_of_range("Коллекция пуста!");
			return _head->Data;
		}
		T& Back() const {
			if (Empty())
				throw out_of_range("Коллекция пуста!");
			return _tail->Data;
		}

		bool Empty() const {
			return !_head;
		}
		int Count() const {
			return _count;
		}
	};
}
