#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	template<typename T>
	class i_binding_coding
	{
	protected:

		std::function<T()> getter = nullptr;
		std::function<void(const T&)> setter = nullptr;

	public:

		i_binding_coding()
		{
			getter = nullptr;
			setter = nullptr;
		}

		virtual ~i_binding_coding() = default;

		virtual void set(const T& _Value) = 0;
		virtual T get() const = 0;
	};
};

