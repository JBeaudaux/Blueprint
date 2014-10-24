#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include "mac.h"
#include "cc1100.h"
#include "ds2411.h"
#include "timerB.h"
#include "leds.h"
#include "conf.h"

#include "stats.h"

#define PACKET_LENGTH_MAX 54

#define HEADER_LENGTH   0x5

#define STATE_WOR       0x0
#define STATE_RX        0x1
#define STATE_TX        0x2
#define STATE_DELAY     0x4

#define TYPE_PREAMBLE 	0x1
#define TYPE_ACK      	0x2
#define TYPE_DATA     	0x3
#define TYPE_DATAACK  	0x4

#define MAX_DELAY_COUNT 0x5     //0x5   max retry

#define OPT 1

// timer alarms
#define ALARM_PREAMBLE TIMERB_ALARM_CCR0
#define ALARM_TIMEOUT TIMERB_ALARM_CCR1
#define ALARM_RETRY TIMERB_ALARM_CCR2
#define ALARM_STAYRX TIMERB_ALARM_CCR3
#define ALARM_POP TIMERB_ALARM_CCR4

// timing (divide by 32.768)
#define SEND_PERIOD             320     //time between preamble 9.7ms
#define MAX_PREAMBLE_COUNT      15      //preamble lenght 9.7*15 = 140ms
#define ACK_TIMEOUT             360     //DACK timeout 11ms
#define CHECK_CHANNEL           655     //listen medium before send a preamble 20ms
#define DATA_TIMEOUT            655     //20ms
#define POP                     360     //11ms
#define STAY_RX                 655    //20ms

// WoR
// EVENT0*2^(5*WOR_RES) * (750/(27*10^6))  = 117ms
//  SLEEP 101ms
//  RX    15ms
#define RX_TIME     		0
#define WOR_RES     		0
#define EVENT0      		4208
#define EVENT1     		 	4

#define DELAY_MIN           327        //10ms
#define RANDOM_DELAY        0x5FF      //46ms (between 10 and 56ms)

#define DELAY_MIN_BK          200         //5ms
#define RANDOM_DELAY_BK       0x00FF      //7,8ms (between 5 and 13ms)

#define HEAR_PREAMBLE 	10		//optimization
#define HEAR_DACK 		20

//-----------------------------
#if 0
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#if 1
#define DEBUG2(...) printf(__VA_ARGS__)
#else
#define DEBUG2(...)
#endif

#if 1
#define DEBUG3(...) printf(__VA_ARGS__)
#else
#define DEBUG3(...)
#endif
//-----------------------------


// lenght of the message queue
#define QUEUE_LEN 8
#define QUEUE_ENABLE

typedef struct {
    uint8_t length;
    uint8_t type;
    uint8_t dst_addr[2];
    uint8_t src_addr[2];
    uint8_t payload[PACKET_LENGTH_MAX];
} frame_t;

typedef struct {
    uint8_t length;
    uint8_t type;
    uint8_t dst_addr[2];
    uint8_t src_addr[2];
} preamble_t;

volatile uint8_t is_mobile = 0;


// node's MAC address
uint16_t node_addr = 0x0000;
uint8_t stop = 0;

// incremental variables
volatile static uint8_t i1;
volatile static uint8_t i2;

volatile static uint8_t is_busy = 0;
volatile static uint8_t is_backoff = 0;

volatile static uint8_t count_as_retry = 0;

volatile static uint8_t when_stay = 0;

volatile static uint8_t short_send_backoff = 0;

volatile static uint8_t kready = 1;

// callback for received packets
static mac_received_t received_cb;
static mac_sent_t sent_cb;
static mac_error_t error_cb;

static uint16_t delay;

// frames
static frame_t frame, txframe;

// internal state
volatile static uint16_t state;
volatile static uint16_t frame_to_send = 0;

volatile static uint16_t waiting_frame = 0;

// count
volatile static uint16_t delay_count;
volatile static uint16_t preamble_count;

#ifdef QUEUE_ENABLE
static uint8_t mess_in_queue = 0;
static frame_t mess_queue[QUEUE_LEN];
#endif

volatile uint8_t short_send = 0;

// prototypes
static uint16_t set_wor(void);
static uint16_t read_frame(void);
static uint16_t pack_sent(void);
static uint16_t try_send(void);
static uint16_t medium_clear(void);
static uint16_t medium_busy(void);
static uint16_t send_preamble(void);
static uint16_t read_pack(void);
static uint16_t send_data(void);
static uint16_t send_done(void);
static uint16_t read_dataack(void);
static uint16_t dack_timeout(void);
static uint16_t delay_send(void);
static uint16_t init_short_send_mode(uint16_t ss_timeout);
static uint16_t pop(void);

