/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : msd.c
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : MSD card driver source file.
*                      Pin assignment:
*             ----------------------------------------------
*             |  STM32F10x    |     MSD          Pin        |
*             ----------------------------------------------
*             | CS            |   ChipSelect      1         |
*             | MOSI          |   DataIn          2         |
*             |               |   GND             3 (0 V)   |
*             |               |   VDD             4 (3.3 V) |
*             | SCLK          |   Clock           5         |
*             |               |   GND             6 (0 V)   |
*             | MISO          |   DataOut         7         |
*             -----------------------------------------------
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "msd.h"
#include "mmc_sd.h"
#include "spi.h"
 
/*******************************************************************************
* Function Name  : MSD_Init
* Description    : Initializes the MSD/SD communication.
* Input          : None
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
*******************************************************************************/
u8 MSD_Init(void)
{
  u32 i = 0;

	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 
	GPIOA->CRL&=0XFFF000FF; 
	GPIOA->CRL|=0X00033300;//PA2.3.4 推挽 	    
	GPIOA->ODR|=0X7<<2;    //PA2.3.4上拉 
	SPIx_Init();
 	SPIx_SetSpeed(SPI_SPEED_256);//设置到低速模式

  /* MSD chip select high */
  SD_CS=1;
  /* Send dummy byte 0xFF, 10 times with CS high*/
  /* rise CS and MOSI for 80 clocks cycles */
  for (i = 0; i <= 9; i++)
  {
    /* Send dummy byte 0xFF */
    MSD_WriteByte(DUMMY);
  }
  /*------------Put MSD in SPI mode--------------*/
  /* MSD initialized and set to SPI mode properly */
  return (MSD_GoIdleState());
}
				    
u8 MSD_WriteBuffer1(u8* pBuffer, u32 WriteAddr, u32 NumByteToWrite)
{
	u32 i = 0, NbrOfBlock = 0, Offset = 0;
	u32 arg;
	u16 cnt;
	u8 rvalue = MSD_RESPONSE_FAILURE;		    
	NbrOfBlock = NumByteToWrite / BLOCK_SIZE;		 
	SD_CS=0;				 
	while (NbrOfBlock --)
	{		
		arg=WriteAddr+Offset;
	    SPIx_ReadWriteByte(24 | 0x40);//分别写入命令
	    SPIx_ReadWriteByte(arg >> 24);
	    SPIx_ReadWriteByte(arg >> 16);
	    SPIx_ReadWriteByte(arg >> 8);
	    SPIx_ReadWriteByte(arg);
	    SPIx_ReadWriteByte(0XFF);

		cnt=0XFFF;
		while(cnt&&(SPIx_ReadWriteByte(0xff)!=0))cnt--;						  						 
	    if(cnt==0)return 1;  //应答不正确，直接返回	
		
			  				   
	    SPIx_ReadWriteByte(0XFF);	 
		SPIx_ReadWriteByte(0XFE);	
				    
		for (i=0;i<BLOCK_SIZE;i++)
		{										 
			SPIx_ReadWriteByte(*pBuffer);											 
			pBuffer++;
		}							   
		Offset += 512;	
														   
		SPIx_ReadWriteByte(0XFF);
		SPIx_ReadWriteByte(0XFF);
 
		cnt=0XFFF;
		do
		{
			rvalue=SPIx_ReadWriteByte(0XFF);
			rvalue&=0x1f;
			if(rvalue==0x05)break;	
		}while(cnt--);	
 	 	while(SPIx_ReadWriteByte(0XFF)==0);//等待写入结束

	    if(cnt==0)//错误了
		{
			rvalue=2;
			printf("2\n");
			//return	 2;  //应答不正确，直接返回 
		}else rvalue=0;//没错  	
							   
//		if (MSD_GetDataResponse() == MSD_DATA_OK)
//		{									  
//			rvalue = MSD_RESPONSE_NO_ERROR;
//		}
//		else
//		{									   
//			rvalue = MSD_RESPONSE_FAILURE;
//		}
	}					 		  
	SD_CS=1;										  
	SPIx_ReadWriteByte(DUMMY);	    
	return rvalue;
}
 
