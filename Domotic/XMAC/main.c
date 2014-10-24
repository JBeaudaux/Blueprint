#include <io.h>
#include <signal.h>
#include <stdio.h>

#include "clock.h"
#include "leds.h"
#include "uart0.h"
#include "cc1100.h"
#include "ds2411.h"
#include "mac.h"
#include "timerB.h"

#include "stats.h"
#include "topo_arbre_30.h"

#if 0
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define DEBUG_LEDS 0

#define SINK 1
#define NODE 0
#define ROUTING 0
#define DATA 1

#define DEFAULT_LEVEL 0
#define UNDEF_LEVEL 255
#define NUM_SEQ_MAX 255

//divide by 32.768
#define SEND_DATA_PERIOD 32768  //32000 one second
#define MAX_COUNTER 8          //send data period 8s

#define SM_IDLE         0
#define SM_TX           1
#define SM_LOOP_TX      2
#define SM_RX           3
uint16_t state = SM_IDLE;

uint16_t packet_received(uint8_t packet[], uint16_t length, uint16_t src_addr, int16_t rssi);

uint8_t data_txframe[64];
uint8_t routing_txframe[64];
uint8_t rxframe[64];

uint8_t counter = 0;
uint8_t level = UNDEF_LEVEL;
uint8_t type = NODE;
uint8_t sourceaddr[4];
uint16_t parent_id = 0x0000;
uint8_t sink = DEFAULT_LEVEL;
uint8_t seq = 0;
uint8_t seq_max = 0;
uint8_t idx = 0;
uint16_t txlength = 0;

uint16_t next_send(void);
uint16_t packet_error(uint8_t packet[]);
uint16_t packet_sent(void);
uint16_t char_rx(uint8_t c);
uint16_t inc_clock(void);
uint16_t nodeaddr = 0x0000;
uint16_t delay = 0;
uint16_t global_clock = 0;

int putchar(int c)
{
    return uart0_putchar(c);
}

/******************************************/
/*              CLOCK                     */
/******************************************/
uint16_t inc_clock(void)
{

    //printf("hello\n");

    global_clock++;
    return 0;
}

/******************************************/
/*  WAIT BEFORE SEND THE NEXT PACKET      */
/******************************************/
uint16_t next_send(void)
{
    counter++;


    if (counter >= MAX_COUNTER)
    {
        state = SM_LOOP_TX;
        counter = 0;
        return 1;
    }
    else
    {
        timerB_set_alarm_from_now(TIMERB_ALARM_CCR5, SEND_DATA_PERIOD, 0);
        timerB_register_cb(TIMERB_ALARM_CCR5, next_send);
        return 0;
    }
    return 0;
}