void mac_init(uint8_t channel) {
    // initialize the unique serial number chip and set node address accordingly
    ds2411_init();
    node_addr = (((uint16_t)ds2411_id.serial1)<<8) + (ds2411_id.serial0);

    // seed the random number generator
    srand(node_addr);

    // reset callbacks
    received_cb = 0x0;
    sent_cb = 0x0;
    error_cb = 0x0;

    // initialize the timerB
    timerB_init();
    timerB_start_ACLK_div(TIMERB_DIV_1);

    // configure the radio
    cc1100_init();
    cc1100_cmd_idle();


    //value from SmartRF
    /*	for (i1 = 0; i1<47; i1++) {
    		cc1100_write_reg(i1,CC1100_CONFIG_REGISTERS[i1]);
    	}

    	cc1100_cfg_append_status(CC1100_APPEND_STATUS_ENABLE);
        cc1100_cfg_crc_autoflush(CC1100_CRC_AUTOFLUSH_DISABLE);
    	cc1100_cfg_crc_en(CC1100_CRC_CALCULATION_ENABLE);
        cc1100_cfg_fs_autocal(CC1100_AUTOCAL_IDLE_TO_TX_RX);
    	cc1100_cfg_sync_mode(CC1100_SYNCMODE_30_32_CS);
    */

    /* configure the radio behaviour */
    cc1100_cfg_append_status(CC1100_APPEND_STATUS_ENABLE);
    cc1100_cfg_crc_autoflush(CC1100_CRC_AUTOFLUSH_DISABLE);
    cc1100_cfg_white_data(CC1100_DATA_WHITENING_ENABLE);
    cc1100_cfg_crc_en(CC1100_CRC_CALCULATION_ENABLE);
    cc1100_cfg_freq_if(0x08);
    cc1100_cfg_fs_autocal(CC1100_AUTOCAL_IDLE_TO_TX_RX);
    cc1100_cfg_mod_format(CC1100_MODULATION_GFSK);
    cc1100_cfg_sync_mode(CC1100_SYNCMODE_30_32_CS);
    cc1100_cfg_manchester_en(CC1100_MANCHESTER_DISABLE);

    // freq = 867.999985MHz
    cc1100_write_reg(CC1100_REG_FREQ2, 0x20);
    cc1100_write_reg(CC1100_REG_FREQ1, 0x25);
    cc1100_write_reg(CC1100_REG_FREQ0, 0xED);

    //packet length to 61
    cc1100_write_reg(CC1100_REG_PKTLEN, 0x3D);

    cc1100_write_reg(CC1100_REG_AGCCTRL2, 0x47);

    // configure the radio channel (199.813843kHz)
    cc1100_cfg_chanspc_e(0x2);
    cc1100_cfg_chanspc_m(0xE5);

    // 32.959kHz
    cc1100_write_reg(CC1100_REG_DEVIATN, 0x42);

    // rise CCA threshold
    //cc1100_cfg_carrier_sense_abs_thr(5);

    // set channel bandwidth (241.071kHz)
    cc1100_cfg_chanbw_e(1);
    cc1100_cfg_chanbw_m(3);

    // set data rate (76.836kBaud)
    cc1100_cfg_drate_e(0xB);
    cc1100_cfg_drate_m(0x75);

    // go to IDLE after RX and TX
    cc1100_cfg_rxoff_mode(CC1100_RXOFF_MODE_IDLE);
    cc1100_cfg_txoff_mode(CC1100_TXOFF_MODE_IDLE);

    // configure WOR
    cc1100_cfg_rx_time(RX_TIME);
    cc1100_cfg_wor_res(WOR_RES);
    cc1100_cfg_event0(EVENT0);
    cc1100_cfg_event1(EVENT1);
    cc1100_cfg_rc_pd(CC1100_RC_OSC_ENABLE);

    //see cc1101 datasheet table 39
    uint8_t table[1];
    table[0] = 0x03;      // -30dBm   12.1mA
    //table[0] = 0x17;      // -20dBm   12.7mA
    //table[0] = 0x1D;      // -15dBm   13.4mA
    //table[0] = 0x27;        // -10dBm   15.0mA
    //table[0] = 0x67;      // -5dBm    //VALUE NOT IN THE TABLE!
    //table[0] = 0x50;      // 0dBm     16.9mA
    //table[0] = 0x81;      // +5dBm    21.0mA
    //table[0] = 0xCB;      // +7dBm    26.8mA
    //table[0] = 0xC2;      // +10dBm   32.4mA
    cc1100_cfg_patable(table, 1);
    cc1100_cfg_pa_power(0);

    // set IDLE state, flush everything, and start rx
    cc1100_cmd_idle();
    cc1100_cmd_flush_rx();
    cc1100_cmd_flush_tx();
    cc1100_cmd_calibrate();

    // configure irq
    cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
    cc1100_gdo0_int_set_falling_edge();
    cc1100_gdo0_int_clear();
    cc1100_gdo0_int_enable();

    // start the machine
    set_wor();
}

void mac_set_rx_cb(mac_received_t cb) {
    received_cb = cb;
}

void mac_set_sent_cb(mac_sent_t cb) {
    sent_cb = cb;
}

void mac_set_error_cb(mac_error_t cb) {
    error_cb = cb;
}


