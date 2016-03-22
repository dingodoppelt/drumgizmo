/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            notifier.h
 *
 *  Thu Sep  3 15:48:39 CEST 2015
 *  Copyright 2015 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#pragma once

#include <functional>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace aux
{
	template<int>
	struct placeholder
	{
	};
}

namespace std
{
	template<int N>
	struct is_placeholder<aux::placeholder<N>>
		: integral_constant<int, N+1>
	{
	};
}

namespace aux
{
	// std::integer_sequence introduced in C++14 so remove this once we start requiring that.

	template<int... Ns>
	struct int_sequence
	{
	};

	template<int N, int... Ns>
	struct gen_int_sequence
		: gen_int_sequence<N-1, N-1, Ns...>
	{
	};

	template<int... Ns>
	struct gen_int_sequence<0, Ns...>
		: int_sequence<Ns...>
	{
	};
};


namespace GUI {

class Listener;
class NotifierBase {
public:
	virtual void disconnect(Listener* object) {}
};

class Listener {
public:
	virtual ~Listener()
	{
		for(auto signal : signals)
		{
			signal->disconnect(this);
		}
	}

	void registerNotifier(NotifierBase* signal)
	{
		signals.insert(signal);
	}

	void unregisterNotifier(NotifierBase* signal)
	{
		signals.erase(signal);
	}

private:
	std::set<NotifierBase*> signals;
};

template<typename... Args>
class Notifier : public NotifierBase {
public:
	Notifier() {}

	//! \brief When dtor is called it will automatically disconnect all its listeners.
	~Notifier()
	{
		for(auto& slot : slots)
		{
			slot.first->unregisterNotifier(this);
		}
	}

	using callback_type = std::function<void(Args...)>;

	//! \brief Connect object to this Notifier.
	template<typename O, typename F>
	void connect(O* p, const F& fn)
	{
		slots[p] = std::move(construct_mem_fn(fn, p, aux::gen_int_sequence<sizeof...(Args)>{}));
		if(p && dynamic_cast<Listener*>(p))
		{
			dynamic_cast<Listener*>(p)->registerNotifier(this);
		}
	}

	//! \brief Disconnect object from this Notifier.
	void disconnect(Listener* object)
	{
		slots.erase(object);
	}

	//! \brief Activate this notifier by pretending it is a function.
	//! Example: Notifier<int> foo; foo(42);
	void operator()(Args... args)
	{
		for(auto& slot : slots)
		{
			slot.second(args...);
		}
	}

private:
	std::map<Listener*, callback_type> slots;

	template<typename F, typename O, int... Ns>
	callback_type construct_mem_fn(const F& fn, O* p, aux::int_sequence<Ns...>) const
	{
		return std::bind(fn, p, aux::placeholder<Ns>{}...);
	}

};

} // GUI::

#define CONNECT(SRC, SIG, TAR, SLO) (SRC)->SIG.connect(TAR, SLO)
