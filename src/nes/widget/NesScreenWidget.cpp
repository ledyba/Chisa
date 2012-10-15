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

#include "NesScreenWidget.h"
#include "../../chisa/geom/Vector.h"
#include "../../chisa/geom/Area.h"

namespace nes {
namespace widget {

NesScreenWidget::NesScreenWidget(chisa::logging::Logger& log, std::weak_ptr<chisa::tk::World> world, tinyxml2::XMLElement* element)
:Widget(log, world, element)
{
}

NesScreenWidget::~NesScreenWidget()
{
}

void NesScreenWidget::render(chisa::gl::Canvas& cv, const chisa::geom::Area& area)
{
}

void NesScreenWidget::idle(const float delta_ms)
{
}

void NesScreenWidget::reshape(const chisa::geom::Box& areaSize)
{

}

chisa::geom::Box NesScreenWidget::measure(const chisa::geom::Box& constraintSize)
{
	return chisa::geom::Box(256,240);
}

}}
