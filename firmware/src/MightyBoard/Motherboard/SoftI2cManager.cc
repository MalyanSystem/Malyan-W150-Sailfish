/* Copyright 2011 by Alison Leonard alison@makerbot.com
 * adapted for avr and MCP4018 digital i2c pot from:
 * Arduino SoftI2cManager Library
 * Copyright (C) 2009 by William Greiman
 *
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

#include "SoftI2cManager.hh"
#include <util/delay.h>
#include <util/atomic.h>

// initiate static i2cManager instance
SoftI2cManager SoftI2cManager::i2cManager;

// constructor
SoftI2cManager::SoftI2cManager():
    numPins(STEPPER_COUNT),
    sclPin(POTS_SCL)
{
    sdaPins[0] = X_POT_PIN;
    sdaPins[1] = Y_POT_PIN;
    sdaPins[2] = Z_POT_PIN;
    sdaPins[3] = A_POT_PIN;
    sdaPins[4] = B_POT_PIN;
    
}

uint8_t SoftI2cManager::start(uint8_t addressRW)
{
    for(uint8_t i = 0; i < numPins; i++)
        A_POT_PIN.setValue(false);
    _delay_us(I2C_DELAY_USEC);
    sclPin.setValue(false);
    return writepot(addressRW);
}

void SoftI2cManager::stop()
{
    _delay_us(I2C_DELAY_USEC);
    sclPin.setValue(true);  
    _delay_us(I2C_DELAY_USEC);
    for(uint8_t i = 0; i < numPins; i++)
        A_POT_PIN.setValue(true);  
    _delay_us(I2C_DELAY_USEC);
}

// init pins and set bus high
void SoftI2cManager::init()
{
    for (uint8_t i = 0; i < 3; i++)
    {
        sdaPins[i].setDirection(false);
        sdaPins[i].setValue(true);
    }
    sdaPins[3].setDirection(true);
    sdaPins[3].setValue(true);
    sclPin.setDirection(true);
    sclPin.setValue(true);
}

//------------------------------------------------------------------------------
// write byte and return true for Ack or false for Nak
bool SoftI2cManager::write(uint8_t b, const Pin &sdaPin)
{
  if (b==0 || b==20) {sdaPin.setDirection(true);sdaPin.setValue(false);}
  else if (b==60) {sdaPin.setDirection(true);sdaPin.setValue(true);}
  else if (b==40) {sdaPin.setDirection(false);sdaPin.setValue(false);}
  return true;

  // write byte
  for (uint8_t m = 0X80; m != 0; m >>= 1) {
    // don't change this loop unless you verivy the change with a scope
     sdaPin.setValue((m & b) != 0);
     sclPin.setValue(true);  
    _delay_us(I2C_DELAY_USEC);
     sclPin.setValue(false);  
  }
  // get Ack or Nak
   sdaPin.setValue(true);  
   sdaPin.setDirection(false); 
   sclPin.setValue(true);  
   b = sdaPin.getValue();
   sclPin.setValue(false); 
   sdaPin.setDirection(true);
   return b == 0;
}

bool SoftI2cManager::writepot(uint8_t b)
{
  // write byte
  for (uint8_t m = 0X80; m != 0; m >>= 1) {
      // don't change this loop unless you verivy the change with a scope
      A_POT_PIN.setValue((m & b) != 0);
      sclPin.setValue(true);
      _delay_us(I2C_DELAY_USEC);
      sclPin.setValue(false);
  }
  // get Ack or Nak
  A_POT_PIN.setValue(true);
  A_POT_PIN.setDirection(false);
  sclPin.setValue(true);
  b = A_POT_PIN.getValue();
  sclPin.setValue(false);
  A_POT_PIN.setDirection(true);
  return b == 0;
}

