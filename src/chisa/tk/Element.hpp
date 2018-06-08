/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include "Action.hpp"
#include <cinamo/ClassUtil.h>
#include <cinamo/Handler.h>
#include <cinamo/VectorMap.h>
#include <cinamo/XMLAttrParser.h>
#include <donut/Donut.hpp>
#include "../geom/Vector.hpp"
#include "../geom/Area.hpp"
#include "../gl/Color.hpp"
#include <string>
#include <cmath>

namespace tinyxml2 {
class XMLElement;
}

namespace chisa {
using namespace cinamo;

namespace gl {
class Canvas;
}

namespace tk {
class ElementFactory;
class ElementObject;
class World;

class Element : public HandlerBody<Element>, public ActionListener {
public:
	struct AttrName {
		const static std::string Id;
		const static std::string Enabled;
		const static std::string Padding;
		const static std::string Margin;
		const static std::string EdgeWidth;
		const static std::string EdgeColor;
		const static std::string ForegroundColor;
		const static std::string BackgroundColor;
		const static std::string DisabledForegroundColor;
		const static std::string DisabledBackgroundColor;
		const static std::string Width;
		const static std::string Height;
	};
private:
	DISABLE_COPY_AND_ASSIGN(Element);
private: /* クラス固定 */
	DEFINE_MEMBER_REF(protected, Logger, log); //ロガー
	VectorMap<std::string, std::function<void(tinyxml2::XMLElement*)> > attrMap_; //コンストラクタでセット
private: /* 所属 */
	HandlerW<World> const world_;
private: /* ツリー */
	DEFINE_MEMBER(public, private, HandlerW<Element>, parent); //親
	DEFINE_MEMBER(public, private, std::string, id); //要素に付けられたID
	geom::Box definedSize_;
	geom::Space margin_;
	geom::Space padding_;
	gl::Color edgeColor_;
	float edgeWidth_;
	gl::Color foregroundColor_;
	gl::Color backgroundColor_;
	gl::Color disabledForegroundColor_;
	gl::Color disabledBackgroundColor_;
	bool relayoutRequested_;
private: /* 画面描画情報 */
	geom::Box size_;
	geom::Box innerSize_;
	geom::Distance offsetFromParent_;
	geom::Distance innerOffsetFromParent_;
	geom::Point lastPositionInRoot_;
	geom::Area lastDrawnAreaInRoot_;
	geom::Point lastInnerPositionInRoot_;
	geom::Area lastInnerDrawnAreaInRoot_;
	bool onFocused_;
	bool enabled_;
private: /* どーなつとの接続 */
	HandlerW<ElementObject> donutObject_;
public:
	inline geom::Point const& lastPositionInRoot() const noexcept { return this->lastPositionInRoot_; };
	inline geom::Area const& lastDrawnAreaInRoot() const noexcept { return this->lastDrawnAreaInRoot_; };
	inline geom::Point const& lastInnerPositionInRoot() const noexcept { return this->lastInnerPositionInRoot_; };
	inline geom::Area const& lastInnerDrawnAreaInRoot() const noexcept { return this->lastInnerDrawnAreaInRoot_; };

