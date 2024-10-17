/** @file packet.c
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Functionality for sending, recieving, and handling packets via IR.
 */

#include "packet.h"

#include <ir_uart.h>

#include "game_data.h"

// Generate a binary number of n 1s. E.g.: ONES(3) -> 0b111
#define ONES(n) ((1 << (n)) - 1)

/**
 * Decode the given byte into the packet structure.
 */
bool packet_decode(uint8_t byte, packet_t* packet)
{
    packet->raw = byte;

    // ignore invalid ID recvd
    if (packet->id < 0 || packet->id >= _PACKET_COUNT)
        return false;

    return true;
}

/**
 * Encodes the packet id and data into a byte to be sent by the IR.
 */
uint8_t packet_encode(packet_t packet)
{
    return packet.raw;
}

/**
 * Gets the byte and packet from a given packet.
 */
bool packet_get(packet_t* packet)
{
    // wait until a byte is ready to be read
    if (!ir_uart_read_ready_p())
        return false;

    uint8_t byte = ir_uart_getc();
    return packet_decode(byte, packet);
}

/**
 * Send the given packet through IR to the other device.
 */
void packet_send(packet_t packet)
{
    uint8_t byte = packet_encode(packet);
    ir_uart_putc(byte);
}

/**
 * Handles what steps to take depending on the type of packet recieved.
 */
void handle_packet(packet_t packet)
{
    switch (packet.id)
    {
    case PAIRING_PACKET:
        {
            // only pair on main menu screen
            if (game_data->game_state != GAME_STATE_MAIN_MENU)
                return;

            // TODO: Does it really matter if both boards try to pair at the same time?
            game_data->host = false;

            // recvd pairing packet, set the rng_seed and respond
            game_data->rng_seed = packet.data;

            packet_t ack = {
                .id = PAIRING_ACK_PACKET,
                .data = 0,
            };
            packet_send(ack);
            game_data->host = false;
            game_data->game_state = GAME_STATE_STARTING;
            break;
        }

    case PAIRING_ACK_PACKET:
        {
            // We haven't sent a Pairing packet, but the other board for some reason is responding to one
            if (!game_data->host)
                return;

            // recvd pairing ack, start the game
            game_data->host = true;
            game_data->game_state = GAME_STATE_STARTING;
            break;
        }

    case PING_PACKET:
        {
            // todo
            // maybe we agree that whoever sent the Pairing packet should be the one sending the Ping packet
            // the other board only sends pong
            // that way each board knows which packet to expect and recv
            game_data->recvd_pingpong = true;
            packet_t pong = {
                .id = PONG_PACKET,
                .data = 0,
            };
            packet_send(pong);
            break;
        }

    case PONG_PACKET:
        {
            // todo
            game_data->recvd_pingpong = true;
            break;
        }

    case LINE_CLEAR_PACKET:
        {
            uint8_t num_cleared = packet.data;
            game_data->their_lines_cleared += num_cleared;
            break;
        }

    case DIE_PACKET:
        {
            // Other player has died
            game_data->other_player_dead = true;

            // Acknowledge the packet
            packet_t ack = {
                .id = DIE_ACK_PACKET,
                .data = 0,
            };
            packet_send(ack);
            break;
        }

    case DIE_ACK_PACKET:
        {
            // For some reason the board thought we sent a die packet, when we aren't dead
            if (game_data->game_state != GAME_STATE_DEAD)
                return;

            game_data->die_packet_acknowledged = true;
            break;
        }

    default:
        break;
    }
}