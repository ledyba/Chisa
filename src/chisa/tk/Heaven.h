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
#include <tarte/ClassUtil.h>
#include <tarte/Handler.h>
#include <tarte/VectorMap.h>
#include <string>
#include <cmath>

namespace tinyxml2 {
class XMLElement;
}

namespace chisa {
using namespace tarte;

namespace gl {
class Canvas;
}

namespace tk {
class ElementFactory;

class World;
class Angel;

class Heaven : public HandlerBody<Heaven> {
private:
	HandlerW<World> world_;
	VectorMap<std::string, Handler<Angel> > angelMap_;
public:
	Heaven(Handler<World> const& world);
	virtual ~Heaven() noexcept = default;
	inline bool onFree() const noexcept { return false; };
public:
	inline HandlerW<World> const& world() const noexcept { return this->world_; };
};


}}
