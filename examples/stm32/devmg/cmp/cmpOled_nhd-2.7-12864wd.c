//******************************************************************************
//* Project :  ceva_oled_imvac_touchgfx
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

//******************************************************************************
// Driver / Service:
// Use:
//******************************************************************************

// INCLUDES ********************************************************************

#include "cmpOled_nhd-2.7-12864wd.h"

#include "gpio.h"
#include "spi.h"

#include "drvTime.h"

// DEFINES *********************************************************************

// Redefinition of Arduino pin states.
#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

// Redefinition of Arduino IO.
#define OLED_DC    4  // Register Select signal
#define OLED_RESET 3  // Reset signal
#define OLED_CS    5  // Chip select signal
#define OLED_CLK   7  // Serial clock signal //PB1 on Arduino Mega
#define OLED_MOSI  6  // Serial data signal  //PB2 on Arduino Mega

// Wiring:
// OLED   Color    Sig  Arduino Stm32
// 1  -   Black  - GND
// 2  -   Red    - VCC
// 4  -   Orange - D/C   - D9  - PC7
// 7  -   Yellow - DCK   - D13 - PA5
// 8  -   Brown  - MOSI  - D11 - PA7
// 16 -   Green  - RES   - D8  - PA9
// 17 -   White  - CS    - D10 - PB6

#define command command_spi
#define data data_spi

#define DELAY_US (0) // Was 100.

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

unsigned char NHD_Logo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xD9, 0x83, 0xC6, 0x31, 0x98, 0xC1, 0x82, 0x0C, 0x3C, 0x63, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0xD9, 0x87, 0xC6, 0x33, 0x98, 0xC1, 0x83, 0x0C, 0x7C, 0x73, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x1D, 0x86,
    0x03, 0x33, 0x18, 0xC3, 0xC3, 0x18, 0xC0, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x7F, 0xDD, 0x8E, 0xE3, 0x7B, 0x1F,
    0xC3, 0x61, 0xB8, 0xFC, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xDF, 0x8F, 0xC1, 0xDF, 0x1F, 0xC6, 0x61, 0xB1,
    0xFC, 0x6F, 0x00, 0x00, 0x00, 0x3C, 0x01, 0xC0, 0x1B, 0x84, 0x01, 0xCE, 0x18, 0xC7, 0xE0, 0xF0, 0xC0, 0x67, 0x00,
    0x00, 0x00, 0x7E, 0x07, 0x9F, 0xDB, 0x87, 0xE1, 0xCE, 0x18, 0xCE, 0x70, 0xE0, 0x7C, 0x67, 0x00, 0x00, 0x00, 0x7F,
    0xFE, 0x3F, 0xD9, 0x83, 0xE0, 0xCC, 0x18, 0xCC, 0x38, 0x60, 0x3C, 0x63, 0x00, 0x00, 0x00, 0xFF, 0xFC, 0xE0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x3C, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1F, 0x80, 0x0E, 0x00, 0x3F, 0x0C, 0x3F, 0x8F, 0xC3, 0x00, 0xC3, 0x18, 0x00, 0x00, 0x00, 0x00, 0x3F,
    0xFF, 0xFC, 0x00, 0x3F, 0x8C, 0x3F, 0x8C, 0xE3, 0x01, 0xC3, 0x98, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xF8, 0x00,
    0x31, 0xCC, 0x30, 0x0C, 0x63, 0x01, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x30, 0xCC, 0x3F,
    0x0E, 0xC3, 0x03, 0x20, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0xCC, 0x03, 0x8F, 0xC3, 0x03,
    0xF0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x8C, 0x3F, 0x8C, 0x03, 0x07, 0xF0, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x8C, 0x3F, 0x0C, 0x03, 0xE6, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x83, 0x41, 0xE0, 0x70, 0x78, 0x24, 0x0C,
    0x0F, 0x06, 0x0F, 0x06, 0x81, 0x80, 0x80, 0x00, 0x01, 0x83, 0xC0, 0xC0, 0xE0, 0x58, 0x34, 0x0C, 0x06, 0x06, 0x09,
    0x07, 0x81, 0x80, 0x80, 0x00, 0x01, 0x82, 0xC0, 0xC0, 0xC0, 0x70, 0x3C, 0x1E, 0x06, 0x06, 0x09, 0x05, 0x83, 0xC0,
    0x80, 0x00, 0x01, 0x82, 0x40, 0xC0, 0x70, 0x58, 0x2C, 0x12, 0x06, 0x06, 0x0F, 0x04, 0x82, 0x40, 0xE0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// FUNCTIONS *******************************************************************

