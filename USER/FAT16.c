/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2008
*  ALL RIGHTS RESERVED
*
*  File Name: FAT16.c
*                                                                          
*  Purpose: This file is for a USB Mass-Storage Device bootloader.  This file 
*           mimics a FAT16 drive in order to enumerate as a disk drive
*                                                                          
*  Assembler:  Codewarrior for Microcontrollers V6.2
*                                            
*  Version:  1.3
*                                                                          
*                                                                          
*  Author: Derek Snell                             
*                                                                                       
*  Location: Indianapolis, IN. USA                                            
*                                                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.3   2009.01.13  Derek Snell   Added linker SEGMENTs for S08 version
* 1.2   2008.11.24  Derek Snell   Added Volume label "BOOTLOADER" to FAT16 root directory
* 1.1   2008.09.17  Derek Snell   Updated to give S19 address error in status
* 1.0   2008.06.10  Derek Snell   Initial version
* 
*
******************************************************************************/                                                                        
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/


#include "string.h"
#include "stdlib.h"
#include "FAT16.h"
#include "stmflash.h"
#include "hex.h"
#include "main.h"
#include "usart.h"


/*
const unsigned char _acpp[0x200] = {
  0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x08, 0x04, 0x02, 0x00,
  0x02, 0x00, 0x02, 0x00, 0x00, 0xF8, 0x0D, 0x00, 0x3F, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xC8, 0x00, 0x00, 0x80, 0x00, 0x29, 0x21, 0xE0, 0x40, 0x60, 0x4E, 0x4F, 0x20, 0x4E, 0x41,
  0x4D, 0x45, 0x20, 0x20, 0x20, 0x20, 0x46, 0x41, 0x54, 0x31, 0x36, 0x20, 0x20, 0x20, 0x33, 0xC9,
  0x8E, 0xD1, 0xBC, 0xF0, 0x7B, 0x8E, 0xD9, 0xB8, 0x00, 0x20, 0x8E, 0xC0, 0xFC, 0xBD, 0x00, 0x7C,
  0x38, 0x4E, 0x24, 0x7D, 0x24, 0x8B, 0xC1, 0x99, 0xE8, 0x3C, 0x01, 0x72, 0x1C, 0x83, 0xEB, 0x3A,
  0x66, 0xA1, 0x1C, 0x7C, 0x26, 0x66, 0x3B, 0x07, 0x26, 0x8A, 0x57, 0xFC, 0x75, 0x06, 0x80, 0xCA,
  0x02, 0x88, 0x56, 0x02, 0x80, 0xC3, 0x10, 0x73, 0xEB, 0x33, 0xC9, 0x8A, 0x46, 0x10, 0x98, 0xF7,
  0x66, 0x16, 0x03, 0x46, 0x1C, 0x13, 0x56, 0x1E, 0x03, 0x46, 0x0E, 0x13, 0xD1, 0x8B, 0x76, 0x11,
  0x60, 0x89, 0x46, 0xFC, 0x89, 0x56, 0xFE, 0xB8, 0x20, 0x00, 0xF7, 0xE6, 0x8B, 0x5E, 0x0B, 0x03,
  0xC3, 0x48, 0xF7, 0xF3, 0x01, 0x46, 0xFC, 0x11, 0x4E, 0xFE, 0x61, 0xBF, 0x00, 0x00, 0xE8, 0xE6,
  0x00, 0x72, 0x39, 0x26, 0x38, 0x2D, 0x74, 0x17, 0x60, 0xB1, 0x0B, 0xBE, 0xA1, 0x7D, 0xF3, 0xA6,
  0x61, 0x74, 0x32, 0x4E, 0x74, 0x09, 0x83, 0xC7, 0x20, 0x3B, 0xFB, 0x72, 0xE6, 0xEB, 0xDC, 0xA0,
  0xFB, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC, 0x98, 0x40, 0x74, 0x0C, 0x48, 0x74, 0x13, 0xB4, 0x0E,
  0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xEF, 0xA0, 0xFD, 0x7D, 0xEB, 0xE6, 0xA0, 0xFC, 0x7D, 0xEB,
  0xE1, 0xCD, 0x16, 0xCD, 0x19, 0x26, 0x8B, 0x55, 0x1A, 0x52, 0xB0, 0x01, 0xBB, 0x00, 0x00, 0xE8,
  0x3B, 0x00, 0x72, 0xE8, 0x5B, 0x8A, 0x56, 0x24, 0xBE, 0x0B, 0x7C, 0x8B, 0xFC, 0xC7, 0x46, 0xF0,
  0x3D, 0x7D, 0xC7, 0x46, 0xF4, 0x29, 0x7D, 0x8C, 0xD9, 0x89, 0x4E, 0xF2, 0x89, 0x4E, 0xF6, 0xC6,
  0x06, 0x96, 0x7D, 0xCB, 0xEA, 0x03, 0x00, 0x00, 0x20, 0x0F, 0xB6, 0xC8, 0x66, 0x8B, 0x46, 0xF8,
  0x66, 0x03, 0x46, 0x1C, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 0xEB, 0x5E, 0x0F, 0xB6, 0xC8,
  0x4A, 0x4A, 0x8A, 0x46, 0x0D, 0x32, 0xE4, 0xF7, 0xE2, 0x03, 0x46, 0xFC, 0x13, 0x56, 0xFE, 0xEB,
  0x4A, 0x52, 0x50, 0x06, 0x53, 0x6A, 0x01, 0x6A, 0x10, 0x91, 0x8B, 0x46, 0x18, 0x96, 0x92, 0x33,
  0xD2, 0xF7, 0xF6, 0x91, 0xF7, 0xF6, 0x42, 0x87, 0xCA, 0xF7, 0x76, 0x1A, 0x8A, 0xF2, 0x8A, 0xE8,
  0xC0, 0xCC, 0x02, 0x0A, 0xCC, 0xB8, 0x01, 0x02, 0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4, 0x42,
  0x8B, 0xF4, 0x8A, 0x56, 0x24, 0xCD, 0x13, 0x61, 0x61, 0x72, 0x0B, 0x40, 0x75, 0x01, 0x42, 0x03,
  0x5E, 0x0B, 0x49, 0x75, 0x06, 0xF8, 0xC3, 0x41, 0xBB, 0x00, 0x00, 0x60, 0x66, 0x6A, 0x00, 0xEB,
  0xB0, 0x42, 0x4F, 0x4F, 0x54, 0x4D, 0x47, 0x52, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x52, 0x65,
  0x6D, 0x6F, 0x76, 0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x73, 0x20, 0x6F, 0x72, 0x20, 0x6F, 0x74,
  0x68, 0x65, 0x72, 0x20, 0x6D, 0x65, 0x64, 0x69, 0x61, 0x2E, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73,
  0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20,
  0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61,
  0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0xCB, 0xD8, 0x55, 0xAA,
};
*/

