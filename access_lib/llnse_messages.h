#pragma once
#include <stdint.h>
#include <stdexcept>

namespace llnse
{

    // If a message is added, deleted, or changed, or if a message-type
    // is added, deleted, or changes, this number MUST be incremented!
    const uint32_t MESSAGE_VERSION = 1;

    // This is the maximum length of any message
    const uint32_t MAX_MSG_LENGTH = 128;

    // Message types
    enum msgtype_t : uint16_t
    {
        MSG_FAULT = 0,
        MSG_PING,
        MSG_SET_LEDS,
        MSG_GET_SWITCHES        
    };

    // This is the maximum size of a message
    #pragma pack(push, 1)
    struct base_msg_t
    {
        uint16_t    msglen;
        msgtype_t   msgtype;
    };

 
    struct fault_req_t : public base_msg_t
    {
        uint32_t    error;        
    };
    struct fault_rsp_t : public base_msg_t
    {
        uint32_t    error;        
    };


    struct ping_req_t : public base_msg_t
    {
        uint32_t    value;
    };
    struct ping_rsp_t : public base_msg_t
    {
        uint32_t    value;
    };


    struct set_leds_req_t : public base_msg_t
    {
        uint32_t    value;
    };
    struct set_leds_rsp_t : public base_msg_t
    {
    };

    struct get_switches_req_t : public base_msg_t
    {
    };
    struct get_switches_rsp_t : public base_msg_t
    {
        uint32_t value;
    };


    #pragma pack(pop)    
}