/* coding: utf-8 */
/**
 * Chisa
 *
 * Copyright 2012-2013, PSI
 */

#pragma once
#include "Disassembler.h"
#include <string>

namespace nes {

Instruction encodeInst(std::string const& dis);

}
