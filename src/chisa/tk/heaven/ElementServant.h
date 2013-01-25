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

#include "../Angel.h"

namespace chisa {
namespace tk {

class ElementServant : public Servant {
private:
	Handler<Element> element_;
	float anim_;
	bool animEnabled_;
public:
	inline Handler<Element> const& element() const { return this->element_; };
public:
	Handler<ElementServant> self() noexcept;
	inline bool const& animEnabled() const noexcept { return this->animEnabled_; };
	inline void animEnabled(bool const& enabled) noexcept { this->animEnabled_ = enabled; };
public:
	ElementServant(Handler<AngelTarget> const& angelTarget, Handler<Element> const& elem);
	virtual ~ElementServant() noexcept = default;
private:
	virtual Handler< ::donut::Object> createDonutObject(Handler< ::donut::Heap> const& heap) override final;
private:
	virtual void onShownImpl() override final;
	virtual void onHiddenImpl() override final;
	virtual void renderImpl(gl::Canvas& canvas) override final;
	virtual void idleImpl(const float delta_ms) override final;
	virtual geom::Box reshapeImpl(geom::Area const& area) override final;
private:
	virtual Handler<Element> findElementById(std::string const& id) override final;
	virtual Handler<Element> findElementByPoint(geom::Point const& screenVector) override final;
};


}}