uint16_t mac_send(uint8_t packet[], uint16_t length, uint16_t dst_addr) {
    DEBUG("[xmac] STAT_MAC_TX\n");
    stat_add(STAT_MAC_TX);


    // check length
    if (length>PACKET_LENGTH_MAX) {
        stat_add(STAT_ERROR_PACKET_TOO_LONG);
        DEBUG("[xmac] STAT_ERROR_PACKET_TOO_LONG\n");
        return 2;
    }

    // check state
    if (mess_in_queue >= QUEUE_LEN) {
        // there is already a frame about to be sent
        DEBUG2("[QUEUE];FULL_QUEUE\n");
        stat_add(STAT_FULL_QUEUE);
        return 1;
    } else if (frame_to_send == 1 || state != STATE_WOR ) {
        if(kready == 1)
        {
            kready = 0;
            DEBUG2("[QUEUE];PUSH;%u\n",mess_in_queue);
            stat_add(STAT_ADD_QUEUE);
            mess_queue[mess_in_queue].length = length + HEADER_LENGTH;
            mess_queue[mess_in_queue].type = TYPE_DATA;
            mess_queue[mess_in_queue].dst_addr[0] = dst_addr>>8;
            mess_queue[mess_in_queue].dst_addr[1] = dst_addr & 0xFF;
            mess_queue[mess_in_queue].src_addr[0] = node_addr>>8;
            mess_queue[mess_in_queue].src_addr[1] = node_addr & 0xFF;

            // copy payload
            for (i1=0; i1<length; i1++) mess_queue[mess_in_queue].payload[i1] = packet[i1];

            mess_in_queue += 1;
            kready = 1;
        }
        else
        {
            DEBUG3("+++++++PUSH NOT READY\n");
        }
        return 0;
    }

    // prepare header
    txframe.length = length + HEADER_LENGTH;
    txframe.type = TYPE_DATA;
    txframe.dst_addr[0] = dst_addr>>8;
    txframe.dst_addr[1] = dst_addr & 0xFF;
    txframe.src_addr[0] = node_addr>>8;
    txframe.src_addr[1] = node_addr & 0xFF;

    // copy payload
    for (i1=0; i1<length; i1++) txframe.payload[i1] = packet[i1];

    // update frame to send flag
    frame_to_send = 1;
    delay_count = 0;
    preamble_count = 0;
    short_send = 0;

    // call try_send to start TX procedure
    delay = rand();
    delay &= RANDOM_DELAY;
    delay += DELAY_MIN;
    timerB_set_alarm_from_now(ALARM_RETRY, delay, 0);
    timerB_register_cb(ALARM_RETRY, try_send);

    return 0;
}

static uint16_t set_wor(void) {
    cc1100_cmd_idle();
    cc1100_cfg_txoff_mode(CC1100_TXOFF_MODE_IDLE);
    cc1100_cfg_rxoff_mode(CC1100_RXOFF_MODE_IDLE);
    cc1100_cfg_cca_mode(CC1100_CCA_MODE_RSSI_PKT_RX);
    cc1100_cfg_rc_pd(CC1100_RC_OSC_ENABLE);
    cc1100_cfg_fs_autocal(CC1100_AUTOCAL_IDLE_TO_TX_RX);

    cc1100_cmd_flush_rx();

    cc1100_gdo0_register_callback(read_frame);
    cc1100_gdo0_int_clear();

    cc1100_cmd_wor();

    state = STATE_WOR;

    when_stay = 0;

    timerB_set_alarm_from_now(ALARM_POP, POP, 0);
    timerB_register_cb(ALARM_POP, pop);


    return 0;
}

static uint16_t pop(void) {
    if (frame_to_send == 0) {
        if (mess_in_queue > 0) {
            
            if(kready == 1)
            {
                kready = 0;
                cc1100_cmd_idle();
                
                //Send packet in queue
                mess_in_queue -= 1;

                // update frame to send flag
                frame_to_send = 1;
                delay_count = 0;
                preamble_count = 0;

                DEBUG2("[QUEUE];POP;%u\n",mess_in_queue);

                txframe.length = mess_queue[mess_in_queue].length;
                txframe.type = mess_queue[mess_in_queue].type;
                txframe.dst_addr[0] = mess_queue[mess_in_queue].dst_addr[0];
                txframe.dst_addr[1] = mess_queue[mess_in_queue].dst_addr[1];
                txframe.src_addr[0] = mess_queue[mess_in_queue].src_addr[0];
                txframe.src_addr[1] = mess_queue[mess_in_queue].src_addr[1];

                // copy payload
                for (i2=0; i2<mess_queue[mess_in_queue].length - HEADER_LENGTH; i2++) txframe.payload[i2] =  mess_queue[mess_in_queue].payload[i2];

                /*delay = rand();
                delay &= RANDOM_DELAY;
                delay += DELAY_MIN;
                //DEBUG2("POP DELAY %u\n",delay);
                timerB_set_alarm_from_now(ALARM_RETRY, delay, 0);
                timerB_register_cb(ALARM_RETRY, try_send);*/
                timerB_set_alarm_from_now(ALARM_RETRY, 10, 0);
                timerB_register_cb(ALARM_RETRY, try_send);
                kready = 1;
            }
            else
            {
                DEBUG3("+++++++POP NOT READY\n");
            }
        }
    }
    return 0;
}


