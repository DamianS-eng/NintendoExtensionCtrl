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

#ifndef NXC_ExtensionController_h
#define NXC_ExtensionController_h

#include "Arduino.h"

#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || \
    defined(__MK64FX512__) || defined(__MK66FX1M0__) // Teensy 3.0/3.1-3.2/LC/3.5/3.6
#include "i2c_t3.h"
#define NXC_I2C_TYPE i2c_t3
#else
#include "Wire.h"  // All other platforms
#define NXC_I2C_TYPE TwoWire
#endif

enum NXC_ControllerType {
	NXC_NoController,
	NXC_UnknownController,
	NXC_Nunchuk,
	NXC_ClassicController,
	NXC_GuitarController,
	NXC_DrumController,
	NXC_DJTurntable,
};

class ExtensionController {
public:
	ExtensionController();

	boolean begin();

	boolean connect();
	boolean reconnect();

	boolean update();

	void printDebug(Stream& stream = Serial);
	void printDebugRaw(uint8_t baseFormat);
	void printDebugRaw(Stream& stream = Serial, uint8_t baseFormat = HEX);

	NXC_ControllerType identifyController();
	NXC_ControllerType getConnectedID() const;
	uint8_t getControlData(uint8_t controlIndex) const;

	void setEnforceID(boolean enforce);

	const NXC_ControllerType controllerID = NXC_UnknownController;
	const uint8_t ControlDataSize = 6;  // Bytes per update

protected:
	ExtensionController(NXC_ControllerType conID, uint8_t datSize);

	boolean extractControlBit(uint8_t arrIndex, uint8_t bitNum) const;

private:
	boolean initialize();

	NXC_ControllerType requestIdentity();
	boolean controllerIDMatches();

	boolean verifyData();

	boolean readDataArray(byte pointer, uint8_t requestSize, uint8_t * dataOut);

	boolean writePointer(byte pointer);
	boolean writeRegister(byte reg, byte value);
	boolean requestMulti(uint8_t requestSize, uint8_t * dataOut);

	static const uint8_t I2C_Addr = 0x52;
	NXC_I2C_TYPE& I2C_Bus = Wire;

	boolean enforceControllerID = false;  // Off for generic controllers
	boolean initSuccess = false;
	NXC_ControllerType connectedID = NXC_NoController;
	uint8_t controlData[6];
};

#endif