	inline geom::Box const& size() const noexcept { return this->size_; };
	inline geom::Box const& innerSize() const noexcept { return this->innerSize_; };
	inline geom::Distance const& offsetFromParent() const noexcept { return this->offsetFromParent_; };
public:
	void margin(geom::Space const& m);
	void padding(geom::Space const& p);
	void foregroundColor(gl::Color const& c);
	void backgroundColor(gl::Color const& c);
	void disabledForegroundColor(gl::Color const& c);
	void disabledBackgroundColor(gl::Color const& c);
	void edgeColor(gl::Color const& c);
	void edgeWidth(float const& f);
	inline HandlerW<World> world() noexcept { return this->world_; }
	inline geom::Space const& margin() noexcept { return this->margin_; };
	inline geom::Space const& padding() const noexcept { return this->padding_; };
	inline gl::Color const& foregroundColor() const noexcept { return this->foregroundColor_; };
	inline gl::Color const& backgroundColor() const noexcept { return this->backgroundColor_; };
	inline gl::Color const& disabledForegroundColor() const noexcept { return this->disabledForegroundColor_; };
	inline gl::Color const& disabledBackgroundColor() const noexcept { return this->disabledBackgroundColor_; };
	inline gl::Color const& edgeColor() const noexcept { return this->edgeColor_; };
	inline bool onFocused() const noexcept { return this->onFocused_; };
	inline bool enabled() noexcept { return this->enabledImpl(); };
	inline void enabled( bool const& newState ) noexcept { this->enabledImpl(newState); };
public: /* レンダリング(非virtual) */
	virtual void onShown();
	virtual void onHidden();
	void render(gl::Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask);
	geom::Box measure(geom::Box const& constraint);
	void layout(geom::Distance const& offsetFromParent, geom::Box const& size);
	virtual bool isValidationRoot() const noexcept;
	void requestRelayout();
	virtual void notifyRelayoutFinished();
	void notifyViewRefreshed();
	virtual void showPoint(geom::Point const& relPtInElement);
	virtual void showPointInner(geom::Point const& relPtFromParent);
private:
	void forceRelayout();
public: /* ツリー操作 */
	Handler<Element> findRootElement();
	virtual Handler<Element> findElementById(std::string const& id);
	virtual Handler<Element> findElementByPoint(geom::Vector const& screenPoint);
public: /* 木の生成 */
	void loadXml(ElementFactory* const factory, tinyxml2::XMLElement* const element);
public: /* バックグラウンドタスク */
	virtual void idle(const float delta_ms);
public:
	virtual std::string toString() const;
	virtual void onFocusGained(float const& timeMs, geom::Point const& lastPtInScreen) override;
	virtual void onFocusLost(float const& timeMs) override;
protected:  /* 子供が必ず実装するメソッド */
	virtual void onShownImpl(){};
	virtual void onHiddenImpl(){};
	virtual void renderImpl(gl::Canvas& canvas, geom::Point const& ptInRoot, geom::Area const& mask) = 0;
	virtual geom::Box measureImpl(geom::Box const& constraint) = 0;
	virtual void layoutImpl(geom::Distance const& offsetFromParent, geom::Box const& size) = 0;
	virtual void loadXmlImpl(ElementFactory* const factory, tinyxml2::XMLElement* const element) = 0;
	virtual bool notifyViewRefreshedImpl();
	virtual bool enabledImpl();
	virtual void enabledImpl(bool const& newstate);
public: /* どーなつとの接続 */
	Handler<ElementObject> donutObject();
protected:
	Element(Logger& log, HandlerW<World> world, HandlerW<Element> parent);
	template <typename T> void addAttribute(std::string const& name, T& ptr)
	{
		this->attrMap_.insert(name, std::bind(xml::parseAttr<T>, std::string(name), std::ref(ptr), std::ref(ptr), std::placeholders::_1));
	}
	template <typename T> Handler<T> createChild(){
		return Handler<T>(new T(this->log(), this->world(), this->self()));
	}
public:
	inline bool onFree() noexcept { return false; };
	virtual ~Element() noexcept = default;
};


#define CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_LIST Logger& log, HandlerW<World> world, HandlerW<Element> parent
#define CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_APPLY log, world, parent

#define CHISA_ELEMENT_SUBKLASS_FINAL(Klass) \
public:\
	Klass(CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_LIST);\
	virtual ~Klass() noexcept;

#define CHISA_ELEMENT_SUBKLASS(Klass) \
public:\
	Klass(CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_LIST);\
	virtual ~Klass() noexcept;

#define CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_SETUP_BASE(Derived)\
Derived(CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_APPLY)

#define CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_DEF_DERIVED(Klass, Derived) \
Klass::Klass(CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_LIST)\
:CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_SETUP_BASE(Derived)

#define CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_DEF(Klass) CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_DEF_DERIVED(Klass, Element)

}}