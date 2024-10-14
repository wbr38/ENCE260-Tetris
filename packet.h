/** @file packet.h
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Functionality for sending recieving packets via IR.
 */

#ifndef PACKET_H
#define PACKET_H

#include <stdbool.h>
#include <stdint.h>

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

typedef struct {
    PacketID id;
    uint8_t data;
} packet_t;

bool packet_get(packet_t* packet);
void packet_send(packet_t packet);

bool packet_decode(uint8_t packet, PacketID* id, uint8_t* data);
uint8_t packet_encode(PacketID id, uint8_t data);
#endif // PACKET_H