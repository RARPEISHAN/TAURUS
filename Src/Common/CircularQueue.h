#pragma once
#ifndef _CIRCULARQUEUE_H
#define _CIRCULARQUEUE_H

#include <limits.h>
#include <vector>
#include <concepts>
#include <initializer_list>
#include <algorithm>
#include <optional>
#include <shared_mutex>
#include <iostream>
#include <condition_variable>
#include <atomic>
#include <chrono>
namespace base {
	using namespace std;
	using size_t = unsigned long long;
	enum class bufferError
	{
		overflow = 0,
		popbigger,
		timeout
	};
	using namespace std::chrono_literals;
	template<bufferError Error>
	struct BufferException {};
	template<std::copyable _Ty>
	class CircularQueue
	{
		vector<_Ty>              _cq;
		size_t                   front = 0;
		size_t                   rear = 0;
		long long                _size = 0;
		std::shared_mutex        _rdMutex;
		long long                max_size = LLONG_MAX;
		std::atomic_bool         isblocked = false;
		std::mutex               _sMutex;
		std::condition_variable  _cv;
		int                      _wait_time = 100;
	public:
		CircularQueue(size_t size = 2) :_cq((::size_t)size) {}
		CircularQueue(CircularQueue& other) = delete;
		CircularQueue(CircularQueue&& other) = delete;
		std::optional<_Ty> pop_back();
		std::optional<std::vector<_Ty>> pop_back(size_t s);
		bool empty()const;
		size_t capacity()const {
			return _cq.size();
		}
		size_t size()const
		{
			return _size;
		}
		void setMaxSize(size_t s)
		{
			max_size = s;
		}
	public:
		bool resize(size_t size);
		void setBlock(void) {
			isblocked = true;
		}
		void setWaitTimer(int time_ms) {
			_wait_time = time_ms;
		}
	public:
		template<typename Ty = _Ty>
		void push_back(Ty& value)
		{
			if (!isblocked)
			{
				std::unique_lock lck{ _sMutex };
				if (_cv.wait_until(lck, _wait_time * 1ms) == std::cv_status::timeout)
					throw base::BufferException<base::bufferError::overflow>{};
			}

			if ((rear + 1) % _cq.size() == front)
				if (!resize((_cq.size() * 2ull) > max_size ? max_size - 1 : (_cq.size() * 2ull)))
				{
					throw base::BufferException<base::bufferError::overflow>{};
				}

			std::shared_lock u{ _rdMutex };
			_cq[rear] = value;
			rear = (rear + 1) % _cq.size();
			++_size;
		}
		template<typename _iterator>
		void push_back(_iterator first, _iterator last)
		{
			for (auto it = first; it != last; ++it)
				push_back(*it);
		}
	};
}

template<std::copyable _Ty>
inline std::optional<_Ty> base::CircularQueue<_Ty>::pop_back()
{
	if (empty())
		return std::nullopt;
	std::shared_lock u{ _rdMutex };
	_Ty t = _cq[front];
	front = (front + 1) % _cq.size();
	--_size;
	return t;
}

template<std::copyable _Ty>
inline std::optional<std::vector<_Ty>> base::CircularQueue<_Ty>::pop_back(size_t s)
{
	assert(s > 0);
	if (s >= max_size)
		throw base::BufferException<base::bufferError::popbigger>{};
	if (empty())
	{
		return std::nullopt;
	}
	if (s > _cq.size() - front)
		if (!resize(_size))
			return std::nullopt;
		else if (_cq.size() - _size >= 50000)
			if (!resize(_size + 1000 > max_size ? max_size - 1 : _size + 1000))
				return std::nullopt;
	std::shared_lock u{ _rdMutex };
	s = s > _size ? _size : s;
	std::vector<_Ty> temp(s);
	memcpy(temp.data(), _cq.data() + front, s * sizeof(_Ty));
	_size -= s;
	front = (front + s) % _cq.size();
	return { temp };
}

template<std::copyable _Ty>
inline bool base::CircularQueue<_Ty>::empty()const
{
	if (front == rear)
		return true;
	return false;
}

template<std::copyable _Ty>
inline bool base::CircularQueue<_Ty>::resize(size_t size)
{
	if (size >= max_size)
		return false;
	std::unique_lock u{ _rdMutex };
	if (_size + 1 >= max_size)
		return false;
	vector<_Ty> temp(size + 1);
	auto i = front;
	if (front > rear)
	{
		int length = _cq.size() - i;
		generate(temp.begin(), temp.begin() + length, [&i, this]()->auto { return _cq[i++]; });
		i = 0;
		generate(temp.begin() + length, temp.begin() + _size, [&i, this]()->auto { return _cq[i++]; });
	}
	else
	{
		generate(temp.begin(), temp.begin() + _size, [&i, this]()->auto {
			return _cq[i++]; });
	}
	front = 0;
	rear = front + _size;
	_cq = std::move(temp);
	return true;
}

#endif // _CIRCULARQUEUE_H