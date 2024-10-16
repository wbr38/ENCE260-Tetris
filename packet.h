/** @file packet.h
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Functionality for sending, recieving, and handling packets via IR.
 */

#ifndef PACKET_H
#define PACKET_H

#include <stdbool.h>
#include <stdint.h>

/**
 * The size of an IR packet is one byte, so together these should sum to 8 bits
 * Packet id is stored in the upper bits.
 * Packet data is stored in the lower bits.
 */
#define PACKET_ID_LEN   3
#define PACKET_DATA_LEN 5

typedef enum {
    /** Used to begin pairing. Contains the RNG seed for the order of spawning the pieces */
    PAIRING_PACKET,

    /** Acknowledgment for PAIRING_PACKET */
    PAIRING_ACK_PACKET,

    /** Empty packet sent periodically to confirm both boards are still in communication. Expect a PONG_PACKET in response */
    PING_PACKET,

    /** Acknowledgment for PING_PACKET */
    PONG_PACKET,

    /** Used to indicate that X amount of lines have been cleared */
    LINE_CLEAR_PACKET,

    /** Used when one of the players has died */
    DIE_PACKET,

    /** Acknowledgement of DIE_PACKET */
    DIE_ACK_PACKET,

    // TODO: Remove after testing
    TEST_PACKET,

    /** Placeholder to determine max value of this enum. Not an actual packet! */
    _PACKET_COUNT,
} PacketID;

typedef union {
    struct {
        uint8_t id : PACKET_ID_LEN;
        uint8_t data : PACKET_DATA_LEN;
    };
    uint8_t raw;
} packet_t;

bool packet_get(packet_t* packet);
void packet_send(packet_t packet);

bool packet_decode(uint8_t byte, packet_t* packet);
uint8_t packet_encode(packet_t packet);

void handle_packet(packet_t packet);
#endif  // PACKET_H