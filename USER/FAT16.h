#ifndef __FAT16_H
#define __FAT16_H

#include <stdint.h>

#define FATBytesPerSec  2048
#define FATBootSec        0
#define FATTable0Sec0     2
#define FATTable1Sec0   245
#define FATRootDirSec0  488
#define FATDataSec0     520

#define FATFileNameSize  11
#define FATDirSize       47
#define FATBootSize      62
#define FATTableSize      4

#define FLASH_START_ADDR         0x08004000       // Flash start address
#define FLASH_SIZE              (100*1024*1024)   // 100M
#define FLASH_PAGE_SIZE         (0x800)       // 2 Kbyte */
#define FLASH_WAIT_TIMEOUT      100000

typedef  struct
{
	uint8_t  BS_jmpBoot[3];         // Jump Instruction to boot mode (e.g EB,3C,90)
	uint8_t  BS_OEMName[8];         // OEM Name Identifier(e.g MSDOS5.0)
	uint16_t BPB_BytesPerSec;       // Count of bytes per sector(512/1024/2048/4096)
	uint8_t  BPB_SecPerClus;        // Number of sectors per allocation unit
	uint16_t BPB_RsvdSecCnt;        // Number of reserved sectors
	uint8_t  BPB_NumFATs;           // The count of FAT Tables
	uint16_t BPB_RootEntCnt;        // For FAT12/16, the count of 32-bytes directory entried in 
	                                // the root directory
	uint16_t BPB_TotSec16;          // total sectors on the volume
	uint8_t  BPB_Media;             // Media Type (0xF0)
	uint16_t BPB_FATSz16;           // The count of sectors occupied by one FAT
	uint16_t BPB_SecPerTrk;         // Sectors per track for interrupt 0x13
	uint16_t BPB_NumHeads;          // Number of heads for interrupt 0x13
	uint32_t BPB_HiddSec;           // Hidden sectors
	uint32_t BPB_TotSec32;          // For FAT12/16, this field is zero

	// FAT12/16
	uint8_t  BS_DrvNum;             // Interrupt 0x13 drive number
	uint8_t  BS_Reserved1;          // Reserved
	uint8_t  BS_BootSig;            // Extended boot signature
	uint32_t BS_VolID;              // Volume serial number
	uint8_t  BSVolLab[11];          // Volume Lable
	uint8_t  BS_FilSysType[8];      // "FAT12"/"FAT16"/"FAT32"
	uint8_t  Code[448];             // Boot Code
	uint16_t SigWord;               // 0xAA55
}FAT_BPB_t;

typedef  struct
{
	uint8_t  DIR_Name[11];          // File Name
	uint8_t  DIR_Attr;              // File Attribute
	uint8_t  DIR_NTRes;             // Reserved
	uint8_t  DIR_CreateTime_Tenth;  // Component of the file creation time
	uint16_t DIR_CreateTime;        // Component of the file creation time
	uint16_t DIR_CreateDate;        // Component of the file creation date
	uint16_t DIR_LastAccessDate;    // Last Access date
	uint16_t DIR_ClusHigh;          // High word of first data cluster
	uint16_t DIR_WriteTime;         // Last modification write time
	uint16_t DIR_WriteDate;         // Last modification write date
	uint16_t DIR_ClusLow;           // Low word of first data cluster
	uint32_t DIR_FileSize;          // Filesize
}FAT_DIR_t;

uint32_t FATReadLBA(uint32_t FAT_LBA,uint8_t* data, uint32_t len);
uint32_t FATWriteLBA(uint32_t FAT_LBA,uint8_t* data, uint32_t len);

uint32_t FATSetStatusFileName(const char * name);

#endif // __FAT16_H
