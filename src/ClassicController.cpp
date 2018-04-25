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

#include "ClassicController.h"

namespace NintendoExtensionCtrl {
	// Core Gamepad
	boolean GamepadCore::dpadUp() const {
		return getControlBit(5, 0);
	}

	boolean GamepadCore::dpadDown() const {
		return getControlBit(4, 6);
	}

	boolean GamepadCore::dpadLeft() const {
		return getControlBit(5, 1);
	}

	boolean GamepadCore::dpadRight() const {
		return getControlBit(4, 7);
	}

	boolean GamepadCore::buttonA() const {
		return getControlBit(5, 4);
	}

	boolean GamepadCore::buttonB() const {
		return getControlBit(5, 6);
	}

	boolean GamepadCore::buttonStart() const {
		return getControlBit(4, 2);
	}

	boolean GamepadCore::buttonSelect() const {
		return getControlBit(4, 4);
	}

	// Super Gamepad
	boolean GamepadSuper::buttonX() const {
		return getControlBit(5, 3);
	}

	boolean GamepadSuper::buttonY() const {
		return getControlBit(5, 5);
	}

	boolean GamepadSuper::buttonL() const {
		return getControlBit(4, 5);
	}

	boolean GamepadSuper::buttonR() const {
		return getControlBit(4, 1);
	}
}

ClassicController::ClassicController(NXC_I2C_TYPE& i2cBus) : ExtensionController(i2cBus, NXC_ClassicController, 6) {}
ClassicController::ClassicController(ExtensionData& busData) : ExtensionController(busData, NXC_ClassicController, 6) {}

uint8_t ClassicController::leftJoyX() const {
	return getControlData(0) & 0x3F;
}

uint8_t ClassicController::leftJoyY() const {
	return getControlData(1) & 0x3F;
}

uint8_t ClassicController::rightJoyX() const {
	return ((getControlData(0) >> 3) & 0x18) | ((getControlData(1) >> 5) &  0x06 ) | (getControlData(2) >> 7);
}

uint8_t ClassicController::rightJoyY() const {
	return getControlData(2) & 0x1F;
}

uint8_t ClassicController::triggerL() const {
	return ((getControlData(2) & 0x60) >> 2) | getControlData(3) >> 5;
}

uint8_t ClassicController::triggerR() const {
	return getControlData(3) & 0x1F;
}

boolean ClassicController::buttonZL() const {
	return getControlBit(5, 7);
}

boolean ClassicController::buttonZR() const {
	return getControlBit(5, 2);
}

boolean ClassicController::buttonPlus() const {
	return buttonStart();
}

boolean ClassicController::buttonMinus() const {
	return buttonSelect();
}

boolean ClassicController::buttonHome() const {
	return getControlBit(4, 3);
}

void ClassicController::printDebug(Stream& stream) const {
	const char fillCharacter = '_';

	char buffer[62];

	char dpadLPrint = dpadLeft() ? '<' : fillCharacter;
	char dpadUPrint = dpadUp() ? '^' : fillCharacter;
	char dpadDPrint = dpadDown() ? 'v' : fillCharacter;
	char dpadRPrint = dpadRight() ? '>' : fillCharacter;

	char aButtonPrint = buttonA() ? 'A' : fillCharacter;
	char bButtonPrint = buttonB() ? 'B' : fillCharacter;
	char xButtonPrint = buttonX() ? 'X' : fillCharacter;
	char yButtonPrint = buttonY() ? 'Y' : fillCharacter;

	char plusPrint = buttonPlus() ? '+' : fillCharacter;
	char minusPrint = buttonMinus() ? '-' : fillCharacter;
	char homePrint = buttonHome() ? 'H' : fillCharacter;

	char ltButtonPrint = buttonL() ? 'X' : fillCharacter;
	char rtButtonPrint = buttonR() ? 'X' : fillCharacter;

	char zlButtonPrint = buttonZL() ? 'L' : fillCharacter;
	char zrButtonPrint = buttonZR() ? 'R' : fillCharacter;

	stream.print("Classic ");
	sprintf(buffer,
		"%c%c%c%c | %c%c%c | %c%c%c%c L:(%2u, %2u) R:(%2u, %2u) | LT:%2u%c RT:%2u%c Z:%c%c",
		dpadLPrint, dpadUPrint, dpadDPrint, dpadRPrint,
		minusPrint, homePrint, plusPrint,
		aButtonPrint, bButtonPrint, xButtonPrint, yButtonPrint,
		leftJoyX(), leftJoyY(), rightJoyX(), rightJoyY(),
		triggerL(), ltButtonPrint, triggerR(), rtButtonPrint,
		zlButtonPrint, zrButtonPrint);
	stream.println(buffer);
}
