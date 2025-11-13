#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "llnseConnImpl.h"


//=============================================================================
// This macros throws exceptions, passing the filename, line number, and error
// message to the exception constructor
//=============================================================================
#define THROW_EXCEPTION(exception, fmt, ...)                 \
    do                                                       \
    {                                                        \
        const std::string file(__FILE__);                    \
        const uint32_t line = __LINE__;                      \
        const std::string what = va_fmt(fmt, __VA_ARGS__);   \
        throw exception(file, line, what);                   \
    } while(false);
//=============================================================================


//=============================================================================
// va_fmt() - Formats variable argument string
//=============================================================================
static std::string va_fmt(const char* fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, ap);
    va_end(ap);
    return buffer;
};
//=============================================================================


namespace llnse {

//=============================================================================
// connect() - Connects to the server on the specified channel
//=============================================================================
void Connection::Impl::connect(const std::string fifo_folder, uint8_t channel)
{
    int rc;
    char cosi_fn[256], ciso_fn[256], signal_fifo[256];
    uint32_t magic;

    // Fetch the name of the folder where our named FIFOS live
    const char* folder = fifo_folder.c_str();

    // Build the COSI (Client-Out-Slave-In) FIFO filename
    sprintf(cosi_fn, "%s/ch%i.cosi.fifo", folder, channel);

    // Build the CISO (Client-In-Slave-Out) FIFO filename
    sprintf(ciso_fn, "%s/ch%i.ciso.fifo", folder, channel);

    // Create the COSI FIFO
    rc = mkfifo(cosi_fn, 0666);
    if (rc != 0 && errno != EEXIST)
    {
        THROW_EXCEPTION(llnse::runtime_error, "Failed to create %s", cosi_fn);
    }

    // Create the CISO FIFO
    rc = mkfifo(ciso_fn, 0666);
    if (rc != 0 && errno != EEXIST)
    {
        THROW_EXCEPTION(llnse::runtime_error, "Failed to create %s", ciso_fn);
    }

    // Build the name of the FIFO we use to inform the server that
    // we'd like to connect
    sprintf(signal_fifo, "%s/signal.fifo", folder);

    // Connect to the signal FIFO and request an open channel
    int fd = open(signal_fifo, O_WRONLY);
    if (fd < 0)
    {
        THROW_EXCEPTION(llnse::runtime_error, "Cant open %s", signal_fifo);
    }
    rc = write(fd, &channel, 1);
    close(fd);

    // Open the COSI half of the channel
    cosi_fd_ = open(cosi_fn, O_WRONLY);
    if (cosi_fd_< 0)
    {
        THROW_EXCEPTION(llnse::runtime_error, "Failed to open %s", cosi_fn);
    }

    // Open the COSI half of the channel
    ciso_fd_ = open(ciso_fn, O_RDONLY);
    if (ciso_fd_ < 0)
    {
        THROW_EXCEPTION(llnse::runtime_error, "Failed to open %s", ciso_fn);
    }


    // Read the magic number
    rc = read(ciso_fd_, &magic, 4);

    // If the magic number doesn't match our desired message version,
    // complain
    if (magic != llnse::MESSAGE_VERSION)
    {
        THROW_EXCEPTION
        (
            llnse::runtime_error,
            "Need version %i - server has %i", MESSAGE_VERSION, magic
        );
    }

}
//=============================================================================


//=============================================================================
// rpc() - Remote Procedure Call on the server
//=============================================================================
void Connection::Impl::rpc(base_msg_t& req, base_msg_t& rsp)
{
    int rc;

    fault_rsp_t message;
    const uint32_t HEADER_SIZE = sizeof(base_msg_t);
    char* payload;
    uint32_t payload_len;

    // Ensure single threaded access to this connection
    std::lock_guard<std::mutex> lock(mutex_);

    // Send our requests
    rc = write(cosi_fd_, &req, req.msglen);

    // Read the response header
    rc = read(ciso_fd_, &message, HEADER_SIZE);

    // If that failed, throw an error
    if (rc != HEADER_SIZE)
    {
        THROW_EXCEPTION
        (
            llnse::runtime_error,
            "On message %i, failed to read response header",
            (uint16_t)req.msgtype
        );
    }

    // Determine how long we expect the response message to be
    uint32_t expected_length = (message.msgtype == MSG_FAULT) ?
               sizeof(fault_rsp_t) : rsp.msglen;

    // Make sure the length of the response message is exactly the
    // number of bytes we're expecting
    if (message.msglen != expected_length)
    {
        THROW_EXCEPTION
        (
            llnse::runtime_error,
            "On message %i, expected %i bytes and found %i",
            (uint16_t)req.msgtype,
            rsp.msglen,
            message.msglen
        );
    }

    // If the header we just fetched is a fault header,
    // then we need to fetch the fault payload, otherwise
    // we need to fetch the expected response payload
    if (message.msgtype == MSG_FAULT)
    {
        payload = ((char*)&message) + HEADER_SIZE;
        payload_len = sizeof(message) - HEADER_SIZE;
    }
    else
    {
        payload = ((char*)&rsp) + HEADER_SIZE;
        payload_len = rsp.msglen - HEADER_SIZE;
    }

    // Read the message payload
    rc = read(ciso_fd_, payload, payload_len);

    // If we couldn't read the payload, fail
    if (rc != payload_len)
    {
        THROW_EXCEPTION
        (
            llnse::runtime_error,
            "On message %i, failed to read response payload",
            (uint16_t)req.msgtype
        );
    }

    // If the response message was a fault, throw an exception
    if (message.msgtype == MSG_FAULT)
    {
        THROW_EXCEPTION
        (
            llnse::runtime_error,
            "%i: %s", message.error, message.text
        );
    }

}
//=============================================================================



//=============================================================================
// Constructor and destructor
//=============================================================================
Connection::Connection() : p_impl(std::make_unique<Connection::Impl>()) {};
Connection::~Connection() = default;
//=============================================================================


//=============================================================================
// connect() - Connects to the llnse server. 
//=============================================================================
void Connection::connect(std::string fifodir, uint8_t channel)
{
    p_impl->connect(fifodir, channel);
}
//=============================================================================



}