#ifndef libcrc_h_
#define libcrc_h_

#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>

#define CRC16 0x8005
unsigned int crc16(unsigned char *buf, int len){
    unsigned int crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++){
        crc ^= (unsigned int)buf[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1; 
                crc ^= 0xA001;
            }else {
                crc >>= 1; 
            }
        }
    }
    crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
    return crc;
}

#endif