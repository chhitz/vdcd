//
//  Copyright (c) 2013-2016 plan44.ch / Lukas Zeller, Zurich, Switzerland
//
//  Author: Lukas Zeller <luz@plan44.ch>
//
//  This file is part of vdcd.
//
//  vdcd is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  vdcd is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with vdcd. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef vdcd_dalidefs_h
#define vdcd_dalidefs_h

// ranges
#define DALI_MAXDEVICES 64
#define DALI_MAXGROUPS 16
#define DALI_MAXSCENES 16


// DALI commands with standard address format
// - normal commands, send once
#define DALICMD_OFF 0x00 // 0000 0000
#define DALICMD_UP 0x01 // 0000 0001
#define DALICMD_DOWN 0x02 // 0000 0010
#define DALICMD_STEP_UP 0x03 // 0000 0011
#define DALICMD_STEP_DOWN 0x04 // 0000 0100
#define DALICMD_RECALL_MAX_LEVEL 0x05 // 0000 0101
#define DALICMD_RECALL_MIN_LEVEL 0x06 // 0000 0110
#define DALICMD_STEP_DOWN_AND_OFF 0x07 // 0000 0111
#define DALICMD_ON AND_STEP_UP 0x08 // 0000 1000
#define DALICMD_ENABLE_DAPC_SEQUENCE 0x09 // 0000 1001
#define DALICMD_GO_TO_SCENE 0x10 // 0001 xxxx, x = scene number 0..15

// - configuration commands, send twice within 100mS
#define DALICMD_RESET 0x20 // 0010 0000
#define DALICMD_STORE_ACTUAL_LEVEL_IN_DTR 0x21 // 0010 0001
#define DALICMD_STORE_DTR_AS_MAX_LEVEL 0x2A // 0010 1010
#define DALICMD_STORE_DTR_AS_MIN_LEVEL 0x2B // 0010 1011
#define DALICMD_STORE_DTR_AS_FAILURE_LEVEL 0x2C // 0010 1100
#define DALICMD_STORE_DTR_AS_POWER_ON_LEVEL 0x2D // 0010 1101
#define DALICMD_STORE_DTR_AS_FADE_TIME 0x2E // 0010 1110
#define DALICMD_STORE_DTR_AS_FADE_RATE 0x2F // 0010 1111
#define DALICMD_STORE_DTR_AS_SCENE 0x40 // 0100 xxxx, x = scene number 0..15
#define DALICMD_REMOVE_FROM_SCENE 0x50 // 0101 xxxx, x = scene number 0..15
#define DALICMD_ADD_TO_GROUP 0x60 // 0110 yyyy, y = group number 0..15
#define DALICMD_REMOVE_FROM_GROUP 0x70 // 0111 yyyy, y = group number 0..15
#define DALICMD_STORE_DTR_AS_SHORT_ADDRESS 0x80 // 1000 0000
#define DALICMD_ENABLE_WRITE_MEMORY 0x81 // 1000 0001

// - query commands, return one response byte
#define DALICMD_QUERY_STATUS 0x90 // 1001 0000
#define DALICMD_QUERY_CONTROL_GEAR 0x91 // 1001 0001
#define DALICMD_QUERY_LAMP_FAILURE 0x92 // 1001 0010
#define DALICMD_QUERY_LAMP_POWER_ON 0x93 // 1001 0011
#define DALICMD_QUERY_LIMIT_ERROR 0x94 // 1001 0100
#define DALICMD_QUERY_RESET_STATE 0x95 // 1001 0101
#define DALICMD_QUERY_MISSING_SHORT_ADDRESS 0x96 // 1001 0110
#define DALICMD_QUERY_VERSION_NUMBER 0x97 // 1001 0111
#define DALICMD_QUERY_CONTENT_DTR 0x98 // 1001 1000
#define DALICMD_QUERY_DEVICE_TYPE 0x99 // 1001 1001
#define DALICMD_QUERY_PHYSICAL_MINIMUM_LEVEL 0x9A // 1001 1010
#define DALICMD_QUERY_POWER_FAILURE 0x9B // 1001 1011
#define DALICMD_QUERY_CONTENT_DTR1 0x9C // 1001 1100
#define DALICMD_QUERY_CONTENT_DTR2 0x9D // 1001 1101
#define DALICMD_QUERY_ACTUAL_LEVEL 0xA0 // 1010 0000
#define DALICMD_QUERY_MAX_LEVEL 0xA1 // 1010 0001
#define DALICMD_QUERY_MIN_LEVEL 0xA2 // 1010 0010
#define DALICMD_QUERY_POWER_ON_LEVEL 0xA3 // 1010 0011
#define DALICMD_QUERY_FAILURE_LEVEL 0xA4 // 1010 0100
#define DALICMD_QUERY_FADE_PARAMS 0xA5 // 1010 0101
#define DALICMD_QUERY_SCENE_LEVEL 0xB0 // 1011 xxxx, x = scene number 0..15
#define DALICMD_QUERY_GROUPS_0_TO_7 0xC0 // 1100 0000
#define DALICMD_QUERY_GROUPS_8_TO_15 0xC1 // 1100 0001
#define DALICMD_QUERY_RANDOM_ADDRESS_H 0xC2 // 1100 0010
#define DALICMD_QUERY_RANDOM_ADDRESS_M 0xC3 // 1100 0011
#define DALICMD_QUERY_RANDOM_ADDRESS_L 0xC4 // 1100 0100
#define DALICMD_READ_MEMORY_LOCATION 0xC5 // 1100 0101
#define DALICMD_QUERY_EXTENDED_VERSION 0xFF // 1111 1111


// - DALI 2-byte special commands, command in first byte
#define DALICMD_TERMINATE 0xA1 // 1010 0001 0000 0000
#define DALICMD_SET_DTR 0xA3 // 1010 0011 XXXX XXXX
#define DALICMD_INITIALISE 0xA5 // 1010 0101 XXXX XXXX
#define DALICMD_RANDOMISE 0xA7 // 1010 0111 0000 0000
#define DALICMD_COMPARE 0xA9 // 1010 1001 0000 0000
#define DALICMD_WITHDRAW 0xAB // 1010 1011 0000 0000
#define DALICMD_SEARCHADDRH 0xB1 // 1011 0001 HHHH HHHH
#define DALICMD_SEARCHADDRM 0xB3 // 1011 0011 MMMM MMMM
#define DALICMD_SEARCHADDRL 0xB5 // 1011 0101 LLLL LLLL
#define DALICMD_PROGRAM_SHORT_ADDRESS 0xB7 // 1011 0111 0AAA AAA1
#define DALICMD_VERIFY_SHORT_ADDRESS 0xB9 // 1011 1001 0AAA AAA1
#define DALICMD_QUERY_SHORT_ADDRESS 0xBB // 1011 1011 0000 0000
#define DALICMD_PHYSICAL_SELECTION 0xBD // 1011 1101 0000 0000
#define DALICMD_ENABLE_DEVICE_TYPE 0xC1 // 1100 0001 XXXX XXXX
#define DALICMD_SET_DTR1 0xC3 // 1100 0011 XXXX XXXX
#define DALICMD_SET_DTR2 0xC5 // 1100 0101 XXXX XXXX
#define DALICMD_WRITE_MEMORY_LOCATION 0xC7 // 1100 0111 XXXX XXXX

// DALI answers
#define DALIANSWER_YES 0xFF
#define DALIVALUE_MASK 0xFF

// DALI memory banks
#define DALIMEM_BANK0_MINBYTES 0x0F
#define DALIMEM_BANK1_MINBYTES 0x10


#endif
