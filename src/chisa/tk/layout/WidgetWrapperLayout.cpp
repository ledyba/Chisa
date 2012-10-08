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

#include "GL/gl.h"
#include "WidgetWrapperLayout.h"
#include "../World.h"
#include "../../util/Param.h"
#include "../../gl/Canvas.h"

namespace chisa {
namespace tk {
namespace layout {

static const string TAG("WidgetWrapperLayout");

CHISA_LAYOUT_SUBKLASS_CONSTRUCTOR_DEF(WidgetWrapperLayout)
,parent_(nullptr)
,widget_(nullptr)
,fitMode_(Center)
,widgetScale_(1.0f)
{
}

WidgetWrapperLayout::~WidgetWrapperLayout()
{
	if(!this->widget()){
		return;
	}
	if(this->parent_){
		if(shared_ptr<World> world = this->world().lock()){
			//ワールドの書き換えと、ウィジットへの現親レイアウトの通知
			if(world->replaceWidget(this->widgetId_, this->parent_)) {
				this->widget()->updateWrapper(dynamic_pointer_cast<WidgetWrapperLayout>(this->self().lock()));
			}
			// TODO　ウィジットにレイアウト通知入れたほうがいい？？
		}
	}else{
		if(shared_ptr<World> world = this->world().lock()){
			//ワールドからの削除と、ウィジットの開放
			if(world->deleteWidget(this->widgetId_, this)){
				delete widget();
				this->widget(nullptr);
			}
		}
	}
}

weak_ptr<Layout> WidgetWrapperLayout::getChildAt(const size_t index) const
{
	return weak_ptr<Layout>();
}
size_t WidgetWrapperLayout::getChildCount() const
{
	return 0;
}
void WidgetWrapperLayout::idle(const float delta_ms)
{
	if(!widget()) {
		return;
	}
	this->widget()->idle(delta_ms);
}
void chisa::tk::layout::WidgetWrapperLayout::renderImpl(gl::Canvas& canvas, const Area& screenArea, const Area& area)
{
	if(!widget()){
		return;
	}
	canvas.translate(this->widgetPoint().x(), this->widgetPoint().y(), 0.0f);
	canvas.scale(this->widgetScale(), this->widgetScale(), this->widgetScale());
	widget()->render(canvas, Area(0,0,widgetSize().width(), widgetSize().height()));
}

Box chisa::tk::layout::WidgetWrapperLayout::onMeasure(const Box& constraint)
{
	if(!widget()){
		return Box(0,0);
	}
	Box box(widget()->measure(constraint));
	if(geom::isUnspecified(box.width()) || geom::isUnspecified(box.height())){
		log().e(TAG, "Widget \"this->widgetId_.c_str()\" box size unspecified.");
	}
	return box;
}

void chisa::tk::layout::WidgetWrapperLayout::onLayout(const Box& size)
{
	if(!widget()){
		return;
	}
	this->widgetSize(widget()->measure(size));
	widget()->reshape(this->widgetSize()); //そのまま要求を飲む
	switch(this->fitMode_)
	{
	case Fit: {
		if(size > this->widgetSize()){ //完全に小さい
			// 拡大。
			this->widgetScale(std::max(
					size.width() / this->widgetSize().width(),
					size.height() / this->widgetSize().height()
			));
		}else{
			//そうでない場合は長い辺に合わせて縮小。
			this->widgetScale(std::min(
					size.width() / this->widgetSize().width(),
					size.height() / this->widgetSize().height()
			));
		}
		Box scaled(this->widgetSize().width() * this->widgetScale(), this->widgetSize().height() * this->widgetScale());
		this->widgetPoint(Point((size.width() - scaled.width())/2, (size.height()-scaled.height())/2));
		break;
	}
	case Center: {
		this->widgetScale(1.0f);
		this->widgetPoint(Point((size.width() - this->widgetSize().width())/2, (size.height()-this->widgetSize().height())/2));
		break;
	}
	}
}

std::string WidgetWrapperLayout::toString()
{
	return util::format( "(WidgetWrapperLayout %s)", this->widgetId_.c_str());
}
void WidgetWrapperLayout::loadXML(LayoutFactory* const factory, XMLElement* const element)
{
	if(element->Attribute("fit", "fit")){
		this->fitMode_ = Fit;
	}else if(element->Attribute("fit", "center")){
		this->fitMode_ = Center;
	}
	const char* widgetKlass = element->Attribute("widget-klass", nullptr);
	const char* widgetId = element->Attribute("widget-id", nullptr);
	if(!widgetKlass){
		this->log().e(TAG, "Oops. widget-klass not defined for id \"%s\".", widgetId);
		return;
	}
	if(shared_ptr<World> world = this->world().lock()){
		if(widgetId && (this->parent_ = world->getWidgetById(widgetId))){
			world->replaceWidget(widgetId, this);
			this->widget(this->parent_->widget());
			this->widget()->updateWrapper(dynamic_pointer_cast<WidgetWrapperLayout>(this->self().lock()));
		}else{
			this->widget(world->createWidget(widgetKlass, element));
			if(!this->widget()){
				this->log().e(TAG, "Oops. widget \"%s\" not registered.", widgetKlass);
			}
		}
	}
}

}}}
