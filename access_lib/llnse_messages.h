#pragma once
#include <stdint.h>
#include <stdexcept>

namespace llnse
{

    // If a message is added, deleted, or changed, or if a message-type
    // is added, deleted, or changes, this number MUST be incremented!
    const uint32_t MESSAGE_VERSION = 2;

    // This is the maximum length of any message
    const uint32_t MAX_MSG_LENGTH = 256;

    // Message types
    enum msgtype_t : uint16_t
    {
        MSG_FAULT = 0,
        MSG_PING,
        MSG_SET_LEDS,
        MSG_GET_SWITCHES,
        MSG_GET_RTL,
        MSG_SET_PX0_IODIR,
        MSG_SET_PX0_PULLUP,        
        MSG_SET_PX0_GPIO,
        MSG_GET_PX0_GPIO,
        MSG_SET_PX0_EMU,
        MSG_GET_PX0_EMU,
        MSG_GET_PS200_VALUE,
        MSG_SET_PS200_EMU,
        MSG_GET_PS200_EMU
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
        char        text[MAX_MSG_LENGTH - 16];       
    };

    struct fault_rsp_t : public base_msg_t
    {
        uint32_t    error;        
        char        text[MAX_MSG_LENGTH - 16];       
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
    struct set_leds_rsp_t : public base_msg_t {};

    struct get_switches_req_t : public base_msg_t
    {
    };
    struct get_switches_rsp_t : public base_msg_t
    {
        uint32_t value;
    };

    struct get_rtl_req_t : public base_msg_t {};
    struct get_rtl_rsp_t : public base_msg_t
    {
        char     version[16];
        char     date[16];
        char     time[16];
        char     hash[64];
        uint32_t type;        
        uint32_t subtype;
    };


    struct set_px0_iodir_req_t : public base_msg_t
    {
        uint16_t inputs;        
    };
    struct set_px0_iodir_rsp_t : public base_msg_t {};


    struct set_px0_pullup_req_t : public base_msg_t
    {
        uint16_t pins;        
    };
    struct set_px0_pullup_rsp_t : public base_msg_t {};


    struct set_px0_gpio_req_t : public base_msg_t
    {
        uint16_t pins;        
    };
    struct set_px0_gpio_rsp_t : public base_msg_t {};


    struct get_px0_gpio_req_t : public base_msg_t {};
    struct get_px0_gpio_rsp_t : public base_msg_t
    {
        uint16_t pins;        
    };


    struct set_px0_emu_req_t : public base_msg_t
    {
        bool     flag;
        uint16_t input_pins;
    };
    struct set_px0_emu_rsp_t : public base_msg_t {};


    struct get_px0_emu_req_t : public base_msg_t {};
    struct get_px0_emu_rsp_t : public base_msg_t
    {
        bool     flag;
        uint16_t input_pins;
    };


    struct get_ps200_value_req_t : public base_msg_t {};
    struct get_ps200_value_rsp_t : public base_msg_t
    {
        int32_t reading;        
    };

    struct set_ps200_emu_req_t : public base_msg_t
    {
        bool    flag;
        int32_t reading;
    };
    struct set_ps200_emu_rsp_t : public base_msg_t {};


    struct get_ps200_emu_req_t : public base_msg_t {};
    struct get_ps200_emu_rsp_t : public base_msg_t
    {
        bool    flag;
        int32_t reading;
    };

    #pragma pack(pop)    
}