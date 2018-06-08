/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include <string>
#include <unordered_map>
#include <cinamo/ClassUtil.h>
#include <cinamo/functional/Base.h>
#include <donut/Donut.hpp>
#include "../gl/Canvas.hpp"
#include "../geom/Area.hpp"
#include "../audio/Quartet.hpp"

#include "Task.hpp"
#include "Stack.hpp"
#include "Universe.hpp"

namespace tinyxml2{
class XMLElement;
class XMLDocument;
}

namespace chisa {
using namespace cinamo;
class JoystickManager;

namespace tk {
class Element;
class Widget;
class ActionMediator;
class Heaven;
class WidgetElement;
class ElementFactory;

class Angel;
class AngelTarget;
class Servant;

class WidgetFactory;

class Patron;
class WorldObject;

class World : public HandlerBody<World> {
	friend WorldObject;
private:
	DEFINE_MEMBER_REF(public, Logger, log);
private:
	HandlerW<Universe> const universe_;
	DEFINE_MEMBER_CONST(public, std::string, name);
private:
	TaskHandler taskHandler_;
	Stack<Handler<Element> > elementStack_;
	Handler<Heaven> heaven_;
	std::unordered_map<std::string, HandlerW<WidgetElement> > widgetMap_;
	DEFINE_MEMBER(public, private, geom::Area, area);
private:
	tinyxml2::XMLDocument* doc_;
	DEFINE_MEMBER(public, private, Handler<WorldGeist>, geist);
private:
	DEFINE_MEMBER(public, private, ElementFactory*, elementFactory);
	DEFINE_MEMBER(public, private, WidgetFactory*, widgetFactory);
	DEFINE_MEMBER(private, private, ActionMediator*, gestureMediator);
	Handler< ::chisa::tk::Patron> patron_;
	Handler< ::donut::Donut> donut_;
	HandlerW< ::chisa::tk::WorldObject> donutObject_;
public:
	void render(gl::Canvas& canvas);
	void idle(const float delta_ms);
	void reshape(geom::Area const& area);
	/******************************************************************************
	 * ど～なっつ
	 ******************************************************************************/
	Handler< ::chisa::tk::Patron> const& patron() const noexcept { return this->patron_; };
	Handler< ::donut::Donut> const& donut() const noexcept { return this->donut_; };
	Handler< ::donut::Object> donutObject(Handler< ::donut::Heap> const& heap);
	void sendMessage( ::cinamo::XValue const& xv, std::string const& machineName="" );
	void back();
	void forward();
	/******************************************************************************
	 * ウィジット管理
	 ******************************************************************************/
public:
	Handler<WidgetElement> findWidgetById(std::string const& name);
	bool replaceWidget(std::string const& widgetId, HandlerW<WidgetElement> const& widget);
	bool deleteWidget(std::string const& widgetId, Handler<WidgetElement> const& widget);
	Handler<Widget> createWidget(std::string const& klass, tinyxml2::XMLElement* elem);
	/******************************************************************************
	 * エレメント管理
	 ******************************************************************************/
public:
	Handler<Element> realizeElement( std::string const& templateId );
	Handler<Element> findElementByPoint(geom::Point const& screenVector);
	Handler<Element> findElementById(std::string const& id);
	Handler<Element> rootElement();
private:
	Handler<Element> popElement();
	Handler<Element> pushElement(std::string const& elementId);
	Handler<Element> pushElement(Handler<Element> const& elm);
	/******************************************************************************
	 * Angel管理
	 ******************************************************************************/
public:
	Handler<Heaven> heaven();
	/******************************************************************************
	 * タスク管理
	 ******************************************************************************/
public:
	template <typename __F>
	void sendTask(__F f) { this->taskHandler_.send(f); };
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
			CINAMO_EXCEPTION(Exception, "Oops. Universe already removed.");
		}
	}
	template <typename... Args>
	std::string resolveUniverseFilepath(Args const&... related_filename) const
	{
		if(Handler<Universe> universe = this->universe_.lock()){
			return universe->resolveUniverseFilepath(related_filename...);
		}else{
			CINAMO_EXCEPTION(Exception, "Oops. Universe already removed.");
		}
	}
	/******************************************************************************
	 * IME Universeへ移譲
	 ******************************************************************************/
public:
	inline void startIME(geom::Area const& area) {
		if(Handler<Universe> universe = this->universe_.lock()){
			universe->startIME(area);
		}else{
			CINAMO_EXCEPTION(Exception, "Oops. Universe already removed.");
		}
	}
	inline void stopIME() {
		if(Handler<Universe> universe = this->universe_.lock()){
			universe->stopIME();
		}else{
			CINAMO_EXCEPTION(Exception, "Oops. Universe already removed.");
		}
	}
	/******************************************************************************
	 * ユーザーアクション。Universeから移譲されてくる
	 ******************************************************************************/
public:
	void onTouchDown(float const& timeMs, const unsigned int pointerIndex, geom::Point const& screenVector);
	void onTouchUp(float const& timeMs, const unsigned int pointerIndex, geom::Point const& screenVector);
	void onTouchMove(float const& timeMs, const unsigned int pointerIndex, geom::Point const& screenVector);
	void onScroll(float const& timeMs, const float delta);
	void onTextInput(float const& timeMs, std::string const& text);
	void onTextEdit(float const& timeMs, std::string const& text, int start, int length);
	void onKeyDown(float const& timeMs, bool isRepeat, SDL_Keysym const& sym);
	void onKeyUp(float const& timeMs, SDL_Keysym const& sym);
	/******************************************************************************
	 * サウンドマネジメント
	 ******************************************************************************/
public: /* サウンド */
	Handler<Quartet> quartet() const;
	/******************************************************************************
	 * ジョイスティック
	 ******************************************************************************/
public:
	Handler<JoystickManager> joystickManager() const;
	/******************************************************************************
	 * 生成
	 ******************************************************************************/
private:
	World(Logger& log, HandlerW<Universe> _universe, std::string const& worldname);
	void init();
	void initGeist(Handler<Universe> const& universe);
public:
	static Handler<World> create(Logger& log, HandlerW<Universe> _universe, std::string const& worldname)
	{
		Handler<World> ptr(new World(log, _universe, worldname));
		ptr->init();
		return ptr;
	}
	virtual ~World() noexcept;
	inline bool onFree() noexcept { return false; };
};

}}