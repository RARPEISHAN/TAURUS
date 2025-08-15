//--------------------------------------------------------------------
//	B_Object.hpp.
//	07/19/2017.				created.
//	01/25/2018.				last modified.
//--------------------------------------------------------------------
//	*	Class Template for Basic Memory Management.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _BOBJECT_HPP
#define _BOBJECT_HPP

//--------------------------------------------------------------------
//	headers...
//--------------------------------------------------------------------
#include <memory>

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
	//	basic memory management, from which all objects inherited.
	//	Utilities of B object - B-Object getter.
	//--------------------------------------------------------------------

	template <typename T>
	class B_Object
	{
	public:
		//--------------------------------------------------------------------
		//	common casting methods.
		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> as(void)
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> as(void) const
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

		//--------------------------------------------------------------------

	protected:
		//--------------------------------------------------------------------
		std::shared_ptr<T> this_obj(void)
		{
			if (!this)
				return nullptr;

			return this->_thisB_Obj.lock();
		}

		//--------------------------------------------------------------------
		//	for the constant this pointer.
		//--------------------------------------------------------------------
		std::shared_ptr<T> this_obj(void) const
		{
			if (!this)
				return nullptr;

			return this->_thisB_Obj.lock();
		}

		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> this_obj(void)
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

		//--------------------------------------------------------------------
		// for the constant this pointer.
		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> this_obj(void) const
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

	private:
		//--------------------------------------------------------------------
		// weak pointer of this derived object from B-Object.
		//--------------------------------------------------------------------
		std::weak_ptr<T> _thisB_Obj;

		//--------------------------------------------------------------------
		//	friend functions.
		//--------------------------------------------------------------------
		template <typename U, typename ...Args>
		friend std::shared_ptr<U> make_obj(Args&& ...args);

		//--------------------------------------------------------------------
	};

	//--------------------------------------------------------------------
	//	Non copyable version of B_Object.
	//--------------------------------------------------------------------
	template <typename T>
	class B_Object_NonCopyable
	{
	public:
		B_Object_NonCopyable(void) = default;
		B_Object_NonCopyable(B_Object_NonCopyable const&) = delete;
		B_Object_NonCopyable(B_Object_NonCopyable&&) = delete;

	public:
		//--------------------------------------------------------------------
		//	common casting methods.
		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> as(void)
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> as(void) const
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

		//--------------------------------------------------------------------

	protected:
		//--------------------------------------------------------------------
		std::shared_ptr<T> this_obj(void)
		{
			if (!this)
				return nullptr;

			return this->_thisB_Obj.lock();
		}

		//--------------------------------------------------------------------
		// for the constant this pointer.
		//--------------------------------------------------------------------
		std::shared_ptr<T> this_obj(void) const
		{
			if (!this)
				return nullptr;

			return this->_thisB_Obj.lock();
		}

		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> this_obj(void)
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

		//--------------------------------------------------------------------
		// for the constant this pointer.
		//--------------------------------------------------------------------
		template <typename U>
		std::shared_ptr<U> this_obj(void) const
		{
			if (!this)
				return nullptr;

			return std::static_pointer_cast<U>(_thisB_Obj.lock());
		}

	private:
		//--------------------------------------------------------------------
		// weak pointer of this derived object from B_Object_NonCopyable.
		//--------------------------------------------------------------------
		std::weak_ptr<T> _thisB_Obj;

		//--------------------------------------------------------------------
		//	friend functions.
		//--------------------------------------------------------------------
		template <typename U, typename ...Args>
		friend std::shared_ptr<U> make_obj(Args&& ...args);

		//--------------------------------------------------------------------
	};

	//--------------------------------------------------------------------
	//	utilities of B Object (or Non Copyable) - B-Object maker.
	//--------------------------------------------------------------------
	template <typename T, typename ...Args>
	inline
		std::shared_ptr<T> make_obj(Args&& ...args)
	{
		auto obj = std::shared_ptr<T>(new T{ std::forward<Args>(args) ... });

		if (obj)
			obj->_thisB_Obj = obj;

		return obj;
	}

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_BOBJECT_HPP