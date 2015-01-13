/*
 * Copyright 2010 by Adam Mayer	 <adam@makerbot.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "Eeprom.hh"
#include "EepromMap.hh"
#include "Configuration.hh"
#include "SoftI2cManager.hh"
#include "DigiPots.hh"
#include "StepperAxis.hh"

DigiPots::DigiPots(const Pin& pot,
                                   const uint16_t &eeprom_base_in) :
    
    pot_pin(pot),
    eeprom_base(eeprom_base_in) {
}

void DigiPots::init(const uint8_t idx) {
	
	eeprom_pot_offset = idx;
    resetPots();
 
}

void DigiPots::resetPots()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
	    SoftI2cManager i2cPots = SoftI2cManager::getI2cManager();
#pragma GCC diagnostic pop

    potValue = eeprom::getEeprom8(eeprom_base + eeprom_pot_offset, 0);
    if (eeprom_pot_offset<=2) setPotValue(100);
    else
    {
        if (eeprom_pot_offset == 4) return;	//ignore B-axis
	/*i2cPots.start(0b01011110 | I2C_WRITE);
	potValue = eeprom::getEeprom8(eeprom_base + eeprom_pot_offset, 0);
	i2cPots.writepot(potValue);
	i2cPots.stop();*/
	setPotValue(127);
    }
}

void DigiPots::setPotValue(const uint8_t val)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
    SoftI2cManager i2cPots = SoftI2cManager::getI2cManager();
#pragma GCC diagnostic pop

	uint8_t current;
	if ( eeprom_pot_offset == X_AXIS && potValue >=100) 
        {
                current=eeprom::getEeprom8(eeprom_offsets::STEPPER_X_CURRENT, 0);
                if (current==2) potValue=60;
                else if (current==1) potValue=40;
                else potValue=20;
        }
	else if ( eeprom_pot_offset == Y_AXIS && potValue >=100) 
        {
                current=eeprom::getEeprom8(eeprom_offsets::STEPPER_Y_CURRENT, 0);
                if (current==2) potValue=60;
                else if (current==1) potValue=40;
                else potValue=20;
        }
	else if ( eeprom_pot_offset == Z_AXIS && potValue >=100) 
        {
                current=eeprom::getEeprom8(eeprom_offsets::STEPPER_Z_CURRENT, 0);
                if (current==2) potValue=60;
                else if (current==1) potValue=40;
                else potValue=20;
        }
	//if ( eeprom_pot_offset == Z_AXIS ) potValue = val > DIGI_POT_MAX_Z ? DIGI_POT_MAX_Z : val;
	//else                             potValue = val > DIGI_POT_MAX_XYAB ? DIGI_POT_MAX_XYAB : val;
	//i2cPots.write(potValue, pot_pin);
	
	if ( eeprom_pot_offset == A_AXIS) 
        {
		current=eeprom::getEeprom8(eeprom_offsets::STEPPER_A_CURRENT, 0);
		i2cPots.start(0b01011110 | I2C_WRITE);
		if (val > 120)
		{
			potValue = eeprom::getEeprom8(eeprom_base + eeprom_pot_offset, 0);
			if (potValue == 127) potValue=76;
		}
		else if (val < 40) potValue = 40;
		else potValue = val;
		potValue = potValue + current * 5 - 10;
		i2cPots.writepot(potValue);
		i2cPots.stop();
                /*current=eeprom::getEeprom8(eeprom_offsets::STEPPER_A_CURRENT, 0);
                if (current==2) potValue=60;
                else if (current==1) potValue=40;
                else potValue=20;*/
        }
	else if (eeprom_pot_offset == B_AXIS) {}
	else i2cPots.write(potValue, pot_pin);
}

/// returns the last pot value set
uint8_t DigiPots::getPotValue() {
    return potValue;
}
