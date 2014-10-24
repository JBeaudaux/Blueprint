#include <stdio.h>
#include "stats.h"

void stats_init()
{
    int i;
    for (i=0; i<NB_STATS; i++)
    {
        exp_stats[i] = 0;
    }
}

void stat_add(int key)
{
    if (key < 0 || key > NB_STATS)
    {
    	printf("[ERROR_STAT];%u\n",key);
        return;
    }
    
    exp_stats[key]++;
}

void stats_clear()
{
    stats_init();
}

void stats_print(uint8_t num_exp)
{
    printf("[STAT];%u;APP_TX %u\n",num_exp, exp_stats[STAT_APP_TX]);
    printf("[STAT];%u;APP_RX %u\n",num_exp, exp_stats[STAT_APP_RX]);
    
    printf("[STAT];%u;MAC_TX %u\n",num_exp, exp_stats[STAT_MAC_TX]);
    printf("[STAT];%u;MAC_RX %u\n",num_exp, exp_stats[STAT_MAC_RX]);
    
    printf("[STAT];%u;FORWARD_COUNT %u\n",num_exp, exp_stats[STAT_FORWARD_CNT]);

    printf("[STAT];%u;MAC_TRY_TX %u\n",num_exp, exp_stats[STAT_MAC_TRY_TX]);
    printf("[STAT];%u;MAC_REAL_TX %u\n",num_exp, exp_stats[STAT_MAC_REAL_TX]);

    printf("[STAT];%u;PKT_DROP_RETRY %u\n",num_exp, exp_stats[STAT_PKT_DROP_RETRY]);
    
    //printf("[STAT];%u;ERROR_TOO_LONG %u\n",num_exp, exp_stats[STAT_ERROR_TOO_LONG]);
    //printf("[STAT];%u;ERROR_PACKET_SENDING %u\n",num_exp, exp_stats[STAT_ERROR_PACKET_SENDING]);
    
    printf("[STAT];%u;ERROR_PACK_CRC %u\n",num_exp, exp_stats[STAT_ERROR_PACK_CRC]);
    printf("[STAT];%u;ERROR_PACK_TOO_LONG %u\n",num_exp, exp_stats[STAT_ERROR_PACK_TOO_LONG]);
    printf("[STAT];%u;ERROR_PACK_BAD_TYPE %u\n",num_exp, exp_stats[STAT_ERROR_PACK_BAD_TYPE]);
    printf("[STAT];%u;ERROR_PACK_NOT_DEST %u\n",num_exp, exp_stats[STAT_ERROR_PACK_NOT_DEST]);
    
    printf("[STAT];%u;ERROR_CRC %u\n",num_exp, exp_stats[STAT_ERROR_CRC]);
    printf("[STAT];%u;ERROR_TOO_LONG %u\n",num_exp, exp_stats[STAT_ERROR_TOO_LONG]);
    printf("[STAT];%u;ERROR_BAD_TYPE %u\n",num_exp, exp_stats[STAT_ERROR_BAD_TYPE]);
    
    printf("[STAT];%u;ERROR_DACK_CRC %u\n",num_exp, exp_stats[STAT_ERROR_DACK_CRC]);
    printf("[STAT];%u;ERROR_DACK_TOO_LONG %u\n",num_exp, exp_stats[STAT_ERROR_DACK_TOO_LONG]);
    printf("[STAT];%u;ERROR_DACK_BAD_TYPE %u\n",num_exp, exp_stats[STAT_ERROR_DACK_BAD_TYPE]);
    printf("[STAT];%u;ERROR_DACK_NOT_DEST %u\n",num_exp, exp_stats[STAT_ERROR_DACK_NOT_DEST]);

	printf("[STAT];%u;READ_FRAME %u\n",num_exp, exp_stats[STAT_READ_FRAME]);

    printf("[STAT];%u;ADD_QUEUE_COUNT %u\n",num_exp, exp_stats[STAT_ADD_QUEUE]);
    printf("[STAT];%u;FULL_QUEUE_COUNT %u\n",num_exp, exp_stats[STAT_FULL_QUEUE]);

    printf("[STAT];%u;MAC_RETRY %u\n",num_exp, exp_stats[STAT_MAC_RETRY]);
    printf("[STAT];%u;MAC_RETRY_MEDIUM_BUSY %u\n",num_exp, exp_stats[STAT_MAC_RETRY_MEDIUM_BUSY]);
    printf("[STAT];%u;MAC_RETRY_ACK_TIMEOUT %u\n",num_exp, exp_stats[STAT_MAC_RETRY_ACK_TIMEOUT]);
    printf("[STAT];%u;MAC_RETRY_NOT_READY %u\n",num_exp, exp_stats[STAT_MAC_RETRY_NOT_READY]);
    
    printf("[STAT];%u;MAC_NO_PACK %u\n",num_exp, exp_stats[STAT_MAC_NO_PACK]);
}
