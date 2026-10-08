#include "i2c.h"
#include "stm32f446xx.h"
#include "time.h"
#include "uart.h"

void _i2c_read_2_bytes(uint8_t i2c_addr, void *result);
void _i2c_read_1_byte(uint8_t i2c_addr, void *result);
void _i2c_read_n_bytes(uint8_t i2c_addr, void *result, uint32_t nbytes);

// Using 7-bit controller receiver
uint8_t i2c_enabled_flag = 0;
uint32_t pclk1_hz = 16000000u; // peripheral clock speed
uint32_t i2c_hz = 100000u;     // i2c speed

void i2c_init() {
  if (i2c_enabled_flag) {
    return;
  }

  RCC->APB1ENR |=
      RCC_APB1ENR_I2C1EN; // Enable clock for the peripheral bus that I2C is on

  // PB8 and PB9 to be pulled up
  // GPIOB->PUPDR &= ~((3u << 16u) | (3u << 18u));
  // GPIOB->PUPDR |= ((1u << 16u) | (1u << 18u));

  // PB8 and PB9 to NOT be pulled up
  GPIOB->PUPDR &= ~((3u << 16u) | (3u << 18u));

  // PB8 = I2C1_SCL
  // PB9 = I2C1_SDA

  GPIOB->MODER &= ~((3u << 16u | 3u << 18u)); // Clear PB8 and PB9
  GPIOB->MODER |=
      (2u << 16u | 2u << 18u); // Set PB8 and PB9 to alternate function mode

  GPIOB->OTYPER |= ((1u << 8u | 1u << 9u)); // set SCL and SDA to open-drain

  GPIOB->AFR[1] &= ~(0xFu << (0 * 4)); // clear alternate function for PB8
  GPIOB->AFR[1] &= ~(0xFu << (1 * 4)); // clear alternate function for PB9
  GPIOB->AFR[1] |= (4u << (0 * 4));    // set to alternate function 4
  GPIOB->AFR[1] |= (4u << (1 * 4));    // set to alternate function 4
  // reset I2C peripheral
  I2C1->CR1 |= I2C_CR1_SWRST;
  I2C1->CR1 &= ~I2C_CR1_SWRST;

  // set I2C clock frequency to match APB1 (16MHz)
  I2C1->CR2 &= ~I2C_CR2_FREQ; // FREQ = 16
  I2C1->CR2 |= 16u;

  I2C1->CR1 &=
      ~(I2C_CR1_PE); // disable PE before configuring clock configure register

  I2C1->CCR = pclk1_hz / (2u * i2c_hz); // configure CCR (80 cycles)

  I2C1->TRISE = 17u; // Set the rise time idfk to why 17 but it is 17

  I2C1->CR1 |= I2C_CR1_PE; // re-enable PE after configuring CCR

  i2c_enabled_flag++;
}

// Wait for a bit to be set in status register
void wait_for_sr1(uint32_t mask) {
  while (!(I2C1->SR1 & mask)) {
  }
}

void clear_sr() {
  // clear status registers
  // This clears the ADDR bit in SR1
  (void)I2C1->SR1;
  (void)I2C1->SR2;
}

void enable_peripheral(uint8_t addr, char rw) {
  // Enable a peripheral given its address for either transmitting or receiving
  if (rw == 'r') {
    I2C1->DR = ((addr << 1) | 1u); // read
  } else if (rw == 'w') {
    I2C1->DR = ((addr << 1) & ~(1u)); // write
  } else {
    uart_printstr("I2C: Invalid read/write character\n");
    return;
  }

  wait_for_sr1(I2C_SR1_ADDR); // Wait for address reception
}

