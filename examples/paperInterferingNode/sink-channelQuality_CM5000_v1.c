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
 *         Example of sink receiving messages from a single node
 * \author
 *         Sergio Diaz
 */

#include "contiki.h"
#include "net/rime/rime.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

#include "node-environment_CM5000.h" // Declares the struct environment

#include "sys/node-id.h" // Include this library in order to be able to set node's ID.

//#include "/home/sink/Desktop/contiki-3.0/dev/cc2420/cc2420.h" // In order to recognize the variable cc2420_last_rssi

//#define IEEE_ADDR_NODE_ID 1

/*---------------------------------------------------------------------------*/
PROCESS(sink_receive, "Sink Receive"); //Through this process the Sink receives messages
AUTOSTART_PROCESSES(&sink_receive); //Start the process when the node boots

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from) //Receives the messages from the nodes and prints them
{

  printf("Data/"); // Print the string "Data"

  struct environment envirRX;
  rtimer_clock_t latency;
  packetbuf_attr_t rss; // Received Signal Strenght Indicator (RSSI)
  packetbuf_attr_t LQI; // Link Quality Indicator 

 //envirRX = packetbuf_dataptr();

//#if TIMESYNCH_CONF_ENABLED
//  latency = timesynch_time() - envirRX->timestamp;
//#else
//  latency = 0;
//#endif

  //printf("Unicast message received from %d.%d, latency %lu ms\n\r",
  //printf("broadcast message received from %d.%d, latency %lu ms, data '%.*s'\n",
  //	 from->u8[0], from->u8[1],
  //	 (1000L * latency) / RTIMER_ARCH_SECOND);



  //static signed char rss_val; // RSSI value capture from register
  //static signed char rss_offset; // Offset de -45 para el RSSI, segun el datasheet del cc2420


  //struct environment envirRX; // Declare the structure envirRX

  packetbuf_copyto(&envirRX ); // Copy the message from the packet buffer to the structure called envirRX

#if TIMESYNCH_CONF_ENABLED
  latency = timesynch_time() - envirRX.timestamp;
#else
  latency = 0;
#endif

  // http://web.stanford.edu/class/cs244e/papers/cc2420.pdf pagina 49 tiene la teoria de RSSI para CC2420
  //rss_val = cc2420_last_rssi;  // Leo el valor del registro
  //rss_offset = -45; // El documento del link dice que se debe sumar un offset de -45
  //rss = rss_val + rss_offset; // Valor real del RSSI en dBm
  rss = (packetbuf_attr(PACKETBUF_ATTR_RSSI)) * (-1); 

  // Link Quality Indicator
  LQI = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
 
  // Nodes Messages
  //printf("Data\t"); // Print the string "Data"
  //printf("%d.%d\t", from->u8[0], from->u8[1] ); // Node ID
  //printf("%d\t", envirRX.sequence);  // Print the sequence number
  //printf("%d\t", envirRX.temp); // Print the temperature value
  //printf("%d\t", envirRX.light);  // Print the light value
  //printf("%d\t", rss);  // Print the Received Signal Strenght Indicator (RSSI) en dBm
  //printf("%d\n", LQI);  // Print the Link Quality Indicator (LQI), es un valor entre 0-255
  

  printf("Data/"); // Print the string "Data"
  printf("%d.%d/", from->u8[0], from->u8[1] ); // Node ID
  printf("%d/", envirRX.sequence);  // Print the sequence number
  printf("%d/", envirRX.temp); // Print the temperature value
  printf("%d/", envirRX.light);  // Print the light value
  printf("%d/", rss);  // Print the Received Signal Strenght Indicator (RSSI) en dBm
  printf("%d/", LQI);  // Print the Link Quality Indicator (LQI), es un valor entre 0-255
  printf("%lu \n\r", (1000L * latency) / RTIMER_ARCH_SECOND );  // Print the Link Quality Indicator (LQI), es un valor entre 0-255



  /*printf("Data-"); // Print the string "Data"
  printf("%d.%d-", from->u8[0], from->u8[1] ); // Node ID
  printf("%d-", envirRX.sequence);  // Print the sequence number
  printf("%d-", envirRX.temp); // Print the temperature value
  printf("%d-", envirRX.light);  // Print the light value
  printf("%d-", rss);  // Print the Received Signal Strenght Indicator (RSSI) en dBm
  printf("%d\n", LQI);  // Print the Link Quality Indicator (LQI), es un valor entre 0-255*/



 /*int txpower;
 txpower = cc2420_get_txpower();
 printf("txpower = %d\n", txpower);  */
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc}; //Every time a packet arrives the function recv_uc is called
static struct unicast_conn uc; // Declares the unicast connection uc

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_receive, ev, data) //Declares the protothread called sink_receive // Listen To nodes
{

  unsigned short id = 1; // This is the ID which will be set in your sky mote

  PROCESS_EXITHANDLER(unicast_close(&uc);) // Specify an action when a process exits
    
  PROCESS_BEGIN(); // Says where the process starts

  //cc2420_set_txpower(31); //Set the output tx power
  unicast_open(&uc, 146, &unicast_callbacks); //Set up an anonymous best-effort broadcast connection 
  node_id_burn(id); // Call this function to burn the defined id
  timesynch_set_authority_level(id);

  while(1) {
    static struct etimer et;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      //PROCESS_WAIT_EVENT(); //Wait for an event to be posted to the process
                            //Wait for the event of receiving a message


  }

  PROCESS_END(); //Says where the process ends
}
/*---------------------------------------------------------------------------*/