// Private
// - Test functions.
void OLED_Init_12864();
void FillPixel_12864();
void ImageDisplay_12864(unsigned char *image);
void ClearPixel_12864();
// - LL.
void data_bb(unsigned char d);
void data_spi(unsigned char d);
void command_bb(unsigned char d);
void command_spi(unsigned char d);
// - Arduino wrapper.
void digitalWrite(uint8_t pin, GPIO_PinState st);
void delay(int delay);
void delayMicroseconds(int delay);

//******************************************************************************
// Name: cmpOled_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void cmpOled_init(void)
{
  digitalWrite(OLED_RESET, LOW);
  delay(150);
  digitalWrite(OLED_RESET, HIGH);
  delay(150);

  OLED_Init_12864(); // Initialization Sequence

  delay(10);

  ClearPixel_12864();
//  Display_Mode_12864(0xA7); // Inverse color.
}

//******************************************************************************
// Name: cmpOled_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void cmpOled_taskTest(void)
{
  FillPixel_12864();
  delay(1000);
  ClearPixel_12864();
  delay(250);

  ImageDisplay_12864(NHD_Logo);
  delay(2000);
  command(0xA7);
  delay(2000);
  command(0xA6);
  ClearPixel_12864();
  delay(250);
}

void cmpOled_setDisplayWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  uint8_t row_start, row_end;
  uint8_t col_start, col_end;
  col_start = x / 2 + 28;
  col_end = col_start + width / 2 - 1;
  row_start = y;
  row_end =  y + height - 1;
  command(0x75); // Set Row Address
  data(row_start);
  data(row_end);
  command(0x15); // Set Column Address
  data(col_start);
  data(col_end);
  command(0x5C); // Enable MCU to Write DATA RAM
}

// Length must be a multiple of 2.
void cmpOled_writeData(uint8_t *pixels, uint8_t length) {
  while(length > 0)
  {
    // Second nibble is a multiplier so let's take full range.
    data(*pixels++ | 0x0F);
    delayMicroseconds(DELAY_US);
    data(*pixels++ | 0x0F);
    delayMicroseconds(DELAY_US);
    length -= 2;
  }
}


// 8 pixels per byte.
//  Byte are arranged as follow:
//  pix: 0 1 2 3 4 5 6 7 8 9...
//  bit: 7 6 5 4 3 2 1 0 7 6...
void cmpOled_writeData_1bpp(uint8_t *pixels, uint8_t length)
{
  while(length > 0) // Byte loop.
  {
    static uint8_t mask;
    mask = 0x80;
    while((length > 0) && (mask != 0x00)) // Bit loop (8 bits per byte).
    {
      static uint8_t bytes[2];
      if(*pixels & mask) {
        bytes[0] = 0xFF;
      }
      else {
        bytes[0] = 0x00;
      }
      mask >>= 1;
      if(*pixels & mask) {
        bytes[1] = 0xFF;
      }
      else {
        bytes[1] = 0x00;
      }
      mask >>= 1;
      data(bytes[0]);
      data(bytes[1]);
      length -= 2;
    }
    pixels++;
  }
}

void cmpOled_writeData_4bpp(uint8_t *pixels, uint8_t length)
{
  while(length > 0)
  {
    uint8_t nib1 = (*pixels & 0x0F);
    uint8_t nib2 = (*pixels & 0xF0);
    data((nib1 << 4) | nib1);
    delayMicroseconds(DELAY_US);
    data(nib2 | (nib2 >> 4));
    delayMicroseconds(DELAY_US);
    pixels++;
    length -= 2;
  }
}

