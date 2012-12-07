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

#include <tinyxml2.h>
#include "Provider.h"
#include "../Exception.h"
#include "../object/Heap.h"
#include "../native/Bind.h"

namespace chisa {
namespace donut {
static const std::string TAG("DonutObjectProvider");


DonutObjectProvider::DonutObjectProvider(const Handler<Heap>& heap)
:Provider(heap, "chisa::donut::DonutObject")
{
}



util::XValue DonutObjectProvider::saveObjectImpl(Handler<HeapObject> const& obj)
{
	if(Handler<Heap> heap = this->heap().lock()){
		//return util::XValue( obj->toFloat(heap) );
	}else{
		throw DonutException(__FILE__, __LINE__, "[BUG] heap was already dead!");
	}
}
Handler<HeapObject> DonutObjectProvider::loadObjectImpl(util::XValue const& data)
{
	if(Handler<Heap> heap = this->heap().lock()){
		return heap->createDonutObject(  );
	}else{
		throw DonutException(__FILE__, __LINE__, "[BUG] heap was already dead!");
	}
}


}}
