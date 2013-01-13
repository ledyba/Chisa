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
#include <tarte/Exception.h>

#include "Button.h"
#include "../../gl/Canvas.h"
#include "../../gl/DrawableManager.h"
#include "../World.h"

namespace chisa {
namespace tk {

static std::string const TAG("Button");

const std::string Button::AttrName::ShadowColor("shadow-color");
const std::string Button::AttrName::ShadowDepth("shadow-depth");
const std::string Button::AttrName::DonutMachineName("donut");

CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_DEF_DERIVED(Button, AbstractButton)
,shadowColor_(gl::DarkGray)
,shadowDepth_(3.0f)
,donutMachineName_()
,script_(nullptr)
{
	this->addAttribute(AttrName::ShadowColor, this->shadowColor_);
	this->addAttribute(AttrName::ShadowDepth, this->shadowDepth_);
	this->addAttribute(AttrName::DonutMachineName, this->donutMachineName_);
}

Button::~Button() noexcept
{
}

std::string Button::toString() const
{
	return ::tarte::format("(Button text:\"%s\" %p)", this->text().c_str(), this);
}

void Button::shadowColor(gl::Color const& color)
{
	if(this->shadowColor_ != color){
		this->shadowColor_ = color;
	}
}
void Button::shadowDepth(float const& depth)
{
	if( this->shadowDepth_ != depth ) {
		this->shadowDepth_ = depth;
	}
}

void Button::renderOn(gl::Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask)
{
	canvas.fillRect(this->shadowColor_, geom::Area(ptInRoot, geom::Box(innerSize().width(), shadowDepth_)) );
	canvas.fillRect(this->shadowColor_, geom::Area(ptInRoot, geom::Box(shadowDepth_, innerSize().height())) );

	geom::Distance const offset(this->renderOffset_+geom::Distance(shadowDepth_, shadowDepth_));
	geom::Area const buttonMask(mask.point()-offset, mask.box());
	this->textImage()->draw(canvas, ptInRoot+offset, buttonMask);
}
void Button::renderOff(gl::Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask)
{
	canvas.fillRect(this->shadowColor_, geom::Area(ptInRoot+geom::Distance(0, innerSize().height()-shadowDepth_), geom::Box(innerSize().width(), shadowDepth_)) );
	canvas.fillRect(this->shadowColor_, geom::Area(ptInRoot+geom::Distance(innerSize().width()-shadowDepth_, 0), geom::Box(shadowDepth_, innerSize().height())) );

	geom::Distance const offset(this->renderOffset_);
	geom::Area const buttonMask(mask.point()-offset, mask.box());
	this->textImage()->draw(canvas, ptInRoot+offset, buttonMask);
}
bool Button::isOn() const noexcept
{
	return this->pushedCount() > 0;
}

geom::Box Button::measureButtonContent(geom::Box const& constraint)
{
	return this->textImage()->size()+geom::Distance(shadowDepth_,shadowDepth_);
}

void Button::layoutButtonContent(geom::Box const& size)
{
	this->renderOffset_ = ((size-this->textImage()->size()-geom::Distance(shadowDepth_,shadowDepth_))/2);
}

void Button::loadXmlImpl(ElementFactory* const factory, tinyxml2::XMLElement* const element)
{
	const char* src = element->GetText();;
	if( src ) {
		Handler<World> world( this->world().lock() );
		if( unlikely(!world) ) {
			TARTE_EXCEPTION(Exception, "[BUG] Oops. World is already dead.");
		}
		this->script_ = world->donut()->parse(src, "Button-Innnter", 0);
	}
}

void Button::onClick()
{
	if(this->script_){
		if(this->log().d()){
			this->log().d(TAG, "Executing script.");
		}
		Handler<World> world( this->world().lock() );
		if( unlikely(!world) ) {
			TARTE_EXCEPTION(Exception, "[BUG] Oops. World is already dead.");
		}
		Handler<Donut> donut(world->donut());
		Handler<Machine> vm(donut->queryMachine(this->donutMachineName_));
		if(this->log().d()){
			this->log().d(TAG, "Execute script on: %s", this->donutMachineName_.c_str());
		}
		vm->start(this->script_);
	}else{
		if(this->log().d()){
			this->log().d(TAG, "Button script is empty.");
		}
	}
}

}}