void cmpOled_writeData_16bpp(uint16_t *pixels, uint8_t length) {
  while(length > 0)
  {
    data(*pixels >> 8);
    pixels++;
    delayMicroseconds(DELAY_US);
    data(*pixels >> 8);
    pixels++;
    delayMicroseconds(DELAY_US);
    length -= 2;
  }
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: ()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void Set_Column_Address_12864(unsigned char a, unsigned char b)
{
  command(0x15); // Set Column Address
  data(a);       //   Default => 0x00
  data(b);       //   Default => 0x77
}
//--------------------------------------------------------------------------

void Set_Write_RAM_12864()
{
  command(0x5C); // Enable MCU to Write DATA RAM
}
//--------------------------------------------------------------------------

void Set_Read_RAM_12864()
{
  command(0x5D); // Enable MCU to Write DATA RAM
}
//--------------------------------------------------------------------------

void Set_Row_Address_12864(unsigned char a, unsigned char b)
{
  command(0x75); // Set Row Start and End Address
  data(a);       // Default => 0x00
  data(b);       // Default => 0x7F
}
//--------------------------------------------------------------------------

void Set_Remap_12864(unsigned char a, unsigned char b)
{
  command(0xA0); // Set Remap
  data(a);       // Default => 0x00
  data(b);       // Default => 0x00
}
//--------------------------------------------------------------------------

void Set_Display_Start_Line_12864(unsigned char a)
{
  command(0xA1); // Set Display RAM Display Start Line
  data(a);       // Default => 0x00
}
//--------------------------------------------------------------------------

void Set_Display_Offset_12864(unsigned char a)
{
  command(0xA2); // Set Verticle Shift
  data(a);       // Default => 0x00
}
//--------------------------------------------------------------------------

void Display_Mode_12864(unsigned char a)
{
  command(a); // 0xA4 => Entire Display OFF
              // 0xA5 => Entire Display ON, all pixels Grayscale level 15
              // 0xA6 => Normal Display (Default)
              // 0xA7 => Inverse Display
}
//--------------------------------------------------------------------------

void Enable_Partial_Display_12864(unsigned char a, unsigned char b)
{
  command(0xA8); // Turns ON partial mode
  data(a);       // Address of start row
  data(b);       // Address of end row.
}
//--------------------------------------------------------------------------

void Exit_Partial_Display_12864()
{
  command(0xA9); // Exit Partial Display Mode
}
//--------------------------------------------------------------------------

void Function_Selection_12864(unsigned char a)
{
  command(0xAB); // Function Selection
  data(a);       // Default => 0x01
}
//--------------------------------------------------------------------------

void Set_Sleep_Mode_12864(unsigned char a)
{
  command(a); // 0xAE => Sleep Mode ON
              // 0xAF => Sleep Mode OFF
}
//--------------------------------------------------------------------------

void Set_Phase_Length_12864(unsigned char a)
{
  command(0xB1); // Set Phase Length
  data(a);       // Default => 0x97
}
//--------------------------------------------------------------------------

void Set_Display_Clock__Oscillator_Frequency_12864(unsigned char a)
{
  command(0xB3); // Set Osc Frequency
  data(a);       //
}
//--------------------------------------------------------------------------

void Enable_External_VSL(unsigned char a, unsigned char b)
{
  command(0xB4); // Enable External VSL
  data(a);       //
  data(b);       //
}
//--------------------------------------------------------------------------

void Set_GPIO_12864(unsigned char a)
{
  command(0xB5); // Set GPIO
  data(a);       //
}
//--------------------------------------------------------------------------

void Set_Second_Precharge_Period_12864(unsigned char a)
{
  command(0xB6); // Set Second Precharge Period
  data(a);       //
}
//--------------------------------------------------------------------------

void Default_Grayscale_Command_12864()
{
  command(0xB9); // Sets Default Grayscale
}
//--------------------------------------------------------------------------

void Set_Precharge_Voltage_12864(unsigned char a)
{
  command(0xBB); // Set Precharge Voltage Level
  data(a);       // Default => 0x17
}
//--------------------------------------------------------------------------

void Set_VCOMH_Voltage_12864(unsigned char a)
{
  command(0xBE); // Set Second Precharge Period
  data(a);       // Default => 0x04
}
//--------------------------------------------------------------------------

void Set_Contrast_Control_12864(unsigned char a)
{
  command(0xC1); // Set Contrast Control Level
  data(a);       // Default => 0x7F
}
//--------------------------------------------------------------------------

void Master_Contrast_Control_12864(unsigned char a)
{
  command(0xC7); // Master Contrast Control
  data(a);       // Default => 0x0F
}
//--------------------------------------------------------------------------

void Set_Multiplex_Ratio_12864(unsigned char a)
{
  command(0xCA); // Set MUX Ratio
  data(a);       // Default => 0x7F
}
//--------------------------------------------------------------------------

void Display_Enhancement_12864(unsigned char a)
{
  command(0xD1); // Display Enhancement
  data(a);       //
  data(0x20);    //
}
//--------------------------------------------------------------------------

void Set_Command_Lock_12864(unsigned char a)
{
  command(0xFD); // Set Lock/Unlock Commands
  data(a);       // Default => 0x12
}
//--------------------------------------------------------------------------


/****************************************************
 *           Initialization For controller
 *****************************************************/
void OLED_Init_12864()
{
  Set_Sleep_Mode_12864(0xAF);                          // Display OFF
  Set_Command_Lock_12864(0x12);                        // Unlock commands for OLED
  Set_Display_Clock__Oscillator_Frequency_12864(0x91); // Set Display Clock / Oscillator Frequency
  Set_Multiplex_Ratio_12864(0x3F);                     // Set Multiplex Ratio
  Set_Display_Offset_12864(0x00);                      // Set Display Offset
  Function_Selection_12864(0x01);                      // Function Selection
  Set_Display_Start_Line_12864(0x00);                  // Set Display Start Line
  Set_Remap_12864(0x16, 0x11);                         // Set Remap. (A[1]) & (A[4]) can be adjusted to flip display orientation
  Master_Contrast_Control_12864(0x0F);                 // Master Contrast Control
  Set_Contrast_Control_12864(0x9F);                    // Set Contrast Control
  Set_Phase_Length_12864(0x72);                        // Set Phase Length
  Set_Precharge_Voltage_12864(0x1F);                   // Set Precharge Voltage
  Enable_External_VSL(0xA0, 0xFD);                     // Enable External VSL
  Set_VCOMH_Voltage_12864(0x04);                       // Set VCOMH Voltage
  Display_Mode_12864(0xA6);                            // Set Normal Display
  Exit_Partial_Display_12864();                        // Exit Partial Display Mode

  Display_Enhancement_12864(0xA2);         // Display Enhancement
  Set_GPIO_12864(0x00);                    // Set GPIO
  Default_Grayscale_Command_12864();       // Default Linear Grayscale Table
  Set_Second_Precharge_Period_12864(0x08); // Set Second Precharge Period

  Set_Sleep_Mode_12864(0xAF); // Display ON
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void FillPixel_12864()
{
  unsigned int i, j;

  Set_Column_Address_12864(0x1C, 0x5B); // 1C & 5B = DISPLAY Start & End address.
  Set_Row_Address_12864(0x00, 0x3F);
  Set_Write_RAM_12864();

  for (i = 0; i < 32; i++) // Columns
  {
    for (j = 0; j < 64; j++) // Rows
    {
      data(0xFF);
      delayMicroseconds(DELAY_US);
      data(0x00);
      delayMicroseconds(DELAY_US);
    }
    for (j = 0; j < 64; j++) // Rows
    {
      data(0x00);
      delayMicroseconds(DELAY_US);
      data(0xFF);
      delayMicroseconds(DELAY_US);
    }
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Image (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  4 bits (1 Nibble) = 1 Pixel
//  Driver IC Mapping = 256x64. Each Pixel written horizontally must be
//  elongated by 2 from 128x64 BMP Image.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void ImageDisplay_12864(unsigned char *image)
{
  unsigned int i, j, buff;

  // Set_Remap_12864(0x04,0x11);   //To flip 180 degrees
  Set_Column_Address_12864(0x1C, 0x5B); // 28 is left-most column, 91 is right-most column
  Set_Row_Address_12864(0x00, 0x3F);
  Set_Write_RAM_12864();

  for (i = 0; i < 64; i++) // Columns
  {
    for (j = 0; j < 16; j++) // Rows
    {
      buff = *image;
      buff = ((buff >> 6) & 0x03);
      if (buff == 0x03)
      {
        data(0xFF);
        data(0xFF);
      }
      else if (buff == 0x02)
      {
        data(0xFF);
        data(0x00);
      }
      else if (buff == 0x01)
      {
        data(0x00);
        data(0xFF);
      }
      else
      {
        data(0x00);
        data(0x00);
      }

      buff = *image;
      buff = ((buff >> 4) & 0x03);
      if (buff == 0x03)
      {
        data(0xFF);
        data(0xFF);
      }
      else if (buff == 0x02)
      {
        data(0xFF);
        data(0x00);
      }
      else if (buff == 0x01)
      {
        data(0x00);
        data(0xFF);
      }
      else
      {
        data(0x00);
        data(0x00);
      }

      buff = *image;
      buff = ((buff >> 2) & 0x03);
      if (buff == 0x03)
      {
        data(0xFF);
        data(0xFF);
      }
      else if (buff == 0x02)
      {
        data(0xFF);
        data(0x00);
      }
      else if (buff == 0x01)
      {
        data(0x00);
        data(0xFF);
      }
      else
      {
        data(0x00);
        data(0x00);
      }

      buff = *image;
      buff = (buff & 0x03);
      if (buff == 0x03)
      {
        data(0xFF);
        data(0xFF);
      }
      else if (buff == 0x02)
      {
        data(0xFF);
        data(0x00);
      }
      else if (buff == 0x01)
      {
        data(0x00);
        data(0xFF);
      }
      else
      {
        data(0x00);
        data(0x00);
      }

      image++;
    }
  }
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Blank (Clear Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void ClearPixel_12864()
{
  unsigned int i, j;

  Set_Column_Address_12864(0x1C, 0x5B);
  Set_Row_Address_12864(0x00, 0x3F);
  Set_Write_RAM_12864();

  for (i = 0; i < 64; i++) // Columns
  {
    for (j = 0; j < 128; j++) // Rows
    {
      data(0x00);
      delayMicroseconds(DELAY_US);
    }
  }
}

// Data Output Serial Interface
void data_bb(unsigned char d)
{
  unsigned int n;
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_DC, HIGH);
  for (n = 0; n < 8; n++)
  {
    if ((d & 0x80) == 0x80)
    {
      digitalWrite(OLED_MOSI, HIGH);
    } else {
      digitalWrite(OLED_MOSI, LOW);
    }
    __asm__("nop");
    d = (d << 1);
    digitalWrite(OLED_CLK, LOW);
    __asm__("nop");
    digitalWrite(OLED_CLK, HIGH);
    __asm__("nop");
    digitalWrite(OLED_CLK, LOW);
  }
  digitalWrite(OLED_CS, HIGH);
}

void data_spi(unsigned char d)
{
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_DC, HIGH);
  HAL_SPI_Transmit(&hspi1, &d, 1, 20);
  digitalWrite(OLED_CS, HIGH);
}

// Command Output Serial Interface
void command_bb(unsigned char d)
{
  unsigned int n;
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_DC, LOW);
  for (n = 0; n < 8; n++)
  {
    if ((d & 0x80) == 0x80)
    {
      digitalWrite(OLED_MOSI, HIGH);
    } else {
      digitalWrite(OLED_MOSI, LOW);
    }
    __asm__("nop");
    d = (d << 1);
    digitalWrite(OLED_CLK, LOW);
    __asm__("nop");
    digitalWrite(OLED_CLK, HIGH);
    __asm__("nop");
    digitalWrite(OLED_CLK, LOW);
  }
  digitalWrite(OLED_CS, HIGH);
}

void command_spi(unsigned char d)
{
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_DC, LOW);
  HAL_SPI_Transmit(&hspi1, &d, 1, 20);
  digitalWrite(OLED_CS, HIGH);
}

void digitalWrite(uint8_t pin, GPIO_PinState st)
{
  switch(pin)
  {
    default:
    case OLED_DC:
//      HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, st);
      HAL_GPIO_WritePin(D9_GPIO_Port, D9_Pin, st);
      break;
    case OLED_RESET:
//      HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, st);
      HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, st);
      break;
    case OLED_CS:
//      HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, st);
      HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, st);
      break;
    case OLED_CLK:
//      HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, st);
      HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, st);
      break;
    case OLED_MOSI:
//      HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, st);
      HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, st);
      break;
  }
}

void delay(int delay) {
  drvTime_wait_ms(delay);
}

void delayMicroseconds(int delay) {
  drvTime_wait_us(delay);
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
