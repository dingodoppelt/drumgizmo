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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <set>
#include <type_traits>
#include <utility>

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
		for(auto signal = signals.begin(); signal != signals.end(); ++signal) {
			(*signal)->disconnect(this);
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

	std::set<NotifierBase*> signals;
};

template<typename T, typename... Args>
class Notifier : public NotifierBase {
public:
	Notifier() {}
	~Notifier()
	{
		for(auto slot = slots.begin(); slot != slots.end(); ++slot) {
			(*slot).first->unregisterNotifier(this);
		}
	}

	void connect(Listener* object, std::function<void(T, Args...)> slot)
	{
		slots[object] = slot;
		if(object) {
			object->registerNotifier(this);
		}
	}

	void disconnect(Listener* object)
	{
		slots.erase(object);
	}

	void notify(T t, Args...args)
	{
		for(auto slot = slots.begin(); slot != slots.end(); ++slot) {
			(*slot).second(t, args...);
		}
	}

	std::map<Listener*, std::function<void(T, Args...)>> slots;
};

} // GUI::

template<unsigned... Is> struct seq{};
template<unsigned I, unsigned... Is>
struct gen_seq : gen_seq<I-1, I-1, Is...>{};
template<unsigned... Is>
struct gen_seq<0, Is...> : seq<Is...>{};

template<unsigned I> struct placeholder{};

namespace std{
template<unsigned I>
struct is_placeholder< ::placeholder<I> > : integral_constant<int, I>{};
} // std::

namespace aux{
template<unsigned... Is, class F, class... Ts>
auto easy_bind(seq<Is...>, F&& f, Ts&&... vs)
	-> decltype(std::bind(std::forward<F>(f), std::forward<Ts>(vs)..., ::placeholder<1 + Is>()...))
{
    return std::bind(std::forward<F>(f), std::forward<Ts>(vs)..., ::placeholder<1 + Is>()...);
}
} // aux::

template<class R, class C, class... FArgs, class... Args>
auto mem_bind(R (C::*ptmf)(FArgs...), Args&&... vs)
	-> decltype(aux::easy_bind(gen_seq<(sizeof...(FArgs) + 1) - sizeof...(Args)>(), ptmf, std::forward<Args>(vs)...))
{
    // the +1s for 'this' argument
    static_assert(sizeof...(Args) <= sizeof...(FArgs) + 1, "too many arguments to mem_bind");
    return aux::easy_bind(gen_seq<(sizeof...(FArgs) + 1) - sizeof...(Args)>(), ptmf, std::forward<Args>(vs)...);
}

template<class T, class C, class... Args>
auto mem_bind(T C::*ptmd, Args&&... vs)
	-> decltype(aux::easy_bind(gen_seq<1 - sizeof...(Args)>(), ptmd, std::forward<Args>(vs)...))
{
    // just 'this' argument
    static_assert(sizeof...(Args) <= 1, "too many arguments to mem_bind");
    return aux::easy_bind(gen_seq<1 - sizeof...(Args)>(), ptmd, std::forward<Args>(vs)...);
}

//#define obj_connect_old(SRC, SIG, TAR, SLO) (SRC).SIG.connect(&(TAR), mem_bind(&decltype(TAR)::SLO, TAR))

#define fun_connect(SRC, SIG, SLO) \
	(SRC).SIG.connect(nullptr, SLO)

#define obj_connect(SRC, SIG, TAR, SLO) \
  (SRC)->SIG.connect(TAR, mem_bind(&SLO, std::ref(*TAR)))
