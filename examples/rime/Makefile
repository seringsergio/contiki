CONTIKI = ../..

all: example-abc example-mesh example-collect example-trickle example-polite node-environment_CM5000_v4 \
     example-rudolph1 example-rudolph2 example-rucb sink-channelQuality_CM5000_v4 \
     example-runicast example-unicast example-neighbors

CONTIKI_WITH_RIME = 1
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include
