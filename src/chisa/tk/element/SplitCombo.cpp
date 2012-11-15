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

#include "SplitCombo.h"
#include "ElementFactory.h"
#include "../../geom/Vector.h"

namespace chisa {
namespace tk {
namespace element {

static const std::string TAG("SplitCombo");
const std::string SplitCombo::AttrName::Weight("split-weight");
const std::string SplitCombo::AttrName::Max("split-max");
const std::string SplitCombo::AttrName::Min("split-min");


CHISA_ELEMENT_SUBKLASS_CONSTRUCTOR_DEF(SplitCombo)
,splitMode_(Vertical)
,totalSize_(geom::Unspecified)
,changed_getter(nullptr)
,changed_setter(nullptr)
,fixed_getter(nullptr)
,fixed_setter(nullptr)
,point_getter(nullptr)
,point_setter(nullptr)
{
	this->setMode(Vertical);
}

void SplitCombo::setMode(enum SplitMode mode)
{
	this->splitMode(mode);
	changed_getter = mode == Vertical ? (float(geom::Box::*)(void) const)&geom::Box::height : (float(geom::Box::*)(void) const)&geom::Box::width;
	changed_setter = mode == Vertical ? (void(geom::Box::*)(float))&geom::Box::height : (void(geom::Box::*)(float))&geom::Box::width;
	fixed_getter = mode == Vertical ? (float(geom::Box::*)(void) const)&geom::Box::width : (float(geom::Box::*)(void) const)&geom::Box::height;
	fixed_setter = mode == Vertical ? (void(geom::Box::*)(float))&geom::Box::width : (void(geom::Box::*)(float))&geom::Box::height;
	point_getter = mode == Vertical ? (float(geom::Point::*)(void) const)&geom::Point::y : (float(geom::Point::*)(void) const)&geom::Point::x;
	point_setter = mode == Vertical ? (void(geom::Point::*)(float))&geom::Point::y : (void(geom::Point::*)(float))&geom::Point::x;
}

SplitCombo::~SplitCombo() noexcept
{
}

std::string SplitCombo::toString() const
{
	if(this->splitMode() == Horizontal){
		return util::format("(HorizontalCombo %p)", this);
	}else if(this->splitMode() == Vertical){
		return util::format("(VerticalCombo %p)", this);
	}else{
		return util::format("(InvalidSplitCombo %p)", this);
	}
}

void SplitCombo::addChild(const SplitDef& def, std::shared_ptr<Element> element)
{
	std::shared_ptr<SplitCtx> ctx(new SplitCtx(def));
	ctx->element = element;
	this->children().push_back(ctx);
}

void SplitCombo::loadXMLimpl(ElementFactory* const factory, tinyxml2::XMLElement* top)
{
	const std::string name(top->Name());
	if(name == "horizontal"){
		this->setMode(Horizontal);
	}else if(name == "vertical"){
		this->setMode(Vertical);
	}else{
		log().e(TAG, "Oops. you might miss-spelled? \"s\"", name.c_str());
		this->setMode(Vertical);
	}
	for(tinyxml2::XMLNode* _node = top->FirstChild(); _node; _node=_node->NextSibling()){
		tinyxml2::XMLElement* elem = _node->ToElement();
		if(!elem){
			continue;
		}
		float weight = geom::Unspecified;
		float min = geom::Unspecified;
		float max = geom::Unspecified;
		elem->QueryFloatAttribute(AttrName::Weight.c_str(), &weight);
		elem->QueryFloatAttribute(AttrName::Min.c_str(), &min);
		elem->QueryFloatAttribute(AttrName::Max.c_str(), &max);
		//デフォルト値の設定
		if(geom::isUnspecified(min)){
			min = 0;
		}
		if(geom::isUnspecified(max)){
			max = geom::Unspecified;
		}
		const SplitDef def(weight, min, max);
		this->addChild(def, factory->parseTree(this->root(), this->self(), elem));
	}
}

void SplitCombo::idle(const float delta_ms)
{
	for(std::shared_ptr<SplitCtx> ctx : this->children()){
		ctx->element->idle(delta_ms);
	}
}

void SplitCombo::resetChildren()
{
	for(std::shared_ptr<SplitCtx> ctx : this->children()){
		ctx->size=geom::Unspecified;
	}
	this->totalSize_ = geom::Unspecified;
}

std::weak_ptr<Element> SplitCombo::getChildAt(const std::size_t index) const
{
	if(index < this->children().size()){
		std::shared_ptr<SplitCtx> ctx = this->children_.at(index);
		return std::weak_ptr<Element>(ctx->element);
	}
	return std::weak_ptr<Element>();
}
std::size_t SplitCombo::getChildCount() const
{
	return this->children().size();
}

void SplitCombo::renderImpl(gl::Canvas& canvas, const geom::Area& screenArea, const geom::Area& area)
{
	const float boxSize = (area.box().*changed_getter)();
	const float drawnStartOffset = (area.point().*point_getter)();
	const float drawnEndOffset = drawnStartOffset+boxSize;
	const float screenStart = (screenArea.point().*point_getter)();
	float offset = 0;
	for(std::shared_ptr<SplitCtx> ctx : this->children()){
		const float size = ctx->size;
		if(offset+size < drawnStartOffset){
			offset += size;
			continue;
		}else if(drawnEndOffset <= offset){
			break;
		}
		const float drawnStart = std::max(drawnStartOffset - offset, 0.0f);
		const float drawnSize = std::min(size-drawnStart, drawnEndOffset-offset-drawnStart);

		geom::Box drawnBox(area.box());
		(drawnBox.*changed_setter)(drawnSize);

		geom::Point drawnPoint(area.point());
		(drawnPoint.*point_setter)(drawnStart);

		geom::Point screenPoint(screenArea.point());
		(screenPoint.*point_setter)(screenStart+offset+drawnStart-drawnStartOffset);

		ctx->element->render(canvas, geom::Area(screenPoint, drawnBox), geom::Area(drawnPoint, drawnBox));
		offset += size;
	}
}

geom::Box SplitCombo::onMeasure(const geom::Box& constraint)
{
	const bool changedSpecified = geom::isSpecified((constraint.*changed_getter)());
	const bool fixedSpecified = geom::isSpecified((constraint.*fixed_getter)());
	geom::Box cbox(constraint);
	(cbox.*changed_setter)(geom::Unspecified);
	if(!changedSpecified){
		//いくらでも伸びてよし
		float totalSize = 0;
		float fixedMaxSize = 0;
		for(std::shared_ptr<SplitCtx> childCtx : this->children()){
			const geom::Box childSize(childCtx->element->measure(cbox));
			const float size = this->wrapSize((childSize.*changed_getter)(), childCtx->def);
			if(geom::isSpecified(size)){
				totalSize += size;
				childCtx->size = size;
			}else if(geom::isSpecified(childCtx->def.max)){
				totalSize += childCtx->def.max;
				childCtx->size = childCtx->def.max;
			}else{
				totalSize += childCtx->def.min;
				childCtx->size = childCtx->def.min;
			}
			if(geom::isSpecified((childSize.*fixed_getter)())) {
				fixedMaxSize = std::max(fixedMaxSize, (childSize.*fixed_getter)());
			}
		}
		geom::Box measured;
		(measured.*changed_setter)(totalSize);
		if(fixedSpecified){
			(measured.*fixed_setter)((constraint.*fixed_getter)());
		}else{
			(measured.*fixed_setter)(fixedMaxSize);
		}
		return measured;
	}else{
		//長さは制限される：ふくざつ！！
		const float limitChangedSize = (constraint.*changed_getter)();
		float fixedMaxSize = 0;
		float totalWeight = 0;

		//まず、親から与えられた長さが十分かどうか調べる
		float intendedSizeTotal = 0;
		float nonWeightedSizeTotal = 0;
		for(std::shared_ptr<SplitCtx> childCtx : this->children()){
			const bool weightSpecified = geom::isSpecified(childCtx->def.weight);
			const geom::Box childSize(childCtx->element->measure(cbox));
			if(weightSpecified){
				//ウェイトがかかっている場合は最小サイズを。
				childCtx->weight = childCtx->def.weight;
				intendedSizeTotal += childCtx->def.min;
				childCtx->size = childCtx->def.min;//足りなかった時に使うための仮置き
				totalWeight += childCtx->weight;
			}else if(geom::isUnspecified((childSize.*changed_getter)())){
				childCtx->weight = 1.0f;
				intendedSizeTotal += childCtx->def.min;
				childCtx->size = childCtx->def.min;//足りなかった時に使うための仮置き
				totalWeight += childCtx->weight;
			}else{
				//そうでない場合は、子レイアウトに言われた通りのサイズを。
				//ただしmin/maxは守る
				const float tempChangedSize = this->wrapSize((childSize.*changed_getter)(), childCtx->def);
				childCtx->size = tempChangedSize;
				intendedSizeTotal += tempChangedSize;
				nonWeightedSizeTotal += tempChangedSize;
			}
			if(geom::isSpecified((childSize.*fixed_getter)())) {
				fixedMaxSize = std::max(fixedMaxSize, (childSize.*fixed_getter)());
			}
		}

		if(intendedSizeTotal <= limitChangedSize){
			//十分足りる
			float leftWeight = totalWeight;
			float leftSize = limitChangedSize - nonWeightedSizeTotal;
			for(std::shared_ptr<SplitCtx> childCtx : this->children()){
				std::shared_ptr<Element> child;
				const bool weightSpecified = geom::isSpecified(childCtx->weight);
				if(weightSpecified){
					//ウェイトがかかっている
					float size = leftSize * childCtx->weight / leftWeight;
					//max/minを考慮しつつサイズを割り当てる
					size = this->wrapSize(size, childCtx->def);
					leftWeight -= childCtx->weight;
					leftSize -= size;
					childCtx->size = size;
				}else{
					//上のループですでに設定済みなのでどうでも良かった
				}
			}
		}else{
			//足りない
			const float changedOverrun = intendedSizeTotal-limitChangedSize;
			for(std::shared_ptr<SplitCtx> childCtx : this->children()){
				//元の割合に応じてサイズを設定
				childCtx->size -= changedOverrun * childCtx->size / intendedSizeTotal;
			}
		}
		geom::Box measured;
		(measured.*changed_setter)(limitChangedSize);
		if(fixedSpecified){
			(measured.*fixed_setter)((constraint.*fixed_getter)());
		}else{
			(measured.*fixed_setter)(fixedMaxSize);
		}
		return measured;
	}
}

void SplitCombo::onLayout(const geom::Box& size)
{
	for(std::shared_ptr<SplitCtx> ctx : this->children()){
		geom::Box box;
		(box.*changed_setter)(ctx->size);
		(box.*fixed_setter)((size.*fixed_getter)());
		ctx->element->layout(box);
	}
}

std::weak_ptr<Element> SplitCombo::getElementByIdImpl(const std::string& id)
{
	for(std::shared_ptr<SplitCtx> childCtx : this->children()){
		std::weak_ptr<Element> res = childCtx->element->getElementById(id);
		if(!res.expired()){
			return res;
		}
	}
	return std::weak_ptr<Element>();
}


}}}