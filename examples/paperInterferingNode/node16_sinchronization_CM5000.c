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
 *         Best-effort single-hop unicast example
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>

//My Includes
#include "node-sinchronization_CM5000.h"
#include "sys/node-id.h" // Include this library in order to be able to set node's ID.
#include "dev/leds.h" // Include Leds to debbug


#define INTERVAL CLOCK_SECOND * 5
/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
//PROCESS(synchronization_toggle_led, "synchronization_toggle_led");
//AUTOSTART_PROCESSES( &synchronization_toggle_led);
AUTOSTART_PROCESSES(&example_unicast_process);
//AUTOSTART_PROCESSES(&example_unicast_process, &synchronization_toggle_led);
/*---------------------------------------------------------------------------*/
struct delay_estructure delay;
int sequence = 0;
static struct rtimer rtimer; // Create the rtimer variable
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{

  packetbuf_attr_t timestamp; // To save the timestamp of the sensor node

  timestamp = packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP); // To get the timestamp of the packet

  struct delay_estructure delay_rx;
  
  packetbuf_copyto( &delay_rx ); // Copy the message from the packet buffer to the structure called envirRX

  rtimer_clock_t current_Time;                // rtimer_clock_t  = unsigned short

  current_Time = timesynch_time();
  //printf("current_Time = %u\n\r", current_Time);


  printf("unicast message received from %d.%d \n\r",from->u8[0], from->u8[1]);
  /*printf("unicast message received from %d.%d Sequence = %d TIME_RX = %u TIME_TX = %u TIMESTAMP = %d \n\r",
	 from->u8[0], from->u8[1], delay_rx.sequence , current_Time, delay_rx.timeTX, timestamp);*/
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{

  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  /*printf("unicast message sent to %d.%d: status %d num_tx %d\n",
    dest->u8[0], dest->u8[1], status, num_tx);*/
}
/*---------------------------------------------------------------------------*/
static void toggle_my_led (struct rtimer* timer, void* ptr)
{

   leds_toggle(LEDS_GREEN);
   rtimer_set(&rtimer, timesynch_time() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot time_next_period
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{

  rtimer_clock_t offset;

  rtimer_clock_t timesynched_time;                // rtimer_clock_t  = unsigned short
  
  static struct etimer sendtimer, intervaltimer;

  linkaddr_t addr;
  static clock_time_t interval; //typedef unsigned short clock_time_t

  int authority_level_read;

  //unsigned short id = 8; // This is the ID which will be set in your sky mote
  unsigned short id = 16; // This is the ID which will be set in your sky mote

  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);
  node_id_burn(id); // Call this function to burn the defined id

  interval = INTERVAL;
  timesynch_set_authority_level(id);

  //rtimer_set(&rtimer, RTIMER_NOW() + timesynch_offset() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
  unsigned short band = 0;
  while(1) {

    if(band == 0){
     rtimer_set(&rtimer, timesynch_time() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
     band = 1;
    }
    etimer_set(&intervaltimer, interval);
    etimer_set(&sendtimer, random_rand() % interval);
    
    
    PROCESS_WAIT_UNTIL(etimer_expired(&sendtimer));

    sequence++;

    //timesynch_set_authority_level((linkaddr_node_addr.u8[0] << 4) + 16);
    //printf("authority_level_write = %d\n\r", (linkaddr_node_addr.u8[0] << 4) + 16);

    authority_level_read = timesynch_authority_level();
    //printf("authority_level_read = %d\n\r", authority_level_read);

    //printf("contiki-sky-main = %d\n\r",   (linkaddr_node_addr.u8[0] << 4) + 16  );

    offset = timesynch_offset(); // Get the current time-synchronized offset from the rtimer clock
    //printf("offset = %08x\n\r", offset);

    timesynched_time = timesynch_time(); //Get the current time-synchronized time
    //printf("timesynched_time = %08x  RTIMER_NOW() = %08x  RTIMER_NOW() + offset = %08x  \n\r", timesynch_time(), RTIMER_NOW() , RTIMER_NOW() + offset);

    delay.timeTX = timesynched_time;
    delay.sequence = sequence;

    //packetbuf_copyfrom("Hello", 5);
    packetbuf_copyfrom(  &delay , sizeof(delay) );     

    //addr.u8[0] = 16;
    addr.u8[0] = 8;
    addr.u8[1] = 0;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
      //printf("unicast message sent to %d.%d: Sequence = %d current_Time = %u authority_level = %d \n", addr.u8[0], addr.u8[1], sequence, current_Time, authority_level_read);
    }

    PROCESS_WAIT_UNTIL(etimer_expired(&intervaltimer));

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/



/*PROCESS_THREAD(synchronization_toggle_led, ev, data) // Process to turn carrier on and off
{ 

  PROCESS_BEGIN(); // Says where the process starts

  //Execute the next real-time task
  rtimer_set(&rtimer, RTIMER_NOW() + timesynch_offset() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
  //SHOW_DEFINE(CLOCK_SECOND); // Show the value of the CLOCK_SECOND in console. To be aware of the etimer resolution
  //SHOW_DEFINE(RTIMER_ARCH_SECOND); // Show the value of the RTIMER_ARCH_SECOND in console. To be aware of the rtimer resolution

  PROCESS_END();  //Says where the process ends
}*/

