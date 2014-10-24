#ifndef _STATS_H
#define _STATS_H

/* COUCHE DE STATS */
#define NB_STATS 29

#define STAT_APP_TX 0
#define STAT_APP_RX 1

#define STAT_MAC_TX 2
#define STAT_MAC_RX 3

#define STAT_MAC_TRY_TX 4
#define STAT_MAC_RETRY 5
#define STAT_MAC_REAL_TX 6

#define STAT_PKT_DROP_RETRY 7   // TOO MUCH RETRY

// mac send
#define STAT_ERROR_PACKET_TOO_LONG 8
#define STAT_ERROR_PACKET_SENDING 9

// RX PREAMBLE ACK
#define STAT_ERROR_PACK_CRC 10
#define STAT_ERROR_PACK_TOO_LONG 11
#define STAT_ERROR_PACK_BAD_TYPE 12
#define STAT_ERROR_PACK_NOT_DEST 13

// General RX
#define STAT_ERROR_CRC 14
#define STAT_ERROR_TOO_LONG 15
#define STAT_ERROR_BAD_TYPE 16

#define STAT_FORWARD_CNT 17

// RX PACKET ACK
#define STAT_ERROR_DACK_CRC 18
#define STAT_ERROR_DACK_TOO_LONG 19
#define STAT_ERROR_DACK_BAD_TYPE 20
#define STAT_ERROR_DACK_NOT_DEST 21

#define STAT_READ_FRAME 22

#define STAT_ADD_QUEUE 23
#define STAT_FULL_QUEUE 24

#define STAT_MAC_RETRY_MEDIUM_BUSY 25
#define STAT_MAC_RETRY_ACK_TIMEOUT 26
#define STAT_MAC_RETRY_NOT_READY 27
#define STAT_MAC_NO_PACK 28

int exp_stats[NB_STATS];
void stats_init(void);
void stat_add(int key);
void stats_clear(void);
void stats_print(uint8_t num_exp);

#endif