/********************************************************************
*********************************************************************
*       FAT16 Boot Sector
*********************************************************************
********************************************************************/
const uint8_t FAT16_BootSector[FATBootSize]= 
{
    0xEB,           /*00 - BS_jmpBoot */
    0x3C,           /*01 - BS_jmpBoot */
    0x90,           /*02 - BS_jmpBoot */
    'M','S','D','O','S','5','.','0',    /* 03-10 - BS_OEMName */
    0x00,           /*11 - BPB_BytesPerSec = 2048 */
    0x08,           /*11 - BPB_BytesPerSec = 2048 */
    0x04,           /*13 - BPB_Sec_PerClus = 2K*4 = 8K*/
    2,              /*14 - BPB_RsvdSecCnt = 2 */
    0,              /*15 - BPB_RsvdSecCnt = 2 */
    2,              /*16 - BPB_NumFATs = 2 */
    0x0,            /*17 - BPB_RootEntCnt = 512 */
    0x2,            /*18 - BPB_RootEntCnt = 512 */
    0,              /*19 - BPB_TotSec16 = 0 */
    0,              /*20 - BPB_TotSec16 = 0 */
    0xF8,           /*21 - BPB_Media = 0xF8 */
    0x0D,           /*22 - BPBFATSz16 = 0x000D */
    0,              /*23 - BPBFATSz16 = 0x000D */
    0x3F,           /*24 - BPB_SecPerTrk = 0x003F */
    0,              /*25 - BPB_SecPerTrk = 0x003F */
    0xFF,           /*26 - BPB_NumHeads = 255 */
    0,              /*27 - BPB_NumHeads = 255 */
    0,              /*28 - BPB_HiddSec = 0 */
    0,              /*29 - BPB_HiddSec = 0 */
    0,              /*30 - BPB_HiddSec = 0 */
    0,              /*31 - BPB_HiddSec = 0 */
    0x00,           /*32 - BPB_TotSec32 = */
    0xC8,           /*33 - BPB_TotSec32 = 0x0000C800 100Mb*/
    0x00,           /*34 - BPB_TotSec32 = */
    0x00,           /*35 - BPB_TotSec32 = */
    0x80,           /*36 - BS_DrvNum = 0x80 */
    0,              /*37 - BS_Reserved1 = 0 , dirty bit = 0*/ /* Updated from FSL*/
    0x29,           /*38 - BS_BootSig = 0x29 */
    0xBD,           /*39 - BS_VolID = 0x02DDA5BD */
    0xA5,           /*40 - BS_VolID = 0x02DDA5BD */
    0xDD,           /*41 - BS_VolID = 0x02DDA5BD */
    0x02,           /*42 - BS_VolID = 0x02DDA5BD */
    'N','O',' ','N','A','M','E',' ',' ',' ',' ',  /*43-53 - BS_VolLab */
    'F','A','T','1','6',' ',' ',' '   /*54-61 - BS_FilSysType */
};

