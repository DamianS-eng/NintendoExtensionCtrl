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

#include "NXC_Identity.h"
#include "NXC_Comms.h"
#include "NXC_Utils.h"
#include "NXC_DataMaps.h"

class ExtensionController {
public:
	struct ExtensionData {
		friend class ExtensionController;

		ExtensionData(NXC_I2C_TYPE& i2cbus = NXC_I2C_DEFAULT) :
			i2c(i2cbus) {}

		static const uint8_t ControlDataSize = 21;  // Largest reporting mode (0x3d)

	protected:
		NXC_I2C_TYPE & i2c;  // Reference for the I2C (Wire) class
		ExtensionType connectedType = ExtensionType::NoController;
		uint8_t controlData[ControlDataSize];
	};

	ExtensionController(ExtensionData& dataRef);

	void begin();

	boolean connect();
	virtual boolean specialInit();

	boolean update();

	void reset();

	ExtensionType getControllerType() const;
	uint8_t getControlData(uint8_t controlIndex) const;
	ExtensionData & getExtensionData() const;
	size_t getRequestSize() const;

	void setRequestSize(size_t size = MinRequestSize);

	void printDebug(Print& output = NXC_SERIAL_DEFAULT) const;
	void printDebugID(Print& output = NXC_SERIAL_DEFAULT) const;
	void printDebugRaw(Print& output = NXC_SERIAL_DEFAULT) const;
	void printDebugRaw(uint8_t baseFormat, Print& output = NXC_SERIAL_DEFAULT) const;

	const ExtensionType id = ExtensionType::AnyController;

	static const uint8_t MinRequestSize = 6;   // Smallest reporting mode (0x37)
	static const uint8_t MaxRequestSize = ExtensionData::ControlDataSize;

public:
	NXC_I2C_TYPE& i2c() const;  // Easily accessible I2C reference

	static const uint8_t I2C_Addr = 0x52;  // Address for all extension controllers
	static const uint8_t ID_Size = 6;  // Number of bytes for ID signature

	static boolean initialize(NXC_I2C_TYPE& i2c);

	static boolean writeRegister(NXC_I2C_TYPE& i2c, byte reg, byte value);

	static boolean requestData(NXC_I2C_TYPE& i2c, uint8_t ptr, size_t size, uint8_t* data);
	static boolean requestControlData(NXC_I2C_TYPE& i2c, size_t size, uint8_t* controlData);
	static boolean requestIdentity(NXC_I2C_TYPE& i2c, uint8_t* idData);

	static ExtensionType identifyController(NXC_I2C_TYPE& i2c);

protected:
	inline boolean initialize() const { return initialize(data.i2c); }

	inline boolean writeRegister(byte reg, byte value) const { return writeRegister(data.i2c, reg, value); }

	inline boolean requestData(uint8_t ptr, size_t size, uint8_t* dataOut) const { return requestData(data.i2c, ptr, size, dataOut); }
	inline boolean requestControlData(size_t size, uint8_t* controlData) const { return requestControlData(data.i2c, size, controlData); }
	inline boolean requestIdentity(uint8_t* idData) const { return requestIdentity(data.i2c, idData); }

	inline ExtensionType identifyController() const { return data.connectedType = identifyController(data.i2c); }

protected:
	ExtensionController(ExtensionData& dataRef, ExtensionType conID);

	typedef NintendoExtensionCtrl::CtrlIndex CtrlIndex;
	typedef NintendoExtensionCtrl::ByteMap   ByteMap;
	typedef NintendoExtensionCtrl::BitMap    BitMap;

	uint8_t getControlData(const ByteMap map) const {
		return (data.controlData[map.index] & map.mask) >> map.offset;
	}

	template<size_t size>
	uint8_t getControlData(const ByteMap(&map)[size]) const {
		uint8_t dataOut = 0x00;
		for (size_t i = 0; i < size; i++) {
			/* Repeated line from the single-ByteMap function above. Apparently the
				constexpr stuff doesn't like being passed through nested functions. */
			dataOut |= (data.controlData[map[i].index] & map[i].mask) >> map[i].offset;
			//dataOut |= getControlData(map[i]);
		}
		return dataOut;
	}

	boolean getControlBit(const BitMap map) const {
		return !(data.controlData[map.index] & (1 << map.position));  // Inverted logic, '0' is pressed
	}

	void setControlData(uint8_t index, uint8_t val);

private:
	ExtensionData &data;  // I2C and control data storage

	void disconnect();
	boolean controllerIDMatches() const;

	uint8_t requestSize = MinRequestSize;
};

namespace NintendoExtensionCtrl {
	template <class ControllerMap>
	class BuildControllerClass : public ControllerMap {
	public:
		BuildControllerClass(NXC_I2C_TYPE& i2cBus = NXC_I2C_DEFAULT) :
			ControllerMap(portData),
			portData(i2cBus) {}

		using Shared = ControllerMap;  // Make controller class easily accessible

	protected:
		// Included data instance. Contains:
		//    * I2C library object reference
		//    * Connected controller identity (type)
		//    * Control data array
		// This data can be shared between controller instances using a single
		// logical endpoint to keep memory down.
		ExtensionController::ExtensionData portData;
	};
}

// Public-facing version of the extension 'port' class that combines the 
// communication (ExtensionController) with a data instance (ExtensionData), but omits
// any controller-specific data maps.
using ExtensionPort = NintendoExtensionCtrl::BuildControllerClass<ExtensionController>;

#endif
