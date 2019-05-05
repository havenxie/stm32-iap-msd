/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : memory.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Memory management layer
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "memory.h"
#include "usb_scsi.h"
#include "usb_bot.h"
#include "usb_regs.h"
#include "usb_mem.h"
#include "usb_conf.h"
#include "hw_config.h"
#include "mass_mal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
vu32 Block_Read_count = 0;
vu32 Block_offset;
vu32 Counter = 0;

u32  Idx;
u32 Data_Buffer[BULK_MAX_PACKET_SIZE *8]; //不使用malloc 
u8 TransferState = TXFR_IDLE;

////////////////////////////自己定义的一个标记USB状态的寄存器///////////////////
//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)
u8 USB_STATUS_REG=0;
////////////////////////////////////////////////////////////////////////////////


/* Extern variables ----------------------------------------------------------*/
//extern u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE]; 
extern u16 Data_Len;
extern u8 Bot_State;
extern u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;			  
							  


/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Read_Memory(u8 lun, u32 Memory_Offset, u32 Transfer_Length)
{
	static u32 Offset, Length;
	u8 STA;
	
	if (TransferState == TXFR_IDLE )
	{
		Offset = Memory_Offset * Mass_Block_Size[lun];
		Length = Transfer_Length * Mass_Block_Size[lun];
		TransferState = TXFR_ONGOING;
	}
    
	if (TransferState == TXFR_ONGOING )
	{
		if (!Block_Read_count)
		{
			STA=MAL_Read(lun ,
					     Offset ,
			   	  		 Data_Buffer,
		    	   	 	 Mass_Block_Size[lun]);
			if(STA) USB_STATUS_REG|=0X08;
			UserToPMABufferCopy((u8 *)Data_Buffer, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
			Block_Read_count = Mass_Block_Size[lun] - BULK_MAX_PACKET_SIZE;
			Block_offset = BULK_MAX_PACKET_SIZE;
		}
		else
		{
			UserToPMABufferCopy((u8 *)Data_Buffer + Block_offset, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
			Block_Read_count -= BULK_MAX_PACKET_SIZE;
			Block_offset += BULK_MAX_PACKET_SIZE;
		}	  
		SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
		SetEPTxStatus(ENDP1, EP_TX_VALID);
		Offset += BULK_MAX_PACKET_SIZE;
		Length -= BULK_MAX_PACKET_SIZE;
		
		CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
		Led_RW_ON();
	}
    
	if (Length == 0)
	{
		Block_Read_count = 0;
		Block_offset = 0;
		Offset = 0;
		Bot_State = BOT_DATA_IN_LAST;
		TransferState = TXFR_IDLE;
		Led_RW_OFF();//读写完成了
	}					 
}

/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Write_Memory (u8 lun, u32 Memory_Offset, u32 Transfer_Length)
{	 
	static u32 W_Offset, W_Length;	
	u8 STA;   
	u32 temp =  Counter + 64;	  
	if (TransferState == TXFR_IDLE )
	{
		W_Offset = Memory_Offset * Mass_Block_Size[lun];
		W_Length = Transfer_Length * Mass_Block_Size[lun];
		TransferState = TXFR_ONGOING;
	}	   
    
	if (TransferState == TXFR_ONGOING )
	{   
		for (Idx = 0 ; Counter < temp; Counter++)
		{
			*((u8 *)Data_Buffer + Counter) = Bulk_Data_Buff[Idx++];
		}  
		W_Offset += Data_Len;
		W_Length -= Data_Len;	  
		if (!(W_Length % Mass_Block_Size[lun]))
		{
			Counter = 0;
			STA=MAL_Write(lun ,
					  	  W_Offset - Mass_Block_Size[lun],
					  	  Data_Buffer,
					  	  Mass_Block_Size[lun]);
			if(STA)USB_STATUS_REG|=0X04;//SD卡写错误!	 
		}				  
		CSW.dDataResidue -= Data_Len;
		SetEPRxStatus(ENDP2, EP_RX_VALID); /* enable the next transaction*/		 
		Led_RW_ON();//提示正在读写
	}	   
    
	if ((W_Length == 0) || (Bot_State == BOT_CSW_Send))
	{
		Counter = 0;
		Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
		TransferState = TXFR_IDLE;
		Led_RW_OFF();//读写完成了
	}
} 













