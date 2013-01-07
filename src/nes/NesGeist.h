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
#include "machine/VirtualMachine.h"
#include "../chisa/Hexe.h"
#include "../chisa/tk/Task.h"
#include "../chisa/gl/Sprite.h"
#include "../chisa/audio/Instrument.h"
#include <tarte/Thread.h>
#include <tarte/Logger.h>

namespace nes {
using namespace tarte;

class NesGeist : public chisa::WorldGeist, public VideoFairy, public AudioFairy, public GamepadFairy, public chisa::tk::Task {
private:
	class Runner {
	private:
		NesGeist& parent_;
		bool stop_;
	public:
		Runner(NesGeist& parent);
		virtual ~Runner() = default;
	public:
		void queryStop();
	public:
		void operator()();
	};
	class Instrument : public chisa::Instrument {
	private:
		NesGeist& self_;
	public:
		Instrument(NesGeist& self);
		virtual ~Instrument() noexcept = default;
	private:
		virtual ::chisa::SoundSpec querySpec(::chisa::SoundSpec const& spec) noexcept override final;
		virtual void playImpl(unsigned char *stream, int len) override final;
	};
public:
	class Lock {
		NesGeist& parent_;
	public:
		Lock(NesGeist& parent);
		virtual ~Lock();
		inline Handler<chisa::gl::Sprite> getSprite() { return parent_.spr_; };
	};
private:
	VirtualMachine* machine_;
	std::thread* runner_t_;
	Runner* runner_;
	Handler<chisa::gl::Sprite> spr_;
	std::mutex spr_mutex_;
	std::mutex frame_mutex_;
	float time_ms_;
	std::condition_variable cond_;
	Handler<Instrument> inst_;
public:
	NesGeist(Logger& log, HandlerW<chisa::tk::World> world);
	virtual ~NesGeist() noexcept;
	virtual std::string toString() const override;
	inline VirtualMachine* machine() const noexcept { return this->machine_; };
public:
	virtual void dispatchRendering(const uint8_t (&nesBuffer)[screenHeight][screenWidth], const uint8_t paletteMask) override;
	virtual void onUpdate() override;
	virtual bool isPressed(uint8_t keyIdx) override;
	virtual bool exec(const float delta_ms) override;
public:
	void stopNES();
	void loadNES(std::string const& abs_filename);
	void startNES();
public:
	XValue saveNES();
	void loadNES(XValue const& data);
};

}
