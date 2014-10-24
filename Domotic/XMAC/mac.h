#ifndef _MAC_H
#define _MAC_H

#define MAC_BROADCAST 0xFFFF

/**
 * Function pointer prototype for callback.
 * \param packet pointer to the received packet
 * \param length number of received bytes
 * \param src_addr address of the source node
 * \return 1 if the CPU should we waken up, 0 if it should stay in LPM
 */
typedef uint16_t (*mac_received_t)(uint8_t packet[], uint16_t length, uint16_t src_addr, int16_t rssi);
typedef uint16_t (*mac_sent_t)(void);
typedef uint16_t (*mac_error_t)();

/**
 * This node MAC address
 */
extern uint16_t node_addr;
extern uint8_t stop;

/**
 * Initialize the MAC layer implementation.
 * \param channel the radio channel to use (values 0-20)
 */
void mac_init(uint8_t channel);

/**
 * Send a packet to a node.
 * \param packet pointer to the packet
 * \param length number of bytes to send
 * \param dst_addr address of the destination node
 * \return 0 if OK, 1 if a packet is being sent, 2 if length too big.
 */
uint16_t mac_send(uint8_t packet[], uint16_t length, uint16_t dst_addr);

/**
 * Register a function callback that'll be called
 * when a packet has been received.
 * \param cb function pointer
 */
void mac_set_rx_cb(mac_received_t cb);

/**
 * Register a function callback that'll be called
 * when a packet has been sent successfully.
 * \param cb function pointer
 */
void mac_set_sent_cb(mac_sent_t cb);

/**
 * Register a function callback that'll be called
 * when a packet has failed to be sent.
 * \param cb function pointer
 */
void mac_set_error_cb(mac_error_t cb);

/**
 * Disable MAC layer. Stop transmitting.
 */
void mac_stop(void);
/**
 * re-enable MAC layer.
 */
void mac_restart(void);


#endif
