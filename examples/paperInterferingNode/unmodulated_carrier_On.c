/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
 
/**
 * \file
 *        Example of an interfering node
 * \author
 *        Sergio Diaz
 */
 
#include "contiki.h"
 
#include "/home/sink/Desktop/contiki-3.0/dev/cc2420/cc2420.h" // Include the CC2420 library
#include "/home/sink/Desktop/contiki-3.0/dev/cc2420/cc2420_const.h" // Include the CC2420 constants 
#include "/home/sink/Desktop/contiki-3.0/core/dev/spi.h" // Include basic SPI macros
 
/*---------------------------------------------------------------------------*/
PROCESS(unmodulated_carrier, "CC2420 Unmodulated Carrier"); // Declares the process unmodulated_carrier
AUTOSTART_PROCESSES(&unmodulated_carrier); // Load the process on boot
 
/*---------------------------------------------------------------------------*/
/** 
 * Writes to a register.
 * Note: the SPI_WRITE(0) seems to be needed for getting the
 * write reg working on the Z1 / MSP430X platform
 */
static void
setreg(enum cc2420_register regname, uint16_t value) // Set the register of the cc2420 radio
{
  CC2420_SPI_ENABLE();
  SPI_WRITE_FAST(regname);
  SPI_WRITE_FAST((uint8_t) (value >> 8));
  SPI_WRITE_FAST((uint8_t) (value & 0xff));
  SPI_WAITFORTx_ENDED();
  SPI_WRITE(0);
  CC2420_SPI_DISABLE();
}
 
/* Sends a strobe */
static void
strobe(enum cc2420_register regname)
{
  CC2420_SPI_ENABLE();
  SPI_WRITE(regname);
  CC2420_SPI_DISABLE();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unmodulated_carrier, ev, data) // Defines the process unmodulated_carrier
{
 
  PROCESS_BEGIN();  // Says where the process starts
 
  // Creates an unmodulated carrier
  setreg(CC2420_MANOR, 0x0100);
  setreg(CC2420_TOPTST, 0x0004);
  setreg(CC2420_MDMCTRL1, 0x0508);
  setreg(CC2420_DACTST, 0x1800);
  strobe(CC2420_STXON); 
 
  PROCESS_END();  //Says where the process ends
}
/*---------------------------------------------------------------------------*/