/********************************************************************
*********************************************************************
*       First Sector of FAT Table
*********************************************************************
********************************************************************/
const uint8_t FAT16_TableSector0[FATTableSize]= 
{
    0xF8,0xFF,0xFF,0xFF
};

/********************************************************************
*********************************************************************
*       FAT Root Directory Sector
*********************************************************************
********************************************************************/

uint8_t FAT16_ReadyFileName[FATFileNameSize]= 
{
    'R','E','A','D','Y',' ',' ',' ','T','X','T'    /*00-10 - Short File Name */
};

// TODO
// Change Finish to Success

const uint8_t FAT16_RootDirSector[FATDirSize]= 
{
    0x20,           /*11 - Archive Attribute set */
    0x00,           /*12 - Reserved */
    0x4B,           /*13 - Create Time Tenth */
    0x9C,           /*14 - Create Time */
    0x42,           /*15 - Create Time */
    0x92,           /*16 - Create Date */
    0x38,           /*17 - Create Date */
    0x92,           /*18 - Last Access Date */
    0x38,           /*19 - Last Access Date */
    0x00,           /*20 - Not used in FAT16 */
    0x00,           /*21 - Not used in FAT16 */
    0x9D,           /*22 - Write Time */
    0x42,           /*23 - Write Time */
    0x92,           /*24 - Write Date */
    0x38,           /*25 - Write Date */
    0x00,           /*26 - First Cluster (none, because file is empty) */
    0x00,           /*27 - First Cluster (none, because file is empty) */
    0x00,           /*28 - File Size */
    0x00,           /*29 - File Size */
    0x00,           /*30 - File Size */
    0x00,           /*31 - File Size */
    'B','O','O','T','L','O','A','D','E','R',' ',  /*32-42 - Volume label */
    0x08,           /*43 - File attribute = Volume label */
    0x00,           /*44 - Reserved */
    0x00,           /*45 - Create Time Tenth */
    0x00,           /*46 - Create Time */
    0x00,           /*47 - Create Time */
    0x00,           /*48 - Create Date */
    0x00,           /*49 - Create Date */
    0x00,           /*50 - Last Access Date */
    0x00,           /*51 - Last Access Date */
    0x00,           /*52 - Not used in FAT16 */
    0x00,           /*53 - Not used in FAT16 */
    0x9D,           /*54 - Write Time */
    0x42,           /*55 - Write Time */
    0x92,           /*56 - Write Date */
    0x38,           /*57 - Write Date */
};

FAT_DIR_t FileAttr;

/*********************************************************
* Name: FATReadLBA
*
* Desc: Read a Logical Block Address 
*
* Parameter: FAT_LBA - Logical Block Address to Read
*            pu8DataPointer - Pointer to array to store data read  
*
* Return: None
*             
**********************************************************/
uint32_t FATReadLBA(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    /* Body */
    int32_t i;
    switch (FAT_LBA) 
    {
        /* Boot Sector */
        case 0:
            /* Write Boot Sector info */
            for(i=0;i<FATBootSize;i++)
            {
                *data++ = FAT16_BootSector[i];
            } /* EndFor */
            /* Rest of sector empty except last two bytes */
            i += 2;
            while (i++ < 512) 
            {
                *data++ = 0;
            } /* EndWhile */
                        
            /* Boot Sector requires these 2 bytes at end */
            //*data++ = 0x55;
            //*data++ = 0xAA;
            
            while(i++ < 2048)
            {
                *data++ = 0;
            }
            
            break;
        /* FAT Table Sector */
        case 0x1000: //26K-FAT0
        case 0xD800: //26K-FAT1
            /* Write FAT Table Sector */
            for(i=0;i<FATTableSize;i++)
            {
                *data++ = FAT16_TableSector0[i];
            } /* EndFor */
            /* Rest of sector empty */
            while (i++ < FATBytesPerSec) 
            {
                *data++ = 0;
            } /*ENdWhile */
            break;
            
        /* Root Directory Sector */
        case 0xE000: //16K
            
            for(i=0;i<FATFileNameSize;i++) 
            {
                *data++ = FAT16_ReadyFileName[i];
            } /* EndFor */
                            
            /* Write rest of file FAT structure */
            for(i=0;i<FATDirSize;i++) 
            {
                *data++ = FAT16_RootDirSector[i];    
            } /* EndFor */
        
            /* Rest of sector empty to signify no more files */
            i += FATFileNameSize;
            while (i++ < FATBytesPerSec) 
            {
                *data++ = 0;
            } /* EndWhile */
            break;
            
          /* All other sectors empty */
        default:
            i = 0;
            while (i++ < FATBytesPerSec) 
            {
                *data++ = 0;
            } /* EndWhile */
            break;
    } /* EndSwitch */
    
    return FATBytesPerSec;
} /* EndBody */

