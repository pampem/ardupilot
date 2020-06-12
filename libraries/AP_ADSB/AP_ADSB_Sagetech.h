#pragma once

/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AP_ADSB_Backend.h"
#include <AP_SerialManager/AP_SerialManager.h>

class AP_ADSB_Sagetech : public AP_ADSB_Backend {
public:
    // constructor
    AP_ADSB_Sagetech(AP_ADSB &adsb);

    void init() override;
    void update() override;

private:

    static const uint8_t PAYLOAD_XP_MAX_SIZE  = 52;
    //static const uint8_t PAYLOAD_MX_MAX_SIZE  = 250;

    const char* _GcsHeader = "Sagetech: ";

    enum MsgTypes_XP {
        INVALID                 = 0,
        Installation_Set        = 0x01,
        Preflight_Set           = 0x02,
        Operating_Set           = 0x03,
        GPS_Set                 = 0x04,
        Request                 = 0x05,

        ACK                     = 0x80,
        Installatioin_Response  = 0x81,
        Preflight_Response      = 0x82,
        Status_Response         = 0x83,
        ADSB_StateVector_Report = 0x91,
        ADSB_ModeStatus_Report  = 0x92,
        TISB_StateVector_Report = 0x93,
        TISB_ModeStatus_Report  = 0x94,
        TISB_CorasePos_Report   = 0x95,
        TISB_ADSB_Mgr_Report    = 0x96,
    };

    enum Protocol {
        NONE        = 0,
        XP          = 1,
        MX          = 2,
    } protocol;

    enum ParseState {
        WaitingFor_Start,
        WaitingFor_AssmAddr,
        WaitingFor_MsgType,
        WaitingFor_MsgId,
        WaitingFor_PayloadLen,
        WaitingFor_PayloadContents,
        WaitingFor_ChecksumFletcher,
        WaitingFor_Checksum,
        WaitingFor_End,
    };

    struct Packet_XP {
        const uint8_t   start = 0xA5;
        const uint8_t   assemAddr = 0x01;
        MsgTypes_XP  type;
        uint8_t         id;
        uint8_t         payload_length;
        uint8_t         payload[PAYLOAD_XP_MAX_SIZE];
        uint8_t         checksumFletcher;
        uint8_t         checksum;
        const uint8_t   end = 0x5A;
    };

    struct {
        ParseState      state;
        uint8_t         index;
        Packet_XP       packet;
    } message_in_xp;

    // protocol XP
    bool checksum_XP(Packet_XP &msg);
    bool parse_byte_XP(const uint8_t data);
    void parse_packet_XP(const Packet_XP &msg);
    void send_msg(Packet_XP &msg);

    // protocol MX
    bool parse_byte_MX(const uint8_t data) { return false; }
    void parse_packet_MX() {}


    void send_Installation();
    void send_PreFlight();
    void send_Operating();
    void send_GPS();
    void send_Request();

    AP_HAL::UARTDriver *uart;
    uint32_t        last_packet_GPS_ms;
    MsgTypes_XP     queued_packet_type = MsgTypes_XP::INVALID;
    uint32_t        last_packet_send_ms;
    MsgTypes_XP     response_expected_type = MsgTypes_XP::INVALID;
    uint32_t        response_timeout_count;

};

