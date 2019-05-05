#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "FAT16.h"
#include "main.h"


void usb_port_set(u8 enable)
{
    uint16_t value;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	if(enable)
    {
        value = _GetCNTR()&(~(1<<1));
        _SetCNTR(value);
    }
	else
	{
        value = _GetCNTR()|((1<<1));
        _SetCNTR(value);

		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;
	}
}  

#define APP_ADDR            FLASH_START_ADDR
#define GetFlashValue(addr) (*((volatile uint32_t*)(addr)))

typedef void (*Fun_t)(void);

uint32_t BL_Init(void)
{
    uint32_t value = 0;
    Fun_t pFun; 

    if((0xFFFFFFFF == GetFlashValue(APP_ADDR)) && (0xFFFFFFFF == GetFlashValue(APP_ADDR+4)))
    {
        return SYS_EVENT_ERR_NOAPP;
    }
    
    // Check Stack Pointer
    value = GetFlashValue(APP_ADDR);
    if(value < 0x20000000)
    {
        return SYS_EVENT_ERR_APP;
    }
    
    // Check Reset Handler
    value = GetFlashValue(APP_ADDR+4);
    if(0 == (value&0x01))
    {
        return SYS_EVENT_ERR_APP;
    }
    
    // Run App
    pFun = (Fun_t)value;
    (void)(pFun)();

    // Never Return
    return SYS_EVENT_ERR_IDLE;
}

__weak void API_delay_ms(uint32_t ms)
{
    volatile uint32_t tick = ms*1000;
    
    while(tick--);
}

volatile uint32_t system_info   = 0;
volatile uint32_t system_status = SYS_STATUS_USB_OFF | SYS_STATUS_UPDATE_OFF;

void ResetUSB(void)
{
    delay_ms(500);
    // Reset USB
    usb_port_set(0);
    delay_ms(500);
    usb_port_set(1);
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
}

void EventDispatch(void)
{
    switch(system_info)
    {
        case SYS_EVENT_ERR_IDLE:
            {
                // Do Nothing
            }
            break;
        
        case SYS_EVENT_ERR_READY:
            {
                system_info = SYS_EVENT_ERR_IDLE;
                FATSetStatusFileName("READY");
            }
            break;
        case SYS_EVENT_ERR_SUCCESS:
            {
                system_info = SYS_EVENT_RUN_APP;
                FATSetStatusFileName("SUCCESS");
                //ResetUSB();
            }
            break;
        case SYS_EVENT_ERR_UNKOWN:
            {
                system_info = 0;
                FATSetStatusFileName("UNKOWN");
                
                //ResetUSB();              
            }
            break;
        case SYS_EVENT_ERR_LARGE:
            {
                system_info = 0;
                FATSetStatusFileName("LARGE");
                
                ResetUSB();             
            }
            break;            
        case SYS_EVENT_ERR_FLASH:
            {
                system_info = 0;
                FATSetStatusFileName("FLASH");
                
                ResetUSB();              
            }
            break;
        case SYS_EVENT_ERR_APP:
            {
                system_info = 0;
                FATSetStatusFileName("ERRAPP");
                
                ResetUSB();
            }
            break;
        case SYS_EVENT_ERR_NOAPP:
            {
                system_info = 0;
                FATSetStatusFileName("NOAPP");
                
                ResetUSB();
            }
            break;
        case SYS_EVENT_RUN_APP:
            {
                system_info = 0;
                FATSetStatusFileName("APPRUN");
                
                usb_port_set(0);
                delay_ms(500);
                // Jump To App
                system_info = BL_Init();
            }
            break;
        default:
            {
                system_info = SYS_EVENT_ERR_READY;
                FATSetStatusFileName("READY");
                
                // Reset USB
                usb_port_set(0);
                delay_ms(500);
                usb_port_set(1);
                USB_Interrupts_Config();
                Set_USBClock();
                USB_Init();
            }
            break;
    }
}

void USB_PlugOn(void)
{
    system_status = SYS_STATUS_USB_ON;
}

int main(void)
{
#if defined IAP_MSD_APP
    
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_START_ADDR-NVIC_VectTab_FLASH);
    delay_init();
 	LED_Init();
    while(1)
    {
        LED = 0;
        delay_ms(500);
        LED = 1;
        delay_ms(500);
    }
    
#else    
    
    static int time_tick = 3; // 1S delay
    
	delay_init();
    LED_Init();
	NVIC_Configuration();
	
	delay_ms(500);
 	usb_port_set(0); 	//USB先断开
	delay_ms(300);
   	usb_port_set(1);	//USB再次连接
    
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();
    
	while(1)
	{
        delay_ms(1000);
        if((time_tick > 0) && (system_status == (SYS_STATUS_USB_OFF | SYS_STATUS_UPDATE_OFF)))
        {
            if(--time_tick <= 0)
            {
                time_tick = 0;
                system_info = SYS_EVENT_RUN_APP;
            }
        }
        
        (void)EventDispatch();
	};
    
#endif
}