uint32_t FAT_RootDirWriteRequest(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    FAT_DIR_t* pFile = (FAT_DIR_t*) data;
    uint32_t   index = 2;
    
    pFile++; // Skip Root Dir
    pFile++; // Skip Status File
    
    while((pFile->DIR_Attr != 0x20) && (index++ < 512))
    {
        pFile++;
    }
    
    // Find it
    if(index <= 512)
    {
        memcpy(&FileAttr, pFile, 32);
        FileAttr.DIR_WriteTime = 0;
        FileAttr.DIR_WriteDate = 0;
    }
    else
    {
        memset(&FileAttr, (int)0, 32);
    }
    
    return len;
}

extern uint32_t flash_flag;
static HEX_OBJ_t* mHex;

uint32_t FAT_DataSectorWriteRequest(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    int32_t filesize_total = (int32_t)FileAttr.DIR_FileSize;
    int32_t* filesize_write = (int32_t*)&(FileAttr.DIR_WriteTime);
    uint32_t i = 0;
    
    if (!memcmp(&(FileAttr.DIR_Name[8]), "BIN", 3))
    {
        uint16_t flash_cnt = *(volatile uint16_t *) 0x1FFFF7E0;
        uint32_t freeflash  =  flash_cnt * FLASH_PAGE_SIZE;
                     
        if(freeflash >= FileAttr.DIR_FileSize)
        {
            // Flash MCU
            STMFLASH_Write(FLASH_START_ADDR + FAT_LBA - 0x12000,(u16*)data, len/2);
            *filesize_write += len;
            if(*filesize_write >= filesize_total)
            {
                *filesize_write = 0;

                system_info = SYS_EVENT_ERR_SUCCESS;
            }
        }
        else
        {
            system_info = SYS_EVENT_ERR_LARGE;
        }
    }
    else if(!memcmp(&(FileAttr.DIR_Name[8]), "HEX", 3))
    {
        uint8_t result;
        static uint16_t erasePageSize = 0;
        uint16_t free = 0;
        printf("\r\nLAB=%8x\r\n", FAT_LBA);
        if(FAT_LBA == 0x16000)
        {
            uint16_t flash_size = *(volatile uint16_t *) 0x1FFFF7E0;        
          #if defined STM32F10X_HD
            erasePageSize = FLASH_PAGE_SIZE;
            free = flash_size/2 - (FLASH_START_ADDR-0x08000000)/FLASH_PAGE_SIZE;
          #elif defined STM32F10X_MD
            erasePageSize = FLASH_PAGE_SIZE / 2;
            free = flash_size - (FLASH_START_ADDR-0x08000000)/(FLASH_PAGE_SIZE/2);
          #endif
            printf("\r\nFalshSize = %d, FreeSize = %d\r\n", flash_size, free);
        }
        
        for(i = 0; i < len; i++)
        {
            result = hex_findobject(mHex, data[i]);
            if(HEX_E_OK == result)
            {
                HEX_DATA_t mData;
                if(hex_getdata(mHex, &mData))
                {

                    if((mData.addr - FLASH_START_ADDR) % erasePageSize == 0)
                    {
                        printf("\r\nEraseAddr=%8x",mData.addr);
                        FLASH_Unlock();
                        FLASH_ErasePage(mData.addr);
                    }
                    FLASH_Unlock();
                    STMFLASH_Write_NoCheck(mData.addr,(u16*) mData.data,(u16)(mData.len/2));
                }
            }
            else if(HEX_E_FINISH == result)
            {
                system_info = SYS_EVENT_ERR_SUCCESS;
            }
        }
    }
    else if(!memcmp(&(FileAttr.DIR_Name[8]), "SEC", 3))
    {
        // TODO
        // De-Encry
        // Add Your Code Here
        system_info = SYS_EVENT_ERR_UNKOWN;
    }
    else
    {
        // Can't Recognize it
        // Cancel and Reset USB
        system_info = SYS_EVENT_ERR_UNKOWN;
    }

    return len;
}

