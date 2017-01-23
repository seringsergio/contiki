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
 *       Improved example of an interfering node
         The node is in ON state for 2 seconds, and
         the node is in OFF state for 1 second
 * \author
 *        Sergio Diaz
 */


/*INCLUDES*/
#include "contiki.h"
#include "/home/sink/Desktop/contiki/dev/cc2420/cc2420_const.h" // Include the CC2420 constants 
#include "/home/sink/Desktop/contiki/core/dev/spi.h" // Include basic SPI macros
#include "dev/leds.h" // Include Leds to debbug
#include "sys/rtimer.h" //Include the real time library
#include "sys/node-id.h" //Library to include a node id

/*DEFINES*/
#define INTERFERENCE  0x01 // I set the less significant bit to indicate there is interference (0x01 = 00000000 00000001)
#define CONSTANT_MICROS 300 // 300 us was defined by the paper [1]
#define TIME_TICK 31 // The time of 1 tick is 30,51 us in rtimer for sky motes in Contiki v3.0. TIME_TICK = 1 / RTIMER_ARCH_SECOND = 1 / 32768 s

//Show defines values: To USE them put them in the process like this: // SHOW_DEFINE(CLOCK_SECOND); // SHOW_DEFINE(RTIMER_ARCH_SECOND);
#define STR(x) #x
#define SHOW_DEFINE(x) printf("%s=%s\n", #x, STR(x))


/*STRUCT DEFINITIONS*/
struct states{
 unsigned char carrier; // Defines the state of the carrier: Either INTERFERENCE = 0x01 or Not INTERFERENCE = ~0x01
};

struct states state; //Create the state of the carrier
static struct rtimer rtimer; // Create the rtimer variable

/*FUNCTION DEFINITIONS*/

/*---------------------------------------------------------------------------*/
/*
* Generate a random number between 0 and (MaxValue - 1)
*/
unsigned int random_number(unsigned int MaxValue){
       return  rand() % MaxValue;
}
/*---------------------------------------------------------------------------*/
/** 
 * Writes to a register.
 * Note: the SPI_WRITE(0) seems to be needed for getting the
 * write reg working on the Z1 / MSP430X platform
 */
static void
setreg(enum cc2420_register regname, uint16_t value)
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

/*
* Function called by the rtimer to turn the carrier on and off.
*/
static void carrier_On1sOff1s(struct rtimer* timer, void* ptr)
{

    unsigned int R; // Uniformly distributed over [1,100]
    unsigned int Qx;  // Uniformly distributed over [1,x], where x = 50
    unsigned int randNum; // Random number between [1-10]
    uint32_t time_next_period; // Next time period. The duration of the interference or not interference state.
    uint32_t num_ticks; // Number of ticks of the time_next_period

    // Calculate a random number between [1-10]
    //randNum = 1 + abs( random_number(10) ) ;
    
    // Decide whether to produce interference or not with equal probability (0.5)
    if( state.carrier & INTERFERENCE ) // If the random number is less than 5 generate no interference 
    {
        // In this case there will be no interference
        state.carrier &= ~INTERFERENCE; // Set the carrier state to no interference (NOT INTERFERENCE)
    }else{
        // In this case there will be interference
        state.carrier |= INTERFERENCE; // Set the carrier state to interference
    }

    // Turn the modulated carrier on or off depending on the carrier state (state.carrier)
    if( state.carrier & INTERFERENCE) // should the node generate interference?
    {
       //The node must generate interference. Turn the carrier on.
       // Creates an modulated carrier by setting the appropiate registers in the CC2420
       setreg(CC2420_MDMCTRL1, 0x000C);
       strobe(CC2420_STXON);
       // Turn the leds for debug. LEDS_RED on means there is interference
       //leds_on(LEDS_RED);
       //leds_off(LEDS_GREEN);
       rtimer_set(&rtimer, RTIMER_NOW() + (RTIMER_ARCH_SECOND * 1.9) , 1, carrier_On1sOff1s, NULL);// Set the rtimer again to the time_next_period 

    }else{
       //The node must not generate interference. Turn the carrier off.
       //Reset the changes and set back the CC2420 radio chip in normal mode.
       //Not generate modulated carrier
       setreg(CC2420_MANOR, 0x0000);
       setreg(CC2420_TOPTST, 0x0010);
       setreg(CC2420_MDMCTRL1, 0x0500);
       setreg(CC2420_DACTST, 0x0000);
       strobe(CC2420_STXON);
       // Turn the leds for debug. LEDS_GREEN on means there is no interference
       //leds_on(LEDS_GREEN);
       //leds_off(LEDS_RED);
       rtimer_set(&rtimer, RTIMER_NOW() + (RTIMER_ARCH_SECOND * 0.1) , 1, carrier_On1sOff1s, NULL);// Set the rtimer again to the time_next_period
    }
    // Calculate the time of the next period ( time_next_period = R*Q(x)*CONSTANT_MICROS ) 
     /*  R  = 1 +  abs( random_number(100) )  ; // Generate random numbers between [1,100]
       Qx = 1 +  abs(random_number(50))   ; // Generate random numbers between [1,50]
       time_next_period = R * Qx ; // Compute the next time period according to the paper [1]
       time_next_period = time_next_period * CONSTANT_MICROS ;  // Compute the next time period according to the paper [1]
    */


    // Set the rtimer to the time_next_period (num_ticks)
   //  num_ticks = time_next_period / TIME_TICK; // Compute the number of ticks that corresponds to time_next_period
       //printf("Interference = %d ,R = %d, Qx = %d, time = %lu, ticks =  %lu \n", state.carrier & INTERFERENCE, R, Qx, time_next_period, num_ticks); // View the results in console

       //rtimer_set(&rtimer, RTIMER_NOW() + num_ticks , 1, carrier_OnOff, NULL);// Set the rtimer again to the time_next_period (num_ticks)


      /* int txpower;
       txpower = cc2420_get_txpower();
       printf("txpower = %d\n", txpower); */
}


/*---------------------------------------------------------------------------*/

PROCESS(turn_carrier_OnOff, "Turn Carrier On Off"); // Declares the process to turn the carrier on and off
AUTOSTART_PROCESSES( &turn_carrier_OnOff); // Load the process on boot

PROCESS_THREAD(turn_carrier_OnOff, ev, data) // Process to turn carrier on and off
{ 
  unsigned short id = 17; //Select node ID
  PROCESS_BEGIN(); // Says where the process starts
  //cc2420_set_txpower(31); //Set the output tx power
  cc2420_set_txpower(3); //Set the output tx power
  node_id_burn(id); //Burn node id
  //Execute the next real-time task
  rtimer_set(&rtimer, RTIMER_NOW() + RTIMER_ARCH_SECOND, 1, carrier_On1sOff1s, NULL); //Initiates the rtimer 1 second after boot
  //SHOW_DEFINE(CLOCK_SECOND); // Show the value of the CLOCK_SECOND in console. To be aware of the etimer resolution
  //SHOW_DEFINE(RTIMER_ARCH_SECOND); // Show the value of the RTIMER_ARCH_SECOND in console. To be aware of the rtimer resolution

  PROCESS_END();  //Says where the process ends
}

/*---------------------------------------------------------------------------*/

