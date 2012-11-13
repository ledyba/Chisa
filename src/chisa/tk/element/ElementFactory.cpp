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

#include "ElementFactory.h"
#include "../Element.h"
#include "../../logging/Exception.h"
#include <memory>
#include <tinyxml2.h>
#include "SplitCombo.h"
#include "Empty.h"
#include "ScrollCombo.h"
#include "WidgetElement.h"

namespace chisa {
namespace tk {
namespace element {

using std::shared_ptr;
using std::weak_ptr;
using std::auto_ptr;
using std::string;
using namespace tinyxml2;
using namespace logging;

const std::string ElementFactory::ElemName::World("world");
const std::string ElementFactory::ElemName::Vertical("vertical");
const std::string ElementFactory::ElemName::Horizontal("horizontal");
const std::string ElementFactory::ElemName::Empty("empty");
const std::string ElementFactory::ElemName::WidgetWrapper("widget");
const std::string ElementFactory::ElemName::Scroll("scroll");
const std::string ElementFactory::ElemName::Tab("tab");

const std::string ElementFactory::AttrName::Id("id");

ElementFactory::ElementFactory(logging::Logger& log, weak_ptr<World> world, const string& filename)
:log_(log)
,world_(world)
,filename_(filename)
,doc_(new XMLDocument())
,doc_free_by_me_(true)
{
	init();
	const int code = doc_->LoadFile(filename.c_str());
	if(code != XML_NO_ERROR){
		throw Exception(__FILE__, __LINE__, "Failed to read xml (%d): %s", code, filename.c_str());
	}
	this->root_ = doc_->RootElement();
	if(!this->root_ && ElemName::World == this->root_->Name()){
		throw Exception(__FILE__, __LINE__, "%s was parsed, but \"world\" element not found.", filename.c_str());
	}
}

ElementFactory::ElementFactory(logging::Logger& log, weak_ptr<World> world, const string& filename, XMLDocument* document, bool doc_free_by_me)
:log_(log)
,world_(world)
,filename_(filename)
,doc_(document)
,doc_free_by_me_(doc_free_by_me)
{
	init();
	this->root_ = doc_->RootElement();
	if(!this->root_ || ElemName::World != this->root_->Name()){
		throw Exception(__FILE__, __LINE__, "%s was parsed, but \"world\" element not found.", filename.c_str());
	}
}

ElementFactory::ElementFactory(logging::Logger& log, weak_ptr<World> world, const string& filename, const char* buffer, size_t lenb)
:log_(log)
,world_(world)
,doc_()
,doc_free_by_me_(true)
{
	init();
	const int code = doc_->Parse(buffer, lenb);
	if(code != XML_NO_ERROR){
		throw Exception(__FILE__, __LINE__, "Failed to read xml (%d): %s", code, filename.c_str());
	}
	this->root_ = doc_->RootElement();
}

void ElementFactory::init()
{
	this->registerElement<SplitCombo>(ElemName::Horizontal);
	this->registerElement<SplitCombo>(ElemName::Vertical);
	this->registerElement<Empty>(ElemName::Empty);
	this->registerElement<ScrollCombo>(ElemName::Scroll);
	this->registerElement<WidgetElement>(ElemName::WidgetWrapper);
	//this->registerElement<TabElement>(ElemName::Tab);
}

ElementFactory::~ElementFactory()
{
	if(doc_free_by_me_){
		delete this->doc_;
	}
	this->doc_ = nullptr;
}

void ElementFactory::registerElement(const std::string& elementName, std::function<shared_ptr<Element>(logging::Logger& log, weak_ptr<World> world, weak_ptr<Element> root, weak_ptr<Element> parent)> constructor)
{
	this->elementMap_.insert(std::make_pair(elementName, constructor));
}

shared_ptr<Element> ElementFactory::parseTree(weak_ptr<Element> root, weak_ptr<Element> parent, XMLElement* top)
{
	const char* name = top->Name();
	auto it = this->elementMap_.find(name);
	if(this->elementMap_.end() == it){
		throw logging::Exception(__FILE__,__LINE__, "Unknwon Element: %s", name);
	}
	shared_ptr<Element> elm(it->second(this->log(), this->world(), root, parent));
	elm->loadXML(this, top);
	return elm;
}

shared_ptr<Element> ElementFactory::parseTree(const string& elementId)
{
	for(XMLElement* elem = this->root_->FirstChildElement(); elem; elem = elem->NextSiblingElement()){
		const char* id = elem->Attribute(AttrName::Id.c_str(), nullptr);
		if(id && elementId == id){
			return this->parseTree(weak_ptr<Element>(), weak_ptr<Element>(), elem);
		}
	}
	throw logging::Exception(__FILE__, __LINE__, "Element ID \"%s\" not found in %s", elementId.c_str(), this->filename_.c_str());
}

}}}
