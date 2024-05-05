#pragma once
#include "List.h"

namespace Deque
{
	template <class T>
	class Deque
	{
	private:
		const int _chunkMaxSize;
		int _count;
		struct Chunk
		{
		private:
			T* _data;
			const int _maxSize;
			int _count;
			int _head = _maxSize - 1;
			int _tail = 0;
		public:
			Chunk(int maxSize) : _data(new T[maxSize]), _count(0), _maxSize(maxSize) {}
			~Chunk() {
				delete[] _data;
				cout << "Deleted" << endl;
			}
			template <class U>
			bool PushFront(U&& value)
			{
				if (_head == -1)
					return false;
				if (Empty())
					_tail = _head + 1;

				_data[_head] = forward<U>(value);
				_head--;
				_count++;

				return true;
			}
			template <class U>
			bool PushBack(U&& value)
			{
				if (_tail == _maxSize)
					return false;
				if (Empty())
					_head = _tail - 1;

				_data[_tail] = forward<U>(value);
				_tail++;
				_count++;

				return true;
			}
			bool PopFront()
			{
				if (Empty())
					return false;

				_head++;
				_count--;

				return true;
			}
			bool PopBack()
			{
				if (Empty())
					return false;

				_tail--;
				_count--;

				return true;
			}

			T& Front() const {
				return _data[_head + 1];
			}
			T& Back() const {
				return _data[_tail - 1];
			}
			int FrontIndex() const {
				return _head + 1;
			}
			int BackIndex() const {
				return _tail - 1;
			}
			T& At(int index) const {
				return _data[index];
			}
			int Count() const {
				return _count;
			}
			bool Empty() const {
				return _count == 0;
			}
			bool Full() const {
				return _count == _maxSize;
			}
		};
		List::List<Chunk*> _chunks;
	public:
		Deque(int chunkMaxSize) : _count(0), _chunkMaxSize(chunkMaxSize) {
			if (chunkMaxSize < 1)
				throw invalid_argument("Размер Чанков - положительное число!");
		}
		Deque() : Deque(50) {}
		Deque(initializer_list<T> il) : Deque() {
			for (const auto& i : il)
				PushBack(i);
		}
		~Deque() {
			auto iter = _chunks.Begin();
			for (; iter != _chunks.End(); ++iter)
				delete* iter;
			delete* iter;
		}

		template<class U>
		void Assign(int newCount, U&& value)
		{
			Clear();
			for (int i = 0; i < newCount; ++i)
				PushBack(forward<U>(value));
		}

		template<class U>
		void PushBack(U&& value)
		{
			if (Empty() || !_chunks.Back()->PushBack(forward<U>(value)))
			{
				Chunk* newBackChunk = new Chunk(_chunkMaxSize);
				newBackChunk->PushBack(forward<U>(value));
				_chunks.PushBack(newBackChunk);
			}

			_count++;
		}
		template<class U>
		void PushFront(U&& value)
		{
			if (Empty() || !_chunks.Front()->PushFront(forward<U>(value)))
			{
				Chunk* newFrontChunk = new Chunk(_chunkMaxSize);
				newFrontChunk->PushFront(forward<U>(value));
				_chunks.PushFront(newFrontChunk);
			}

			_count++;
		}

		template<class U>
		void Insert(U&& value, int index)
		{
			if (index < 0 || index > _count)
				throw out_of_range("Индекс за пределами коллекции!");
			if (index == _count)
				return PushBack(forward<U>(value));
			if (index == 0)
				return PushFront(forward<U>(value));

			if (index < _count / 2)
			{
				PushFront(Front());
				for (int i = 1; i < index; ++i)
					(*this)[i] = (*this)[i + 1];
			}
			else
			{
				PushBack(Back());
				for (int i = _count - 2; i > index; --i)
					(*this)[i] = (*this)[i - 1];
			}
			(*this)[index] = forward<U>(value);
		}

		bool PopBack()
		{
			if (Empty())
				return false;

			if (_chunks.Back()->Count() == 1) {
				delete _chunks.Back();
				_chunks.PopBack();
			}
			else
				_chunks.Back()->PopBack();

			_count--;
			return true;
		}
		bool PopFront()
		{
			if (Empty())
				return false;

			if (_chunks.Front()->Count() == 1) {
				delete _chunks.Front();
				_chunks.PopFront();
			}
			else
				_chunks.Front()->PopFront();

			_count--;
			return true;
		}

		template<class U>
		bool Remove(U&& target)
		{
			return RemoveAt(Find(forward<U>(target)));
		}
		bool RemoveAt(int index)
		{
			if (index == 0)
				return PopFront();
			if (index == _count - 1)
				return PopBack();
			if (index < 0 || index > _count - 1)
				return false;

			if (index > _count / 2)
			{
				for (int i = index; i < _count - 1; i++)
					(*this)[i] = (*this)[i + 1];
				return PopBack();
			}
			else
			{
				for (int i = index; i > 0; --i)
					(*this)[i] = (*this)[i - 1];
				return PopFront();
			}
		}

		template<class U>
		int Find(U&& target)
		{
			int i = 0;
			for (Iterator iter = Begin(); iter != End(); ++iter, ++i)
				if (*iter == target)
					return i;
			return -1;
		}

		template<class U>
		void Resize(int newCount, U&& newValue)
		{
			while (newCount < _count)
				PopBack();
			while (newCount > _count)
				PushBack(newValue);
		}
		void Resize(int newCount) {
			return Resize(newCount, T());
		}

