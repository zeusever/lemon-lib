/**
* 
* @file     allocator
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/26
*/
#ifndef LEMON_IO_ALLOCATOR_HPP
#define LEMON_IO_ALLOCATOR_HPP

#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/mpl/list.hpp>
#include <lemonxx/mpl/list/end.hpp>
#include <lemonxx/mpl/list/begin.hpp>
#include <lemonxx/mpl/list/iterator.hpp>
#include <lemonxx/mpl/generate.hpp>
#include <lemonxx/memory/fixobj.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	//////////////////////////////////////////////////////////////////////////
	//allocator generate class

	template<typename B,typename E> struct basic_allocator_helper;

	template<typename B,typename E> 
	struct basic_allocator_helper : public basic_allocator_helper<typename mpl::next<B>::type,E>
	{
	public:

		typedef typename mpl::deref<B>::type	current_type;

		typedef memory::fixed::allocator<sizeof(current_type)>	allocator_type;

		void* alloc()
		{
			mutex_t::scope_lock lock(_mutex);

			return _allocator.alloc();
		}

		void free(void * block)
		{
			mutex_t::scope_lock lock(_mutex);

			_allocator.free(block);
		}

	private:
		
		allocator_type			_allocator;

		mutex_t					_mutex;
	};


	template<typename I> struct basic_allocator_helper<I,I>
	{

	};

	template<typename Seq,typename T> struct basic_allocator_cast
	{
		typedef typename mpl::begin<Seq>::type	B;

		typedef typename mpl::end<Seq>::type	E;

		typedef typename mpl::find<B,E,T>::result_type I;

		typedef basic_allocator_helper<I,E>	type;
	};

	template<typename Seq> struct basic_allocator 
		: public basic_allocator_helper<typename mpl::begin<Seq>::type, typename mpl::end<Seq>::type>
	{
		template<typename T> void * alloc()
		{
			return ((typename basic_allocator_cast<Seq,T>::type &)*this).alloc();
		}

		template<typename T> void free(void * block)
		{
			((typename basic_allocator_cast<Seq,T>::type &)*this).free(block);
		}
	};

}}}

#endif //LEMON_IO_ALLOCATOR_HPP