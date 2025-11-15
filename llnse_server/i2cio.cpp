#include <unistd.h>
#include <cstdarg>
#include "i2cio.h"
#include "logger.h"
#include "global.h"



//=============================================================================
// Register definitions - See AMD/Xilinx PG090 "AXI IIC Bus Interface"
//=============================================================================
const uint32_t IIC_GIE          = 0x01C / 4;
const uint32_t IIC_ISR          = 0x020 / 4;
const uint32_t IIC_IER          = 0x028 / 4;
const uint32_t IIC_SOFTR        = 0x040 / 4;
const uint32_t IIC_CR           = 0x100 / 4;
const uint32_t IIC_SR           = 0x104 / 4;
const uint32_t IIC_TX_FIFO      = 0x108 / 4;
const uint32_t IIC_RX_FIFO      = 0x10C / 4;
const uint32_t IIC_TX_FIFO_OCY  = 0x114 / 4;
const uint32_t IIC_RX_FIFO_OCY  = 0x118 / 4;
const uint32_t IIC_RX_FIFO_PIRQ = 0x120 / 4;
//=============================================================================


//=============================================================================
// Some bit definitions
//=============================================================================
#define I2C_RD          1
#define I2C_WR          0
#define I2C_START   0x100  /* In IIC_TX_FIFO */
#define I2C_STOP    0x200  /* In IIC_TX_FIFO */
#define I2C_EN          1  /* In IIC_CR      */
#define I2C_RX_EMPTY 0x40  /* In IIC_SR      */
#define I2C_BB          4  /* in IIC_SR      */
#define I2C_ERR_ARB     1  /* In IIC_ISR     */
#define I2C_ERR_TX      2  /* In IIC_ISR     */
#define I2C_RX_DONE     2  /* In IIC_ISR     */
//=============================================================================


#define reg ((volatile uint32_t*)iobase_)


//=============================================================================
// byte() - Extracts the specified byte from a 32-bit value
//=============================================================================
static uint8_t byte(uint32_t value, int which)
{
    switch(which)
    {
        case 3: return (value >> 24) & 0xFF;
        case 2: return (value >> 16) & 0xFF;
        case 1: return (value >>  8) & 0xFF;
        case 0: return (value      ) & 0xFF;
        default: return 0;
    }
}
//=============================================================================



//=============================================================================
// A convenience method for writing data to a device
//=============================================================================
void CI2CIO::write(uint8_t device_id, uint8_t data_bytes, uint32_t data_value)
{
    write_register(device_id, 0, 0, data_bytes, data_value);
}
//=============================================================================


//=============================================================================
// This writes data to a register of a device in the I2C bus
//=============================================================================
void CI2CIO::write_register(uint8_t device_id,
                            uint8_t addr_bytes, uint32_t addr_value, 
                            uint8_t data_bytes, uint32_t data_value)
{
    int i;
    std::string s;

    // Enforce single-threaded access
    const std::lock_guard<std::recursive_mutex> lock(mutex_);

    // Save the device ID for error message
    device_id_ = device_id;

    // Reset the I2C controller
    reg[IIC_SOFTR] = 0x0A;

    // Start a write transaction to the specified I2C device
    reg[IIC_TX_FIFO] = (I2C_START | (device_id << 1) | I2C_WR);

    // Write the address bytes to TX_FIFO
    if (addr_bytes) for (i=addr_bytes-1; i >= 0; --i)
    {
        reg[IIC_TX_FIFO] = byte(addr_value, i);
    }

    // Write the data-bytes to TX_FIFO
    for (i=data_bytes-1; i >= 0; --i)
    {
        if (i)
            reg[IIC_TX_FIFO] = byte(data_value, i);
        else
            reg[IIC_TX_FIFO] = byte(data_value, i) | I2C_STOP;
    }

    // Start the I2C transaction
    reg[IIC_CR] = I2C_EN;    

    // Sit in a loop for either 10msec, or until the IIC_SR and 
    // IIC_ISR registers tell us that the transaction is complete
    for (i=0; i<1000; ++i)
    {
        usleep(10);
        if (reg[IIC_ISR] & (I2C_ERR_ARB | I2C_ERR_TX))
        {
            s = format("I2C TX fault on bus %i, device 0x%02X", bus_id_, device_id);
            LOG_FATAL(s);
            throwRuntime(s);    
        }

        // If the I2C bus is no longer busy, we're done
        else if ((reg[IIC_SR] & I2C_BB) == 0) 
        {
            return;
        }

    }

    // The bus never went idle!
    s = format("I2C TX timeout on bus %i, device 0x%02X", bus_id_, device_id);
    LOG_FATAL(s);
    throwRuntime(s);    
}
//=============================================================================


//=============================================================================
// This does a raw read of the specified device
//=============================================================================
uint32_t CI2CIO::read(uint8_t device_id, uint8_t data_bytes)
{
    return read_register(device_id, 0, 0, data_bytes, false);    
}
//=============================================================================


//=============================================================================
// This writes data to a register of a device in the I2C bus
//=============================================================================
uint32_t CI2CIO::read_register(uint8_t device_id,
                               uint8_t addr_bytes, uint32_t addr_value, 
                               uint8_t data_bytes, bool stop_before_read)
{
    int i;
    uint32_t retval = 0;

    // Enforce single-threaded access
    const std::lock_guard<std::recursive_mutex> lock(mutex_);

    // Save the device ID for error message
    device_id_ = device_id;

    // Reset the I2C controller
    reg[IIC_SOFTR] = 0x0A;

    // If there's an address, start a write transaction
    if (addr_bytes)
    {
        // Start a write transaction to the specified I2C device
        reg[IIC_TX_FIFO] = (I2C_START | (device_id << 1) | I2C_WR);
    
        // Write the address bytes to TX_FIFO
        for (i=addr_bytes-1; i >= 0; --i)
        {
            if (i == 0 && stop_before_read)
                reg[IIC_TX_FIFO] = byte(addr_value, i) | I2C_STOP;
            else
                reg[IIC_TX_FIFO] = byte(addr_value, i);        
        }
    }

    // We want to read data from the device
    reg[IIC_TX_FIFO] = (I2C_START | (device_id << 1) | I2C_RD);
 
    // Tell the I2C how many bytes of data to read
    reg[IIC_TX_FIFO] = data_bytes | I2C_STOP;

    // Start the I2C transaction
    reg[IIC_CR] = I2C_EN;    

    // Wait for the transaction to complete
    for (i=0; i<1000; ++i)
    {
        usleep(10);
        if (reg[IIC_ISR & I2C_RX_DONE]) break;
    }
    
    // Build the return value by reading bytes from the RX_FIFO
    for (i=0; i<data_bytes; ++i)
    {
        retval = (retval << 8) | receive_byte(1000);
    }

    // Hand the resulting value to the caller
    return retval;
}
//=============================================================================


//=============================================================================
// receive_byte() - Waits for a byte to arrive in the RX_FIFO
//=============================================================================
uint8_t CI2CIO::receive_byte(uint32_t timeout_us)
{
    uint32_t elapsed_us = 0;

    while (elapsed_us <= timeout_us)
    {
        if ((reg[IIC_SR] & I2C_RX_EMPTY) == 0)
        {
            return reg[IIC_RX_FIFO];
        }

        usleep(10);
        elapsed_us += 10;
    }

    // Log and throw the fault
    std::string s = format("I2C RX fault on bus %i, device 0x%02X", bus_id_, device_id_);
    LOG_FATAL(s);
    throwRuntime(s);

    // This is here to keep the compiler happy
    return 0;
}
//=============================================================================
