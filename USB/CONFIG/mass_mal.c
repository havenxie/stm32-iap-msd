/**
  ******************************************************************************
  * @file    mass_mal.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Medium Access Layer interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "mass_mal.h"
#include "stmflash.h"

#include "mass_mal.h"
#include "usart.h"
#include "FAT16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Mass_Memory_Size[1];
uint32_t Mass_Block_Size[1];
uint32_t Mass_Block_Count[1];
__IO uint32_t Status = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : MAL_Init
* Description    : 
* Input          : lun
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Init(uint8_t lun)
{
    uint16_t result = MAL_FAIL;

    if(0 == lun)
    {
        FLASH_Unlock(); 
        result = MAL_OK;
    }

    return result;
}
/*******************************************************************************
* Function Name  : MAL_Write
* Description    : ???
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
    uint16_t result = MAL_FAIL;
    uint16_t i = 0;
    if(0 == lun)
    {
        FATWriteLBA(Memory_Offset,(uint8_t*) Writebuff, (uint32_t) Transfer_Length);
        //STMFLASH_Write(FLASH_START_ADDR + Memory_Offset,(u16*)Writebuff,Transfer_Length/2);
        
//        printf("\r\nF = %4x", FLASH_START_ADDR + Memory_Offset  );
//        for( i = 0; i < Transfer_Length; i++ )
//        {
//            if(i % 4 == 0) 
//            {
//                 printf("\r\nA=%8x  D=", FLASH_START_ADDR + Memory_Offset + i);
//            }
//            printf("%8x", Writebuff[i]);
//        }
        
        result = MAL_OK;
    }
    
    return result;
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : ????
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
    uint16_t result = MAL_FAIL;

    if(0 == lun)
    {
        if(0 == FATReadLBA(Memory_Offset,(uint8_t*)Readbuff, (uint32_t)Transfer_Length))
        {
            STMFLASH_Read(FLASH_START_ADDR+Memory_Offset,(u16 *)Readbuff, Transfer_Length/2);
        }
        result = MAL_OK;
    }
    
    return result;
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_GetStatus (uint8_t lun)
{
    uint16_t result = MAL_FAIL;
    
   	if (lun == 0)
   	{
  		Mass_Block_Count[0] = FLASH_SIZE/FLASH_PAGE_SIZE; 
		Mass_Block_Size[0]  = FLASH_PAGE_SIZE;                    
		Mass_Memory_Size[0] = FLASH_SIZE;        
        
		result = MAL_OK;
	}
 
  	return result;
}
