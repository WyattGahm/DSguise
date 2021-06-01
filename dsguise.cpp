#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <libkern/OSByteOrder.h>
#include "libcrc.hpp"
#include "EasyBMP.hpp"
#include "help.hpp"

#define HEADER_SIZE 350

struct Banner{
	uint16_t version;
	uint16_t crc;
	uint8_t reserved[28];
	uint8_t tile_data[4][4][8][4];
	uint16_t palette[16];
	uint16_t title[6][128];	
};

struct Header{
	uint8_t gameTitle[0xC];
	uint8_t gameCode[4];
	uint16_t makerCode;
	uint8_t unitCode;
	uint8_t encryptionSeedSelect;
	uint8_t deviceCapacity;
	uint8_t reserved0[9];
	uint8_t romVersion;
	uint8_t internalFlag;
	uint32_t arm9RomOffset;
	uint32_t arm9EntryAddress;
	uint32_t arm9RamAddress;
	uint32_t arm9Size;
	uint32_t arm7RomOffset;
	uint32_t arm7EntryAddress;
	uint32_t arm7RamAddress;
	uint32_t arm7Size;
	uint32_t fntOffset;
	uint32_t fntSize;
	uint32_t fatOffset;
	uint32_t fatSize;
	uint32_t arm9OverlayOffset;
	uint32_t arm9OverlaySize;
	uint32_t arm7OverlayOffset;
	uint32_t arm7OverlaySize;
	uint8_t normalCardControlRegSettings[4];
	uint8_t secureCardControlRegSettings[4];
	uint32_t icon_bannerOffset;
	uint16_t secureAreaCrc;
	uint8_t secure_transfer_timeout[2];
	uint32_t arm9Autoload;
	uint32_t arm7Autoload;
	uint8_t secureDisable[8];
	uint32_t ntrRomSize;
	uint32_t headerSize;
	uint8_t reserved1[0x38];
	uint8_t nintendoLogo[0x9C];
	uint8_t nintendoLogoCrc[2];
	uint16_t headerCrc;
	uint8_t debugReserved[0x20];
};

void createBMP(Banner banner){
	EasyBMP::RGBColor palette[16];
	EasyBMP::Image img(32, 32, "icon.bmp",EasyBMP::RGBColor(0,0,0));
	for(int i = 0; i < 16;i++)
		palette[i] = EasyBMP::RGBColor((banner.palette[i] & 0b0000000000011111) << 3,(banner.palette[i] & 0b0000001111100000) >> 2,(banner.palette[i] & 0b0111110000000000) >>7);
	for(int j= 0; j < 4;j++)
		for(int i = 0; i < 4;i++)
			for(int k = 0; k < 8;k++)
				for(int l = 0; l < 8;l++)
					if(k%2 == 1) 
						img.SetPixel(i * 8 + k,j * 8 + l,palette[banner.tile_data[j][i][l][k /2] >> 4]);
					else 
						img.SetPixel(i * 8 + k,j * 8 + l,palette[banner.tile_data[j][i][l][k /2] & 0x0F]);
					
	img.Write();
}

void headerInfo(Header h,const char prefix[]){
    printf("[%s] Title: %s\n",prefix,h.gameTitle);
    printf("[%s] Game Code: %s\n",prefix,h.gameCode);
    printf("[%s] Maker Code: 0x%X\n",prefix,h.makerCode);
    printf("[%s] Unit Code: 0x%X\n",prefix,h.unitCode);
    printf("[%s] Rom Version: %i\n",prefix,h.romVersion);
    printf("[%s] Arm 9 Rom Offset: 0x%X\n",prefix,h.arm9RomOffset);
    printf("[%s] Arm 9 Entry Address: 0x%X\n",prefix,h.arm9EntryAddress);
    printf("[%s] Arm 9 Ram Address: 0x%X\n",prefix,h.arm9RamAddress);
    printf("[%s] Arm 9 Size: 0x%X\n",prefix,h.arm9Size);
    printf("[%s] Arm 7 Rom Offset: 0x%X\n",prefix,h.arm7RomOffset);
    printf("[%s] Arm 7 Entry Address: 0x%X\n",prefix,h.arm7EntryAddress);
    printf("[%s] Arm 7 Ram Address: 0x%X\n",prefix,h.arm7RamAddress);
    printf("[%s] Arm 7 Size: 0x%X\n",prefix,h.arm7Size);
    printf("[%s] Arm 9 Overlay Offset: 0x%X\n",prefix,h.arm9OverlayOffset);
    printf("[%s] Arm 9 Overlay Size: 0x%X\n",prefix,h.arm9OverlaySize);
    printf("[%s] Arm 7 Overlay Offset: 0x%X\n",prefix,h.arm7OverlayOffset);
    printf("[%s] Arm 7 Overlay Size: 0x%X\n",prefix,h.arm7OverlaySize);
    printf("[%s] Banner Offset: 0x%X\n",prefix,h.icon_bannerOffset);
    printf("[%s] Secure Area MODBUS CRC16: 0x%X\n",prefix,h.secureAreaCrc);
    printf("[%s] Arm 9 Auto Load: 0x%X\n",prefix,h.arm9Autoload);
    printf("[%s] Arm 7 Auto Load: 0x%X\n",prefix,h.arm7Autoload);
    printf("[%s] NTR Rom Size: 0x%X\n",prefix,h.ntrRomSize);
    printf("[%s] Header Size: 0x%X\n",prefix,h.headerSize);
    printf("[%s] Header MODBUS CRC16: 0x%X\n",prefix,h.headerCrc);
}

