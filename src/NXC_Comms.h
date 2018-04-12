/*
*  Project     Nintendo Extension Controller Library
*  @author     David Madison
*  @link       github.com/dmadison/NintendoExtensionCtrl
*  @license    LGPLv3 - Copyright (c) 2018 David Madison
*
*  This file is part of the Nintendo Extension Controller Library.
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NXC_Comms_h
#define NXC_Comms_h

#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || \
    defined(__MK64FX512__) || defined(__MK66FX1M0__) // Teensy 3.0/3.1-3.2/LC/3.5/3.6
#include <i2c_t3.h>
#define NXC_I2C_TYPE i2c_t3
#else
#include <Wire.h>  // All other platforms
#define NXC_I2C_TYPE TwoWire
#endif

#define NXC_I2C_DEFAULT Wire

namespace NXC_Comms {
	const uint8_t I2C_Addr = 0x52;  // Address for all controllers
	const int I2C_ConversionDelay = 175;  // Microseconds, ~200 on AVR

	boolean writePointer(NXC_I2C_TYPE& i2c, byte ptr);
	boolean writeRegister(NXC_I2C_TYPE& i2c, byte reg, byte value);
	boolean requestMultiple(NXC_I2C_TYPE& i2c, uint8_t requestSize, uint8_t * dataOut);

	boolean readDataArray(NXC_I2C_TYPE& i2c, byte ptr, uint8_t requestSize, uint8_t * dataOut);
}

#endif