u8 MSD_ReadBuffer2(u8* pBuffer, u32 ReadAddr, u32 NumByteToRead)
{
  u32 i = 0, NbrOfBlock = 0, Offset = 0;
  u8 rvalue = MSD_RESPONSE_FAILURE;

  /* Calculate number of blocks to read */
  NbrOfBlock = NumByteToRead / BLOCK_SIZE;
  /* MSD chip select low */
  SD_CS=0;

  /* Data transfer */
  while (NbrOfBlock --)
  {
    /* Send CMD17 (MSD_READ_SINGLE_BLOCK) to read one block */
    MSD_SendCmd (MSD_READ_SINGLE_BLOCK, ReadAddr + Offset, 0xFF);
    /* Check if the MSD acknowledged the read block command: R1 response (0x00: no errors) */
    if (MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
    {
      return  MSD_RESPONSE_FAILURE;
    }
    /* Now look for the data token to signify the start of the data */
    if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
    {
      /* Read the MSD block data : read NumByteToRead data */
      for (i = 0; i < BLOCK_SIZE; i++)
      {
        /* Read the pointed data */
        *pBuffer = MSD_ReadByte();
        /* Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /* Set next read address*/
      Offset += 512;
      /* get CRC bytes (not really needed by us, but required by MSD) */
      MSD_ReadByte();
      MSD_ReadByte();
      /* Set response value to success */
      rvalue = MSD_RESPONSE_NO_ERROR;
    }
    else
    {
      /* Set response value to failure */
      rvalue = MSD_RESPONSE_FAILURE;
    }
  }

  /* MSD chip select high */
  SD_CS=1;
  /* Send dummy byte: 8 Clock pulses of delay */
  MSD_WriteByte(DUMMY);
  /* Returns the reponse */
  return rvalue;
}
 

/*******************************************************************************
* Function Name  : MSD_GetCIDRegister
* Description    : Read the CID card register.
*                  Reading the contents of the CID register in SPI mode
*                  is a simple read-block transaction.
* Input          : - MSD_cid: pointer on an CID register structure
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
*******************************************************************************/
//u8 MSD_GetCIDRegister(sMSD_CID* MSD_cid)
//{
//  u32 i = 0;
//  u8 rvalue = MSD_RESPONSE_FAILURE;
//  u8 CID_Tab[16];
//
//  /* MSD chip select low */
//  SD_CS=0;
//  /* Send CMD10 (CID register) */
//  MSD_SendCmd(MSD_SEND_CID, 0, 0xFF);
//
//  /* Wait for response in the R1 format (0x00 is no errors) */
//  if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
//  {
//    if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
//    {
//      /* Store CID register value on CID_Tab */
//      for (i = 0; i < 16; i++)
//      {
//        CID_Tab[i] = MSD_ReadByte();
//      }
//    }
//    /* Get CRC bytes (not really needed by us, but required by MSD) */
//    MSD_WriteByte(DUMMY);
//    MSD_WriteByte(DUMMY);
//    /* Set response value to success */
//    rvalue = MSD_RESPONSE_NO_ERROR;
//  }
//
//  /* MSD chip select high */
//  SD_CS=1;
//  /* Send dummy byte: 8 Clock pulses of delay */
//  MSD_WriteByte(DUMMY);
//
//  /* Byte 0 */
//  MSD_cid->ManufacturerID = CID_Tab[0];
//  /* Byte 1 */
//  MSD_cid->OEM_AppliID = CID_Tab[1] << 8;
//  /* Byte 2 */
//  MSD_cid->OEM_AppliID |= CID_Tab[2];
//  /* Byte 3 */
//  MSD_cid->ProdName1 = CID_Tab[3] << 24;
//  /* Byte 4 */
//  MSD_cid->ProdName1 |= CID_Tab[4] << 16;
//  /* Byte 5 */
//  MSD_cid->ProdName1 |= CID_Tab[5] << 8;
//  /* Byte 6 */
//  MSD_cid->ProdName1 |= CID_Tab[6];
//  /* Byte 7 */
//  MSD_cid->ProdName2 = CID_Tab[7];
//  /* Byte 8 */
//  MSD_cid->ProdRev = CID_Tab[8];
//  /* Byte 9 */
//  MSD_cid->ProdSN = CID_Tab[9] << 24;
//  /* Byte 10 */
//  MSD_cid->ProdSN |= CID_Tab[10] << 16;
//  /* Byte 11 */
//  MSD_cid->ProdSN |= CID_Tab[11] << 8;
//  /* Byte 12 */
//  MSD_cid->ProdSN |= CID_Tab[12];
//  /* Byte 13 */
//  MSD_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
//  /* Byte 14 */
//  MSD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;
//  /* Byte 15 */
//  MSD_cid->ManufactDate |= CID_Tab[14];
//  /* Byte 16 */
//  MSD_cid->msd_CRC = (CID_Tab[15] & 0xFE) >> 1;
//  MSD_cid->Reserved2 = 1;
//
//  /* Return the reponse */
//  return rvalue;
//}

/*******************************************************************************
* Function Name  : MSD_SendCmd
* Description    : Send 5 bytes command to the MSD card.
* Input          : - Cmd: the user expected command to send to MSD card
*                  - Arg: the command argument
*                  - Crc: the CRC
* Output         : None
* Return         : None
*******************************************************************************/
void MSD_SendCmd(u8 Cmd, u32 Arg, u8 Crc)
{
  u32 i = 0x00;
  u8 Frame[6];

  /* Construct byte1 */
  Frame[0] = (Cmd | 0x40);
  /* Construct byte2 */
  Frame[1] = (u8)(Arg >> 24);
  /* Construct byte3 */
  Frame[2] = (u8)(Arg >> 16);
  /* Construct byte4 */
  Frame[3] = (u8)(Arg >> 8);
  /* Construct byte5 */
  Frame[4] = (u8)(Arg);
  /* Construct CRC: byte6 */
  Frame[5] = (Crc);

  /* Send the Cmd bytes */
  for (i = 0; i < 6; i++)
  {
    MSD_WriteByte(Frame[i]);
  }
}

/*******************************************************************************
* Function Name  : MSD_GetDataResponse
* Description    : Get MSD card data response.
* Input          : None
* Output         : None
* Return         : The MSD status: Read data response xxx0<status>1
*                   - status 010: Data accecpted
*                   - status 101: Data rejected due to a crc error
*                   - status 110: Data rejected due to a Write error.
*                   - status 111: Data rejected due to other error.
*******************************************************************************/
u8 MSD_GetDataResponse(void)
{
  u32 i = 0;
  u8 response, rvalue;

  while (i <= 64)
  {
    /* Read resonse */
    response = MSD_ReadByte();
    /* Mask unused bits */
    response &= 0x1F;

    switch (response)
    {
      case MSD_DATA_OK:
      {
        rvalue = MSD_DATA_OK;
        break;
      }

      case MSD_DATA_CRC_ERROR:
        return MSD_DATA_CRC_ERROR;

      case MSD_DATA_WRITE_ERROR:
        return MSD_DATA_WRITE_ERROR;

      default:
      {
        rvalue = MSD_DATA_OTHER_ERROR;
        break;
      }
    }
    /* Exit loop in case of data ok */
    if (rvalue == MSD_DATA_OK)
      break;
    /* Increment loop counter */
    i++;
  }
  /* Wait null data */
  while (MSD_ReadByte() == 0);
  /* Return response */
  return response;
}

/*******************************************************************************
* Function Name  : MSD_GetResponse
* Description    : Returns the MSD response.
* Input          : None
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
*******************************************************************************/
u8 MSD_GetResponse(u8 Response)
{
  u32 Count = 0xFFF;

  /* Check if response is got or a timeout is happen */
  while ((MSD_ReadByte() != Response) && Count)
  {
    Count--;
  }

  if (Count == 0)
  {
    /* After time out */
    return MSD_RESPONSE_FAILURE;
  }
  else
  {
    /* Right response got */
    return MSD_RESPONSE_NO_ERROR;
  }
}

/*******************************************************************************
* Function Name  : MSD_GetStatus
* Description    : Returns the MSD status.
* Input          : None
* Output         : None
* Return         : The MSD status.
*******************************************************************************/
u16 MSD_GetStatus(void)
{
  u16 Status = 0;

  /* MSD chip select low */
  SD_CS=0;
  /* Send CMD13 (MSD_SEND_STATUS) to get MSD status */
  MSD_SendCmd(MSD_SEND_STATUS, 0, 0xFF);

  Status = MSD_ReadByte();
  Status |= (u16)(MSD_ReadByte() << 8);

  /* MSD chip select high */
  SD_CS=1;
  /* Send dummy byte 0xFF */
  MSD_WriteByte(DUMMY);

  return Status;
}

/*******************************************************************************
* Function Name  : MSD_GoIdleState
* Description    : Put MSD in Idle state.
* Input          : None
* Output         : None
* Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
*                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
*******************************************************************************/
u8 MSD_GoIdleState(void)
{
  /* MSD chip select low */
  SD_CS=0;
  /* Send CMD0 (GO_IDLE_STATE) to put MSD in SPI mode */
  MSD_SendCmd(MSD_GO_IDLE_STATE, 0, 0x95);

  /* Wait for In Idle State Response (R1 Format) equal to 0x01 */
  if (MSD_GetResponse(MSD_IN_IDLE_STATE))
  {
    /* No Idle State Response: return response failue */
    return MSD_RESPONSE_FAILURE;
  }
  /*----------Activates the card initialization process-----------*/
  do
  {
    /* MSD chip select high */
    SD_CS=1;
    /* Send Dummy byte 0xFF */
    MSD_WriteByte(DUMMY);

    /* MSD chip select low */
    SD_CS=0;

    /* Send CMD1 (Activates the card process) until response equal to 0x0 */
    MSD_SendCmd(MSD_SEND_OP_COND, 0, 0xFF);
    /* Wait for no error Response (R1 Format) equal to 0x00 */
  }
  while (MSD_GetResponse(MSD_RESPONSE_NO_ERROR));

  /* MSD chip select high */
  SD_CS=1;
  /* Send dummy byte 0xFF */
  MSD_WriteByte(DUMMY);
 	SPIx_SetSpeed(SPI_SPEED_4);//设置到低速模式
  return MSD_RESPONSE_NO_ERROR;
}

/*******************************************************************************
* Function Name  : MSD_WriteByte
* Description    : Write a byte on the MSD.
* Input          : Data: byte to send.
* Output         : None
* Return         : None.
*******************************************************************************/
void MSD_WriteByte(u8 Data)
{
	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return;
	}			  
	SPI1->DR=Data;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return ;
	}	  						    
	retry= SPI1->DR;          //返回收到的数据	 
}

/*******************************************************************************
* Function Name  : MSD_ReadByte
* Description    : Read a byte from the MSD.
* Input          : None.
* Output         : None
* Return         : The received byte.
*******************************************************************************/
u8 MSD_ReadByte(void)
{				  
	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=0XFF;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //返回收到的数据		 
}
 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
