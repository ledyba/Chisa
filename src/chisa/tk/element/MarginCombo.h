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
#include "../Element.h"
#include "../../geom/Area.h"

namespace chisa {
namespace tk {
namespace element {

class MarginCombo : public Element {
	CHISA_ELEMENT_SUBKLASS(MarginCombo);
	std::weak_ptr<Element> child_;
	geom::Margin margin_;
public:
	virtual std::weak_ptr<Element> getChildAt(const size_t index) const override;
	virtual std::size_t getChildCount() const override;
	virtual std::string toString() const override;
private:
	virtual void renderImpl(gl::Canvas& canvas, const geom::Area& screenArea, const geom::Area& area) override;
	virtual geom::Box onMeasure(const geom::Box& constraint) override;
	virtual void onLayout(const geom::Box& size) override;
	virtual void loadXMLimpl(element::ElementFactory* const factory, tinyxml2::XMLElement* const element) override;
	virtual std::weak_ptr<Element> getElementByIdImpl(const std::string& id) override;
};

}}}