static uint16_t stay_rx(void) {
    DEBUG("::::STAY IN RX\n");
    when_stay = 1;


    frame_to_send = 0;
    timerB_set_alarm_from_now(ALARM_STAYRX, STAY_RX, 0);
    timerB_register_cb(ALARM_STAYRX, set_wor);

    // set RX
    cc1100_cmd_idle();
    cc1100_cmd_flush_rx();
    cc1100_gdo0_int_disable();
    cc1100_gdo0_int_set_falling_edge();
    cc1100_gdo0_int_clear();
    cc1100_gdo0_int_enable();
    cc1100_gdo0_register_callback(read_frame);
    cc1100_cmd_rx();

    // change GDO to EoP, and its callback
   /* cc1100_gdo0_int_disable();
    cc1100_gdo0_int_set_falling_edge();
    cc1100_gdo0_int_clear();
    cc1100_gdo0_int_enable();
    cc1100_gdo0_register_callback(read_frame);
*/


    return 0;
}

static uint16_t try_send(void) {
    if (stop == 1) {
        return 0;
    }

    DEBUG("[xmac] STAT_MAC_TRY_TX\n");
    stat_add(STAT_MAC_TRY_TX);

    if (state == STATE_WOR || state == STATE_DELAY || short_send > 0) {
        state = STATE_TX;

        // start RX
        cc1100_cmd_idle();
        cc1100_cmd_rx();

        // set a timer to check if the channel is free, the time to have a valid RSSI

        if (short_send == HEAR_PREAMBLE) {
            DEBUG3(":::WAITING FOR DACK -> TIMEOUT\n");
            short_send = 0;
            //count_as_retry = 1; //TODO: ???
            preamble_count = 0;
            return delay_send();
        }
        else if (short_send == HEAR_DACK) {
            DEBUG2("::::BACKOFF %u\n",short_send_backoff);
            timerB_set_alarm_from_now(ALARM_PREAMBLE, short_send_backoff, 0);
        } else {
            timerB_set_alarm_from_now(ALARM_PREAMBLE, CHECK_CHANNEL, 0);
        }

        //is_backoff = 0;
        timerB_register_cb(ALARM_PREAMBLE, medium_clear);
        cc1100_cfg_rc_pd(CC1100_RC_OSC_DISABLE);

        // change GDO to SyncWord, and its callback
        cc1100_gdo0_int_disable();
        cc1100_gdo0_int_set_falling_edge();
        cc1100_gdo0_int_clear();
        cc1100_gdo0_int_enable();
        cc1100_gdo0_register_callback(medium_busy);
    } else {
        if (cc1100_status_marcstate() == 0x01) {
            // this is an error situation
            set_wor();
            DEBUG("[xmac] try_send, bad state for sending (%u)\n", state);
        }
        stat_add(STAT_MAC_RETRY_NOT_READY);
        return delay_send();
    }
    return 0;
}

static uint16_t delay_send(void) {
    if (delay_count >= MAX_DELAY_COUNT) {
        if (error_cb) {
            error_cb();
        }

        DEBUG("[xmac] STAT_PKT_DROP_RETRY\n");
        stat_add(STAT_PKT_DROP_RETRY);

        frame_to_send = 0;
        delay_count = 0;
        preamble_count = 0;
        
        set_wor();

    } else {
        //retransmit the frame * time
        DEBUG("[xmac] STAT_MAC_RETRY\n");
        stat_add(STAT_MAC_RETRY);

        if (short_send == HEAR_DACK) { //quick delay to insert our data asap
            init_short_send_mode(ACK_TIMEOUT);
            return 0;
        }

        if (count_as_retry == 1) {
            delay_count++;
            preamble_count = 0;
            count_as_retry = 0;
        }

        state = STATE_DELAY;

        delay = rand();
        delay &= RANDOM_DELAY;
        delay += DELAY_MIN;
        delay = delay * (delay_count + 1);
        //DEBUG2("CONGESTION DELAY %u\n",delay);

        timerB_set_alarm_from_now(ALARM_PREAMBLE, delay, 0);
        timerB_register_cb(ALARM_PREAMBLE, try_send);

        // change GDO to SyncWord, and its callback
        cc1100_gdo0_int_disable();
        cc1100_gdo0_int_set_falling_edge();
        cc1100_gdo0_int_clear();
        cc1100_gdo0_int_enable();
        cc1100_gdo0_register_callback(medium_busy);

        cc1100_cmd_idle();
        cc1100_cmd_rx();
    }
    return 0;
}

static uint16_t medium_clear(void) {
    // the timer elapsed, thus the channel is clear
    cc1100_cmd_idle();
    cc1100_cfg_fs_autocal(CC1100_AUTOCAL_NEVER);
    cc1100_cfg_txoff_mode(CC1100_TXOFF_MODE_RX);
    cc1100_cfg_cca_mode(CC1100_CCA_MODE_ALWAYS);

    // change GDO to EoP, and its callback
    cc1100_gdo0_int_disable();
    cc1100_gdo0_int_set_falling_edge();
    cc1100_gdo0_int_clear();
    cc1100_gdo0_int_enable();

    cc1100_gdo0_register_callback(read_pack);

    // remove irq in case it happened
    cc1100_gdo0_int_clear();

    preamble_count = 0;

    // prepare frame
    frame.length = HEADER_LENGTH;
    frame.type = TYPE_PREAMBLE;
    frame.dst_addr[0] = txframe.dst_addr[0];
    frame.dst_addr[1] = txframe.dst_addr[1];
    frame.src_addr[0] = node_addr>>8;
    frame.src_addr[1] = node_addr & 0xFF;

    // set periodic alarm for sending preambles
    timerB_set_alarm_from_now(ALARM_PREAMBLE, SEND_PERIOD, SEND_PERIOD);
    timerB_register_cb(ALARM_PREAMBLE, send_preamble);

    stat_add(STAT_MAC_REAL_TX);
    send_preamble();

    return 0;
}

