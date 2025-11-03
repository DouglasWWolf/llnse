#include "gpioAccessApi.h"
#include "llnseConnImpl.h"

namespace gpioAccess {


//=============================================================================
// Constructor and destructor
//=============================================================================
CGpioAccess::CGpioAccess(llnse::Connection& conn) : conn_(conn) {}
CGpioAccess::~CGpioAccess() = default;
//=============================================================================


//=============================================================================
// A convenience method for declaring request and response structures
//=============================================================================
#define MAKE_STRUCTS(structure, msg_id) \
    structure##_req_t req;              \
    structure##_rsp_t rsp;              \
    req.msglen  = sizeof(req);          \
    rsp.msglen  = sizeof(rsp);          \
    req.msgtype = msg_id
//=============================================================================


//=============================================================================
// fault() - This will force the server to generate a fault response
//=============================================================================
void CGpioAccess::fault(uint32_t error)
{
    MAKE_STRUCTS(llnse::fault, llnse::MSG_FAULT);
    req.error = error;
    conn_.p_impl->rpc(req, rsp);
}
//=============================================================================


//=============================================================================
// ping() - The server (hopefully) returns the same value we send it
//=============================================================================
uint32_t CGpioAccess::ping(uint32_t value)
{
    MAKE_STRUCTS(llnse::ping, llnse::MSG_PING);
    req.value = value;
    conn_.p_impl->rpc(req, rsp);
    return rsp.value;
}
//=============================================================================



//=============================================================================
// setLeds() - Sets the 4 GPIO on-board LEDs to the defined bit pattern
//=============================================================================
void CGpioAccess::setLeds(uint32_t value)
{
    MAKE_STRUCTS(llnse::set_leds, llnse::MSG_SET_LEDS);
    req.value = value;
    conn_.p_impl->rpc(req, rsp);
}
//=============================================================================


//=============================================================================
// getSwitches() - Returns the 4 GPIO on-board DIP switches
//=============================================================================
uint32_t CGpioAccess::getSwitches()
{
    MAKE_STRUCTS(llnse::get_switches, llnse::MSG_GET_SWITCHES);
    conn_.p_impl->rpc(req, rsp);
    return rsp.value;
}
//=============================================================================



} /* End of namespace */