/******************************************/
/*              XMAC                      */
/******************************************/
int main(void)
{
    WDTCTL = WDTPW+WDTHOLD;

    set_mcu_speed_xt2_mclk_8MHz_smclk_1MHz();
    set_aclk_div(1);

    LEDS_INIT();
    LEDS_OFF();


    ds2411_init();
    nodeaddr = (((uint16_t)ds2411_id.serial1)<<8) + (ds2411_id.serial0);

    uart0_init(UART0_CONFIG_1MHZ_115200);
    uart0_register_callback(char_rx);
    eint();

    printf("[APP];BOOTING;%.4x\n",nodeaddr);


    //check if this node is the sink
    if (nodeaddr == sink_nodes)
    {
        type = SINK;
        level = DEFAULT_LEVEL;
    }
    else
    {
        //retrieve father
        for (idx=0; idx<NUMBER_NODES; idx++)
        {
            if (list_nodes[idx] == nodeaddr)
            {
                if(father_nodes1[idx] != 0x0000)
                {
                    parent_id = father_nodes1[idx];
                    level = 12;
                    break;
                }
            }
        }
    }
    
    //hack for mobile
    /*if(nodeaddr == 0x1f5d)
    {
        parent_id = 0x0000;
        mac_set_mobile(1);
        level = 12;
    }*/

    mac_init(10);
    mac_set_rx_cb(packet_received);
    mac_set_error_cb(packet_error);
    mac_set_sent_cb(packet_sent);

    timerB_set_alarm_from_now(TIMERB_ALARM_CCR6, 32768, 32768);
    timerB_register_cb(TIMERB_ALARM_CCR6, inc_clock);

    while (1)
    {
        LPM1;

        if (state == SM_TX)
        {
            if (level != UNDEF_LEVEL && type != SINK)
            {
                seq_max = NUM_SEQ_MAX;
                delay = rand();
                delay &= 0xCFFF;
                delay += 12000; //(369ms < delay < 1991ms)
                timerB_set_alarm_from_now(TIMERB_ALARM_CCR5, delay, 0);
                timerB_register_cb(TIMERB_ALARM_CCR5, next_send);
            }
            else
            {
                printf("[APP];NOROUTE\n");
            }

            state = SM_IDLE;
        }
        else if (state == SM_LOOP_TX)
        {
            if (level != UNDEF_LEVEL)
            {
                sprintf(sourceaddr,"%.4x",nodeaddr);
                data_txframe[0] = DATA;
                data_txframe[1] = level-1;
                data_txframe[2] = sourceaddr[0];
                data_txframe[3] = sourceaddr[1];
                data_txframe[4] = sourceaddr[2];
                data_txframe[5] = sourceaddr[3];
                data_txframe[6] = seq;	//sequence
                data_txframe[7] = 1;    //hops
                txlength = 8;

                stat_add(STAT_APP_TX);
                printf("[APP];NODE_TX;%.4x;%.4x;%u;%u-%u\n", nodeaddr, parent_id, seq, global_clock, timerB_time()/32);

                seq++;

                mac_send(data_txframe, txlength, parent_id);

                if (DEBUG_LEDS == 1)
                {
                    LED_GREEN_ON();
                }

                if (seq < seq_max)
                {
                    timerB_set_alarm_from_now(TIMERB_ALARM_CCR5, SEND_DATA_PERIOD, 0);
                    timerB_register_cb(TIMERB_ALARM_CCR5, next_send);
                }
            }
            state = SM_IDLE;
        }
     
    }

    return 0;
}


/******************************************/
/*              RECEIVED CALLBACK         */
/******************************************/
uint16_t packet_received(uint8_t packet[], uint16_t length, uint16_t src_addr, int16_t rssi)
{
    if (packet[0] == DATA)
    {
        if (type == SINK)
        {
            stat_add(STAT_APP_RX);
            printf("[APP];SINK_RX;%c%c%c%c;%.4x;%u;%u\n", packet[2], packet[3], packet[4], packet[5],src_addr, packet[6],packet[7]);
            return 0;
        }
        else
        {
            routing_txframe[0] = DATA;
            routing_txframe[1] = level-1; //node level
            routing_txframe[2] = packet[2]; //node id
            routing_txframe[3] = packet[3];
            routing_txframe[4] = packet[4];
            routing_txframe[5] = packet[5];
            routing_txframe[6] = packet[6];	// num sequence
            routing_txframe[7] = packet[7] + 1; // nb hops
            txlength = 8;

            printf("[ROUTING];NODE_FORWARD;%.4x;%c%c%c%c;%u;%u;%u-%u\n", nodeaddr, routing_txframe[2], routing_txframe[3], routing_txframe[4], routing_txframe[5], routing_txframe[6],routing_txframe[7],global_clock, timerB_time()/32);
            stat_add(STAT_FORWARD_CNT);

            mac_send(routing_txframe, txlength, parent_id);
        }
    }
    return 0;
}

/******************************************/
/*              SEND CALLBACK             */
/******************************************/
uint16_t packet_sent(void)
{
    printf("[APP];SEND_PACKET_DONE;%u-%u\n",global_clock, timerB_time()/32);
    return 1;
}

/******************************************/
/*              ERROR CALLBACK            */
/******************************************/
uint16_t packet_error(uint8_t packet[])
{
    printf("[APP];SEND_PACKET_ERROR;%u-%u\n",global_clock, timerB_time()/32);
    return 1;
}

/******************************************/
/*              PORT SERIE                */
/******************************************/
uint16_t char_rx(uint8_t c)
{
    switch (c)
    {
        case 'm':
            state = SM_TX;
            break;
        case 'h':   //stop sending
            seq_max = 0;
            break;
        case 'f':   //stop queue pop
            stop = 1;
            break;                          
        case 's':
            stats_print(0);
            break;
        case 'z':
            stats_clear();
            break;
    }

    return 1;
}

