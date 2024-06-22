#include <Arduino.h>

#define F_CPU 16000000UL    // CPU frequency
#define SCL_CLOCK 100000L    // I2C clock frequency
#define RTC_ADDRESS 0x68    // I2C address for DS3231 RTC module
#define SEC_REGISTER 0x00   // RTC register address for seconds
#define MIN_REGISTER 0x01   // RTC register address for minutes
#define HOUR_REGISTER 0x02  // RTC register address for hours
#define MAX_STRING_LENGTH 100 // Maximum length for received strings


// Function prototypes
void I2C_Init(void);
uint8_t I2C_Start(void);
uint8_t I2C_Stop(void);
uint8_t I2C_Write(uint8_t data);
uint8_t I2C_ReadAck(void);
uint8_t I2C_ReadNack(void);
void I2C_WriteData(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data);
uint8_t I2C_ReadData(uint8_t deviceAddress, uint8_t registerAddress);
void uart_init(void);
void uart_transmit(unsigned char data);
void uart_transmit_string(const char *str);
unsigned char uart_receive(void);
void uart_receive_string(char *buffer);
void uart_transmit_number(uint8_t number);
uint8_t decToBcd(uint8_t val);
uint8_t bcdToDec(uint8_t val);
void display_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t isPM);

// I2C functions
void I2C_Init(void)
{
  TWSR = 0x00;                           // Set prescaler to 1
  TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2; // Set bit rate register
  TWCR = (1 << TWEN);                    // Enable TWI
}

uint8_t I2C_Start(void)
{
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Send START condition
  while (!(TWCR & (1 << TWINT)))
    ;                   // Wait for TWINT flag to be set
  return (TWSR & 0xF8); // Return status
}

uint8_t I2C_Stop(void)
{
  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); // Send STOP condition
  while (TWCR & (1 << TWSTO))
    ;                   // Wait for STOP to be executed
  return (TWSR & 0xF8); // Return status
}

uint8_t I2C_Write(uint8_t data)
{
  TWDR = data;                       // Load data into data register
  TWCR = (1 << TWEN) | (1 << TWINT); // Start transmission
  while (!(TWCR & (1 << TWINT)))
    ;                   // Wait for TWINT flag to be set
  return (TWSR & 0xF8); // Return status
}

uint8_t I2C_ReadAck(void)
{
  TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); // Enable ACK
  while (!(TWCR & (1 << TWINT)))
    ;          // Wait for TWINT flag to be set
  return TWDR; // Return received data
}

uint8_t I2C_ReadNack(void)
{
  TWCR = (1 << TWEN) | (1 << TWINT); // Enable NACK
  while (!(TWCR & (1 << TWINT)))
    ;          // Wait for TWINT flag to be set
  return TWDR; // Return received data
}

void I2C_WriteData(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data)
{
  I2C_Start();
  I2C_Write(deviceAddress << 1); // Write address of the I2C device with the write bit (0)
  I2C_Write(registerAddress);    // Write the register address
  I2C_Write(data);               // Write the data to the register
  I2C_Stop();
}

uint8_t I2C_ReadData(uint8_t deviceAddress, uint8_t registerAddress)
{
  uint8_t data;

  I2C_Start();
  I2C_Write(deviceAddress << 1); // Write address of the I2C device with the write bit (0)
  I2C_Write(registerAddress);    // Write the register address
  I2C_Start();
  I2C_Write((deviceAddress << 1) | 0x01); // Write address of the I2C device with the read bit (1)
  data = I2C_ReadNack();                  // Read the data from the register
  I2C_Stop();

  return data;
}

