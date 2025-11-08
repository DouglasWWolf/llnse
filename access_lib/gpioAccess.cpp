#include <cstring>
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

//=============================================================================
// getRtl() - Returns identifying information about the RTL build
//=============================================================================
rtl_t CGpioAccess::getRtl()
{
    rtl_t rtl;

    MAKE_STRUCTS(llnse::get_rtl, llnse::MSG_GET_RTL);
    conn_.p_impl->rpc(req, rsp);

    // Copy the various fields from response message into the structure
    // that we are going to return to the caller
    strcpy(rtl.date,    rsp.date);
    strcpy(rtl.time,    rsp.time);
    strcpy(rtl.hash,    rsp.hash);
    strcpy(rtl.version, rsp.version);
    rtl.type    = rsp.type;
    rtl.subtype = rsp.subtype;

    // Hand the RTL identity structure to the caller
    return rtl;
}
//=============================================================================



} /* End of namespace */
