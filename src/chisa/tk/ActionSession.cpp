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

#include "Action.h"
#include "World.h"
#include "Element.h"
#include <algorithm>

namespace chisa {
namespace tk {

static const std::string TAG("GestureSession");

ActionSession::ActionSession(Logger& log, const unsigned int pointerIndex, HandlerW<Element> targetElement, geom::Point const& startPoint, const float startTimeMs)
:log_(log)
,target_(targetElement)
,pointerIndex_(pointerIndex)
,startPoint_(startPoint)
,startTimeMs_(startTimeMs)
,lastPoint_(startPoint)
,lastTimeMs_(startTimeMs)
,totalMoved_(0,0)
{
	const Handler<Element> orig_target = targetElement.lock();
	if(!orig_target){
		log.e(TAG, "[Touch Session %d] oops. Target Element was already deleted.", this->pointerIndex_);
		return;
	}
	//このセッションに関わるレイアウトを列挙
	Handler<Element> _it = orig_target;
	while( _it ) {
		this->elementChain_.push_back( _it );
		_it = _it->parent().lock();
	}
	std::reverse(this->elementChain_.begin(), this->elementChain_.end());
	for(ElementIterator it = this->elementChain_.begin(); it != this->elementChain_.end(); ++it){
		Handler<Element> target = it->lock();
		if(log.t()){
			log.t(TAG, "Touch Session creating: %s at %f index: %d element: %s", startPoint.toString().c_str(), startTimeMs, pointerIndex, target->toString().c_str());
		}
		if(target->onDownRaw(this->startTimeMs_, this->startPoint_)){
			//onDownイベントをconsumeした先には一切イベントを分け与えない。
			if(it+1 != this->elementChain_.end()){
				this->elementChain_.erase(it+1, this->elementChain_.end());
				HandlerW<Element>().swap(this->target_);
			}
			break;
		}
	}
}

ActionSession::~ActionSession()
{
}

void ActionSession::onTouchUp(float const& timeMs, geom::Point const& pt)
{
	this->lastTimeMs_ = timeMs;
	this->lastPoint_ = pt;
	this->invokeUpRaw(timeMs, pt);

	this->totalMoved_ += (pt-this->lastPoint_);

	const float timeDiff = this->lastTimeMs_-this->startTimeMs_;
	const geom::Velocity vel(this->totalMoved_, timeDiff);
	if(fabs(vel.x()) > MinFlingVelocity || fabs(vel.y()) > MinFlingVelocity ){
		this->invokeFling(timeMs, this->startPoint_, pt, vel);
		return;
	}
	if( Handler<Element> target = this->target_.lock() ){
		if(target->screenArea().contain(pt)){
			target->onSingleTapUp(timeMs, pt);
			return;
		}
	}
}

void ActionSession::onTouchMove(float const& timeMs, geom::Point const& pt)
{
	this->invokeMoveRaw(timeMs, pt);
	this->invokeScroll(timeMs, this->lastPoint_, pt, pt-this->lastPoint_);
	this->totalMoved_ += (pt-this->lastPoint_);
	this->lastTimeMs_ = timeMs;
	this->lastPoint_ = pt;
}

void ActionSession::onScroll(float const& timeMs, float const ratio)
{
	geom::Distance d(0, ratio);
	this->lastTimeMs_ = timeMs;
	this->invokeScroll(timeMs, this->lastPoint_, this->lastPoint_, d);
}

void ActionSession::onTextInput(float const& timeMs, std::string const& text)
{
	this->invokeTextInput(timeMs, text);
}
void ActionSession::onTextEdit(float const& timeMs, std::string const& text, int const& start, int const& length)
{
	this->invokeTextEdit(timeMs, text, start, length);
}

void ActionSession::onFocusGained(float const& timeMs)
{
	this->lastTimeMs_ = timeMs;
	this->invokeFocusGained(timeMs);
}
void ActionSession::onFocusLost(float const& timeMs)
{
	this->lastTimeMs_ = timeMs;
	this->invokeFocusLost(timeMs);
}

void ActionSession::invokeDownRaw(float const& timeMs, geom::Point const& pt)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			if(this->log().t()){
				this->log().t(TAG, "Touch Session creating: %s at %f index: %d element: %s", pt.toString().c_str(), this->startTimeMs_, this->pointerIndex_, target->toString().c_str());
			}
			if(target->onDownRaw(timeMs, pt)){
				break;
			}
		}
	}
}

void ActionSession::invokeUpRaw(float const& timeMs, geom::Point const& pt)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			if(this->log().t()){
				this->log().t(TAG, "Touch Session ending: %s at %f index: %d element: %s", pt.toString().c_str(), this->lastTimeMs_, this->pointerIndex_, target->toString().c_str());
			}
			if(target->onUpRaw(timeMs, pt)){
				break;
			}
		}
	}
}

void ActionSession::invokeMoveRaw(float const& timeMs, geom::Point const& pt)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			if(target->onMoveRaw(timeMs, pt)){
				break;
			}
		}
	}
}

void ActionSession::invokeFling(float const& timeMs, geom::Point const& start, geom::Point const& end, geom::Velocity const& velocity)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			if(target->onFling(timeMs, start, end, velocity)){
				break;
			}
		}
	}
}

void ActionSession::invokeScroll(float const& timeMs, geom::Point const& start, geom::Point const& end, geom::Distance const& distance)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			if(target->onScroll(timeMs, start, end, distance)){
				break;
			}
		}
	}
}



void ActionSession::invokeZoom(float const& timeMs, geom::Point const& center, const float ratio)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			if(target->onZoom(timeMs, center, ratio)){
				break;
			}
		}
	}
}

void ActionSession::invokeFocusGained(float const& timeMs)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			target->onFocusGained(timeMs);
		}
	}
}

void ActionSession::invokeFocusLost(float const& timeMs)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			target->onFocusLost(timeMs);
		}
	}
}

void ActionSession::invokeTextInput(float const& timeMs, std::string const& text)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			target->onTextInput(timeMs, text);
		}
	}
}

void ActionSession::invokeTextEdit(float const& timeMs, std::string const& text, int const start, int const length)
{
	for(HandlerW<Element> const& it : this->elementChain_) {
		if(Handler<Element> target = it.lock()){
			target->onTextEdit(timeMs, text, start, length);
		}
	}
}
}}