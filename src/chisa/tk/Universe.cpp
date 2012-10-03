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

#include "Universe.h"

static const std::string TAG("Universe");

namespace chisa {
namespace tk {

Universe::Universe(logging::Logger& log) : log(log) {
}

Universe::~Universe() {
}

void Universe::render()
{
	//TODO: 下のスタックについて、オフスクリーンレンダリングしたほうがいい？？
	if(shared_ptr<World> topWorld = this->worldStack.top()){
		topWorld->render();
	}
}
void Universe::idle(const float delta_ms)
{
	if(shared_ptr<World> topWorld = this->worldStack.top()){
		topWorld->idle(delta_ms);
	}
}
void Universe::reshape(const Box& area)
{
	if(log.t()){
		log.t(TAG, "reshaped: %s", area.toString().c_str());
	}
	this->size(area);
	if(shared_ptr<World> topWorld = this->worldStack.top()){
		topWorld->reshape(area);
	}
}

void Universe::notifyWorldEnd(World& me)
{
	const int idx = this->worldStack.indexOf(me);
	if(idx < 0){
		log.w(TAG, "oops. notified unknown world.");
		return;
	}
	this->worldStack.erase(idx);

	// 下の画面について、以前よりサイズが変わってるようならreshape
	if(shared_ptr<World> topWorld = this->worldStack.top()){
		if(!topWorld->size().near(this->size(), 1)){
			topWorld->reshape(this->size());
		}
	}
}

}}