		T& Front() const {
			return _chunks.Front()->Front();
		}
		T& Back() const {
			return _chunks.Back()->Back();
		}
		T& operator[](int index) const
		{
			int listIndex = (index + _chunkMaxSize - _chunks.Front()->Count()) / _chunkMaxSize;
			if (_chunks.Front() != _chunks.Back())
				index -= (_chunks.Front() == _chunks[listIndex])
				? _chunks[listIndex]->Count() - _chunkMaxSize
				: _chunks.Front()->Count();
			return _chunks[listIndex]->At(index % _chunkMaxSize);
		}
		T& At(int index) const
		{
			if (index < 0 || index > _count)
				throw out_of_range("Индекс вне коллекции!");
			return (*this)[index];
		}

		class Iterator
		{
		private:
			List::List<Chunk*>* _list;
			int _currentChunk;
			int _currentElem;
			int _chunkMaxSize;
		public:
			Iterator(List::List<Chunk*>* list, int currentChunk, int currentElem, int chunkMaxSize) : _list(list), _currentChunk(currentChunk),
				_currentElem(currentElem), _chunkMaxSize(chunkMaxSize)
			{
				if (_currentElem == _chunkMaxSize)
					++(*this);
				else if (_currentElem == -1)
					--(*this);
			}
			Iterator() : Iterator(nullptr, 0, 0, 0) {}
			Iterator(const Iterator& other)
			{
				*this = other;
			}
			Iterator& operator=(const Iterator& other)
			{
				if (this != &other)
				{
					_list = other._list;
					_currentChunk = other._currentChunk;
					_currentElem = other._currentElem;
					_chunkMaxSize = other._chunkMaxSize;
				}
				return *this;
			}
			~Iterator() {}

			T& operator*() const {
				return (*_list)[_currentChunk]->At(_currentElem);
			}
			Iterator& operator++()
			{
				if (_currentElem < _chunkMaxSize - 1)
					_currentElem++;
				else
				{
					_currentElem = 0;
					_currentChunk++;
				}
				return *this;
			}
			Iterator& operator--()
			{
				if (_currentElem > 0)
					_currentElem--;
				else
				{
					_currentChunk--;
					_currentElem = _chunkMaxSize - 1;
				}
				return *this;
			}

			friend Iterator& operator+(Iterator& iter, int n)
			{
				if (n < 0)
					return iter - (-n);

				Iterator* newIter = new Iterator(iter);
				iter._currentElem += n;
				while (iter._currentElem > iter._chunkMaxSize - 1) {
					iter._currentElem -= iter._chunkMaxSize;
					iter._currentChunk++;
				}
				return *newIter;
			}
			friend Iterator& operator-(Iterator& iter, int n)
			{
				if (n < 0)
					return iter + (-n);

				Iterator* newIter = new Iterator(iter);
				iter._currentElem -= n;
				while (iter._currentElem < 0) {
					iter._currentElem += iter._chunkMaxSize;
					iter._currentChunk--;
				}
				return *newIter;
			}

			Iterator& operator+=(int n)
			{
				if (n < 0)
					return (*this) -= -n;

				for (int i = 0; i < n; ++i)
					(*this)++;
				return *this;
			}
			Iterator& operator-=(int n)
			{
				if (n < 0)
					return (*this) += -n;

				for (int i = 0; i < n; ++i)
					(*this)--;
				return *this;
			}

			friend int operator - (const Iterator& lhs, const Iterator& rhs)
			{
				if (lhs < rhs)
				{
					int i = 0;
					for (Iterator iter(lhs); iter != rhs; ++iter)
						i++;
					return i;
				}
				if (lhs > rhs)
				{
					int i = 0;
					for (Iterator iter(rhs); iter != lhs; ++iter)
						i++;
					return i;
				}
				return 0;
			}

			friend bool operator<(const Iterator& lhs, const Iterator& rhs)
			{
				if (lhs == rhs)
					return false;

				Iterator endIter(_list, _list.Count() - 1, _list.Back()->BackIndex(), _chunkMaxSize);
				++endIter;

				for (Iterator i(lhs + 1); i != endIter; ++i)
					if (i == rhs)
						return true;
				return false;
			}
			friend bool operator>(const Iterator& lhs, const Iterator& rhs)
			{
				if (lhs == rhs)
					return false;

				Iterator beginIter = Iterator(&_chunks, 0, _chunks.Front()->FrontIndex(), _chunkMaxSize);

				for (Iterator i(rhs - 1); i != beginIter; --i)
					if (i == lhs)
						return true;
				return false;
			}
			friend bool operator>=(const Iterator& lhs, const Iterator& rhs) {
				return !(lhs < rhs);
			}
			friend bool operator<=(const Iterator& lhs, const Iterator& rhs) {
				return !(lhs > rhs);
			}

			friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
				return lhs._currentChunk == rhs._currentChunk && lhs._currentElem == rhs._currentElem && lhs._list == rhs._list;
			}
			friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
				return !(lhs == rhs);
			}
		};
		Iterator& Begin()
		{
			Iterator* iter = new Iterator(&_chunks, 0, _chunks.Front()->FrontIndex(), _chunkMaxSize);
			return *iter;
		}
		Iterator& End()
		{
			Iterator* iter = new Iterator(&_chunks, _chunks.Count() - 1, _chunks.Back()->BackIndex(), _chunkMaxSize);
			++(*iter);
			return *iter;
		}

		void Clear() {
			for (int i = 0; i < _chunks.Count(); i++)
				delete _chunks[i];
			_chunks.Clear();
			_count = 0;
		}

		int Count() const {
			return _count;
		}
		bool Empty() const {
			return _count == 0;
		}
	};
}

