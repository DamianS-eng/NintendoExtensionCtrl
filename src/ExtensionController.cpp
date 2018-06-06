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

#include "ExtensionController.h"

ExtensionData::ExtensionData(NXC_I2C_TYPE& i2cBus)
	: I2C_Bus(i2cBus) {}

ExtensionController::ExtensionController(NXC_I2C_TYPE& i2cBus) : busData(*new ExtensionData(i2cBus)), AllocatedData(true), i2c(i2cBus) {}

ExtensionController::ExtensionController(ExtensionData& busData) : busData(busData), i2c(busData.I2C_Bus) {}

ExtensionController::ExtensionController(NXC_I2C_TYPE& i2cBus, NXC_ControllerType conID, uint8_t datSize)
	: ControllerID(conID), ControlDataSize(datSize), enforceControllerID(true), busData(*new ExtensionData(i2cBus)), AllocatedData(true), i2c(i2cBus) {}

ExtensionController::ExtensionController(ExtensionData& busData, NXC_ControllerType conID, uint8_t datSize)
	: ControllerID(conID), ControlDataSize(datSize), enforceControllerID(true), busData(busData), i2c(busData.I2C_Bus) {}

ExtensionController::~ExtensionController() {
	if (AllocatedData) { delete &busData; }
}

void ExtensionController::begin() {
	i2c.startBus();
}

boolean ExtensionController::initialize() {
	/* Initialization for unencrypted communication.
	* *Should* work on all devices, genuine + 3rd party.
	* See http://wiibrew.org/wiki/Wiimote/Extension_Controllers
	*/
	if (!i2c.writeRegister(0xF0, 0x55)) { return false; }
	delay(10);
	//if (!i2c.writeRegister(0xFB, 0x00)) { return false; }
	//delay(20);
	return true;
}

boolean ExtensionController::connect() {
	if (initialize()) {
		identifyController();
		if (controllerIDMatches()) {
			return update();  // Seed with initial values
		}
	}
	else {
		busData.connectedID = NXC_NoController;  // Bad init, nothing connected
	}

	return false;
}

boolean ExtensionController::reconnect() {
	reset();
	return connect();
}

NXC_ControllerType ExtensionController::identifyController() {
	return busData.connectedID = id.identifyController();
}

void ExtensionController::reset() {
	busData.connectedID = NXC_NoController;
	for (int i = 0; i < NXC_CONTROL_DATA_MAX; i++) {
		busData.controlData[i] = 0;
	}
}

boolean ExtensionController::controllerIDMatches() const {
	if (busData.connectedID == ControllerID) {
		return true;  // Match!
	}
	else if (enforceControllerID == false && busData.connectedID != NXC_NoController) {
		return true;  // No enforcing and some sort of controller connected
	}

	return false;  // Enforced types or no controller connected
}

NXC_ControllerType ExtensionController::getConnectedID() const {
	return busData.connectedID;
}

void ExtensionController::setEnforceID(boolean enforce) {
	enforceControllerID = enforce;
}

boolean ExtensionController::update() {
	if (controllerIDMatches()){
		if (i2c.readDataArray(0x00, ControlDataSize, busData.controlData)) {
			return NXCtrl::verifyData(busData.controlData, ControlDataSize);
		}
	}
	
	return false;  // Something went wrong :(
}

uint8_t ExtensionController::getControlData(uint8_t controlIndex) const {
	return busData.controlData[controlIndex];
}

boolean ExtensionController::getControlBit(uint8_t arrIndex, uint8_t bitNum) const {
	return !(busData.controlData[arrIndex] & (1 << bitNum));
}

void ExtensionController::printDebug(Stream& stream) const {
	printDebugRaw(stream);
}

void ExtensionController::printDebugID(Stream& stream) const {
	uint8_t idData[id.IDSize];
	boolean success = id.requestIdentity(idData);

	if (success) {
		stream.print("ID: ");
		NXCtrl::printRaw(idData, id.IDSize, HEX, stream);
	}
	else {
		stream.println("Bad ID Read");
	}
}

void ExtensionController::printDebugRaw(Stream& stream) const {
	printDebugRaw(HEX, stream);
}

void ExtensionController::printDebugRaw(uint8_t baseFormat, Stream& stream) const {
	NXCtrl::printRaw(busData.controlData, ControlDataSize, baseFormat, stream);
}
