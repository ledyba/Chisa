/**
 * Donut
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

#include <tarte/String.h>
#include <donut/Exception.h>
#include <donut/object/Heap.h>
#include <donut/provider/BoolProvider.h>

namespace donut {
using namespace tarte;

static const std::string TAG("BoolProvider");

BoolProvider::BoolProvider(Handler<Heap> const& heap)
:Provider(heap, "Boolean")
{
	this->registerPureNativeClosure("opAnd", [&](bool self, bool v){
		return self && v;
	});
	this->registerPureNativeClosure("opOr", [&](bool self, bool v){
		return self || v;
	});
	this->registerPureNativeClosure("opNot", [&](bool self){
		return !self;
	});
	this->registerPureNativeClosure("opEq", [&](bool self, Object* obj){
		return obj->isBool() && obj->toBool(this->heap().lock()) == self;
	});
	this->registerPureNativeClosure("opNe", [&](bool self, Object* obj){
		return obj->isBool() && obj->toBool(this->heap().lock()) != self;
	});
	this->registerPureNativeClosure("toString", [&](bool self){
		return ::tarte::toString(self);
	});
}

std::string BoolProvider::repr(const Object* ptr) const
{
	return BoolProvider::fromPointer(ptr) ? "bool: true" : "bool: false";
}

std::string BoolProvider::print(const Object* ptr) const
{
	return BoolProvider::fromPointer(ptr) ? "true" : "false";
}

bool BoolProvider::toBool(const Object* ptr) const
{
	return BoolProvider::fromPointer(ptr);
}

Handler<Object> BoolProvider::create( bool const& val )
{
	return Handler<Object>::__internal__fromRawPointerWithoutCheck( BoolProvider::toPointer(val) );
}

}
