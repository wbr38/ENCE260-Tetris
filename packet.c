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

bool packet_decode(uint8_t byte, packet_t* packet)
{
    packet->raw = byte;

    // ignore invalid ID recvd
    if (packet->id < 0 || packet->id >= _PACKET_COUNT)
        return false;

    return true;
}

uint8_t packet_encode(packet_t packet)
{
    return packet.raw;
}

bool packet_get(packet_t* packet)
{
    // wait until a byte is ready to be read
    if (!ir_uart_read_ready_p())
        return false;

    uint8_t byte = ir_uart_getc();
    return packet_decode(byte, packet);
}

void packet_send(packet_t packet)
{
    uint8_t byte = packet_encode(packet);
    ir_uart_putc(byte);
}

void handle_packet(packet_t packet)
{
    switch (packet.id)
    {
    case PAIRING_PACKET:
        {
            // recvd pairing packet, set the rng_seed and respond
            game_data->rng_seed = packet.data;

            packet_t ack = {
                .id = PAIRING_ACK_PACKET,
                .data = 0,
            };
            packet_send(ack);
            game_data->game_state = GAME_STATE_STARTING;
            break;
        }

    case PAIRING_ACK_PACKET:
        {
            // TODO: Confirm we were the board sending Pairing packet
            // recvd pairing ack, start the game
            game_data->game_state = GAME_STATE_STARTING;
            break;
        }

    case PING_PACKET:
        {
            // todo
            // maybe we agree that whoever sent the Pairing packet should be the one sending the Ping packet
            // the other board only sends pong
            // that way each board knows which packet to expect and recv
            break;
        }

    case PONG_PACKET:
        {
            // todo
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
            // TODO
            break;
        }

    case DIE_ACK_PACKET:
        {
            // TODO
            break;
        }

    default:
        break;
    }
}