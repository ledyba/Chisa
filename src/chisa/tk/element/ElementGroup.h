/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include "../Element.h"
#include <vector>

namespace chisa {
using namespace cinamo;

namespace tk {

class ElementGroup : public Element {
	CHISA_ELEMENT_SUBKLASS_FINAL(ElementGroup);
public: /* ツリー操作 */
	virtual std::size_t getChildCount() const noexcept = 0;
	virtual Handler<Element> getChildAt( std::size_t const& idx ) const noexcept = 0;
	virtual void addChild(Handler<Element> const& h) = 0;
	virtual void addChild(std::size_t const& idx, Handler<Element> const& h) = 0;
	virtual Handler<Element> removeChild(std::size_t const& idx) = 0;
	virtual Handler<Element> removeChild(Handler<Element> const& h) = 0;
	virtual Handler<Element> lastChild() const noexcept = 0;
	virtual Handler<Element> frontChild() const noexcept = 0;
	virtual std::size_t bringChildToLast(Handler<Element> const& e) = 0;
	virtual std::size_t bringChildToFront(Handler<Element> const& e) = 0;
public: /* ツリー操作 */
	virtual void onShown() override = 0;
	virtual void onHidden() override = 0;
	virtual Handler<Element> findElementById(std::string const& id) override = 0;
	virtual Handler<Element> findElementByPoint(geom::Vector const& screenPoint) override = 0;
	virtual void notifyRelayoutFinished() = 0;
public: /* バックグラウンドタスク */
	virtual void idle(const float delta_ms) override = 0;
public: /* 実装メソッド */
	virtual std::string toString() const override = 0;
};

template <typename __Context>
class ElementGroupBase : public ElementGroup {
protected:
	ElementGroupBase(CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_LIST)
	:ElementGroup(CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_PARAM_APPLY){}
	virtual ~ElementGroupBase() noexcept {}
public:
	typedef __Context Context;
protected:
	typedef std::pair<Handler<Element>,Context> ContainerType;
	typedef ElementGroupBase<Context> Super;
private:
	std::vector<ContainerType> children_;
protected:
	inline std::vector<ContainerType>& children() noexcept { return this->children_; };
	inline std::vector<ContainerType> const& children() const noexcept { return this->children_; };
public: /* ツリー操作 */
	virtual void onShown() override final{
		for(auto p : this->children_){
			p.first->onShown();
		}
		Element::onShown();
	}
	virtual void onHidden() override final{
		for(auto p : this->children_){
			p.first->onHidden();
		}
		Element::onHidden();
	}
	virtual std::size_t getChildCount() const noexcept override final{
		return this->children_.size();
	}
	virtual Handler<Element> getChildAt( std::size_t const& idx ) const noexcept override {
		return children_.at(idx).first;
	}
	virtual void addChild(Handler<Element> const& h, Context const& ctx) {
		this->children_.push_back(ContainerType(h,ctx));
	}
	virtual void addChild(std::size_t const& idx, Handler<Element> const& h, Context const& ctx) {
		this->children_.insert(this->children_.begin() + idx, ContainerType(h,ctx));
	}
	virtual void addChild(Handler<Element> const& h) override final {
		this->addChild(h, Context());
	}
	virtual void addChild(std::size_t const& idx, Handler<Element> const& h) override final {
		this->addChild(idx, h, Context());
	}
	virtual Handler<Element> removeChild(std::size_t const& idx) override final {
		if(idx >= children_.size()) {
			CINAMO_EXCEPTION(Exception, "[BUG] Invalid index: %d >= %d", idx, children_.size());
		}
		auto it = this->children_.begin()+idx;
		Handler<Element> element ((*it).first);
		this->children_.erase(it);
		return element;
	}
	virtual Handler<Element> removeChild(Handler<Element> const& h) override final {
		auto it = std::find_if(children_.begin(), children_.end(), PairEq<Handler<Element>,Context>(h));
		if(it == children_.begin()) {
			CINAMO_EXCEPTION(Exception, "[BUG] %s does not have %s.", this->toString().c_str(), h->toString().c_str());
		}
		this->children_.erase(it);
		return h;
	}
	virtual Handler<Element> lastChild() const noexcept override final {
		return this->children_.empty() ? Handler<Element>() : this->children_.back().first;
	}
	virtual Handler<Element> frontChild() const noexcept override final {
		return this->children_.empty() ? Handler<Element>() : this->children_.front().first;
	}
	virtual std::size_t bringChildToLast(Handler<Element> const& e) override final {
		auto it = std::find_if(children_.begin(), children_.end(), PairEq<Handler<Element>,Context>(e));
		std::size_t s = std::distance(children_.begin(), it);
		if(it == this->children_.end()){
			CINAMO_EXCEPTION(Exception, "Element: %s is not contained in this combo.", e->toString().c_str());
		}
		auto d = *it;
		children_.erase(it);
		children_.push_back(d);
		this->requestRelayout();
		return s;
	}
	virtual std::size_t bringChildToFront(Handler<Element> const& e) override final {
		auto it = std::find_if(children_.begin(), children_.end(), PairEq<Handler<Element>,Context>(e));
		std::size_t s = std::distance(children_.begin(), it);
		if(it == this->children_.end()){
			CINAMO_EXCEPTION(Exception, "Element: %s is not contained in this combo.", e->toString().c_str());
		}
		auto d = *it;
		children_.erase(it);
		children_.insert(this->children_.begin(), d);
		this->requestRelayout();
		return s;
	}
public: /* ツリー操作 */
	virtual Handler<Element> findElementById(std::string const& id) override final {
		if(id==this->id()){
			return self();
		}
		for(ContainerType& child : this->children_) {
			if(Handler<Element> r = child.first->findElementById(id)){
				return r;
			}
		}
		return Handler<Element>();
	}
	virtual Handler<Element> findElementByPoint(geom::Vector const& screenPoint) override final {
		if(!this->lastDrawnAreaInRoot().contain(screenPoint)){
			return Handler<ElementGroup>();
		}
		for(ContainerType& child : this->children_) {
			if(child.first->lastDrawnAreaInRoot().contain(screenPoint)){
				return child.first->findElementByPoint(screenPoint);
			}
		}
		return this->self();
	}
	virtual void notifyRelayoutFinished() override final{
		Element::notifyRelayoutFinished();
		for(ContainerType& child : this->children_) {
			child.first->notifyRelayoutFinished();
		}
	}
public: /* バックグラウンドタスク */
	virtual void idle(const float delta_ms) override {
		for(ContainerType& child : this->children_) {
			child.first->idle(delta_ms);
		}
	}
};


}}
