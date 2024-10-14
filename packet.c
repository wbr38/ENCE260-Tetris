#include "packet.h"
#include "ir_uart.h"

// Packet ID is stored in the upper bits. Packet data is the lower bits
// Together these must sum to sizeof(uint8_t) == 8 
#define PACKET_ID_LEN 3
#define PACKET_DATA_LEN 5

// Generate a binary number of n 1s. E.g.: ONES(3) -> 0b111
#define ONES(n) ((1 << (n)) - 1)

bool packet_decode(uint8_t byte, PacketID* id, uint8_t* data)
{
    // id is the uppber bits, isolate the id by shifting the data bits to the right
    *id = byte >> PACKET_DATA_LEN;

    // data is the lower bits, mask these to isolate them
    *data = byte & ONES(PACKET_DATA_LEN);

    if (*id < 0 || *id >= _PACKET_COUNT)
        return false;

    return true;
}

uint8_t packet_encode(PacketID id, uint8_t data)
{
    // shift the id to the upper bits, and combine with the lower data bits
    return (id << PACKET_DATA_LEN) | (data & ONES(PACKET_DATA_LEN));
}

bool packet_get(packet_t* packet)
{
    // wait until a byte is ready to be read
    if (!ir_uart_read_ready_p())
        return false;
    
    int8_t byte = ir_uart_getc();
    return packet_decode(byte, &packet->id, &packet->data);
}

void packet_send(packet_t packet)
{
    uint8_t byte = packet_encode(packet.id, packet.data);
    ir_uart_putc(byte);
}