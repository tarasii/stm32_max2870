/*
 * max2870.h
 *
 *  Created on: Jan 29, 2026
 *      Author: taras
 */

#ifndef INC_MAX2870_H_
#define INC_MAX2870_H_

#include "stm32f1xx_hal.h"
//#include "spi.h"
#include <math.h>

#define MAX2870_PIN_CE GPIO_PIN_1
#define MAX2870_PIN_LE GPIO_PIN_1
#define MAX2870_PORT_LE GPIOB
#define MAX2870_PIN_LD GPIO_PIN_0
#define MAX2870_PORT_LD GPIOB
#define MAX2870_DELAY_LE 1
#define MAX2870_DELAY_INIT 20
#define MAX2870_PIN_SLCK GPIO_PIN_5
#define MAX2870_PORT_SLCK GPIOA
#define MAX2870_PIN_MUX GPIO_PIN_6
#define MAX2870_PORT_MUX GPIOA
#define MAX2870_PIN_DATA GPIO_PIN_7
#define MAX2870_PORT_DATA GPIOA

//MAX2870 Registers
enum Registers_e
{
  REG0          = 0x00,
  REG1          = 0x01,
  REG2          = 0x02,
  REG3          = 0x03,
  REG4          = 0x04,
  REG5          = 0x05,
  REG6          = 0x06
};

//Register 0 bits
typedef union REG0_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField0_s
  {
    uint32_t addr       : 3;
    uint32_t frac       : 12;
    uint32_t n          : 16;
    uint32_t intfrac    : 1;
  } bits;
} REG0_t;

//Register 1 bits
typedef union REG1_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField1_s
  {
    uint32_t addr         : 3;
    uint32_t m            : 12;
    uint32_t p            : 12;
    uint32_t cpt          : 2;
    uint32_t cpl          : 2;
    uint32_t cpoc         : 1;
  } bits;
} REG1_t;

//Register 2 bits
typedef union REG2_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField2_s
  {
    uint32_t addr      : 3;
    uint32_t rst       : 1;
    uint32_t tri       : 1;
    uint32_t shdn      : 1;
    uint32_t pdp       : 1;
    uint32_t ldp       : 1;
    uint32_t ldf       : 1;
    uint32_t cp        : 4;
    uint32_t reg4db    : 1;
    uint32_t r         : 10;
    uint32_t rdiv2     : 1;
    uint32_t dbr       : 1;
    uint32_t mux       : 3;
    uint32_t sdn       : 2;
    uint32_t lds       : 1;
  } bits;
} REG2_t;

//Register 3 bits
typedef union REG3_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField3_s
  {
    uint32_t addr      : 3;
    uint32_t cdiv      : 12;
    uint32_t cdm       : 2;
    uint32_t mutedel   : 1;
    uint32_t csm       : 1;
    uint32_t reserved  : 5;
    uint32_t retune    : 1; //vas_temp
    uint32_t vas_shdn  : 1;
    uint32_t vco       : 6;
  } bits;
} REG3_t;

//Register 4 bits
typedef union REG4_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField4_s
  {
    uint32_t addr       : 3;
    uint32_t apwr       : 2;
    uint32_t rfa_en     : 1;
    uint32_t bpwr       : 2;
    uint32_t rfb_en     : 1;
    uint32_t bdiv       : 1;
    uint32_t mtld       : 1;
    uint32_t sdvco      : 1;
    uint32_t bs         : 8;
    uint32_t diva       : 3;
    uint32_t fb         : 1;
    uint32_t bs2        : 2;
    uint32_t sdref      : 1;
    uint32_t sddiv      : 1;
    uint32_t sdldo      : 1;
    uint32_t reservered1: 3;
  } bits;
} REG4_t;

//Register 5 bits
typedef union REG5_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField5_s
  {
    uint32_t addr      : 3;
    uint32_t adcm      : 3;
    uint32_t adcs      : 1;
    uint32_t reserved1 : 11;
    uint32_t mux       : 1;
    uint32_t reserved2 : 3;
    uint32_t ld        : 2;
    uint32_t f01       : 1;
    uint32_t sdpll     : 1;
    uint32_t reserved3 : 3;
    uint32_t vas_dly   : 2;
    uint32_t reserved4 : 1;

  } bits;
} REG5_t;

//Register 6 bits
typedef union REG6_u
{
  //Access all bits
  uint32_t all;

  //Access individual bits
  struct BitField6_s
  {
    uint32_t addr      : 3;
    uint32_t v         : 6;
    uint32_t vasa      : 1;  //max2871?
    uint32_t reserved1 : 5;  //max2871?
    uint32_t adcv      : 1;  //max2871?
    uint32_t adc       : 7;  //max2871?
    uint32_t por       : 1;  //max2871?
    uint32_t reserved2 : 4;  //max2871?
    uint32_t die       : 4;
  } bits;
} REG6_t;

typedef struct max2870_regs_s {
  REG5_t reg5;
  REG4_t reg4;
  REG3_t reg3;
  REG2_t reg2;
  REG1_t reg1;
  REG0_t reg0;
  REG6_t reg6;
} max2870_regs_t;


void max2870_init(double freq);
void max2870_set_pfd(const double ref_in, const uint16_t rdiv);
void max2870_power(uint8_t pwr);
void max2870_outA(uint8_t pwr);
double max2870_set_rf_out_a(double freq);
void max2870_dump();
void max2870_write_data(uint8_t init);

#endif /* INC_MAX2870_H_ */
