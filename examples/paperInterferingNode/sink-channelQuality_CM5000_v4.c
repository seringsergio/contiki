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
#include "sys/node-id.h" // Include this library in order to be able to set node's ID.
#include "/home/sink/Desktop/contiki/dev/cc2420/cc2420.h" // Include the CC2420 library
#include "node-environment_CM5000.h"  // Declares the struct environment
/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{

  struct environment envirRX;
  rtimer_clock_t latency;
  packetbuf_attr_t rss; // Received Signal Strenght Indicator (RSSI)
  packetbuf_attr_t LQI; // Link Quality Indicator 

  packetbuf_copyto(&envirRX ); // Copy the message from the packet buffer to the structure called envirRX

#if TIMESYNCH_CONF_ENABLED
  latency = timesynch_time() - envirRX.timestamp;
#else
  latency = 0;
#endif

  rss = (packetbuf_attr(PACKETBUF_ATTR_RSSI)) * (-1); 
  LQI = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);

  printf("unicast message received/"); // Print the string "Data"
  printf("%d.%d/", from->u8[0], from->u8[1] ); // Node ID
  printf("%d/", envirRX.sequence);  // Print the sequence number
  printf("%d/", envirRX.temp); // Print the temperature value
  printf("%d/", envirRX.light);  // Print the light value
  printf("%d/", rss);  // Print the Received Signal Strenght Indicator (RSSI) en dBm
  printf("%d/", LQI);  // Print the Link Quality Indicator (LQI), es un valor entre 0-255
  printf("%lu \n\r", (1000L * latency) / RTIMER_ARCH_SECOND );  // Print the Link Quality Indicator (LQI), es un valor entre 0-255

//  printf("unicast message received from %d.%d\n\r",
//	 from->u8[0], from->u8[1]);
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
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{

  unsigned short id = 1; // This is the ID which will be set in your sky mote
  //unsigned short id = 2; // This is the ID which will be set in your sky mote

  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  node_id_burn(id); // Call this function to burn the defined id
  timesynch_set_authority_level(id);
  unicast_open(&uc, 146, &unicast_callbacks);
  cc2420_set_txpower(3); //Min value. Set the output power of the node
  
  while(1) {
    static struct etimer et;
    linkaddr_t addr;
        
    /* Delay 2-4 seconds */
    //etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
    etimer_set(&et, CLOCK_SECOND * 1);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("Hello", 5);
    addr.u8[0] = 2;
    //addr.u8[0] = 1;
    addr.u8[1] = 0;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      //unicast_send(&uc, &addr);
    }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
