#include "i2c.h"
#include "stm32f446xx.h"
#include "uart.h"

uint8_t i2c_enabled_flag = 0;
uint32_t pclk1_hz = 16000000u; // peripheral clock speed
uint32_t i2c_hz   = 100000u;   // i2c speed

void i2c_init() {
	if (i2c_enabled_flag) {
		return;
	}

  RCC->APB1ENR |=
      RCC_APB1ENR_I2C1EN; // Enable clock for the peripheral bus that I2C is on
	
	// PB8 and PB9 to be pulled up
	GPIOB->PUPDR &= ~((3u << 16u) | (3u << 18u));
	GPIOB->PUPDR |= ((1u << 16u) | (1u << 18u));

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

  I2C1->CR1 &= ~(I2C_CR1_PE); // disable PE before configuring clock configure register
 
  I2C1->CCR = pclk1_hz / (2u * i2c_hz); // configure CCR (80 cycles)

  I2C1->TRISE = 17u; // Set the rise time idfk to why 17 but it is 17
	
  I2C1->CR1 |= I2C_CR1_PE; // re-enable PE after configuring CCR

	i2c_enabled_flag++;
}

// Wait for a bit to be set in status register
void wait_for_sr1(uint32_t mask) {
  while (!(I2C1->SR1 & mask)) {}
}

void clear_sr() {
  // clear status registers
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
    uart_printstr("I2C: Invalid read/write character");
		return;
  }

  wait_for_sr1(I2C_SR1_ADDR); // Wait for address reception
}

void i2c_write_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {

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
    clear_sr(); // clear address

    // wait for TXE bit to be set
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {
    }
    I2C1->DR = reg_addr; // send MPU register address 
    
    // wait for TXE bit to be set again
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {
    }
    
    I2C1->DR = data; // send value to be set in register address

    // wait for BTF to be set
    while(!(I2C1->SR1 & I2C_SR1_BTF)) {
    }
    
    I2C1->CR1 |= I2C_CR1_STOP; // stop request, clears TxE and BTF
}

uint8_t i2c_read_byte(uint8_t i2c_addr, uint8_t reg_addr) {

    // make sure previous transaction released I2C bus
    while (I2C1->SR2 & I2C_SR2_BUSY) {
    }
  // Generate a start
  I2C1->CR1 |= I2C_CR1_START;
  wait_for_sr1(I2C_SR1_SB);

  enable_peripheral(i2c_addr, 'w'); 
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
