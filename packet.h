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

// equivalent to `2^(PACKET_ID_LEN)`
#define PACKET_ID_MAX_VAL (1 << PACKET_ID_LEN)

// equivalent to `2^(PACKET_DATA_LEN)`
#define PACKET_DATA_MAX_VAL (1 << PACKET_DATA_LEN)

/**
 * Enum of ids of packets that can be sent or received.
 */
typedef enum {
    /** Used to begin pairing. Contains the RNG seed for the order of spawning the pieces */
    PAIRING_PACKET,

    /** Acknowledgment for PAIRING_PACKET */
    PAIRING_ACK_PACKET,

    /** Empty packet sent periodically to confirm both boards are still in communication. Expect a PONG_PACKET in response */
    PING_PACKET,

    /** Sent in acknowledgment for PING_PACKET */
    PONG_PACKET,

    /** Used to indicate that X amount of lines have been cleared */
    LINE_CLEAR_PACKET,

    /** Used when one of the players has died */
    DIE_PACKET,

    /** Acknowledgement of DIE_PACKET */
    DIE_ACK_PACKET,

    /**
     * Placeholder to determine max value of this enum. Not an actual packet!
     * There should not be more than `PACKET_ID_MAX_VAL` packet ids.
     * If there are, more bits should be assigned for the ID.
     */
    _PACKET_COUNT,
} PacketID;

/**
 * Defined structure for a packet sent/received by IR.
 * The size of this structure is equal to one byte: `sizeof(uint8_t)`.
 *
 * This is a union providing two ways to access the data:
 * - `id` and `data` are defined as bitfields, to access these parts of the byte.
 * - `raw` is used to access the raw uint8_t byte.
 */
typedef union {
    struct {
        uint8_t id : PACKET_ID_LEN;
        uint8_t data : PACKET_DATA_LEN;
    };
    uint8_t raw;
} packet_t;

// /**
//  * @brief Decode the given `byte` into the `packet`.
//  * @param byte The unmodified uint8_t received from the IR sensor.
//  * @param packet Pass by reference `packet` object.
//  * @return Whether the byte that was decoded is valid.
//  *         i.e. the packet ID received is valid.
//  */
// bool packet_decode(uint8_t byte, packet_t* packet);

// /**
//  * @brief Encode the given packet into a uint8_t byte to be sent by IR.
//  * @param packet The packet to be sent
//  * @return The byte to be sent by the IR transmitter.
//  */
// uint8_t packet_encode(packet_t packet);

/**
 * @brief Read the next byte from the IR receiver and decode into the given `packet`.
 * @param packet Pass by reference `packet` object for the byte to be decoded into.
 * @return true if a *valid* packet was received from IR and decoded into `packet`.
 * @return false if there wasn't a byte ready to be received, or if an invalid packet was read.
 */
bool packet_get(packet_t* packet);

/**
 * @brief Encode the given `packet` into a byte and transmit it via IR.
 * @param packet The packet to be encoded and sent
 */
void packet_send(packet_t packet);

/**
 * @brief Contains the functionality to handle a received packet.
 * @param packet A valid packet received from `packet_get`.
 */
void handle_packet(packet_t packet);

/**
 * @brief Checks to see if we should send the die packet.
 * Additionally, checks if both players are dead, then the game is over.
 */
void check_die_packet(void);

/**
 * @brief Send the Ping packet, if game_data->host is true (if we were the board that sent the Pairing)
 */
void check_ping_pong_packet(void);
#endif  // PACKET_H