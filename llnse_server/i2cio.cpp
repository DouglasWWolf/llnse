#include <unistd.h>
#include <cstdarg>
#include "i2cio.h"


void throwRuntime(const char* fmt, ...);


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
void CI2CIO::write(uint8_t deviceID, uint8_t dataBytes, uint32_t dataValue)
{
    writeRegister(deviceID, 0, 0, dataBytes, dataValue);
}
//=============================================================================


//=============================================================================
// This writes data to a register of a device in the I2C bus
//=============================================================================
void CI2CIO::writeRegister(uint8_t deviceID,
                           uint8_t addrBytes, uint32_t addrValue, 
                           uint8_t dataBytes, uint32_t dataValue)
{
    int i;

    // Enforce single-threaded access
    const std::lock_guard<std::recursive_mutex> lock(mutex_);

    // Reset the I2C controller
    reg[IIC_SOFTR] = 0x0A;

    // Start a write transaction to the specified I2C device
    reg[IIC_TX_FIFO] = (I2C_START | (deviceID << 1) | I2C_WR);

    // Write the address bytes to TX_FIFO
    if (addrBytes) for (i=addrBytes-1; i >= 0; --i)
    {
        reg[IIC_TX_FIFO] = byte(addrValue, i);
    }

    // Write the data-bytes to TX_FIFO
    for (i=dataBytes-1; i >= 0; --i)
    {
        if (i)
            reg[IIC_TX_FIFO] = byte(dataValue, i);
        else
            reg[IIC_TX_FIFO] = byte(dataValue, i) | I2C_STOP;
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
            throwRuntime("I2C TX fault on device 0x%02X", deviceID);    
        }

        // If the I2C bus is no longer busy, we're done
        else if ((reg[IIC_SR] & I2C_BB) == 0) 
        {
            return;
        }

    }

    // The bus never went idle!
    throwRuntime("I2C TX timeout on device 0x%02X", deviceID);    
}
//=============================================================================



//=============================================================================
// This writes data to a register of a device in the I2C bus
//=============================================================================
uint32_t CI2CIO::readRegister(uint8_t deviceID,
                              uint8_t addrBytes, uint32_t addrValue, 
                              uint8_t dataBytes, bool stopBeforeRead)
{
    int i;
    uint32_t retval = 0;

    // Enforce single-threaded access
    const std::lock_guard<std::recursive_mutex> lock(mutex_);

    // Reset the I2C controller
    reg[IIC_SOFTR] = 0x0A;

    // Start a write transaction to the specified I2C device
    reg[IIC_TX_FIFO] = (I2C_START | (deviceID << 1) | I2C_WR);
    
    // Write the address bytes to TX_FIFO
    for (i=addrBytes-1; i >= 0; --i)
    {
        if (i == 0 && stopBeforeRead)
            reg[IIC_TX_FIFO] = byte(addrValue, i) | I2C_STOP;
        else
            reg[IIC_TX_FIFO] = byte(addrValue, i);        
    }

    // We want to read data from the device
    reg[IIC_TX_FIFO] = (I2C_START | (deviceID << 1) | I2C_RD);
 
    // Tell the I2C how many bytes of data to read
    reg[IIC_TX_FIFO] = dataBytes | I2C_STOP;

    // Start the I2C transaction
    reg[IIC_CR] = I2C_EN;    

    // Wait for the transaction to complete
    for (i=0; i<1000; ++i)
    {
        usleep(10);
        if (reg[IIC_ISR & I2C_RX_DONE]) break;
    }
    
    // Build the return value by reading bytes from the RX_FIFO
    for (i=0; i<dataBytes; ++i)
    {
        retval = (retval << 8) | receiveByte(1000, deviceID);
    }

    // Hand the resulting value to the caller
    return retval;
}
//=============================================================================


//=============================================================================
// receiveByte() - Waits for a byte to arrive in the RX_FIFO
//=============================================================================
uint8_t CI2CIO::receiveByte(uint32_t timeout_us, uint8_t deviceID)
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

    throwRuntime("I2C Read fault on device 0x%02X", deviceID);

    // This is here to keep the compiler happy
    return 0;
}
//=============================================================================
