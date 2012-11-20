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

#include "Machine.h"
#include "../Exception.h"
#include "../../util/StringUtil.h"
#include "../object/NativeObject.h"
#include "../object/DonutObject.h"
#include "../source/Source.h"

namespace chisa {
namespace donut {

const static std::string TAG("Machine");

Context::Context(const Handler<Clock>& clk)
:time_(clk->now())
,local_(32)
{

}

Machine::Machine(logging::Logger& log, const Handler<Clock>& clock, const Handler<Source>& src, const Handler<Heap>& heap, const Handler<ConstPool>& constPool)
:log_(log)
,clock_(clock)
,src_(src)
,heap_(heap)
,constPool_(constPool)
{
}

Handler<Object> const& Machine::self()
{
	Context& ctx =  this->contextRevs_.back();
	Callchain& chain = ctx.callStack_.back();
	return chain.self_;
}

Handler<DonutObject> const& Machine::scope()
{
	Context& ctx =  this->contextRevs_.back();
	Callchain& chain = ctx.callStack_.back();
	return chain.scope_;
}

Handler<DonutClosureObject> const& Machine::closure()
{
	Context& ctx =  this->contextRevs_.back();
	Callchain& chain = ctx.callStack_.back();
	return chain.closure_;
}

pc_t& Machine::pc()
{
	Context& ctx =  this->contextRevs_.back();
	Callchain& chain = ctx.callStack_.back();
	return chain.pc_;
}

bool Machine::fetchPC( Instruction& inst )
{
	Context& ctx =  this->contextRevs_.back();
	Callchain& chain = ctx.callStack_.back();
	std::vector<donut::Instruction> const& instList = chain.closure_->closureCode()->instlist();
	if( chain.pc_ > instList.size() ){
		return false;
	}
	inst = instList[chain.pc_++];
	return true;
}

std::vector<Callchain>& Machine::callStack()
{
	Context& ctx =  this->contextRevs_.back();
	return ctx.callStack_;
}

std::vector<Handler<Object> >& Machine::local()
{
	Context& ctx =  this->contextRevs_.back();
	return ctx.local_;
}

std::vector<Handler<Object> >& Machine::stack()
{
	Context& ctx =  this->contextRevs_.back();
	return ctx.stack_;
}

Handler<Object> Machine::start( const std::size_t closureIndex )
{
	this->enterClosure(heap_->createNull(), createClosure( src_->getClosure( closureIndex ) ), heap_->createNull());
	return this->run();
}

Handler<DonutClosureObject> Machine::createClosure(const Handler<Closure>& closureCode)
{
	if( this->scope() ){
		return heap_->createDonutClosureObject(closureCode, this->scope());
	}else{
		return heap_->createDonutClosureObject(closureCode, heap_->global());
	}
}

void Machine::enterClosure(const Handler<Object>& self, const Handler<DonutClosureObject>& clos, const Handler<Object>& args)
{
	Handler<DonutObject> scope = heap_->createEmptyDonutObject();
	scope->store(heap_, "__scope__", clos);
	pc_t pc = 0;
	{
		Handler<Closure> c = clos->closureCode();
		const std::size_t max = c->arglist().size();
		for(std::size_t i=0;i<max;++i){
			const std::string arg = c->arglist().at(i);
			this->scope()->store( heap_, arg, args->load(heap_, i) );
		}
	}
	this->callStack().push_back( Callchain(pc, self, clos, scope) );
}

bool Machine::returnClosure()
{
	if(this->callStack().empty()){
		return false;
	}
	this->callStack().pop_back();
	return true;
}
Handler<Object> Machine::run()
{
	bool running = true;
	Instruction inst;
	while( running ){
		if(!this->fetchPC( inst )){
			running &= this->returnClosure();
			continue;
		}
		Instruction opcode, constKind;
		int constIndex;
		this->src_->disasm(inst, opcode, constKind, constIndex);
		if(this->log().t()){
			this->log().t(TAG, util::format("$%04x ", this->pc())+this->src_->disasm(inst));
		}
		switch(opcode){
		case Inst::Nop:
			break;
		case Inst::Push: {
			switch(constKind){
			case Inst::ConstBool: {
				this->stack().push_back( this->heap_->createBool( this->src_->getBool( constIndex ) ) );
				break;
			}
			case Inst::ConstFloat: {
				this->stack().push_back( this->constPool_->getFloat( constIndex ) );
				break;
			}
			case Inst::ConstClosure: {
				this->stack().push_back( createClosure( this->src_->getClosure(constIndex) ) );
				break;
			}
			case Inst::ConstInt: {
				this->stack().push_back( this->heap_->createInt( this->src_->getInt( constIndex ) ) );
				break;
			}
			case Inst::ConstString: {
				this->stack().push_back( this->constPool_->getString(constIndex) );
				break;
			}
			case Inst::ConstNull: {
				this->stack().push_back( heap_->createNull() );
				break;
			}
			default:
				throw DonutException(__FILE__, __LINE__, "Unknwon const type: %d", constKind);
			}
			break;
		}
		case Inst::PushCopy: {
			Handler<Object> obj(this->stack().back());
			this->stack().push_back(obj);
			break;
		}
		case Inst::Pop:
			this->stack().pop_back();
			break;
		case Inst::SearchScope: {
			Handler<Object> nameObj = this->stack().back();
			const std::string name = nameObj->toString(heap_);
			this->stack().pop_back();

			bool found = false;
			Handler<Object> obj = this->scope();
			while(!found){
				if(obj->have(heap_, name)){
					this->stack().push_back( obj );
					found = true;
					break;
				}else if( obj->have(heap_, "__scope__") ){
					obj = obj->load(heap_, "__scope__");
				}else{
					break;
				}
			}
			if(!found){
				this->stack().push_back( this->scope() );
			}
			break;
		}
		case Inst::StoreObj: {
			Handler<Object> storeObj = this->stack().back();
			this->stack().pop_back();
			Handler<Object> nameObj = this->stack().back();
			this->stack().pop_back();
			Handler<Object> destObj = this->stack().back();
			this->stack().pop_back();
			this->stack().push_back( destObj->store(heap_, nameObj->toString(heap_), storeObj) );
			break;
		}
		case Inst::LoadObj: {
			Handler<Object> nameObj = this->stack().back();
			this->stack().pop_back();

			Handler<Object> destObj = this->stack().back();
			this->stack().pop_back();

			this->stack().push_back( destObj->load(heap_, nameObj->toString(heap_)) );
			break;
		}
		case Inst::LoadLocal: {
			Handler<Object> obj = this->local()[constIndex&31];
			this->stack().push_back(obj);
			break;
		}
		case Inst::StoreLocal: {
			Handler<Object> obj = this->stack().back();
			this->local()[constIndex&31] = obj;
			break;
		}
		case Inst::Apply: {
			Handler<DonutObject> obj(heap_->createEmptyDonutObject());
			for(unsigned int i=constIndex;i>0;--i){
				Handler<Object> val = this->stack().back();
				this->stack().pop_back();
				obj->store(heap_, i-1, val);
			}

			Handler<Object> closureObj = this->stack().back();
			this->stack().pop_back();

			Handler<Object> destObj = this->stack().back();
			this->stack().pop_back();

			//XXX: ちゃんと型を使う
			if(!closureObj->isObject()){
				throw DonutException(__FILE__, __LINE__, "[BUG] Oops. \"%s\" is not callable.", closureObj->toString(heap_).c_str());
			} else if ( Handler<DonutClosureObject> closObj = closureObj.tryCast<DonutClosureObject>() ) {
				this->enterClosure(destObj, closObj, obj);
			} else if ( Handler<PureNativeClosureObject> clos = closureObj.tryCast<PureNativeClosureObject>() ) {
				this->stack().push_back( clos->apply(heap_, destObj, obj) );
			}else{
				throw DonutException(__FILE__, __LINE__, "[BUG] Oops. \"%s\" is not callable.", closureObj->toString(heap_).c_str());
			}
			break;
		}
		case Inst::ConstructArray: {
			Handler<Object> obj(heap_->createEmptyDonutObject() );
			for(unsigned int i=constIndex;i>0;--i){
				Handler<Object> val = this->stack().back();
				this->stack().pop_back();
				obj->store(heap_, util::toString(i-1), val);
			}
			this->stack().push_back(obj);
			break;
		}
		case Inst::ConstructObject: {
			Handler<Object> obj(heap_->createEmptyDonutObject() );
			for(int i=0;i<constIndex;++i){
				Handler<Object> val = this->stack().back();
				this->stack().pop_back();
				Handler<Object> name = this->stack().back();
				this->stack().pop_back();
				obj->store(heap_, name->toString(heap_), val);
			}
			this->stack().push_back(obj);
			break;
		}
		case Inst::Branch: {
			this->pc() += constIndex;
			break;
		}
		case Inst::BranchTrue: {
			Handler<Object> val = this->stack().back();
			this->stack().pop_back();
			if(val->toBool(heap_)){
				this->pc() += constIndex;
			}
			break;
		}
		case Inst::BranchFalse: {
			Handler<Object> val = this->stack().back();
			this->stack().pop_back();
			if(!val->toBool(heap_)){
				this->pc() += constIndex;
			}
			break;
		}
		default:
			throw DonutException(__FILE__, __LINE__, "[BUG] Oops. Unknwon opcode: closure<%s>:%08x", closure()->toString(heap_).c_str(), this->pc()-1);
		}
	}
	Handler<Object> result(this->stack().back());
	this->stack().pop_back();
	return result;
}

}}