void i2c_write_reg_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {

  // make sure previous transaction released I2C bus
  while (I2C1->SR2 & I2C_SR2_BUSY) {
  }

  I2C1->CR1 |= I2C_CR1_START; // Generate a start
  // wait for start bit to be set and read SR1
  while (!(I2C1->SR1 & I2C_SR1_SB)) {
  }

  // send mpu address + write bit
  I2C1->DR = i2c_addr << 1u;

  wait_for_sr1(I2C_SR1_ADDR); // wait for address reception
  clear_sr();                 // clear address

  // wait for TXE bit to be set
  while (!(I2C1->SR1 & I2C_SR1_TXE)) {
  }
  I2C1->DR = reg_addr; // send MPU register address

  // wait for TXE bit to be set again
  while (!(I2C1->SR1 & I2C_SR1_TXE)) {
  }

  I2C1->DR = data; // send value to be set in register address

  // wait for BTF to be set
  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  I2C1->CR1 |= I2C_CR1_STOP; // stop request, clears TxE and BTF
}

uint8_t i2c_read_reg_byte(uint8_t i2c_addr, uint8_t reg_addr) {

  // make sure previous transaction released I2C bus
  while (I2C1->SR2 & I2C_SR2_BUSY) {
  }

  // Generate a start
  I2C1->CR1 |= I2C_CR1_START;
  wait_for_sr1(I2C_SR1_SB);

  enable_peripheral(i2c_addr, 'r');
  clear_sr();

  wait_for_sr1(I2C_SR1_TXE);
  I2C1->DR = reg_addr; // Send out the register address

  // Wait for register address to finish transmitting
  wait_for_sr1(I2C_SR1_BTF);

  // Generate ANOTHER start
  I2C1->CR1 |= I2C_CR1_START;
  wait_for_sr1(I2C_SR1_SB);

  enable_peripheral(i2c_addr, 'r'); // Enable for reading

  // Send a NACK
  I2C1->CR1 &= ~(I2C_CR1_ACK);
  clear_sr();

  I2C1->CR1 |= I2C_CR1_STOP; // send stop

  // Wait for RxNE to indicate theres data in the DR
  wait_for_sr1(I2C_SR1_RXNE);

  // Read data register (clears RxNE btw)
  uint8_t ret = (uint8_t)I2C1->DR;

  while (I2C1->CR1 & I2C_CR1_STOP) {
  }

  return ret;
}
/*
 * Read an arbitrary amount of bytes from an i2c slave
 * If not reading from a register, set reg_addr to be negative.
 *
 * @param uint8_t i2c_addr - Address of the I2C peripheral
 * @param void* result - Buffer to store packet data in
 * @param int nbytes - Buffer size in bytes
 * */
void i2c_read(uint8_t i2c_addr, int16_t reg_addr, void *result,
              uint32_t nbytes) {

  if (reg_addr >= 0) {
    // Tell the peripheral what register we want to read from

    // Generate a start
    I2C1->CR1 |= I2C_CR1_START;
    wait_for_sr1(I2C_SR1_SB);

    enable_peripheral(i2c_addr, 'w');
    clear_sr();

    wait_for_sr1(I2C_SR1_TXE);
    I2C1->DR = (uint8_t)reg_addr; // Send out the register address

    // Wait for register address to finish transmitting
    wait_for_sr1(I2C_SR1_BTF);
  }

  // Generate a start
  I2C1->CR1 |= I2C_CR1_START;
  wait_for_sr1(I2C_SR1_SB);

  enable_peripheral(i2c_addr, 'r');

  if (nbytes == 1) {
    _i2c_read_1_byte(i2c_addr, result);
  } else if (nbytes == 2) {
    _i2c_read_2_bytes(i2c_addr, result);
  } else {
    _i2c_read_n_bytes(i2c_addr, result, nbytes);
  }

  // wait_for_sr1(I2C_SR1_TXE);
}

