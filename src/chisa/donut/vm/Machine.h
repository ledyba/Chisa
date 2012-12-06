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
#include <vector>
#include "../object/Object.h"
#include "../object/Heap.h"
#include "../source/Source.h"

namespace chisa {
namespace donut {
class Heap;
typedef unsigned int pc_t;

/**
 * クロージャの実行状態
 */
struct Callchain {
	pc_t pc_; // Program counter
	unsigned int stackBase_;
	Handler<Object> self_; //このクロージャの対象self
	Handler<DonutClosureObject> closure_; //クロージャ本体
	Handler<DonutObject> scope_; //ローカル変数の格納されるオブジェクト
public:
	Callchain(pc_t pc, unsigned int const& stackBase, const Handler<Object>& self, const Handler<DonutClosureObject>& closure, const Handler<DonutObject>& scope)
	:pc_(pc), stackBase_(stackBase), self_(self), closure_(closure), scope_(scope){
	}
	Callchain(Handler<Heap> const& heap, tinyxml2::XMLElement* data);
	tinyxml2::XMLElement* save(tinyxml2::XMLDocument* doc);
};

/**
 * 機械の実行状態。
 * 命令実行とは、このコンテキストの状態が変化していくこと
 */
struct Context {
	unsigned int time_;
	std::vector<Handler<Object> > stack_;
	std::vector<Handler<Object> > local_;
	std::vector<Callchain> callStack_;
	Context(const Handler<Clock>& clk);
	Context(const Handler<Clock>& clk, const Context& other);
	Context(Handler<Heap> const& heap, tinyxml2::XMLElement* data);
	~Context() noexcept = default;
	tinyxml2::XMLElement* save(tinyxml2::XMLDocument* doc);
public:
	struct TimeComparator : std::binary_function<Context const&, Context const&, bool> {
		bool operator()(Context const& a, Context const& b) const noexcept
		{
			return a.time_ < b.time_;
		}
	};
};

class Machine : public HandlerBody<Machine> {
	DEFINE_MEMBER_REF(private, logging::Logger, log);
private: /* 何があっても不変なもの。*/
	const Handler<Clock>& clock_;
	Handler<Heap> const heap_;
private: /* 時とともに変わっていくもの */
	std::vector<Context> contextRevs_;
private: /* それへのアクセス手段の提供。 */
	Handler<Object> const& self();
	Handler<DonutObject> const& scope();
	Handler<DonutClosureObject> const& closureObject();
	Handler<Source> const& src();
	Handler<Closure> const& closure();
	std::vector<Handler<Object> >& local();
	std::vector<Handler<Object> >& stack();
	unsigned int stackBase();
	std::vector<Callchain>& callStack();
	pc_t& pc();
public: /* 生成 */
	Machine(logging::Logger& log, const Handler<Clock>& clock, const Handler<Heap>& heap);
	virtual ~Machine() noexcept = default;
	bool onFree() noexcept { return false; };
public: /* 外部からの実行指示 */
	Handler<Object> start( const Handler<Source>& src );
	Handler<Object> startContinue( const Handler<Object>& obj );
	bool isInterrupted() const noexcept;
private: /* スタック操作 */
	void pushStack( const Handler<Object>& obj );
	Handler<Object> popStack();
	Handler<Object> topStack();
private: /* 実行 */
	void enterClosure(const Handler<Object>& self, const Handler<DonutClosureObject>& clos, const Handler<Object>& args);
	bool leaveClosure();
	bool fetchPC( Instruction& inst );
	Handler<Object> run();
public: /* 処理系の保存・復帰をします。 */
	void bootstrap();
	tinyxml2::XMLElement* save(tinyxml2::XMLDocument* doc);
	void load(tinyxml2::XMLElement* data);
public: /* 時間操作 */
	void onSeekNotify();
	void onDiscardFutureNotify();
	void onDiscardHistoryNotify();
};

}}