static uint16_t send_preamble(void) {
    preamble_count++;

    if (preamble_count >= MAX_PREAMBLE_COUNT) {
        //send_data();
        //return 0;

        //DEBUG3("NO PACK...%u\n",short_send);
        DEBUG2("[MAC];SEND;%d STROBES;RETRY %d\n", preamble_count, -2);
        
        preamble_count = 0;
        timerB_unset_alarm(ALARM_PREAMBLE);
        count_as_retry = 1;
        stat_add(STAT_MAC_NO_PACK);
        delay_send();
        return 0;
    }
    cc1100_cmd_idle();
    cc1100_cmd_flush_rx();
    cc1100_cmd_flush_tx();
    cc1100_cmd_tx();

    cc1100_fifo_put((uint8_t*)(&frame.length), frame.length+1);
    return 0;
}

static uint16_t read_pack(void) {
    static struct {
        uint8_t length, type, dst_addr[2], src_addr[2], status[2];
    } ack;

    // check if radio state is idle
    if ( (cc1100_status() & 0x70) != 0x0) {
        // if not this means a preamble has been sent
        return 0;
    }
    // Check CRC
    if ( !(cc1100_status_crc_lqi() & 0x80) ) {
        DEBUG("[xmac] STAT_ERROR_PACK_CRC\n");
        stat_add(STAT_ERROR_PACK_CRC);
        // CRC error, abort
        return 0;
    }

    // we got a frame
    // Check Length is correct
    cc1100_fifo_get( (uint8_t*) &(ack.length), 1);
    if (ack.length != HEADER_LENGTH) {
        // length doesn't match the frame
        stat_add(STAT_ERROR_PACK_TOO_LONG);
        DEBUG("[xmac] STAT_ERROR_PACK_TOO_LONG\n");

        DEBUG2("[MAC];SEND;%d STROBES;RETRY %d\n", preamble_count, -1);
        timerB_unset_alarm(ALARM_PREAMBLE);
        count_as_retry = 1;
        delay_send();

        return 0;
    }

    // Get Frame bytes and status
    cc1100_fifo_get( (uint8_t*) &(ack.length)+1, ack.length+2);

    // check type
    if (ack.type != TYPE_ACK) {
        stat_add(STAT_ERROR_PACK_BAD_TYPE);
        DEBUG("[xmac] STAT_ERROR_PACK_BAD_TYPE\n");
        // type error

        DEBUG2("[MAC];SEND;%d STROBES;RETRY %d\n", preamble_count, -1);
        timerB_unset_alarm(ALARM_PREAMBLE);
        count_as_retry = 1;
        delay_send();

        return 0;
    }

    //TODO: fix for preamble
    /*if (ack.dst_addr[0] == txframe.dst_addr[0] && ack.dst_addr[1] == txframe.dst_addr[1]) {
        DEBUG3(":::PREAMBULE FOR THE SAME DEST WHEN PACK %.2x%.2x\n",txframe.dst_addr[0], txframe.dst_addr[1]);
        short_send = HEAR_PREAMBLE;
        init_short_send_mode(655);
        return 0;
    }*/
    
    // Check address
    if ( (ack.dst_addr[0] != (node_addr>>8)) || (ack.dst_addr[1] != (node_addr&0xFF)) ||
            (ack.src_addr[0] != txframe.dst_addr[0]) || (ack.src_addr[1] != txframe.dst_addr[1]) ) {
        stat_add(STAT_ERROR_PACK_NOT_DEST);
        DEBUG("[xmac] STAT_ERROR_PACK_NOT_DEST\n");
        // addresses don't match

        DEBUG2("[MAC];SEND;%d STROBES;RETRY %d\n", preamble_count, -1);
        timerB_unset_alarm(ALARM_PREAMBLE);
        count_as_retry = 1;
        delay_send();

        return 0;
    }

    // everything's good, send data
    send_data();
    return 0;
}

static uint16_t send_data(void) {
    DEBUG2("[MAC];SEND;%d STROBES;RETRY %d\n", preamble_count, delay_count);
    preamble_count = 0;
    //short_send = 0;
    // unset alarm
    timerB_unset_alarm(ALARM_PREAMBLE);
    cc1100_cmd_idle();
    cc1100_cmd_flush_rx();
    cc1100_cmd_flush_tx();

    cc1100_cfg_txoff_mode(CC1100_TXOFF_MODE_IDLE);
    cc1100_gdo0_register_callback(send_done);
    cc1100_cmd_tx();
    cc1100_fifo_put((uint8_t*)(&txframe.length), txframe.length+1);


    return 0;
}

