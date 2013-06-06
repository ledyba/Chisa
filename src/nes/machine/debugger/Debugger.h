/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include "MemoryComparator.h"
#include "Disassembler.h"
#include "Watcher.h"

namespace nes {

class Debugger final {
private:
	VirtualMachine& vm_;
	Disassembler disasm_;
	Watcher watcher_;
	MemoryComparator comparator_;
public:
	template <typename Archiver>
	void serialize(Archiver& arc){
		arc & disasm_;
		arc & watcher_;
		arc & comparator_;
	}
public:
	Debugger(VirtualMachine& vm, DebuggerFairy& debuggerFairy);
	~Debugger() {}
public:
	inline Disassembler const& disassembler() const noexcept { return this->disasm_; };
	inline Disassembler& disassembler() noexcept { return this->disasm_; };

	inline Watcher const& watcher() const noexcept { return this->watcher_; };
	inline Watcher& watcher() noexcept { return this->watcher_; };

	inline MemoryComparator const& comparator() const noexcept { return this->comparator_; };
	inline MemoryComparator& comparator() noexcept { return this->comparator_; };
public:
	/******************************************************************************************************************
	 * callback functions
	 ******************************************************************************************************************/
private:
	uint8_t inline memoryRead(uint16_t const addr, uint8_t const value) noexcept {
		return watcher_.onMemoryRead(addr, value);
	}
	uint8_t inline memoryWrite(uint16_t const addr, uint8_t now) noexcept {
		return watcher_.onMemoryWrite(addr, now);
	}
	void inline memoryExecute(uint16_t const addr) noexcept {
		disassembler().onMemoryExecute(addr);
		watcher_.onMemoryExecute(addr);
	}
private:
	uint8_t vramRead(uint16_t const addr, uint8_t const value){
		return value;
	}
	uint8_t vramWrite(uint16_t const addr, uint8_t const value){
		return value;
	}
public:
	void inline processorExecute(uint16_t const addr) noexcept {
		this->memoryExecute(addr);
	}
public: /* ram */
	uint8_t inline ramRead(uint16_t const addr, uint8_t const value) noexcept {
		return memoryRead(addr, value);
	}
	uint8_t inline ramWrite(uint16_t const addr, uint8_t now) noexcept {
		return memoryWrite(addr, now);
	}
public: /* video */
	uint8_t inline videoReadReg(uint16_t const addr, uint8_t const value) noexcept {
		return memoryRead(addr, value);
	}
	uint8_t inline videoWriteReg(uint16_t const addr, uint8_t now) noexcept {
		return memoryWrite(addr, now);
	}
	uint8_t inline videoWriteDMA(uint8_t now) noexcept {
		return memoryWrite(0x4014, now);
	}
public: /* audio */
	uint8_t inline audioRead(uint16_t const addr, uint8_t const value) noexcept {
		return memoryRead(addr, value);
	}
	uint8_t inline audioWrite(uint16_t const addr, uint8_t now) noexcept {
		return memoryWrite(addr, now);
	}
public: /* io */
	uint8_t inline ioRead(uint16_t const addr, uint8_t const value) noexcept {
		return memoryRead(addr, value);
	}
	uint8_t inline ioWrite(uint8_t now) noexcept {
		return memoryWrite(0x4016, now);
	}
private: /* cartridge */
	uint8_t inline cartridgeReadCPU(uint16_t const addr, uint8_t const value) {
		return memoryRead(addr, value);
	}
	uint8_t inline cartridgeWriteCPU(uint16_t const addr, uint8_t const now) {
		return memoryWrite(addr, now);
	}
public:
	uint8_t inline cartridgeReadCPUReg(uint16_t const addr, uint8_t const value) {
		return cartridgeReadCPU(addr, value);
	}
	uint8_t inline cartridgeWriteCPUReg(uint16_t const addr, uint8_t const now) {
		return cartridgeWriteCPU(addr, now);
	}
	uint8_t inline cartridgeReadCPUSaveArea(uint16_t const addr, uint8_t const value) {
		return cartridgeReadCPU(addr, value);
	}
	uint8_t inline cartridgeWriteCPUSaveArea(uint16_t const addr, uint8_t const now) {
		return cartridgeWriteCPU(addr, now);
	}
	uint8_t inline cartridgeReadCPUBankLow(uint16_t const addr, uint8_t const value) {
		return cartridgeReadCPU(addr, value);
	}
	uint8_t inline cartridgeWriteCPUBankLow(uint16_t const addr, uint8_t const now) {
		return cartridgeWriteCPU(addr, now);
	}
	uint8_t inline cartridgeReadCPUBankHigh(uint16_t const addr, uint8_t const value) {
		return cartridgeReadCPU(addr, value);
	}
	uint8_t inline cartridgeWriteCPUBankHigh(uint16_t const addr, uint8_t const now) {
		return cartridgeWriteCPU(addr, now);
	}
public:
	uint8_t videoReadPatternTable(uint16_t const addr, uint8_t const value) {
		return vramRead(addr, value);
	}
	uint8_t videoWritePatternTable(uint16_t const addr, uint8_t const value) {
		return vramWrite(addr, value);
	}
	uint8_t videoReadNameTable(uint16_t const addr, uint8_t const value) {
		return vramRead(addr, value);
	}
	uint8_t videoWriteNameTable(uint16_t const addr, uint8_t const value) {
		return vramWrite(addr, value);
	}
	uint8_t videoReadPalette(uint16_t const addr, uint8_t const value) {
		return vramRead(addr, value);
	}
	uint8_t videoWritePalette(uint16_t const addr, uint8_t const value) {
		return vramWrite(addr, value);
	}

};

}
