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

#include "NesGeist.h"
#include "../chisa/tk/World.h"
#include "../chisa/gl/DrawableManager.h"
#include "../chisa/input/JoystickManager.h"
#include "Hexe.h"

namespace nes {

NesGeist::NesGeist(chisa::Logger& log, ::tarte::Handler<Hexe> const& hexe, ::tarte::HandlerW<chisa::tk::World> world)
:chisa::WorldGeist(log, hexe, world)
,chisa::tk::Task(log)
,machine_(nullptr)
,runner_t_(nullptr)
,runner_(nullptr)
,time_ms_(0.0f)
,joyState_(0)
{
	using namespace chisa::gl;
	this->machine_ = new VirtualMachine(*this, *this, this, nullptr);
	if( chisa::Handler<chisa::tk::World> world = this->world_.lock() ){
		this->spr_ = world->drawableManager()->queryRawSprite(ImageFormat::RGBA8, 256, 240);
	}
	;
	this->world()->quartet()->addInstrument(
			this->inst_ = Handler<Instrument>(new Instrument(*this))
			);
	Handler<chisa::JoystickManager> mgr = this->world()->joystickManager();
	const unsigned int numJoysticks = mgr->numJoysticks();
	if(numJoysticks > 0) {
		this->joystick_ = mgr->joystick(0);
	}
}

NesGeist::~NesGeist() noexcept
{
	if(Handler< ::chisa::tk::World> world = this->world()){
		if(Handler< ::chisa::Quartet> quartet = world->quartet()){
			quartet->removeInstrument(inst_);
		}
	}
	this->stopNES();
	delete this->machine_;
}

Handler<Hexe> NesGeist::hexe() {
	return ::tarte::Handler<Hexe>::__internal__fromRawPointerWithoutCheck(static_cast<Hexe*>(WorldGeist::hexe().get()));
}

Handler< ::donut::Object> NesGeist::createDonutObject(Handler< ::donut::Heap> const& heap)
{
	return hexe()->nesGeistProvider()->newInstance(heap, self());
}

std::string NesGeist::toString() const
{
	return format("(NesGeist %p)", this);
}

void NesGeist::dispatchRendering(const uint8_t (&nesBuffer)[screenHeight][screenWidth], const uint8_t paletteMask)
{
	using namespace chisa::gl;
	{
		NesGeist::Lock lock(*this);
		Sprite::Session s(lock.getSprite());
		unsigned char* mem8 = s.data();
		unsigned int* mem32 = nullptr;
		const int stride = s.stride();
		for(int y=0;y<screenHeight;++y){
			mem32 = reinterpret_cast<unsigned int*>(mem8);
			for(int x=0;x<screenWidth;++x){
				mem32[x] = nesPaletteARGB[nesBuffer[y][x] & paletteMask];
			}
			mem8+=stride;
		}
	}
	{
		std::unique_lock<std::mutex> lock(this->frame_mutex_);
		if(this->runner_){
			this->cond_.wait_for(lock, std::chrono::milliseconds(20));
		}
	}
}

void NesGeist::onUpdate()
{

}

void NesGeist::updateJoystick() {
	if(!this->joystick_) {
		return;
	}
	this->joystick_->update();
	this->joyState_ = 0;
	const int32_t x = this->joystick_->axis(0);
	const int32_t y = this->joystick_->axis(1);

	if(x > 100){
		joyState_ |= GamepadFairy::MASK_RIGHT;
	}else if(x < -100){
		joyState_ |= GamepadFairy::MASK_LEFT;
	}

	if(y > 100){
		joyState_ |= GamepadFairy::MASK_DOWN;
	}else if(y < -100){
		joyState_ |= GamepadFairy::MASK_UP;
	}

	joyState_ |= (this->joystick_->button(2)) << GamepadFairy::A;
	joyState_ |= (this->joystick_->button(1)) << GamepadFairy::B;
	joyState_ |= (this->joystick_->button(8)) << GamepadFairy::START;
	joyState_ |= (this->joystick_->button(0)) << GamepadFairy::SELECT;
}

bool NesGeist::isPressed(uint8_t keyIdx)
{
	return (joyState_ >> keyIdx) & 1;
}

void NesGeist::loadNES(std::string const& abs_filename)
{
	this->stopNES();
	this->machine_->loadCartridge(abs_filename.c_str());
	this->machine_->sendHardReset();
}
void NesGeist::stopNES()
{
	if(this->runner_){
		this->runner_->queryStop();
		this->runner_t_->join();
		delete this->runner_t_;
		delete this->runner_;
		this->runner_t_ = nullptr;
		this->runner_ = nullptr;
		if( chisa::Handler<chisa::tk::World> world = this->world() ) {
			world->unregisterTask(this);
		}
	}
}

bool NesGeist::exec(const float delta_ms)
{
	std::unique_lock<std::mutex> lock(this->frame_mutex_);
	this->time_ms_ += delta_ms;
	while(this->time_ms_ > (1.0f/60)){
		this->time_ms_ -= (1.0f/60);
		this->cond_.notify_one();
	}
	this->updateJoystick();
	return true;
}

void NesGeist::startNES()
{
	this->stopNES();
	this->runner_ = new Runner(*this);
	this->runner_t_ = new std::thread(std::ref(*this->runner_));
	this->time_ms_ = 0.0f;
	if( chisa::Handler<chisa::tk::World> world = this->world() ) {
		world->registerTask(this);
	}
}

XValue NesGeist::saveNES()
{
	this->stopNES();
	XValue const savedata = this->machine_->save();
	this->startNES();
	return savedata;
}

void NesGeist::loadNES(XValue const& data)
{
	this->stopNES();
	this->machine_->load(data);
	this->startNES();
}

NesGeist::Instrument::Instrument(NesGeist& self)
:self_(self)
{

}

//---------------------------------------------------------------------------------------------------------------------
// サウンド
//---------------------------------------------------------------------------------------------------------------------

::chisa::SoundSpec NesGeist::Instrument::querySpec(::chisa::SoundSpec const& spec) noexcept
{
	return ::chisa::SoundSpec(::chisa::SoundSpec::DataFormat::S16SYS, 1, 44100, 8192);
}

void NesGeist::Instrument::playImpl(unsigned char *stream, int len)
{
	self_.popAudio(reinterpret_cast<int16_t*>(stream), len/sizeof(int16_t));
}


//---------------------------------------------------------------------------------------------------------------------
// 実行スレッドの実装
//---------------------------------------------------------------------------------------------------------------------

NesGeist::Runner::Runner(NesGeist& parent)
:parent_(parent)
,stop_(false)
{

}

void NesGeist::Runner::queryStop()
{
	this->stop_=true;
}

void NesGeist::Runner::operator ()()
{
	while(!this->stop_){
		for(size_t i=0;i<1000*1000;++i){
			this->parent_.machine_->run();
		}
	}
}

NesGeist::Lock::Lock(NesGeist& parent)
:parent_(parent)
{
	parent_.spr_mutex_.lock();
}
NesGeist::Lock::~Lock()
{
	parent_.spr_mutex_.unlock();
}

}

