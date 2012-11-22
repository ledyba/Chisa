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
#include "source/Source.h"
#include "object/Heap.h"
#include "vm/Machine.h"
#include "../logging/Logger.h"
#include "../Handler.h"
#include "Clock.h"

namespace tinyxml2 {
class XMLElement;
class XMLDocument;
}

namespace chisa {
namespace donut {
class Provider;

class Donut : public HandlerBody<Donut> {
	inline bool onFree() noexcept { return false; };
	DEFINE_MEMBER_REF(public, logging::Logger, log);
private:
	Handler<Clock> clock_;
	Handler<Source> source_;
	Handler<Heap> heap_;
	std::map<std::string, Handler<Machine> > machines_;
public:
	Donut(logging::Logger& log);
	virtual ~Donut() noexcept = default;
public:
	void onSeek( timestamp_t const& time );
public:
	Handler<Machine> queryMachine( const std::string& name = "" );
	void registerGlobalObject( const std::string& name, const Handler<Object>& obj );
	void registerProvider( const Handler<Provider>& provider );
	bool existsGlobalObject( const std::string& name );
	Handler<Object> readGlobalObject( const std::string& name );
public:
	Handler<Heap> heap() const noexcept { return this->heap_; };
	Handler<Source> parse(const std::string& src, const std::string& filename="<DEFAULT>", const int& lineno=0);
public:
	void bootstrap();
	void restore( tinyxml2::XMLElement* from );
	tinyxml2::XMLElement* save( tinyxml2::XMLDocument* doc );
};

}}

