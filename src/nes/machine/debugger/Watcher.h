/**
 * Chisa
 * Copyright (C) 2013 psi
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
#include <cstdint>
#include <vector>
#include <tuple>
#include <cinamo/ClassUtil.h>

namespace nes {

typedef uint16_t break_id_t;

struct MemoryRange final{
	STACK_OBJECT(MemoryRange);
	DEFAULT_COPY_AND_ASSIGN(MemoryRange);
public:
	break_id_t id;
	uint16_t addr_first;
	uint16_t addr_last;
public:
	template <typename Archiver>
	void serialize(Archiver& arc){
		arc & id & addr_first & addr_last;
	}
public:
	constexpr MemoryRange(break_id_t id, uint16_t addr_begin, uint16_t addr_end) noexcept
	:id(id), addr_first(addr_begin), addr_last(addr_end) {

	}
	constexpr MemoryRange() noexcept
	:id(0), addr_first(0), addr_last(0) {

	}
	~MemoryRange() noexcept = default;
public:
	constexpr bool inline contain(uint16_t const addr) const noexcept{
		return addr_first <= addr && addr <= addr_last;
	}
public:
	struct CompareByID final {
		constexpr inline bool operator()(MemoryRange const& a, MemoryRange const& b){
			return a.id < b.id;
		}
		constexpr inline bool operator()(break_id_t const& a, MemoryRange const& b){
			return a < b.id;
		}
		constexpr inline bool operator()(MemoryRange const& a, break_id_t const& b){
			return a.id < b;
		}
	};
	struct CompareByRange final {
		inline bool operator()(MemoryRange const& a, MemoryRange const& b){
			return std::tie(a.addr_first, a.addr_last) < std::tie(b.addr_first, b.addr_last);
		}
		inline bool operator()(std::pair<uint16_t, uint16_t> const& a, MemoryRange const& b){
			return std::tie(a.first, a.second) < std::tie(b.addr_first, b.addr_last);
		}
		inline bool operator()(MemoryRange const& a, std::pair<uint16_t, uint16_t> const& b){
			return std::tie(a.addr_first, a.addr_last) < std::tie(b.first, b.second);
		}
	};
};

class VirtualMachine;
class DebuggerFairy;
class Watcher final {
	STACK_OBJECT(Watcher);
	DISABLE_COPY_AND_ASSIGN(Watcher);
private:
	VirtualMachine& vm_;
	DebuggerFairy& debuggerFairy_;
	break_id_t breakUniq_;
	std::vector<MemoryRange> readBreaks_;
	std::vector<MemoryRange> writeBreaks_;
	std::vector<MemoryRange> execBreaks_;
public:
	template <typename Archiver>
	void serialize(Archiver& arc){
		arc & breakUniq_;
		arc & readBreaks_;
		arc & writeBreaks_;
		arc & execBreaks_;
	}
public:
	Watcher(VirtualMachine& vm, DebuggerFairy& debuggerFairy);
	~Watcher() {}
public:
	void stepRunning();
	void resumeRunning();
	void onBreak(); //XXX: 本当はプライベート
private:
	inline break_id_t createBreakUniq() noexcept { return ++this->breakUniq_; };
public:
	break_id_t addMemoryReadBreak(uint16_t addr_begin,uint16_t addr_end);
	break_id_t addMemoryWriteBreak(uint16_t addr_begin,uint16_t addr_end);
	break_id_t addMemoryExecBreak(uint16_t addr_begin,uint16_t addr_end);
	bool removeMemoryReadBreak(break_id_t id);
	bool removeMemoryWriteBreak(break_id_t id);
	bool removeMemoryExecBreak(break_id_t id);
	std::vector<MemoryRange> removeMemoryExecBreakAll();
public:
	uint8_t onMemoryRead(uint16_t const addr, uint8_t const value){
		/*for(MemoryRange const& range : this->readBreaks_) {
			if(range.contain(addr)){

				break;
			}
		}*/
		return value;
	}
	uint8_t onMemoryWrite(uint16_t const addr, uint8_t const value){
		/*for(MemoryRange const& range : this->writeBreaks_) {
			if(range.contain(addr)){

				break;
			}
		}*/
		return value;
	}
	void onMemoryExecute(uint16_t const addr){
		for(MemoryRange const& range : this->execBreaks_) {
			if(range.contain(addr)){
				onBreak();
				break;
			}
		}
	}
};

}