// UART functions
void uart_init(void)
{
  // Set baud rate to 9600
  UBRR0H = 0;
  UBRR0L = 103;

  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // Set frame format: 8 data bits, 1 stop bit
  // UCSZ01 and UCSZ00 bits are already 0 (default condition)
  // UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(unsigned char data)
{
  // Wait for the transmit buffer to be empty
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into the buffer, sends the data
  UDR0 = data;
}

void uart_transmit_string(const char *str)
{
  // Transmit each character until null terminator is reached
  while (*str != '\0')
  {
    uart_transmit(*str);
    str++;
  }
}

unsigned char uart_receive(void)
{
  // Wait for data to be received
  while (!(UCSR0A & (1 << RXC0)))
    ;

  // Get and return received data from buffer
  return UDR0;
}

void uart_receive_string(char *buffer)
{
  char received_char;
  uint8_t index = 0;

  // Receive characters until '\r' or '\n' is received
  while (1)
  {
    received_char = uart_receive();
    if (received_char == '\r' || received_char == '\n')
    {
      break;
    }
    buffer[index++] = received_char;

    // Check if buffer overflow occurred
    if (index >= (MAX_STRING_LENGTH - 1))
    {
      break;
    }
  }

  // Null-terminate the string
  buffer[index] = '\0';
}

void uart_transmit_number(uint8_t number)
{
  char buffer[3]; // Create a small buffer with 3 slots to hold the formatted number

  // Place the tens digit in the first slot
  buffer[0] = '0' + (number / 10);

  // Place the ones digit in the second slot
  buffer[1] = '0' + (number % 10);

  // Place the null terminator in the third slot
  buffer[2] = '\0';

  // Transmit the buffer as a string
  uart_transmit_string(buffer);
}

// Utility functions
uint8_t decToBcd(uint8_t val)
{
  return ((val / 10 * 16) + (val % 10)); // Convert from decimal to BCD
}

uint8_t bcdToDec(uint8_t val)
{
  return ((val / 16 * 10) + (val % 16)); // Convert from BCD to decimal
}

void display_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t isPM)
{
  uart_transmit_string("Time: ");
  uart_transmit_number(hour); // Transmit formatted hours
  uart_transmit(':');
  uart_transmit_number(min); // Transmit formatted minutes
  uart_transmit(':');
  uart_transmit_number(sec); // Transmit formatted seconds
  uart_transmit_string(isPM ? " PM" : " AM");
  uart_transmit('\n'); // Move to the next line
}

// Main program
void setup()
{
  // Initialize BCD values for time (12:11:11 PM)
  uint8_t bcd_sec = decToBcd(11);
  uint8_t bcd_min = decToBcd(11);
  uint8_t bcd_hour = decToBcd(11); // Assuming initial hour is 12
  bcd_hour |= (1 << 6);           // Set bit 6 for 12-hour mode (AM/PM)
  bcd_hour |= (1 << 5);           // Set bit 5 for PM (or AM if needed)

  // Initialize the I2C communication
  I2C_Init();

  // Set initial time on RTC
  I2C_WriteData(RTC_ADDRESS, SEC_REGISTER, bcd_sec);   // Set initial seconds
  I2C_WriteData(RTC_ADDRESS, MIN_REGISTER, bcd_min);   // Set initial minutes
  I2C_WriteData(RTC_ADDRESS, HOUR_REGISTER, bcd_hour); // Set initial hour (12 PM)

  // Initialize UART communication
  uart_init();

  // Print initialization message
  uart_transmit_string("DS3231 Real-Time Clock Initialized\n");
}
void loop()
{
  uint8_t sec, min, hour; // Variables to store time values
  bool isPM;              // Variable to indicate if it's PM or AM

  // Read seconds from RTC
  sec = I2C_ReadData(RTC_ADDRESS, SEC_REGISTER);
  sec = bcdToDec(sec & 0x7F);

  // Read minutes from RTC
  min = I2C_ReadData(RTC_ADDRESS, MIN_REGISTER);
  min = bcdToDec(min & 0x7F);

  // Read hours from RTC
  hour = I2C_ReadData(RTC_ADDRESS, HOUR_REGISTER);
  isPM = (hour & 0x20);         // Check if it's PM (bit 5 of hour register)
  hour = bcdToDec(hour & 0x1F); // Extract and convert hours from BCD to decimal (mask out AM/PM bit)

  // Display time on Serial Monitor
  display_time(hour, min, sec, isPM);

  _delay_ms(1000); // Delay for 1 second (1000 milliseconds)
}

