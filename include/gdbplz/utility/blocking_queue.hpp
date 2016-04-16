#ifndef WIERTLO_BLOCKING_QUEUE_HPP_64DBB25B87274FDFAF56711505F295DA
#define WIERTLO_BLOCKING_QUEUE_HPP_64DBB25B87274FDFAF56711505F295DA

#include <mutex>
#include <condition_variable>
#include <deque>
#include <utility>
#include <boost/optional.hpp>

namespace wiertlo
{
	// adapted from Dietmar Kühl's SO answer
	// http://stackoverflow.com/a/12805690/1012936
	// and Howard Hinnant's one
	// http://stackoverflow.com/a/29988626/1012936
	//
	// all the functions are thread-safe unless specified
	template<typename T, typename Container = std::deque<T>>
	class blocking_queue
	{
	private:
		typedef std::mutex MutexType;
		typedef std::unique_lock<MutexType> ReadLock;
		typedef std::unique_lock<MutexType> WriteLock;
		
		MutexType               d_mutex;
		std::condition_variable d_condition;
		Container               d_queue;
		
		blocking_queue(const blocking_queue& other, ReadLock other_lock) :
			d_queue(other.d_queue)
		{
			static_cast<void>(other_lock);
		}
		
		blocking_queue(blocking_queue&& other, WriteLock other_lock) :
			d_queue(std::move(other.d_queue))
		{
			static_cast<void>(other_lock);
		}
	public:
		// push a value at the end of the queue
		void push(T const& value)
		{
			{
				WriteLock lock(this->d_mutex);
				d_queue.push_front(value);
			}
			this->d_condition.notify_one();
		}
		
		// pop the value from the beginning of the queue
		// blocks until the queue is not empty
		T pop()
		{
			WriteLock lock(this->d_mutex);
			this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
			T rc(std::move(this->d_queue.back()));
			this->d_queue.pop_back();
			return rc;
		}
		
		// get the value from the beginning of the queue, without removing it from the queue
		// blocks until the queue is not empty
		T front() const
		{
			ReadLock lock(this->d_mutex);
			this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
			return this->d_queue.back();
		}
		
		// try to pop the value from the beginning of the queue
		// returns empty optional if the queue is empty, else it pops the value from the queue
		boost::optional<T> try_pop()
		{
			WriteLock lock(this->d_mutex);
			if(this->d_queue.empty())
				return boost::none;
			else
			{
				T rc(std::move(this->d_queue.back()));
				this->d_queue.pop_back();
				return rc;
			}
		}
		
		// try to get the value from the beginning of the queue, without removing it from the queue
		// returns empty optional if the queue is empty, else it pops the value from the queue
		boost::optional<T> try_front() const
		{
			ReadLock lock(this->d_mutex);
			if(this->d_queue.empty())
				return boost::none;
			else
			{
				return this->d_queue.back();
			}
		}
		
		// clear all the values stored in the queue
		void clear()
		{
			WriteLock lock(this->d_mutex);
			this->d_queue.clear();
		}
		
		// 
		void swap(blocking_queue& other)
		{
			if(this != &other)
	        {
				WriteLock lhs_lk(this->d_mutex, std::defer_lock);
				WriteLock rhs_lk(other.d_mutex, std::defer_lock);
				std::lock(lhs_lk, rhs_lk);
				using std::swap;
				swap(this->d_queue, other.d_queue);
			}
		}
		
		blocking_queue(const blocking_queue& other) :
			blocking_queue(other, ReadLock(other.d_mutex))
		{
			
		}
		
		blocking_queue(blocking_queue&& other) :
			blocking_queue(std::move(other), WriteLock(other.d_mutex))
		{
			
		}
		
		blocking_queue()
		{
			
		}
		
		blocking_queue& operator=(const blocking_queue& other)
		{
			if(this != &other)
	        {
				WriteLock lhs_lk(this->d_mutex, std::defer_lock);
				ReadLock rhs_lk(other.d_mutex, std::defer_lock);
				std::lock(lhs_lk, rhs_lk);
				using std::swap;
				swap(this->d_queue, other.d_queue);
			}
		}
		
		blocking_queue& operator=(blocking_queue&& other)
		{
			if(this != &other)
	        {
				WriteLock lhs_lk(this->d_mutex, std::defer_lock);
				WriteLock rhs_lk(other.d_mutex, std::defer_lock);
				std::lock(lhs_lk, rhs_lk);
				this->d_queue = std::move(other.d_queue);
			}
		}
	};
}

#endif