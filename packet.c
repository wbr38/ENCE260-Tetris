/** @file packet.c
 *  @authors William Brown (wbr38), Matthew Wills (mwi158)
 *  @date 12 October 2024
 *  @brief Functionality for sending, recieving, and handling packets via IR.
 */

#include "packet.h"

#include <ir_uart.h>

#include "game_data.h"

/**
 * @brief Decode the given `byte` into the `packet`.
 * @param byte The unmodified uint8_t received from the IR sensor.
 * @param packet Pass by reference `packet` object.
 * @return Whether the byte that was decoded is valid.
 *         i.e. the packet ID received is valid.
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
 * @brief Encode the given packet into a uint8_t byte to be sent by IR.
 * @param packet The packet to be sent
 * @return The byte to be sent by the IR transmitter.
 */
uint8_t packet_encode(packet_t packet)
{
    return packet.raw;
}

/**
 * @brief Read the next byte from the IR receiver and decode into the given `packet`.
 * @param packet Pass by reference `packet` object for the byte to be decoded into.
 * @return true if a *valid* packet was received from IR and decoded into `packet`.
 * @return false if there wasn't a byte ready to be received, or if an invalid packet was read.
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
 * @brief Encode the given `packet` into a byte and transmit it via IR.
 * @param packet The packet to be encoded and sent
 */
void packet_send(packet_t packet)
{
    uint8_t byte = packet_encode(packet);
    ir_uart_putc(byte);
}

/**
 * @brief Contains the functionality to handle a received packet.
 * @param packet A valid packet received from `packet_get`.
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

/**
 * Checks to see if we should send the die packet.
 * This function checks if both players are dead, then the game is over.
 */
void check_die_packet(void)
{
    // We are dead but board has not acknlowdged our die packet
    if (game_data->game_state == GAME_STATE_DEAD && !game_data->die_packet_acknowledged)
    {
        packet_t die = {
            .id = DIE_PACKET,
            .data = 0,
        };
        packet_send(die);
    }
}

/**
 * @brief Send the Ping packet, if game_data->host is true (if we were the board that sent the Pairing)
 */
void check_ping_pong_packet(void)
{
    if (game_data->host)
    {
        packet_t ping = {
            .id = PING_PACKET,
            .data = 0,
        };
        packet_send(ping);
    }
}