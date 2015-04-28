#include "stdafx.h"

#include "RandomQueue.h"

template <typename T>
T RandomQueue<T>::pop ()
{
	T tmp = first ();
	pop_front ();
	return tmp;
}