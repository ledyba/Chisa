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
#include "Node.h"

namespace chisa {
namespace tk {
namespace widget {

Node::Node()
{
}

std::shared_ptr<Document> Node::createRootDocument()
{
	std::shared_ptr<Document> node(new Document);
	node->root(node);
	node->parent(std::weak_ptr<Node>());
	return node;
}


TreeNode::TreeNode()
{
	this->addAttribute("id", this->id_);
}

void TreeNode::parseAttribute(tinyxml2::XMLElement* node)
{
	for(auto it : this->attrMap_){
		it.second(node);
	}
}

BlockNode::BlockNode()
{
	this->addAttribute("margin", this->margin_);
	this->direction(None);
	this->addAttribute("float", this->direction_);
	this->width(geom::Unspecified);
	this->addAttribute("width", this->width_);
	this->height(geom::Unspecified);
	this->addAttribute("height", this->height_);
}

InlineNode::InlineNode()
{

}

Document::Document()
{

}

Paragraph::Paragraph()
{

}

Heading::Heading(int const level)
:level_(level)
{
}

BreakLine::BreakLine()
{
}

Link::Link()
{
	//this->addAttribute("src", )
}

Font::Font()
:size_(geom::Unspecified)
{
	this->color(gl::UNCHANGED_COLOR);
	this->addAttribute("color", this->color_);
	this->size(NAN);
	this->addAttribute("size", this->size_);
}

Text::Text(std::string text)
:text_(text)
{
}

}}}

