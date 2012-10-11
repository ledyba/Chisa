/*
 * ScrollLayout.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: psi
 */

#include "ScrollLayout.h"
#include "../World.h"

namespace chisa {
namespace tk {
namespace layout {


CHISA_LAYOUT_SUBKLASS_CONSTRUCTOR_DEF(ScrollLayout)
,scrollMode_(None)
,scrollOffset_(0,0)
,lastMovedFrom_(0)
{

}

ScrollLayout::~ScrollLayout()
{
}

weak_ptr<Layout> ScrollLayout::getChildAt(const size_t index) const
{
	if(index == 0 && this->child_){
		return this->child_;
	}else{
		return weak_ptr<Layout>();
	}
}

size_t ScrollLayout::getChildCount() const
{
	if(this->child_){
		return 1;
	}else{
		return 0;
	}
}

void ScrollLayout::loadXMLimpl(LayoutFactory* const factory, tinyxml2::XMLElement* element)
{
	if( const char* _mode = element->Attribute("mode", nullptr) ){
		std::string mode(_mode);
		if(mode == "both"){
			this->scrollMode_ = Both;
		}else if(mode == "horizontal"){
			this->scrollMode_ = Horizontal;
		}else if(mode == "vertical"){
			this->scrollMode_ = Vertical;
		}
	}
	XMLElement* const childElement = element->FirstChildElement();
	if(childElement) {
		this->child_ = factory->parseTree(root(), self(), childElement);
	}
}

string ScrollLayout::toString()
{
	return util::format("(ScrollLayout %p)", this);
}

void ScrollLayout::renderImpl(gl::Canvas& canvas, const geom::Area& screenArea, const geom::Area& area)
{
	const geom::Area clipArea(this->scrollOffset_, this->size());
	const geom::Area logicalArea(area.point()+this->scrollOffset_, area.box());
	this->child_->render(canvas, screenArea, clipArea.intersect(logicalArea));
	if(this->lastMovedFrom_ < ScrollBarTimeOut){
		const float alpha = (ScrollBarTimeOut - this->lastMovedFrom_) / ScrollBarTimeOut;
		if((this->scrollMode_ & Vertical)){
			const float scrollMax = area.height() - this->childSize_.height();
			const float scrollRatio = this->scrollOffset_.y() / scrollMax;
			const float x = screenArea.x()+screenArea.width()-7.0f;
			const float len = (screenArea.height()-4.0f) * area.height() / this->childSize_.height();
			const float y = screenArea.y()+2.0f - (screenArea.height()-len-4.0f) * scrollRatio;
			canvas.drawLine(5.0f, .5f, .5f, .5f, alpha, geom::Point(x, std::max(y, screenArea.y())), geom::Point(x, std::min(y+len, screenArea.y()+screenArea.height())));
		}
		if((this->scrollMode_ & Horizontal)){
			const float scrollMax = area.width() - this->childSize_.width();
			const float scrollRatio = this->scrollOffset_.x() / scrollMax;

			const float y = screenArea.y()+screenArea.height()-7.0f;
			const float len = (screenArea.width()-4.0f) * area.width() / this->childSize_.width();
			const float x = screenArea.x()+2.0f - (screenArea.width()-len-4.0f) * scrollRatio;
			canvas.drawLine(5.0f, .5f, .5f, .5f, alpha, geom::Point(std::max(x, screenArea.x()), y), geom::Point(std::min(x+len, screenArea.x()+screenArea.width()), y));
		}
	}
}

geom::Box ScrollLayout::onMeasure(const geom::Box& constraint)
{
	return geom::Box(geom::VeryLarge, geom::VeryLarge);
}

void ScrollLayout::onLayout(const geom::Box& size)
{
	if(this->child_){
		geom::Box childBox(this->scrollMode_ & Horizontal ? geom::Unspecified : size.width(), this->scrollMode_ & Vertical ? geom::Unspecified : size.height());
		this->childSize_ = this->child_->measure(childBox);
		this->child_->layout(this->childSize_);
	}
}

void ScrollLayout::idle(const float delta_ms)
{
	geom::Point ptStart(this->scrollOffset_);
	geom::Point ptEnd(this->scrollOffset_+this->size());
	if((this->scrollMode_ & Horizontal) && ptStart.x() < 0){
		this->scrollOffset_.x(this->scrollOffset_.x() - (ptStart.x()*delta_ms/100));
	}
	if((this->scrollMode_ & Vertical) && ptStart.y() < 0){
			this->scrollOffset_.y(this->scrollOffset_.y() - (ptStart.y()*delta_ms/100));
	}
	if((this->scrollMode_ & Horizontal) && ptEnd.x() > this->childSize_.width()){
		this->scrollOffset_.x(this->scrollOffset_.x() - ((ptEnd.x() - this->childSize_.width()) * delta_ms/100));
	}
	if((this->scrollMode_ & Vertical) && ptEnd.y() > this->childSize_.height()){
		this->scrollOffset_.y(this->scrollOffset_.y() - ((ptEnd.y() - this->childSize_.height()) * delta_ms/100));
	}
	this->lastMovedFrom_ += delta_ms;
}

bool ScrollLayout::onScroll(const float timeMs, const geom::Point& start, const geom::Point& end, const geom::Distance& distance)
{
	if(this->scrollMode_ == Both){
		this->scrollOffset_ -= distance;
	}else if(this->scrollMode_ == Vertical){
		this->scrollOffset_.y(this->scrollOffset_.y() - distance.y());
	}else if(this->scrollMode_ == Horizontal){
		this->scrollOffset_.x(this->scrollOffset_.x() - distance.x());
	}
	this->lastMovedFrom_ = 0.0f;
	return true; //イベントを消費する
}


weak_ptr<Layout> ScrollLayout::getLayoutByIdImpl(const std::string& id)
{
	if(this->child_){
		return this->child_->getLayoutById(id);
	}else{
		return weak_ptr<Layout>();
	}
}

}}}
