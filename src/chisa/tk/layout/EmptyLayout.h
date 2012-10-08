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

#ifndef Chisa_EMPTYLAYOUT_H__CPP_
#define Chisa_EMPTYLAYOUT_H__CPP_

#include <memory>
#include <tinyxml2.h>
#include "../Layout.h"

namespace chisa {
namespace tk {
namespace layout {
using namespace tinyxml2;
using namespace std;
class LayoutFactory;

class EmptyLayout : public Layout {
	CHISA_LAYOUT_SUBKLASS_FINAL(EmptyLayout);
public:
	virtual weak_ptr<Layout> getChildAt(const size_t index) const override;
	virtual size_t getChildCount() const override;
	virtual void loadXML(LayoutFactory* const factory, tinyxml2::XMLElement* element) override;
	virtual string toString() override;
public:
	virtual void renderImpl(gl::Canvas& canvas, const Area& screenArea, const Area& area) override;
	virtual Box onMeasure(const Box& constraint) override;
	virtual void onLayout(const Box& size) override;
};

}}}
#endif /* INCLUDE_GUARD */