static uint16_t send_done(void) {

    // data has been sent
    // if broadcast, don't wait for ACK
    if ((txframe.dst_addr[0]==0xFF) && (txframe.dst_addr[1])==0xFF) {
        if (sent_cb) {
            sent_cb();
        }

        delay_count = 0;
        frame_to_send = 0;
        preamble_count = 0;
        set_wor();
    } else {
        // we need an ACK
        // set RX
        cc1100_cmd_idle();
        cc1100_cmd_flush_rx();
        cc1100_cmd_rx();

        // change GDO to EoP, and its callback
        cc1100_gdo0_int_disable();
        cc1100_gdo0_int_set_falling_edge();
        cc1100_gdo0_int_clear();
        cc1100_gdo0_int_enable();
        cc1100_gdo0_register_callback(read_dataack);

        // set a timer alarm in case we never get an ACK
        timerB_set_alarm_from_now(ALARM_TIMEOUT, ACK_TIMEOUT, 0);
        timerB_register_cb(ALARM_TIMEOUT, dack_timeout);

    }

    return 0;
}

static uint16_t read_dataack(void) {

    static struct {
        uint8_t length, type, dst_addr[2], src_addr[2];
    } ack;

    
    DEBUG("[xmac] STAT_READ_ACK\n");

    // Check CRC
    if ( !(cc1100_status_crc_lqi() & 0x80) ) {
        // CRC error, abort
        cc1100_cmd_flush_rx();
        cc1100_cmd_rx();

        DEBUG("[xmac] STAT_ERROR_DACK_CRC\n");
        stat_add(STAT_ERROR_DACK_CRC);
        return 0;
    }

    // we got a frame
    // Check Length is correct
    cc1100_fifo_get( (uint8_t*) &(ack.length), 1);
    if (ack.length != HEADER_LENGTH) {
        // length doesn't match the frame
        cc1100_cmd_flush_rx();
        cc1100_cmd_rx();

        DEBUG("[xmac] STAT_ERROR_DACK_TOO_LONG\n");
        stat_add(STAT_ERROR_DACK_TOO_LONG);
        return 0;
    }

    // Get Frame bytes
    cc1100_fifo_get( (uint8_t*) &(ack.length)+1, ack.length);

    // check type
    if (ack.type != TYPE_DATAACK) {
        
        if(short_send == HEAR_PREAMBLE) {
            if(ack.type == TYPE_PREAMBLE) { //if serie of preamble, extend timeout
                if ((ack.dst_addr[0] == txframe.dst_addr[0]) && (ack.dst_addr[1] == txframe.dst_addr[1])) {
                    init_short_send_mode(1300);
                    return 0;
                }
            }
        }
     
        // type error
        cc1100_cmd_flush_rx();
        cc1100_cmd_rx();

        DEBUG("[xmac] STAT_ERROR_DACK_BAD_TYPE\n");
        stat_add(STAT_ERROR_DACK_BAD_TYPE);
        return 0;
    }
    
    //standard mode
    if (short_send == HEAR_PREAMBLE) {
        if ((ack.src_addr[0] != txframe.dst_addr[0]) || (ack.src_addr[1] != txframe.dst_addr[1])) {
            // Addresses don't match
            cc1100_cmd_flush_rx();
            cc1100_cmd_rx();

            return 0;
        }

        //OK, kill timeout
        timerB_unset_alarm(ALARM_TIMEOUT);

        // RX DACK I was waiting for
        short_send = HEAR_DACK;

        DEBUG2(":::READ DACK\n");

        // Short random delay
        short_send_backoff = rand();
        short_send_backoff &= RANDOM_DELAY_BK;
        short_send_backoff += DELAY_MIN_BK;

        //cc1100_cmd_idle();
        //cc1100_cmd_rx();

        //TODO: init backoff + timeout (3)
        timerB_set_alarm_from_now(ALARM_PREAMBLE, short_send_backoff, 0);
        timerB_register_cb(ALARM_PREAMBLE, send_data);

        // change GDO to SyncWord, and its callback
        //cc1100_gdo0_int_disable();
        //cc1100_gdo0_int_set_rising_edge();
        //cc1100_gdo0_int_clear();
        //cc1100_gdo0_int_enable();
        //cc1100_gdo0_register_callback(medium_busy);
        return 0;
    } else {
        // Check address
        if ( (ack.dst_addr[0] != (node_addr>>8)) || (ack.dst_addr[1] != (node_addr&0xFF)) ||
                (ack.src_addr[0] != txframe.dst_addr[0]) || (ack.src_addr[1] != txframe.dst_addr[1]) ) {
            // addresses don't match
            cc1100_cmd_flush_rx();
            cc1100_cmd_rx();

            stat_add(STAT_ERROR_DACK_NOT_DEST);
            DEBUG("[xmac] STAT_ERROR_DACK_NOT_DEST\n");
            return 0;
        }

        // everything's good, send is really done
        timerB_unset_alarm(ALARM_TIMEOUT);


        if (short_send != 0) {
            DEBUG3(":::SS MODE COMPLETE\n");
            short_send = 0;
        }

        if (sent_cb) {
            sent_cb();
        }

        delay_count = 0;
        frame_to_send = 0;
        preamble_count = 0;
        short_send = 0;

        set_wor();
        return 0;
    }

    return 0;
}

static uint16_t dack_timeout(void) {
    DEBUG3("::::::DACK TIMEOUT\n");
    stat_add(STAT_MAC_RETRY_ACK_TIMEOUT);
    count_as_retry = 1;
    short_send = 0;
    preamble_count = 0;
    

    delay_send();
    return 0;
}