/*
uint32_t FAT_DataSectorWriteRequest(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    int32_t filesize_total = (int32_t)FileAttr.DIR_FileSize;
    int32_t* filesize_write = (int32_t*)&(FileAttr.DIR_WriteTime);
    uint32_t i = 0;
    
    if (!memcmp(&(FileAttr.DIR_Name[8]), "BIN", 3))
    {
        uint16_t flash_cnt = *(volatile uint16_t *) 0x1FFFF7E0;
        uint32_t freeflash  =  flash_cnt * FLASH_PAGE_SIZE;
                     
        if(freeflash >= FileAttr.DIR_FileSize)
        {
            // Flash MCU
            STMFLASH_Write(FLASH_START_ADDR + FAT_LBA - 0x12000,(u16*)data, len/2);
            *filesize_write += len;
            if(*filesize_write >= filesize_total)
            {
                *filesize_write = 0;

                system_info = SYS_EVENT_ERR_SUCCESS;
            }
        }
        else
        {
            system_info = SYS_EVENT_ERR_LARGE;
        }
    }
    else if(!memcmp(&(FileAttr.DIR_Name[8]), "HEX", 3))
    {
        uint8_t result;
        printf("\r\nLAB=%8x\r\n", FAT_LBA);
        if(FAT_LBA == 0x16000)
        {
            uint16_t flash_size = *(volatile uint16_t *) 0x1FFFF7E0;
            uint16_t i = 0;            
          #if defined STM32F10X_HD
            uint16_t free = flash_size/2 - (FLASH_START_ADDR-0x08000000)/FLASH_PAGE_SIZE;
          #elif defined STM32F10X_MD
            uint16_t free = flash_size - (FLASH_START_ADDR-0x08000000)/(FLASH_PAGE_SIZE/2);
          #endif
            printf("\r\nFalshSize = %d, FreeSize = %d\r\n", flash_size, free);
            FLASH_Unlock();
            for(i = 0; i < free; i++)
            {
                #if defined STM32F10X_HD
                FLASH_ErasePage(FLASH_START_ADDR+i*FLASH_PAGE_SIZE);
                #elif defined STM32F10X_MD
                FLASH_ErasePage(FLASH_START_ADDR+i*(FLASH_PAGE_SIZE/2));
                printf("\r\nErrA=%8x", FLASH_START_ADDR+i*(FLASH_PAGE_SIZE/2));
                #endif
            }
        }
        
        for(i = 0; i < len; i++)
        {
            result = hex_findobject(mHex, data[i]);
            if(HEX_E_OK == result)
            {
                HEX_DATA_t mData;
                if(hex_getdata(mHex, &mData))
                {
                    if(mData.addr >= FLASH_START_ADDR)
                    {
                        printf("\r\nA=%8x",mData.addr);
                        FLASH_Unlock();
                        STMFLASH_Write_NoCheck(mData.addr,(u16*) mData.data,(u16)(mData.len/2));
                    }
                }
            }
            else if(HEX_E_FINISH == result)
            {
                system_info = SYS_EVENT_ERR_SUCCESS;
            }
        }
    }
    else if(!memcmp(&(FileAttr.DIR_Name[8]), "SEC", 3))
    {
        // TODO
        // De-Encry
        // Add Your Code Here
        system_info = SYS_EVENT_ERR_UNKOWN;
    }
    else
    {
        // Can't Recognize it
        // Cancel and Reset USB
        system_info = SYS_EVENT_ERR_UNKOWN;
    }

    return len;
}
*/
uint32_t FATWriteLBA(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    switch(FAT_LBA)
    {
        case 0x0000: // 4K   Boot Sector
        case 0x1000: // 26K  FAT0
        case 0xD800: // 26K  FAT1
             break;
        case 0xE000: // 16K  Root Directory
             FAT_RootDirWriteRequest(FAT_LBA, data, len);
             mHex = hex_newobject();
             break;
        default:
             {
                if(FAT_LBA >= 0x12000)
                {    
                    FAT_DataSectorWriteRequest(FAT_LBA, data, len);
                }
             }
             break;
    }
    
    return FATBytesPerSec;
}

uint32_t FATSetStatusFileName(const char * name)
{
    uint8_t i;
    uint8_t len = (uint8_t)strlen(name);
    
    for(i=0; i<8 && i<len; i++)
    {
        FAT16_ReadyFileName[i] = name[i];
    }
    
    for(; i < 8; i++)
    {
        FAT16_ReadyFileName[i] = ' ';
    }
    
    return i;
}

