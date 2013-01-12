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

#include "HaloServant.h"
#include "../Element.h"
#include "../World.h"
#include "../donut/Patron.h"

namespace chisa {
namespace tk {

HaloServant::HaloServant(Handler<AngelTarget> const& angelTarget, gl::Color const& color)
:Servant(angelTarget)
,color_(color)
{

}

Handler<HaloServant> HaloServant::self() noexcept
{
	return Handler<HaloServant>::__internal__fromRawPointerWithoutCheck(this);
}

Handler< ::donut::Object> HaloServant::createDonutObject(Handler< ::donut::Heap> const& heap)
{
	return Handler< ::donut::Object>( world()->patron()->haloServantProvider()->newInstance(heap, self()) );
}

void HaloServant::renderImpl(gl::Canvas& canvas)
{
	canvas.drawRect(2, this->color_, this->target()->findScreenArea());
}

void HaloServant::idleImpl(const float delta_ms)
{
}

geom::Box HaloServant::reshapeImpl(const geom::Area& area)
{
	return this->target()->findScreenArea().box();
}

}}