static uint16_t init_short_send_mode(uint16_t ss_timeout) {

    //DEBUG3(":::INIT SS MODE %u\n",ss_timeout);
    // we need a DACK
    // set RX
    cc1100_cmd_idle();
    cc1100_cmd_flush_rx();
    cc1100_cmd_rx();

    // change GDO to EoP, and its callback
    cc1100_gdo0_int_disable();
    cc1100_gdo0_int_set_falling_edge();
    cc1100_gdo0_int_clear();
    cc1100_gdo0_int_enable();
    cc1100_gdo0_register_callback(read_dataack);

    // set a timer alarm in case we never get an ACK
    // Security: do not send data
    timerB_unset_alarm(ALARM_TIMEOUT);
    timerB_set_alarm_from_now(ALARM_TIMEOUT, ss_timeout, 0);
    timerB_register_cb(ALARM_TIMEOUT, try_send);

    return 0;
}


static uint16_t medium_busy(void) {

    // stop the timer
    timerB_unset_alarm(ALARM_PREAMBLE);

    /*    
    // change GDO to EOP, and its callback
    cc1100_gdo0_int_disable();
    cc1100_gdo0_int_set_falling_edge();
    cc1100_gdo0_int_clear();
    cc1100_gdo0_int_enable();
    cc1100_gdo0_register_callback(read_frame);

    // check if frame received
    if ( !cc1100_gdo0_read() ) {
        cc1100_gdo0_int_clear();
        read_frame();
    }
    */
    
    read_frame();
    
    if(frame_to_send == 1)
    {
        if (short_send == HEAR_PREAMBLE) {	//from read_frame() go to read_dataack for 40ms to find DACK
            init_short_send_mode(1300);
            return 0;
        } else if (short_send == HEAR_DACK) {
            delay_send();
            return 0;
        } else {
            // set a delay for a retry
            stat_add(STAT_MAC_RETRY_MEDIUM_BUSY);

            delay_send();
            return 0;
        }
    }
    else
    {
       // set_wor();
    }

    return 0;

}

/*------------------------RX--------------------------*/

