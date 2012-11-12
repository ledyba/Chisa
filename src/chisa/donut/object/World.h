/**
 * Chisa
 * Copyright (C) 2012 psi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "../../logging/Logger.h"
#include "../../util/ClassUtil.h"
#include "../../Handler.h"
#include "../code/Code.h"
#include "Object.h"
#include "proxy/Proxy.h"

namespace chisa {
namespace donut {

class World {
	DEFINE_MEMBER_REF(public, logging::Logger, log)
private:
	Handler<Code> code_;
private:
	unsigned int generation_;
private:
	BoolProxy boolProxy_;
	IntProxy intProxy_;
	NullProxy nullProxy_;
private:
	Handler<BaseObject> globalObject_;
	Handler<BaseObject> objectProto_;
	Handler<FloatObject> floatProto_;
	Handler<StringObject> stringProto_;
public:
	World(logging::Logger& log, Handler<Code> code);
	virtual ~World() noexcept = default;
public:
	unsigned int nextGeneration();
	BoolProxy& boolProxy() {return boolProxy_;};
	IntProxy& intProxy() {return intProxy_;};
	NullProxy& nullProxy() {return nullProxy_;};
	Handler<Code> code() { return this->code_; }
	Handler<BaseObject> global() { return this->globalObject_; }
	Handler<BaseObject> objectPrototype() { return this->objectProto_; };
public:
	template <typename T, typename... Args>
	Handler<T> create(Args... args)
	{
		Handler<T> obj(new T(this, args...));
		return obj;
	}
	Handler<Object> createInt(const int& val);
	Handler<Object> createBool(const bool& val);
	Handler<Object> createNull();
};

template <> Handler<StringObject> World::create<StringObject>(std::string const& val);
template <> Handler<FloatObject> World::create<FloatObject>(float const& val);

}}
