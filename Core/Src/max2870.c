#include "max2870.h"
#include "usart.h"
#include <stdio.h>

max2870_regs_t max2870_regs;
double f_rfouta;
double f_pfd;

extern uint8_t tmp[64];

void max2870_delay(uint16_t val)
{
	//HAL_Delay(MAX2870_DELAY_LE);
	for (int j = 0; j < val; j++)
	{
		int k = 1;
	}
}

void max2870_pulse_le()
{
	HAL_GPIO_WritePin(MAX2870_PORT_LE, MAX2870_PIN_LE, GPIO_PIN_SET);
	max2870_delay(50);
	HAL_GPIO_WritePin(MAX2870_PORT_LE, MAX2870_PIN_LE, GPIO_PIN_RESET);
}


void max2870_pulse_slck()
{
	HAL_GPIO_WritePin(MAX2870_PORT_SLCK, MAX2870_PIN_SLCK, GPIO_PIN_SET);
	max2870_delay(1);
	HAL_GPIO_WritePin(MAX2870_PORT_SLCK, MAX2870_PIN_SLCK, GPIO_PIN_RESET);
}

void max2870_set_bit(uint8_t b)
{
	if (b) HAL_GPIO_WritePin(MAX2870_PORT_DATA, MAX2870_PIN_DATA, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(MAX2870_PORT_DATA, MAX2870_PIN_DATA, GPIO_PIN_RESET);
}

void max2870_read_data()
{
	uint32_t tmp = 0;

	//HAL_GPIO_WritePin(MAX2870_PORT_SLCK, MAX2870_PIN_SLCK, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX2870_PORT_LE, MAX2870_PIN_LE, GPIO_PIN_SET);
	//max2870_delay(1);
	//HAL_GPIO_WritePin(MAX2870_PORT_SLCK, MAX2870_PIN_SLCK, GPIO_PIN_RESET);
	//max2870_delay(1);
	max2870_pulse_slck();

	for (int j = 0; j < 32; j++)
	{
		max2870_pulse_slck();
		uint8_t b = HAL_GPIO_ReadPin(MAX2870_PORT_MUX, MAX2870_PIN_MUX);
		tmp |= (b << (31 - j));
	}
	HAL_GPIO_WritePin(MAX2870_PORT_LE, MAX2870_PIN_LE, GPIO_PIN_RESET);

	max2870_regs.reg6.all = tmp;
}

void max2870_write_data(uint8_t init)
{
	uint32_t * ptn = (uint32_t *) &max2870_regs;
	max2870_regs.reg6.all = 0x00000006;
	for (int i = 0; i < 7; i++)
	{
		uint32_t val =  *(ptn + i);
		for (int k = 0; k < 32; k++)
		{
			uint32_t b = (val >> (31 - k)) & 1;

			max2870_set_bit((uint8_t)b);
			max2870_pulse_slck();
		}

		if (i < 6) max2870_pulse_le();

		if ((i == 0) && (init == 1)) HAL_Delay(MAX2870_DELAY_INIT);
	}

	max2870_read_data();
}


void max2870_init(double freq)
{
	HAL_GPIO_WritePin(MAX2870_PORT_LE, MAX2870_PIN_LE, GPIO_PIN_RESET);

	//max2870_regs.reg0.all = 0x007d0000;
	//max2870_regs.reg1.all = 0x2000fff9;
	//max2870_regs.reg2.all = 0x00004042;
	//max2870_regs.reg3.all = 0x0000000b;
	//max2870_regs.reg4.all = 0x6180b23c;
	//max2870_regs.reg5.all = 0x00400005;
	//max2870_regs.reg6.all = 0x00000006;

	max2870_regs.reg0.all = 0x00000000;
	max2870_regs.reg1.all = 0x00000001;
	max2870_regs.reg2.all = 0x00000002;
	max2870_regs.reg3.all = 0x00000003;
	max2870_regs.reg4.all = 0x00000004;
	max2870_regs.reg5.all = 0x00000005;
	max2870_regs.reg6.all = 0x00000006;

	max2870_write_data(1);
	max2870_write_data(0);

	//max2870_regs.reg0.all = 0x003C0000;
	//max2870_regs.reg1.all = 0x20007D01;
	//max2870_regs.reg2.all = 0x01005E42;
	//max2870_regs.reg3.all = 0x00000013;
	//max2870_regs.reg4.all = 0x610F423c;
	//max2870_regs.reg5.all = 0x00400005;
	//max2870_regs.reg6.all = 0x00000006;

	//max2870_regs.reg0.all = 0x007d0000;
	//max2870_regs.reg1.all = 0x2000fff9;
	//max2870_regs.reg2.all = 0x00004042;
	//max2870_regs.reg3.all = 0x0000000b;
	//max2870_regs.reg4.all = 0x6180b23c;
	//max2870_regs.reg5.all = 0x00400005;
	//max2870_regs.reg6.all = 0x00000006;

	max2870_regs.reg2.bits.cp  = 0x0f;
	max2870_regs.reg1.bits.cpl = 1;
	max2870_regs.reg2.bits.ldp = 1;
	max2870_regs.reg2.bits.pdp = 1;
	max2870_regs.reg2.bits.mux = 4; //1-vdd, 2-gnd, 3-rdiv, 4-ndiv,
	max2870_regs.reg5.bits.mux = 1;
	max2870_regs.reg4.bits.fb  = 1;
	max2870_regs.reg5.bits.ld  = 1; //digital lock detection
	max2870_regs.reg1.bits.cpl = 1;

	max2870_set_pfd(36, 1);

	max2870_write_data(0);

}



void max2870_set_pfd(const double ref_in, const uint16_t rdiv) {
  // fPFD = fREF * [(1 + DBR)/(R * (1 + RDIV2))]
  // DBR=0 RDIV2=0
  // fPFD = fREF * [1/R] = fREF / R

  f_pfd = ref_in / rdiv;

  if (f_pfd > 32.0) {
	  max2870_regs.reg2.bits.lds = 1;
  }
  else {
	  max2870_regs.reg2.bits.lds = 0;
  }

  max2870_regs.reg3.bits.cdiv = round(f_pfd * 10);

  max2870_regs.reg2.bits.dbr = 0;
  max2870_regs.reg2.bits.rdiv2 = 0;
  max2870_regs.reg2.bits.r = rdiv;

  uint32_t bs = f_pfd * 20;

  if (bs > 1023) {
    bs = 1023;
  }
  else if (bs < 1) {
    bs = 1;
  }

  max2870_regs.reg4.bits.bs = 0x03FF & bs;
  max2870_regs.reg4.bits.bs2 = 0x03 & (bs >> 8);

}

void max2870_power(uint8_t pwr) {
	max2870_regs.reg2.bits.shdn =  !pwr;
	//max2870_regs.reg4.bits.sdldo = !pwr;
	//max2870_regs.reg4.bits.sddiv = !pwr;
	//max2870_regs.reg4.bits.sdref = !pwr;
	//max2870_regs.reg4.bits.sdvco = !pwr;
	//max2870_regs.reg5.bits.sdpll = !pwr;

}

void max2870_outA(uint8_t pwr) {
	max2870_regs.reg4.bits.rfa_en = (pwr == 0) ? 0 : 1;
	max2870_regs.reg4.bits.apwr = pwr & 3;
}

//****************************************************************************
double max2870_set_rf_out_a(double freq) {
  uint32_t n, frac, m, rf_div = 1, diva = 0;
  double f_vco, fractional = 0;

  f_vco = freq;
  while (f_vco < 3000.0)  {
    rf_div = rf_div << 1;
    f_vco *= 2;
    diva++;
  }
  n = floor(f_vco / f_pfd);

  fractional = f_vco - n;
  //m = 4000;
  m = 4095;
  frac = round(m * fractional);

  max2870_regs.reg0.bits.frac = frac;
  max2870_regs.reg0.bits.n = n;
  max2870_regs.reg1.bits.m = m;
  max2870_regs.reg4.bits.diva = diva;

  f_rfouta = f_pfd * (max2870_regs.reg0.bits.n + 1.0 * max2870_regs.reg0.bits.frac / max2870_regs.reg1.bits.m) / rf_div;
  return f_rfouta;
}

void max2870_dump()
{

	sprintf(tmp,"reg0:%08x\n\r", max2870_regs.reg0.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x frac:%04x n:%04x int:%01x\n\r", max2870_regs.reg0.bits.addr, max2870_regs.reg0.bits.frac, max2870_regs.reg0.bits.n, max2870_regs.reg0.bits.intfrac);
	HAL_UART_Transmit(&huart2, tmp, 32, HAL_MAX_DELAY);

	sprintf(tmp,"reg1:%08x\n\r", max2870_regs.reg1.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x m:%03x p:%04x cpt:%01x cpl:%01x cpoc:%01x\n\r", max2870_regs.reg1.bits.addr, max2870_regs.reg1.bits.m, max2870_regs.reg1.bits.p, max2870_regs.reg1.bits.cpt, max2870_regs.reg1.bits.cpl, max2870_regs.reg1.bits.cpoc);
	HAL_UART_Transmit(&huart2, tmp, 41, HAL_MAX_DELAY);

	sprintf(tmp,"reg2:%08x\n\r", max2870_regs.reg2.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x rst:%01x tri:%01x shdn:%01x pdp:%01x ldp:%01x ldf:%01x cp:%01x\n\r",
			max2870_regs.reg2.bits.addr, max2870_regs.reg2.bits.rst,
			max2870_regs.reg2.bits.tri, max2870_regs.reg2.bits.shdn,
			max2870_regs.reg2.bits.pdp, max2870_regs.reg2.bits.ldp,
			max2870_regs.reg2.bits.ldf, max2870_regs.reg2.bits.cp
			);
	HAL_UART_Transmit(&huart2, tmp, 51, HAL_MAX_DELAY);
	sprintf(tmp,"        reg4db:%01x r:%03x rdiv2:%01x dbr:%01x mux:%01x sdn:%01x lds:%01x\n\r",
			max2870_regs.reg2.bits.reg4db,
			max2870_regs.reg2.bits.r, max2870_regs.reg2.bits.rdiv2, max2870_regs.reg2.bits.dbr,
			max2870_regs.reg2.bits.mux, max2870_regs.reg2.bits.sdn,
			max2870_regs.reg2.bits.lds
			);
	HAL_UART_Transmit(&huart2, tmp, 56, HAL_MAX_DELAY);
	sprintf(tmp,"reg3:%08x\n\r", max2870_regs.reg3.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x cdiv:%03x cdm:%01x retune:%01x vas_shdn:%01x vco:%02x\n\r", max2870_regs.reg3.bits.addr, max2870_regs.reg3.bits.cdiv, max2870_regs.reg3.bits.cdm, max2870_regs.reg3.bits.retune, max2870_regs.reg3.bits.vas_shdn, max2870_regs.reg3.bits.vco);
	HAL_UART_Transmit(&huart2, tmp, 51, HAL_MAX_DELAY);
	sprintf(tmp,"reg4:%08x\n\r", max2870_regs.reg4.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x apwr:%01x rfa_en:%01x bpwr:%01x rfb_en:%01x bdiv:%01x bs:%02x diva:%01x\n\r",
			max2870_regs.reg4.bits.addr, max2870_regs.reg4.bits.apwr,
			max2870_regs.reg4.bits.rfa_en, max2870_regs.reg4.bits.bpwr, max2870_regs.reg4.bits.rfb_en,
			max2870_regs.reg4.bits.bdiv, max2870_regs.reg4.bits.bs,
			max2870_regs.reg4.bits.diva
			);
	HAL_UART_Transmit(&huart2, tmp, 62, HAL_MAX_DELAY);
	sprintf(tmp,"        fb:%01x bs_msb:%01x\n\r",
			max2870_regs.reg4.bits.fb, max2870_regs.reg4.bits.bs2
			);
	HAL_UART_Transmit(&huart2, tmp, 23, HAL_MAX_DELAY);

	sprintf(tmp,"reg5:%08x\n\r", max2870_regs.reg5.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x mux_msb:%01x ld:%01x f01:%01x\n\r", max2870_regs.reg5.bits.addr, max2870_regs.reg5.bits.mux, max2870_regs.reg5.bits.ld, max2870_regs.reg5.bits.f01);
	HAL_UART_Transmit(&huart2, tmp, 30, HAL_MAX_DELAY);
	sprintf(tmp,"reg6:%08x\n\r", max2870_regs.reg6.all);
	HAL_UART_Transmit(&huart2, tmp, 15, HAL_MAX_DELAY);
	sprintf(tmp,"addr:%02x v:%02x adc:%01x por:%01x\n\r", max2870_regs.reg6.bits.addr, max2870_regs.reg6.bits.v, max2870_regs.reg6.bits.adc, max2870_regs.reg6.bits.por);
	HAL_UART_Transmit(&huart2, tmp, 27, HAL_MAX_DELAY);

	sprintf(tmp,"bs:%03x\n\r", (max2870_regs.reg4.bits.bs2 << 3) + max2870_regs.reg4.bits.bs);
	HAL_UART_Transmit(&huart2, tmp, 8, HAL_MAX_DELAY);
	sprintf(tmp,"mux:%01x\n\r", (max2870_regs.reg5.bits.mux << 3) + max2870_regs.reg2.bits.mux);
	HAL_UART_Transmit(&huart2, tmp, 7, HAL_MAX_DELAY);

}
