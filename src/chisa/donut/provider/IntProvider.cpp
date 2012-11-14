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

#include "PrimitiveProvider.h"
#include "../../util/StringUtil.h"
#include "../object/World.h"
#include "../Exception.h"
#include <tinyxml2.h>

namespace chisa {
namespace donut {

static const std::string TAG("IntProvider");

IntProvider::IntProvider(World* world)
:ObjectProvider(world, "__native_int__")
{
}

std::string IntProvider::toString(const Object* ptr) const
{
	return util::format("%d", fromPointer(ptr));
}

int IntProvider::toInt(const Object* ptr) const
{
	return fromPointer(ptr);
}

float IntProvider::toFloat(const Object* ptr) const
{
	return fromPointer(ptr);
}

bool IntProvider::toBool(const Object* ptr) const
{
	return fromPointer(ptr) != 0;
}

tinyxml2::XMLElement* IntProvider::serialize( tinyxml2::XMLDocument* doc, Handler<Object> obj )
{
	tinyxml2::XMLElement* elm = doc->NewElement("int");
	elm->SetAttribute("val", IntProvider::fromPointer(obj.get()));
	return elm;
}
Handler<Object> IntProvider::deserialize( tinyxml2::XMLElement* xml )
{
	if( std::string("int") != xml->Name() ){
		throw DonutException(__FILE__, __LINE__, "[BUG] Oops. wrong element name: %s != \"bool\"", xml->Name());
	}
	int val;
	if(xml->QueryIntAttribute("val", &val) != tinyxml2::XML_SUCCESS){
		throw DonutException(__FILE__, __LINE__, "[BUG] Oops. failed to read xml");
	}
	return Handler<Object>::__internal__fromRawPointerWithoutCheck( IntProvider::toPointer(val) );
}

}}
