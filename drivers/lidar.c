#include "lidar.h"
#include "i2c.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define PACKET_MAX 128
#define LIDAR_ADDR 0x33 // Configured on the physical sensor
#define INIT_PKT_SIZE 8

#define CMD_SETMODE 1
#define CMD_ALLDATA 2
#define CMD_FIXED_POINT 3

#define PKT_MAGIC_OFFSET 0
#define PKT_LEN_OFFSET 1
#define PKT_CMD_OFFSET 3
#define PKT_PAYLOAD_OFFSET 4

// Payload lengths
#define FIXED_POINT_PAYLOAD_LEN 2 // for fixed point data

// Packet structure:
// | Magic Byte |
// | Payload Length High Byte |
// | Payload Length Low Byte |
// | Command Byte |
// | Payload ... |
//

typedef struct __attribute__((packed)) {
  uint8_t status;          // Response packet status
  uint16_t len;            // Length of response payload
  uint8_t cmd;             // Response command
  uint8_t buf[PACKET_MAX]; // Response payload
} recvpkt_t;

void _sendpkt(uint8_t cmd, uint8_t *payload, uint16_t size);
void _recvpkt(uint8_t cmd, recvpkt_t *recvpkt); 
void set_ranging_mode(uint8_t range);

void lidar_init(uint8_t range) {
  set_ranging_mode(range);
  // uint8_t init_cmd[] = {0x55, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x08};
  // i2c_write(LIDAR_ADDR, init_cmd, INIT_PKT_SIZE);
}

uint32_t ranging_mode = -1;

// Set the ranging mode for the LIDAR
// Either 4x4 or 8x8
void set_ranging_mode(uint8_t range) {
  if (range != 4 && range != 8) {
    uart_printf("Ranging mode: %dx%d is out of bounds.\n", range, range);
  }
  ranging_mode = range;
  uint8_t ranging_payload[] = {0x00, 0x00, 0x00, range};
  _sendpkt(CMD_SETMODE, ranging_payload, 4);
}

// fixed-point data
uint16_t get_lidar(uint8_t x, uint8_t y) {
  uint16_t result = 0;

  uint8_t errorCode;

  // 6-byte command packet for requesting fixed-point data
  uint8_t buf[2] = {x, y};
  _sendpkt(CMD_FIXED_POINT, buf, 2);

	recvpkt_t data;
	_recvpkt(CMD_FIXED_POINT, &data);
	// TODO: Check receive packet status and wtv

	uint16_t ret;
	memcpy(&ret, data.buf, 2);
	return ret;

}

// Talk to the lidar with its weird custom chinese protocol
void _sendpkt(uint8_t cmd, uint8_t *payload, uint16_t size) {
  /*
   * Assemble and send a packet over I2C to the LIDAR
   * */
  if (size > PACKET_MAX) {
    uart_printf("ERROR: Attempted to send LIDAR packet > %d bytes (attempted: "
                "%d bytes)\n",
                PACKET_MAX, size);
    return;
  }
  uint8_t pkt[PACKET_MAX];
  uint8_t magic_byte = 0x55;
  uint16_t length = size + 1; // +1 for the command byte

  memcpy(pkt + PKT_MAGIC_OFFSET, &magic_byte, sizeof(magic_byte));
  memcpy(pkt + PKT_LEN_OFFSET, &length, sizeof(length));
  memcpy(pkt + PKT_CMD_OFFSET, &cmd, sizeof(cmd));
  memcpy(pkt + PKT_PAYLOAD_OFFSET, payload, size);

  uint32_t pkt_size = sizeof(magic_byte) + sizeof(length) + sizeof(cmd) + size;

  i2c_write(LIDAR_ADDR, pkt, pkt_size);
}

void _recvpkt(uint8_t cmd, recvpkt_t *recvpkt) {
  /*
   * Wait for a response packet and fill an error code
   * */
  if (ranging_mode == -1) {
    uart_printf("Ranging mode not initialized in _recvpkt.\n");
    return;
  }

  uint32_t pkt_length = -1;

  if (ranging_mode == 4) {
    pkt_length = 36; // (4 * 4) * 2 + 4
  } else {
    pkt_length = 132; // (8 * 8) * 2 + 4
  }

  i2c_read(LIDAR_ADDR, &recvpkt, pkt_length);
}

// void lidar_butthole() {
//     // measures how much dih u can take
//     int dih_inches;
//     if(dih_inches > 3) {
//         printf("i cant take it >.<");
//     }
// }
// void lidar_dih()

// void lidar_

// int get_lidar(uint8_t x, uint8_t y)  {
// 	uint16_t data[PACKET_SIZE];
// 	i2c_read(LIDAR_ADDR, data, PACKET_SIZE);
// 	return (int)data[y * 8 + x];
// }
