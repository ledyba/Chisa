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

#include "World.h"

namespace chisa {
namespace donut {

const static std::string TAG("World");

World::World(logging::Logger& log)
:log_(log)
,code_(new Code)
,generation_(0)
,objectId_(0)
,donutObjectProvider_(new DonutObjectProvider(this))
,boolProvider_(new BoolProvider(this))
,intProvider_(new IntProvider(this))
,nullProvider_(new NullProvider(this))
{
	this->registerProvider( this->donutObjectProvider() );
	this->registerProvider( this->boolProvider() );
	this->registerProvider( this->intProvider() );
	this->registerProvider( this->nullProvider() );
	this->registerProvider(Handler<Provider>( new FloatProvider(this) ));
	this->registerProvider(Handler<Provider>( new StringProvider(this) ));

	this->objectProto_ = this->donutObjectProvider()->prototype();
	this->globalObject_ = this->createEmptyDonutObject();

	this->intProto_ = this->intProvider()->prototype();
	this->boolProto_ = this->boolProvider()->prototype();
	this->nullProto_ = this->nullProvider()->prototype();
	this->globalObject_->store(this, "Object", objectProto_);
	this->globalObject_->store(this, "Int", intProto_);
	this->globalObject_->store(this, "Boolean", boolProto_);
	this->globalObject_->store(this, "Null", nullProto_);

}

unsigned int World::nextGeneration()
{
	return ++this->generation_;
}

uintptr_t World::nextObjectId()
{
	return ++this->objectId_;
}

Handler<Provider> World::getProvider( const std::string& name ) const
{
	auto it = this->providers_.find(name);
	if(it != this->providers_.end()){
		return it->second;
	}
	return Handler<Provider>();
}

Handler<DonutObject> World::createDonutObject()
{
	Handler<DonutObject> obj(new DonutObject(this));
	obj->store(this, "__proto__", this->objectProto());
	obj->id(nextObjectId());

	return obj;
}

Handler<DonutObject> World::createEmptyDonutObject()
{
	Handler<DonutObject> obj(new DonutObject(this));
	obj->id(nextObjectId());

	return obj;
}

Handler<StringObject> World::createStringObject(const std::string& val)
{
	Handler<StringObject> obj(new StringObject(this, val));
	obj->id(nextObjectId());

	return obj;
}

Handler<FloatObject> World::createFloatObject(const float& val)
{
	Handler<FloatObject> obj(new FloatObject(this, val));
	obj->id(nextObjectId());

	return obj;
}

Handler<DonutClosureObject> World::createDonutClosureObject( Handler<Closure> closure, Handler<Object> scope )
{
	Handler<DonutClosureObject> obj(new DonutClosureObject(this, closure, scope));
	obj->id(nextObjectId());

	return obj;
}

Handler<PureNativeClosureObject> World::createPureNativeClosureObject(std::string objectProviderName, std::string closureName, PureNativeClosureEntry::Signature f)
{
	Handler<PureNativeClosureObject> obj(new PureNativeClosureObject(this, objectProviderName, closureName, f));
	obj->id(nextObjectId());

	return obj;
}

Handler<Object> World::createInt(const int& val)
{
	return this->intProvider()->create(val);
}

Handler<Object> World::createBool(const bool& val)
{
	return this->boolProvider()->create(val);
}

Handler<Object> World::createNull()
{
	return this->nullProvider()->create();
}

void World::registerProvider( Handler<Provider> provider )
{
	this->providers_.insert(
			std::pair<std::string, Handler<Provider> >(provider->name(), provider)
			);
}

tinyxml2::XMLElement* World::serialize(tinyxml2::XMLDocument* doc)
{

}

void World::deserialize(tinyxml2::XMLElement* xml)
{

}

}}
