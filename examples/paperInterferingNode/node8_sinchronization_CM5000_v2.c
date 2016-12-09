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
#include "sys/rtimer.h" //Include the rtimer library
#include "lib/random.h"

#define INTERVAL CLOCK_SECOND * 5

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
struct delay_estructure delay;
struct etimer led_event_timer;
struct rtimer Mi_rtimer2;
struct rtimer Mi_rtimer1;
/*---------------------------------------------------------------------------*/
static void toggle_my_led (struct rtimer* rt, void* ptr);
static void iniciar_Mi_rtimer2 (struct rtimer* rt, void* ptr);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{

  printf("unicast message received from %d.%d \n\r",from->u8[0], from->u8[1]);
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{

  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  printf("unicast message sent to %d.%d: status %d num_tx %d\n\r",
    dest->u8[0], dest->u8[1], status, num_tx);
}
/*---------------------------------------------------------------------------*/
static void iniciar_Mi_rtimer2 (struct rtimer* rt, void* ptr)
{
  
  rtimer_set(&Mi_rtimer2, RTIMER_NOW() + (RTIMER_ARCH_SECOND*0.5) , 1, toggle_my_led , NULL); //Initiates the rtimer 1 second after boot

}
/*---------------------------------------------------------------------------*/
static void toggle_my_led (struct rtimer* rt, void* ptr)
{


   //etimer_set(&led_event_timer, (CLOCK_SECOND * 0.5) );

    leds_toggle(LEDS_GREEN);
    rtimer_set(&Mi_rtimer1, RTIMER_NOW()  , 1, iniciar_Mi_rtimer2 , NULL); //Initiates the rtimer 1 second after boot
   //printf("etimer_set\n\r");
   //leds_blink();
   //leds_toggle(LEDS_GREEN);
   //printf("leds_toggle\n\r");
   //rtimer_set(&My_rtimer, timesynch_time() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
   //rtimer_set(&My_rtimer, timesynch_time() + (RTIMER_ARCH_SECOND * 0.1) , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
   //rtimer_set(rt, RTIMER_NOW() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
   //rtimer_set(rt, RTIMER_TIME(rt) + RTIMER_ARCH_SECOND , 1, toggle_my_led, ptr); //Initiates the rtimer 1 second after boot
   //rtimer_set(rt, timesynch_time_to_rtimer(timesynch_time()) + (RTIMER_ARCH_SECOND * 1) , 1, toggle_my_led, ptr); //Initiates the rtimer 1 second after boot
}


/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{

  static struct etimer sendtimer, intervaltimer, inicio;

  linkaddr_t addr;
  static clock_time_t interval; //typedef unsigned short clock_time_t

    unsigned short id = 8; // This is the ID which will be set in your sky mote
  //unsigned short id = 16; // This is the ID which will be set in your sky mote

  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);
  node_id_burn(id); // Call this function to burn the defined id

  interval = INTERVAL;
  timesynch_set_authority_level(id);

  unsigned short band = 0;

  etimer_set(&inicio, CLOCK_SECOND * 10);

  PROCESS_WAIT_UNTIL(etimer_expired(&inicio));

  printf("inicio\n\r");
  rtimer_set(&Mi_rtimer1, timesynch_time()  , 1, iniciar_Mi_rtimer2 , NULL); //Initiates the rtimer 1 second after boot

  while(1) {

    //rtimer_set(&rtimer, timesynch_time() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
    //if(band == 0){
     //rtimer_set(&rtimer, timesynch_time() + RTIMER_ARCH_SECOND , 1, toggle_my_led, NULL); //Initiates the rtimer 1 second after boot
     //band = 1;
    //}
    etimer_set(&intervaltimer, interval);
    etimer_set(&sendtimer, random_rand() % interval);
    
    PROCESS_WAIT_UNTIL(etimer_expired(&sendtimer));

    packetbuf_copyfrom(  &delay , sizeof(delay) );

    //pruebas - typedef unsigned short rtimer_clock_t
    /*rtimer_clock_t ref      = 2 ;
    rtimer_clock_t not_ref  = 8 ;
    rtimer_clock_t offset = ref - not_ref;
    rtimer_clock_t tiempo_global = not_ref + offset;
    printf("offset =  %u\n\r", offset);
    printf("tiempo_global =  %u\n\r", tiempo_global);*/

    addr.u8[0] = 16;
    //addr.u8[0] = 8;
    addr.u8[1] = 0;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }

    PROCESS_WAIT_UNTIL(etimer_expired(&intervaltimer));

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/



