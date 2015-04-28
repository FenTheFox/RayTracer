#pragma once

template <typename T>
class RandomQueue : public std::deque<T>
{
public:
	T pop ();
};