void _i2c_read_1_byte(uint8_t i2c_addr, void *result) {
  // Send a NACK
  I2C1->CR1 &= ~(I2C_CR1_ACK);

  // Clears ADDR bit
  clear_sr();

  I2C1->CR1 |= I2C_CR1_STOP; // send stop

  // Wait for RxNE to indicate theres data in the DR
  wait_for_sr1(I2C_SR1_RXNE);

  // Read data register (clears RxNE btw)
  *(uint8_t *)result = (uint8_t)I2C1->DR;

  // Set ACK High
  I2C1->CR1 |= I2C_CR1_ACK;
}
void _i2c_read_2_bytes(uint8_t i2c_addr, void *result) {
  // Using sequence described in p.753 of RM-0390-stm32f446xx
  I2C1->CR1 &= ~(I2C_CR1_ACK);
  I2C1->CR1 |= I2C_CR1_POS;

  // Clears ADDR bit
  clear_sr();

  // Wait until BTF is high
  wait_for_sr1(I2C_SR1_BTF);

  // Set stop high
  I2C1->CR1 |= I2C_CR1_STOP;

  // Read data
  *(uint8_t *)result = I2C1->DR;
  *((uint8_t *)result + 1) = I2C1->DR;

  // Set ACK High and POS low
  I2C1->CR1 |= I2C_CR1_ACK;
  I2C1->CR1 &= ~(I2C_CR1_POS);
}
void _i2c_read_n_bytes(uint8_t i2c_addr, void *result, uint32_t nbytes) {
  // Using sequence described in p.753 of RM-0390-stm32f446xx
  
  // Set ACK High
  I2C1->CR1 |= I2C_CR1_ACK;

  // Clears ADDR bit
  clear_sr();
  // Read until N - 3 bytes read 
  int i = 0;
  while (i < nbytes - 3) {
    wait_for_sr1(I2C_SR1_RXNE);
    *((uint8_t *)result + i++) = (uint8_t)I2C1->DR;
  }

  // Wait for BTF. N-2 in DR, N-1 in SR
  wait_for_sr1(I2C_SR1_BTF);

  // Send a NACK
  I2C1->CR1 &= ~(I2C_CR1_ACK);
  
  // Read N-2
  *((uint8_t *)result + i++) = (uint8_t)I2C1->DR;

  // Wait for BTF. N-1 in DR, N in SR
  wait_for_sr1(I2C_SR1_BTF);

  // Send stop
  I2C1->CR1 |= I2C_CR1_STOP; // stop request, clears TxE and BTF

  // Read N-1 and N
  *((uint8_t *)result + i++) = (uint8_t)I2C1->DR;
  *((uint8_t *)result + i) = (uint8_t)I2C1->DR;




  //
  //   // Read up to the n-1th byte
  //   for (int i = 0; i < nbytes - 1; i++) {
  //     // Wait for RxNE to indicate theres data in the DR
  //     wait_for_sr1(I2C_SR1_RXNE);
  //
  //     // Read data register (clears RxNE btw)
  //     arr[i] = (uint8_t)I2C1->DR;
  //   }
  //
  //   // Send a NACK
  //   I2C1->CR1 &= ~(I2C_CR1_ACK);
  //   clear_sr();
  //
  //   // Read the last byte
  //   arr[nbytes - 1] = (uint8_t)I2C1->DR;
  //
  //   I2C1->CR1 |= I2C_CR1_STOP; // send stop
  //
  //   while (I2C1->CR1 & I2C_CR1_STOP) {
  //   }
}

void i2c_write(uint8_t i2c_addr, int16_t reg_addr, uint8_t *payload,
               uint32_t size) {
  // make sure previous transaction released I2C bus
  while (I2C1->SR2 & I2C_SR2_BUSY) {
  }

  I2C1->CR1 |= I2C_CR1_START; // Generate a start

  // wait for start bit to be set and read SR1
  wait_for_sr1(I2C_SR1_SB);

  // Enable peripheral for writing
  enable_peripheral(i2c_addr, 'w');

  // Clears ADDR bit
  clear_sr();

  for (int i = 0; i < size; i++) {
    wait_for_sr1(I2C_SR1_TXE);
    I2C1->DR = payload[i]; // send value to be set in register address
  }
  wait_for_sr1(I2C_SR1_BTF); // wait for BTF to be set
                             //
  I2C1->CR1 |= I2C_CR1_STOP; // stop request, clears TxE and BTF
  // while (I2C1->CR1 & I2C_CR1_STOP) {
  // } // make sure stop is sent
}
