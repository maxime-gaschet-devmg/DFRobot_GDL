/*
 * maincpp.h
 *
 *  Created on: Sep 27, 2023
 *      Author: emix0
 */

#ifndef ADD_DEVMG_MAINCPP_H_
#define ADD_DEVMG_MAINCPP_H_

#include "DFRobot_GDL.h"

namespace myNameSpace
{

class DFRobot_ILI9341_240x320_HW_SPI: public DFRobot_GDL{
public:
//  DFRobot_ILI9341_240x320_HW_SPI(uint8_t dc, uint8_t cs = GDL_PIN_NC, uint8_t rst = GDL_PIN_NC, uint8_t bl = GDL_PIN_NC);
  DFRobot_ILI9341_240x320_HW_SPI(int){};
  ~DFRobot_ILI9341_240x320_HW_SPI(){};
//  void begin(uint32_t freq = 0);
//  void begin(devInterfaceInit fun, uint32_t freq = 0);
//  void setDisplayArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
//  void pushColor(uint8_t *color,uint32_t len);
};

}

#endif /* ADD_DEVMG_MAINCPP_H_ */