static uint16_t read_frame(void) {
    uint16_t src, dst;
    uint8_t status[2];
    int16_t rssi;

    stat_add(STAT_READ_FRAME);

    // deactivate timeout alarm
    timerB_unset_alarm(ALARM_TIMEOUT);
    timerB_unset_alarm(ALARM_POP);
    timerB_unset_alarm(ALARM_STAYRX);

    // Check CRC
    if ( !(cc1100_status_crc_lqi() & 0x80) ) {
        // CRC error, abort
        DEBUG("[xmac] STAT_ERROR_CRC\n");
        stat_add(STAT_ERROR_CRC);
        cc1100_cmd_idle();
        cc1100_cmd_flush_rx();
        cc1100_cmd_flush_tx();
        set_wor();
        return 0;
    }


    // Check Length is correct
    cc1100_fifo_get( (uint8_t*) &(frame.length), 1);
    if (frame.length < HEADER_LENGTH || frame.length > PACKET_LENGTH_MAX) {
        // length error
        DEBUG("[xmac] STAT_ERROR_TOO_LONG\n");
        stat_add(STAT_ERROR_TOO_LONG);
        cc1100_cmd_idle();
        cc1100_cmd_flush_rx();
        cc1100_cmd_flush_tx();
        set_wor();
        return 0;
    }




    state = STATE_RX;

    // Get Frame
    cc1100_fifo_get( (uint8_t*) &(frame.length)+1, frame.length);

    // Get Status
    cc1100_fifo_get( (uint8_t*) status, 2);

    // Compute addresses
    dst = (((uint16_t)frame.dst_addr[0])<<8) + frame.dst_addr[1];
    src = (((uint16_t)frame.src_addr[0])<<8) + frame.src_addr[1];


    // Check Frame Type
    if (frame.type == TYPE_PREAMBLE) {

        // preamble
        if ( dst == node_addr) { //preamble to me
            if (when_stay == 1) {
                //DEBUG(":::READ PREAMBLE WHEN STAYRX\n");
            }

            if (frame_to_send == 1) {
                //DEBUG2(":::RX PREAMBLE FOR ME WHEN FRAME_TO_SEND\n");
                if (mess_in_queue >= QUEUE_LEN -1) {
                    // there is already a frame about to be sent
                    //DEBUG2("[QUEUE];FULL_QUEUE\n");
                    //stat_add(STAT_FULL_QUEUE);
                    //frame_to_send = 0;
                    return 0;
                } else {
                
                    if(kready == 1)
                    {
                        kready = 0;
                        DEBUG2("[QUEUE];PUSH;%u\n",mess_in_queue);
                        stat_add(STAT_ADD_QUEUE);
                        mess_queue[mess_in_queue].length = txframe.length;
                        mess_queue[mess_in_queue].type = txframe.type;
                        mess_queue[mess_in_queue].dst_addr[0] = txframe.dst_addr[0];
                        mess_queue[mess_in_queue].dst_addr[1] = txframe.dst_addr[1];
                        mess_queue[mess_in_queue].src_addr[0] = txframe.src_addr[0];
                        mess_queue[mess_in_queue].src_addr[1] = txframe.src_addr[1];

                        // copy payload
                        for (i1=0; i1<txframe.length; i1++) mess_queue[mess_in_queue].payload[i1] = txframe.payload[i1];

                        mess_in_queue += 1;
                        frame_to_send = 0;
                        kready = 1;
                    }
                    else
                    {
                        DEBUG3("+++++++PUSH NOT READY\n");
                    }
                }    
            }
            

            // for me !
            // Prepare ACK Frame
            frame.length = HEADER_LENGTH;
            frame.type = TYPE_ACK;
            frame.dst_addr[0] = frame.src_addr[0];
            frame.dst_addr[1] = frame.src_addr[1];
            frame.src_addr[0] = node_addr>>8;
            frame.src_addr[1] = node_addr & 0xFF;

            // configure auto switch
            cc1100_cfg_txoff_mode(CC1100_TXOFF_MODE_RX);
            cc1100_cfg_cca_mode(CC1100_CCA_MODE_ALWAYS);
            cc1100_cfg_rc_pd(CC1100_RC_OSC_DISABLE);
            cc1100_cfg_fs_autocal(CC1100_AUTOCAL_NEVER);
            cc1100_cmd_flush_rx();
            cc1100_cmd_flush_tx();

            // update callback
            cc1100_gdo0_register_callback(pack_sent);

            // Start TX
            cc1100_cmd_tx();

            // Put frame in TX FIFO
            cc1100_fifo_put((uint8_t*)&frame.length, frame.length+1);

            return 0;   //2
        } else if (dst == 0xFFFF) {
            // for broadcast
            // we keep listening until we get data
            cc1100_cmd_flush_rx();
            cc1100_cmd_flush_tx();
            cc1100_cmd_rx();

            // set a timer alarm in case we never get a data frame
            timerB_set_alarm_from_now(ALARM_TIMEOUT, SEND_PERIOD*MAX_PREAMBLE_COUNT, 0);
            timerB_register_cb(ALARM_TIMEOUT, set_wor);
            return 0;
        }
        else if (frame_to_send == 1) { // RX preamble while trying to send a packet
            if (frame.dst_addr[0] == txframe.dst_addr[0] && frame.dst_addr[1] == txframe.dst_addr[1]) {
                DEBUG3(":::PREAMBULE FOR THE SAME DEST %.2x%.2x\n",txframe.dst_addr[0], txframe.dst_addr[1]);
                
                #ifdef OPT
                short_send = HEAR_PREAMBLE;
                #endif
                
                return 0;
            } else {
                DEBUG(":::BAD DEST %.2x%.2x NEED %.2x%.2x\n",frame.dst_addr[0], frame.dst_addr[1],txframe.dst_addr[0], txframe.dst_addr[1]);
                set_wor();
            }
            return 0;
        } else {
            // not for me
            set_wor();
            return 0;
        }

    } else if (frame.type == TYPE_DATA) {
        // data
        int len = frame.length-HEADER_LENGTH;

        // if for me, send ACK
        if (dst == node_addr) {

            if (when_stay == 1) {
                //DEBUG2(":::READ DATA WHEN STAYRX\n");
            }


            if (mess_in_queue >= QUEUE_LEN) {
                // there is already a frame about to be sent
                DEBUG2("[QUEUE];REJECT\n");
                stat_add(STAT_FULL_QUEUE);
                set_wor();
                return 0;
            } else {

                // send DATAACK
                frame.length = HEADER_LENGTH;
                frame.type = TYPE_DATAACK;
                frame.dst_addr[0] = frame.src_addr[0];
                frame.dst_addr[1] = frame.src_addr[1];
                frame.src_addr[0] = node_addr>>8;
                frame.src_addr[1] = node_addr & 0xFF;

                // update callback
                //cc1100_gdo0_register_callback(set_wor);

                cc1100_gdo0_register_callback(stay_rx);

                // Start TX
                cc1100_cmd_tx();

                // Put frame in TX FIFO
                cc1100_fifo_put((uint8_t*)&frame.length, frame.length+1);
            }
        } else {
            set_wor();
        }

        if (dst == node_addr || dst == 0xFFFF) {
            // for me, call handler

            //same as APP_RX, but for each node (not only the sink)
            DEBUG("[xmac] STAT_MAC_RX\n");
            stat_add(STAT_MAC_RX);

            if (received_cb) {
                rssi = status[0] >= 128 ? status[0]-256 : status[0];
                rssi -= 140;

                return received_cb(frame.payload, len, src, rssi);
            }
        }
    } else {
        // type error
        //ecoute d'un ack alors qu'on n'est pas destinataire
        DEBUG("[xmac] STAT_ERROR_BAD_TYPE\n");
        DEBUG("[xmac] read_frame, type error (%.2x)\n", frame.type);
        stat_add(STAT_ERROR_BAD_TYPE);
        set_wor();
        return 0;
    }

    return 0;
}

static uint16_t pack_sent(void) {
    //DEBUG3(":::ACK SENT\n");
    // now we wait for data
    cc1100_gdo0_register_callback(read_frame);

    // set a timer alarm in case we never get a data frame
    timerB_set_alarm_from_now(ALARM_TIMEOUT, DATA_TIMEOUT , 0);
    timerB_register_cb(ALARM_TIMEOUT, set_wor);
    return 0;
}

