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

#include "Task.h"

namespace chisa {
namespace tk {



TaskHandler::TaskHandler(Logger& log)
:log(log)
{

}
TaskHandler::~TaskHandler()
{

}
void TaskHandler::registerTask(Task* task)
{
	this->taskPool.insert(task);
}

void TaskHandler::unregisterTask(Task* task)
{
	this->taskPool.erase(task);
}

void TaskHandler::run(const float delta_ms)
{
	for(auto it = taskPool.begin(); it != taskPool.end(); ++it){
		if( !(*it)->exec(delta_ms) ){
			it = taskPool.erase(it);
		}
	}
}

}}
