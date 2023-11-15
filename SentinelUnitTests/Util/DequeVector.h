#pragma once

///	<summary>
/// A double ended, fixed-length queue using a vector.
///	</summary>
///	<remarks>
/// Unlike std::deque, this uses a vector. As a result adding and removing elements
/// is not as fast as std::dequeu, but memory is used much more efficiently, the
/// queue is fixed length and, if needed, individual items can be accessed much
/// quicker. (And the contents are easier to view in the debugger.)
///	</remarks>
using namespace std;
template <class T, size_t N>
class deque_vector : public std::vector<T>
{
private:
	// All of these methods have been disabled.
	size_t max_size() = delete;
	size_t resize()   = delete;
	size_t insert()   = delete;
	void   erase()    = delete;
	void   reserve()  = delete;
	void   clear()    = delete;

public:
	///	<summary>
	///	Constructor.
	///	</summary>
	deque_vector()
	{
		vector<T>::resize(N);
	}

	///	<summary>
	///	Constructor.
	///	</summary>
	///	<param name="val">Value to fill the container with.</param>
	explicit deque_vector(const T& val)
		: vector<T>(N, val)
	{
	}

	///	<summary>
	///	Copy constructor.
	///	</summary>
	deque_vector(const deque_vector& other) = default;

	///	<summary>
	///	Assignment operator.
	///	</summary>
	deque_vector& operator=(const deque_vector& other) = default;

	///	<summary>
	///	Assigns new contents, replace the current contents.
	///	</summary>
	///	<param name="val">Value to fill the container with.</param>
	void assign(const T& val)
	{
		vector<T>::assign(N, val);
	}

	///	<summary>
	/// Adds new element to the front, moving all items toward the back by one position.
	///	</summary>
	///	<param name="val">The elment to add.</param>
	/// <remarks>
	/// Because the vector is fixed length, this will delete the last element.
	/// </remarks>
	void push_front(const T& val)
	{
		vector<T>::pop_back();
		vector<T>::insert(vector<T>::begin(), val);
	}

	///	<summary>
	/// Adds new element to the end, moving all items toward the front by one position.
	///	</summary>
	///	<param name="val">The elment to add.</param>
	/// <remarks>
	/// Because the vector is fixed length, this will delete the last element.
	/// </remarks>
	void push_back(const T& val)
	{
		vector<T>::erase(vector<T>::begin());
		vector<T>::push_back(val);
	}

	///	<summary>
	/// Removes the first element, moving all items toward the front by one position.
	///	</summary>
	///	<returns>The first element.</returns>
	/// <remarks>
	/// Because the vector is fixed length, this will result in a default object at
	/// the back of the queue.
	/// </remarks>
	T pop_front()
	{
		T val = vector<T>::front();
		vector<T>::erase(vector<T>::begin());
		vector<T>::resize(N);
		return val;
	}

	///	<summary>
	/// Removes the first element, moving all items toward the front by one position
	/// and adding backVal to the back.
	///	</summary>
	///	<param name="val">The elment to add to the back.</param>
	///	<returns>The first element.</returns>
	T pop_front(const T& backVal)
	{
		T val = vector<T>::front();
		vector<T>::erase(vector<T>::begin());
		vector<T>::push_back(backVal);
		return val;
	}

	///	<summary>
	/// Removes the last element, moving all items toward the back by one position.
	///	</summary>
	///	<returns>The last element.</returns>
	/// <remarks>
	/// Because the vector is fixed length, this will result in a default object at
	/// the front of the queue.
	/// </remarks>
	T pop_back()
	{
		T val = vector<T>::back();
		vector<T>::pop_back();
		vector<T>::emplace(vector<T>::begin());
		return val;
	}

	///	<summary>
	/// Removes the last element, moving all items toward the back by one position
	/// and adding backVal to the front.
	///	</summary>
	///	<param name="val">The elment to add to the front.</param>
	///	<returns>The last element.</returns>
	T pop_back(const T& frontVal)
	{
		T val = vector<T>::back();
		vector<T>::pop_back();
		vector<T>::insert(vector<T>::begin(), frontVal);
		return val;
	}
};