void bannerInfo(Banner b,const char prefix[]){
    printf("[%s] Game Title: %s\n",prefix,b.title[0]);
    printf("[%s] Game Version: %i\n",prefix,b.version);
    printf("[%s] Banner MODBUS CRC16: 0x%X\n",prefix,b.crc);
}


int main(int argc, char* argv[]) {
    //handle arguments
    printf(intro);
    bool title = false;
    bool icon = false;
    bool name = false;
    bool verbose = false;
    for(int i = 0; i < argc;i++){
        if(strcmp(argv[i],"-?") == 0){
            printf(help);
            return(0);
        } else if(strcmp(argv[i],"-v") == 0){
            verbose = true;
        }else if(argv[i][0] == '-'){
            if(strlen(argv[i]) < 2){
                printf(error);
                return(1);
            }
            for(int j = 1; j<strlen(argv[i]);j++){
                if(argv[i][j] == 'I') icon = true;
                else if(argv[i][j] == 'T') title = true;
                else if(argv[i][j] == 'N') name = true;
            }
            if(!(title || icon || name)){
                 printf(error);
                 return(1);
            } 
        }
    }
    //open source file
    printf("Opening source file: %s\n",argv[1]);
    std::fstream source(argv[1], std::ios::in | std::ios::binary);
    //open recipient
    printf("Opening recipient file: %s\n",argv[2]);
    std::fstream recipient(argv[2], std::ios::in | std::ios::out);
    // create variables...
    printf("Allocating %i bytes for structures\n",2 * (sizeof(Banner) + sizeof(Header)));
    Header src, rec; 
    Banner src_banner, rec_banner;
    //read from zero
    source.seekg(0, source.beg);
    recipient.seekg(0, recipient.beg);
    //deserialize headers
    printf("Deserializing headers...\n");
    source.read((char*)&src, sizeof(src));
    recipient.read((char*)&rec, sizeof(rec));
    //seek to banner offset
    source.seekg(src.icon_bannerOffset, source.beg);
    recipient.seekg(rec.icon_bannerOffset, recipient.beg);
    if(verbose){
        printf("Read banner from source at offset: 0x%X\n",src.icon_bannerOffset);
        printf("Read banner from recipient at offset: 0x%X\n",rec.icon_bannerOffset);
    }
    //deserialize banners
    printf("Deserializing headers...\n");
    source.read((char*)&src_banner, sizeof(src_banner));
    recipient.read((char*)&rec_banner, sizeof(rec_banner));
    //print some information
    if(verbose){
        headerInfo(src,"Source");
        bannerInfo(src_banner,"Source Banner");
        headerInfo(rec,"Recipient");
        bannerInfo(rec_banner,"Recipient Banner");
    }
    //copy information
    if(title){
        memcpy((char *)&rec.gameTitle[0],(char *)&src.gameTitle[0],sizeof(src.gameTitle));
        memcpy((char *)&rec.gameCode[0],(char *)&src.gameCode[0],sizeof(src.gameCode));
        int range = sizeof(src) - (sizeof(src.headerCrc) + sizeof(src.debugReserved));
        rec.headerCrc = OSSwapInt16(crc16((unsigned char *)&rec, range));
        printf("Title copied\n");
        printf("Gamecode copied\n");
        printf("Header signed with MODBUS CRC16: 0x%X\n",rec.headerCrc);
    } 
    if(icon){
        memcpy((char *)&rec_banner.tile_data[0],(char *)&src_banner.tile_data[0],sizeof(src_banner.tile_data));
        int offset = sizeof(rec_banner.version) + sizeof(rec_banner.crc) + sizeof(rec_banner.reserved);
        int range = sizeof(rec_banner) - offset;
        rec_banner.crc = OSSwapInt16(crc16((unsigned char *)&rec_banner + offset, range));
        createBMP(src_banner);
        printf("Icon copied\n");
        printf("Dumped new icon to icon.bmp\n");
        printf("Signed banner with MODBUS CRC16: 0x%X\n",rec_banner.crc);
    }
    if(name){
        memcpy((char *)&rec_banner.title[0],(char *)&src_banner.title[0],sizeof(src_banner.title));
        int offset = sizeof(rec_banner.version) + sizeof(rec_banner.crc) + sizeof(rec_banner.reserved);
        int range = sizeof(rec_banner) - offset;
        rec_banner.crc = OSSwapInt16(crc16((unsigned char *)&rec_banner + offset, range));
        printf("Name copied\n");
        printf("Signed banner with MODBUS CRC16: 0x%X\n",rec_banner.crc);
    }

    if(verbose){
        headerInfo(rec,"New");
        bannerInfo(rec_banner,"New");
    }
    //write to file
    recipient.seekg(0, recipient.beg);
    recipient.write((char*)&rec,sizeof(rec));
    recipient.seekg(rec.icon_bannerOffset, recipient.beg);
    recipient.write((char*)&rec_banner,sizeof(rec_banner));

    printf("Saved file as: %s\n",argv[2]);
    //close stuff
    source.flush();
    source.close();
    recipient.close();

    printf("Done!\n");
	return 0;
}