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
#include <string>
#include <unordered_map>
#include "../util/ClassUtil.h"
#include "../gl/Canvas.h"
#include "../geom/Area.h"
#include "../donut/DozenBox.h"

#include "Task.h"
#include "Stack.h"
#include "Universe.h"

namespace tinyxml2{
class XMLElement;
class XMLDocument;
}

namespace chisa {

namespace tk {
class Element;
class Widget;
class GestureMediator;

namespace element {
class WidgetElement;
class ElementFactory;
}
namespace widget{
class WidgetFactory;
}

class World : public HandlerBody<World> {
private:
	DEFINE_MEMBER_REF(public, logging::Logger, log);
	HandlerW<Universe> const universe_;
	DEFINE_MEMBER_CONST(public, std::string, name);
	TaskHandler taskHandler_;
	Handler<donut::DozenBox> dozenBox_;
	Stack<Handler<Element> > elementStack_;
	std::unordered_map<std::string, element::WidgetElement*> widgetMap_;
	DEFINE_MEMBER(private, private, geom::Area, area);
private:
	tinyxml2::XMLDocument* doc_;
	DEFINE_MEMBER(public, private, Handler<WorldGeist>, geist);
private:
	element::ElementFactory* elementFactory_;
	widget::WidgetFactory* widgetFactory_;
	GestureMediator* gestureMediator_;
public:
	void render(gl::Canvas& canvas);
	void idle(const float delta_ms);
	void reshape(geom::Area const& area);
private:
	void popElement();
	void pushElement(std::string const& filename);
public:
	element::WidgetElement* getWidgetById(std::string const& name);
	bool replaceWidget(std::string const& widgetId, element::WidgetElement* const newHandler);
	bool deleteWidget(std::string const& widgetId, element::WidgetElement* const handler);
	Widget* createWidget(std::string const& klass, tinyxml2::XMLElement* elem);
public:
	Handler<Element> getElementByPoint(geom::Point const& screenVector);
	Handler<Element> getElementById(std::string const& id);
	/******************************************************************************
	 * タスク管理
	 ******************************************************************************/
public:
	void registerTask(Task* task);
	void unregisterTask(Task* task);
	/******************************************************************************
	 * Universeへの移譲
	 ******************************************************************************/
public:
	Handler<gl::DrawableManager> drawableManager() const;
	template <typename... Args>
	std::string resolveWorldFilepath(Args const&... related_filename) const
	{
		if(Handler<Universe> universe = this->universe_.lock()){
			return universe->resolveWorldFilepath(this->name(), related_filename...);
		}else{
			throw logging::Exception(__FILE__, __LINE__, "Oops. Universe already removed.");
		}
	}
	template <typename... Args>
	std::string resolveUniverseFilepath(Args const&... related_filename) const
	{
		if(Handler<Universe> universe = this->universe_.lock()){
			return universe->resolveUniverseFilepath(related_filename...);
		}else{
			throw logging::Exception(__FILE__, __LINE__, "Oops. Universe already removed.");
		}
	}
	/******************************************************************************
	 * タッチイベント。Universeから移譲されてくる
	 ******************************************************************************/
public:
	void onTouchDown(const float timeMs, const unsigned int pointerIndex, geom::Point const& screenVector);
	void onTouchUp(const float timeMs, const unsigned int pointerIndex, geom::Point const& screenVector);
	void onTouchMove(const float timeMs, const unsigned int pointerIndex, geom::Point const& screenVector);

	/******************************************************************************
	 * 生成
	 ******************************************************************************/
private:
	World(logging::Logger& log, HandlerW<Universe> _universe, std::string const& worldname);
	void init();
public:
	static Handler<World> create(logging::Logger& log, HandlerW<Universe> _universe, std::string const& worldname)
	{
		Handler<World> ptr(new World(log, _universe, worldname));
		ptr->init();
		return ptr;
	}
	virtual ~World() noexcept;
	inline bool onFree() noexcept { return false; };
};

}}
