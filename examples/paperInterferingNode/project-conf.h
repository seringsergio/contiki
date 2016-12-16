#ifndef __PROJECT_CONF_H__
#define __PROJECT_CONF_H__


//Define the network stack
//#define NETSTACK_CONF_NETWORK rime_driver // Define the network driver to use
//#define NETSTACK_CONF_MAC     csma_driver // Define the MAC driver to use
//#define NETSTACK_CONF_RDC     contikimac_driver // Define the RDC driver to use
#define NETSTACK_CONF_RDC     nullrdc_driver // Define the RDC driver to use. Para el nodo interferencia uso este
#define NETSTACK_CONF_MAC     nullmac_driver // Define the MAC driver to use. Para el nodo interferencia uso este
//#define NETSTACK_CONF_FRAMER  framer_802154 // Define the framer driver to use
//#define NETSTACK_CONF_FRAMER  framer_nullmac // Define the framer driver to use
//#define NETSTACK_CONF_RADIO   cc2420_driver // Define the radio driver to use. Only for the CM5000, NOT for the re-mote

//#undef TIMESYNCH_CONF_ENABLED  // Para habilitar la sincronizacion entre nodos
//#define TIMESYNCH_CONF_ENABLED 1 // Para habilitar la sincronizacion entre nodos

//Checkear si hay alguna TX en el canal 128 veces por segundo
//#undef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 128

//Define the channel to be used
#define RF_CHANNEL 20

#endif /* __PROJECT_CONF_H__ */




