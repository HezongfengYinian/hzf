#include <rtthread.h>
#include "board.h"

#ifdef RT_USING_RTC
#include "stm32f2_rtc.h"
#endif /* RT_USING_RTC */

#ifdef RT_USING_LWIP
extern void rt_hw_stm32_eth_init(void);
#endif

#ifdef RT_USING_SPI
#include "stm32f20x_40x_spi.h"
#include "spi_flash_w25qxx.h"





#define I2C_SLAVE_ADDR_FM11GE300    (0x60)      // no r/w bit (0xC0)

// FM11GE300 command define
#define FM11GE300_MEM_WRITE         (0x3B)
#define FM11GE300_MEM_READ          (0x37)
#define FM11GE300_SREG_WRITE        (0x68)
#define FM11GE300_LREG_WRITE        (0x6A)
#define FM11GE300_REG_READ          (0x60)


// FM11GE300 register addr and value define
/*the following parameter can only be changed at initialization stage*/
#define FM11GE300_DVENABLE          (0x1E30)    //General enable
#define DVENABLE_UART_ENABLE        (0x400)
#define DVENABLE_PARSER_ENABLE      (0x200)
#define DVENABLE_SER_ENABLE         (0x100)
#define DVENABLE_LINEOUT_ENABLE     (0x20)
#define DVENABLE_LINEIN_ENABLE      (0x10)
#define DVENABLE_SPKOUT_ENABLE      (0x08)
#define DVENABLE_MIC1_ENABLE        (0x02)
#define DVENABLE_MIC0_ENABLE        (0x01)

#define FM11GE300_MICRVMODE         (0x1E32)    // Revert mode, and mic1 will be selected

#define FM11GE300_MICPGAGAIN        (0x1E34)    // SW gain to preset. It will transfer to HW gain later

#define FM11GE300_LINEINPGAGAIN     (0x1E35)    // as above

#define FM11GE300_LINEOUTPGAGAIN    (0x1E36)    // as above

#define FM11GE300_SPKPGAGAIN        (0x1E37)    // as above

#define FM11GE300_DIGICTRL          (0x1E38)    // Choose clk, fsync, and digi length 
#define DIGICTRL_SERCLK_INT         (0x4000)
#define DIGICTRL_SERSYN_INT         (0x100)
#define DIGICTRL_SERCLK_DLY         (0x10)
#define DIGICTRL_SERWORD_LEN16      (0x0f)
#define DIGICTRL_SERWORD_LEN8       (0x07)

#define FM11GE300_DVFLAG            (0x1E3A)    // set it 0 to start device
#define DVFLAG_READY                (0x8000)

#define FM11GE300_PWDSET            (0x1E51)    // put to powerdonw mode
#define PWDSET_NEEDRELOAD           (0xD000)
#define PWDSET_NONEEDRELOAD         (0xC000)

#define FM11GE300_DSPMIPS           (0x1E52)

#define FM11GE300_GPI               (0x1E6A)

#define FM11GE300_EARSPKGAIN        (0x1E75)

/*the following parameters can be changed at run time only with precaution*/
#define FM11GE300_HW_MICPGAGAIN     (0x3FC0)    // Transfered from SW gain. Do not modify

#define FM11GE300_HW_LINEINPGAGAIN  (0x3FC1)    // as above

#define FM11GE300_HW_SPKPGAGAIN     (0x3FC6)    // as above

#define FM11GE300_UARTBAUDRATE      (0x3FD9)    

/*the following parameters need to be preset according to application, placement and component*/
#define FM11GE300_SPKOUTDRCSLANT    (0x1E00)    // 

#define FM11GE300_SPKOUTDRCLEVEL    (0x1E01)

#define FM11GE300_LOUTDRCSLANT      (0x1E07)

#define FM11GE300_LOUTDRCLEVEL      (0x1E08)

#define FM11GE300_SPKDBDROP         (0x1E0F)

#define FM11GE300_SPKDBDECAY        (0x1E10)

#define FM11GE300_LECREFPOWTH        (0x1E1D)

#define FM11GE300_MICHPFTYPE        (0x1E3B)

#define FM11GE300_MICVOLUME         (0x1E3D)

#define FM11GE300_SPKVOLUME         (0x1E3E)

#define FM11GE300_SPKMUTE           (0x1E3F)

#define FM11GE300_MICMUTE           (0x1E40)

#define FM11GE300_NUMOFMICS         (0x1E41)    //quite important, must preset

#define FM11GE300_MURXEXC           (0x1E42)

#define FM11GE300_MUTXEXC           (0x1E43)

#define FM11GE300_KLCONFIG          (0x1E44)

#define FM11GE300_SPFLAG            (0x1E45)

#define FM11GE300_FTFLAG            (0x1E46)

#define FM11GE300_MICNSSLEVEL       (0x1E47)

#define FM11GE300_MICNSLOWBANDGAIN  (0x1E48)

#define FM11GE300_MICNSHIGHBANDGAIN (0x1E49)

#define FM11GE300_AECREFGAIN        (0x1E4D)

#define FM11GE300_LECREFGAIN        (0x1E4E)

#define FM11GE300_FFTIFFT           (0x1E4F)

#define FM11GE300_SPKVOLUMECAP      (0x1E50)

#define FM11GE300_MICSATTH          (0x1E57)

#define FM11GE300_LOUTCLIPTH        (0x1E59)

#define FM11GE300_YOUTSATTH         (0x1E5A)

#define FM11GE300_STHDTIME          (0x1E5C)

#define FM11GE300_MICTESTFLAG       (0x1E61)

#define FM11GE300_AECDELAYLENGTH    (0x1E63)    // extra time to compensate AD DA delay

#define FM11GE300_LECDELAYLENGTH    (0x1E64)    // extra time to compensate AD DA delay

#define FM11GE300_VOLINCSTEP        (0x1E6E)    // Step for volup and voldn button

#define FM11GE300_PWDDEVICEOFF      (0x1E70)

#define FM11GE300_PFZFACTOREXPHIGH  (0x1E86)

#define FM11GE300_PFZFACTOREXPLOW   (0x1E87)

#define FM11GE300_FETHYOUT          (0x1E88)

#define FM11GE300_PFCOEFGAIN        (0x1E8B)

#define FM11GE300_FEVADTHBIG        (0x1E8C)

#define FM11GE300_AECNWSHIFT        (0x1E90)

#define FM11GE300_AECFEVADSHIFT     (0x1E91)

#define FM11GE300_LECPFZFACTOREXP   (0x1E94)

#define FM11GE300_LECPOSTMU         (0x1E95)

#define FM11GE300_LECMUSHIFT        (0x1E96)

#define FM11GE300_NOISEGAIN         (0x1E9A)

#define FM11GE300_MICGAIN0          (0x1E9B)

#define FM11GE300_MICGAIN1          (0x1E9C)

#define FM11GE300_MICAGCMINAGC      (0x1E9F)

#define FM11GE300_MICAGCMAXAGC      (0x1EA0)

#define FM11GE300_MICAGCREFLOW      (0x1EA3)

#define FM11GE300_MICAGCREFHIGH     (0x1EA4)

#define FM11GE300_LINEINAGCREF      (0x1EA8)

#define FM11GE300_LINEINAGCMINAGC   (0x1EA9)

#define FM11GE300_LINEINAGCMAXAGC   (0x1EAA)

#define FM11GE300_FQPERIOD          (0x1EBC)

#define FM11GE300_FQBETAV           (0x1EBD)

#define FM11GE300_FQBETAUV          (0x1EBE)

#define FM11GE300_FEVADGAINLIMIT    (0x1EC5)

#define FM11GE300_FQFEVADGAINLOW    (0x1EC6)

#define FM11GE300_FQFEVADGAINHIGH   (0x1EC7)

#define FM11GE300_LINEOUTEQUAL0     (0x1EC9)

#define FM11GE300_LINEOUTEQUAL1     (0x1ECA)

#define FM11GE300_LINEOUTEQUAL2     (0x1ECB)

#define FM11GE300_LINEOUTEQUAL3     (0x1ECC)

#define FM11GE300_LINEOUTEQUAL4     (0x1ECD)

#define FM11GE300_VAD0CEILLOW       (0x1ED4)

#define FM11GE300_VAD0CEILHIGH      (0x1ED5)

#define FM11GE300_VAD0RATTHRDFE     (0x1ED6)

#define FM11GE300_VAD0RATTHRDNOFE   (0x1ED7)

#define FM11GE300_INVCONST1         (0x1ED8)

#define FM11GE300_CLIPTH1           (0x1EDA)

#define FM11GE300_CLIPTH2           (0x1EDB)

#define FM11GE300_YOUTDESCON0HIGH   (0x1EDE)

#define FM11GE300_YOUTDESCON0LOW    (0x1EDF)

#define FM11GE300_VAD3RATTHRD       (0x1EED)

#define FM11GE300_VAD3MULT          (0x1EEE)

#define FM11GE300_VAD12DIFFCEIL     (0x1EF3)

#define FM11GE300_VAD12DIFFMAX      (0x1EF4)

#define FM11GE300_VAD1RATTHRD       (0x1EFC)

#define FM11GE300_VAD1ADDTHRD       (0x1EFE)

#define FM11GE300_VAD2RATTHRD       (0x1F06)

#define FM11GE300_VAD2ADDTHRD       (0x1F07)

#define FM11GE300_LECPFMINGAIN      (0x1F08)

/*Parameters associated with PCM extention mode*/
#define FM11GE300_PCMEXTMODE        (0x1E77)

#define FM11GE300_PCMEXTMICSWITCH   (0x1E78)

#define FM11GE300_PCMEXTMICGAIN     (0x1E79)

/*Read only parameters*/
#define FM11GE300_FRAMECOUNTER      (0x1E65)

#define FM11GE300_WATCHDOGCOUNT     (0x1E5E)

#define FM11GE300_MIC_SAT_COUNT     (0x1E5F)

#define FM11GE300_VADLEDFLAGS       (0x1E80)

#define FM11GE300_STRAPOPTIONSTATUS (0x3FCE)

#define FM11GE300_REFMICCALIBRATIONGAIN (0xB82)

#define FM11GE300_LINEINAGCGAIN     (0xB41)

#define FM11GE300_MICINAGCGAIN      (0x1BC3)


#define FM11GE300_MEM_WRITE         (0x3B)
#define FM11GE300_MEM_READ          (0x37)
#define FM11GE300_SREG_WRITE        (0x68)
#define FM11GE300_LREG_WRITE        (0x6A)
#define FM11GE300_REG_READ          (0x60)


rt_int8_t InitialCodec(void);

void delay_us(uint32_t us)
{
    volatile rt_uint32_t s ;
    // volatile uint32_t value;
    // uint32_t t = us;
#if 0
    s = _get_microsecond();

    while (1)
    {
        value = _get_microsecond() - s;
        if (value >= t)
            return;
    }
#else
    for (s = 0; s < us; s++)
    {
        // for (value = 0; value < 32; value++);
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    }
#endif
}


void fm11_gpio_init(void)
{
	//reset stm32l151
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);    
    //GPIO_ResetBits(GPIOD,GPIO_Pin_5);    
    GPIO_SetBits(GPIOD,GPIO_Pin_5);



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;   

    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    GPIO_ResetBits(GPIOA,GPIO_Pin_12);    
    //GPIO_SetBits(GPIOD,GPIO_Pin_5);
    delay_us(2003);

	GPIO_SetBits(GPIOA, GPIO_Pin_12);

}





rt_uint8_t Codec_Uart_Puts(rt_uint8_t *p, rt_uint8_t len)
{
	rt_uint8_t i,j;
	if(p==NULL)
    {   
        return 1;
    }
    
    GPIO_SetBits(GPIOD,GPIO_Pin_5);
    delay_us(326);
    
    for(i=0; i<len; i++)
    {            
        rt_uint8_t dat = *(p+i);
        GPIO_ResetBits(GPIOD,GPIO_Pin_5);  //start bit
        delay_us(326);
        for(j=0; j<8; j++)
        {
            //CODEC_UART_TX = dat & 0x01;  //data bit 0-7, LSB
            if(dat & 0x01)
				GPIO_SetBits(GPIOD,GPIO_Pin_5);
			else
				GPIO_ResetBits(GPIOD,GPIO_Pin_5); 
            delay_us(326);
            dat >>=1;
        }
        GPIO_SetBits(GPIOD,GPIO_Pin_5);  //stop bit;
        delay_us(326);
    }
    return 0;
}



/******************************************************************************
** Description: write fm11ge300 memory
** Input param: 
        uint16 addr      address of writing-memory
        uint8 *pdata    pointer of write-buf
        uint8 len       length of write-buf
** Return value:
        0 :  success
        others : fail
*******************************************************************************/
rt_int8_t fm11ge300_write_memory(rt_uint16_t *paddr, rt_uint16_t *pdata, rt_uint8_t len)
{
    rt_uint8_t i = 0;
    rt_uint8_t rs = 0;
    rt_uint8_t tmp[7];
    rt_uint16_t tmp16 = 0;
    
    tmp[0] = 0xFC;
    tmp[1] = 0xF3;
    tmp[2] = FM11GE300_MEM_WRITE;
    
    for (i=0; i<len; i++) {
        tmp16 = *paddr + i;
        tmp[3] = (tmp16 >> 8) & 0xff;
        tmp[4] = tmp16 & 0xff;

        tmp16 = *(pdata + i);
        tmp[5] = (tmp16 >> 8) & 0xff;
        tmp[6] = tmp16 & 0xff;
        
        rs = Codec_Uart_Puts(tmp, 7);
        if (rs != 0) {
            return -1;
        }
    }
    return 0;
}

__IO rt_uint32_t SPI1_TX_Buf[320];
__IO rt_uint32_t SPI2_RX_Buf[320];


/******************************************************************************
** Description: initial fm11ge300 
** Return value:
        0 :  success
        others : fail
*******************************************************************************/
rt_int8_t InitialCodec(void)
{
    rt_uint16_t tmp = 0;
    rt_uint16_t tmp2 = 0;
    rt_uint8_t rs = 0;
    //uint8 cnt = 0;
	GPIO_ResetBits(GPIOD,GPIO_Pin_5);    
    delay_us(32900);   // wait for FM11GE300 setup
    
    //Codec_Uart_Init();
    GPIO_SetBits(GPIOD,GPIO_Pin_5);
    //Codec_Uart_DelayUs(codec_uart_dalay);
    delay_us(326);

	delay_us(326);


#if 0

	//Eaable Mic0、speaker out、Serial Port、Parser
	//Write_by_Uart(0x1e30,(1 | (1 << 3) | (1 << 8) | (1 << 9)));
	tmp = (1 | (1 << 3) | (1 << 8) | (1 << 9));
    tmp2 = 0x1e30;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
    if (rs != 0) {
        return -1;
    }

	
//	Write_Command(0x1e30,0x033b);	 //default
	//disable Mic_revert_mode
	//Write_by_Uart(0x1e32,0x0); 		//default
	tmp = 0;
	tmp2 = 0x1e32;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//Mic0 PGA Gain +26DB
	//Write_by_Uart(0x1e34,0x00ff);

	tmp = 0x00ff;
	tmp2 = 0x1e34;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	
	//speaker Gain	+2dB
	//Write_by_Uart(0x1e37,0x001f);	//min valume is -10db

	tmp = 0x001f;
	tmp2 = 0x1e37;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//Data format :16Bit; FM1188 Slave MCU Master PCM:one clock dealy
//	Write_Command(0x1e38,0x411f);	//master	  
	Write_by_Uart(0x1e38,0x001f);	//slave
	tmp = 0x001f;
	tmp2 = 0x1e38;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	
	//The DSP is ready to receive external host parameter downloading through UART or SHI only when ready flag is set.
//	Write_Command(0x1e3a,0x8000);
	//need to reload parameter after wakeup from power down
	//Write_by_Uart(0x1e51,0xd000);
	
	tmp = 0xd000;
	tmp2 = 0x1e51;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//This parameter defines the spk_pgagain in earpiece mode. SCL (Pin23) set to high will set FM1188 to earpiece mode when not in SHI mode
	//Write_by_Uart(0x1e75,0x001f);

	tmp = 0x001f;
	tmp2 = 0x1e75;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}

	//Write_by_Uart(0x1e4a,0x7fff);

	tmp = 0x7fff;
	tmp2 = 0x1e4a;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
/********************************************************************************************
has to be preset according to the application and physical placement of micropone and speaker
*********************************************************************************************/
	//Mic voluma  2.5X
	//Write_by_Uart(0x1e3d,0x0300);	//2.5X
	tmp = 0x0300;
	tmp2 = 0x1e3d;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}

	
	//speaker voluma 2.5x
	//Write_by_Uart(0x1e3e,0x0200); 	//4X
	tmp = 0x0200;
	tmp2 = 0x1e3e;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//speaker mute no mute
	//Write_by_Uart(0x1e3f,0xffff);
	tmp = 0xffff;
	tmp2 = 0x1e3f;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//mic mute		no mute
	//Write_by_Uart(0x1e40,0xffff);
	tmp = 0xffff;
	tmp2 = 0x1e40;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//Mic num is one
	//Write_by_Uart(0x1e41,0x0101);
	tmp = 0x0101;
	tmp2 = 0x1e41;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//K1 config	 Disable DRC   0x1095 = Enable DRC
	//Write_by_Uart(0x1e44,0x10a5);
	tmp = 0x10a5;
	tmp2 = 0x1e44;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//ft_flag	magnitude can be set by parameter 0x1EA0 (noisegain)
	//Write_by_Uart(0x1e46,0x007d);	// 待验证		//1Ea0,1F44 1F45
	tmp = 0x007d;
	tmp2 = 0x1e46;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//扬声器衰减及衰减时间 衰减使能取决于  0x1e44  bit5
	//Write_by_Uart(0x1e4a,0x5a9d);	//-3db
	tmp = 0x5a9d;
	tmp2 = 0x1e4a;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//Write_by_Uart(0x1eab,0x0044);
	tmp = 0x0044;
	tmp2 = 0x1eab;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
//	Write_Command(0x1e47,0x2600);	//default
	//Mic AGC default、Max、Min
	//Write_by_Uart(0x1eab,0x0074);	//ref
	tmp = 0x0074;
	tmp2 = 0x1eab;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//Write_by_Uart(0x1ea8,0x2000);	//+6Db
	tmp = 0x2000;
	tmp2 = 0x1ea8;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	//Write_by_Uart(0x1ea7,0x0400);	//-6db
	tmp = 0x0400;
	tmp2 = 0x1ea7;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}

	//echo cancellation parameter
	/*This parameter specifies the gain for mic0. Since the omni directional microphone normally has higher 
	sensitivity, so that we tend to lower the gain for mic1. Unit gain is 0x2000. The micgain0 is applied to mic0 
	after Linear AEC process.*/
	//Write_by_Uart(0x1ea1,0x2000);	// default 	0x4000	 0x2000
	tmp = 0x2000;
	tmp2 = 0x1ea1;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the Vad1 detection threshold. Higher values make it harder to detect vad1.*/
	//Write_by_Uart(0x1eff,0x1800); 	//default;
	tmp = 0x1800;
	tmp2 = 0x1eff;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This is a VAD1 detection factor under noise environment. Smaller values make the detection easier.*/
	//Write_by_Uart(0x1efd,0x0700);	//default
	tmp = 0x0700;
	tmp2 = 0x1efd;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the VAD2 detection threshold. Higher values make it harder to detect VAD2.*/
	//Write_by_Uart(0x1f00,0x0500);	//default 0x1000
	tmp = 0x0500;
	tmp2 = 0x1f00;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This is a VAD2 detection factor under noise environment. Smaller values make the detection easier.*/
	//Write_by_Uart(0x1efe,0x0800); 	//default 0x0400 //值还可以往上调	0x0800
	tmp = 0x0800;
	tmp2 = 0x1efe;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*The magnitude can be calculated as 20 log (coefficient/0x7fff) db. For example, the default suppression level is 
	20 log (0x2000/0x7fff) = -12dB. If the coefficient sets to 0x7fff, then the noise suppression is 0 db, i.e. no noise 
	suppression.*/
	//Write_by_Uart(0x1e47,0x0c00);	//noise suppression is -20dB; default -10dB
	tmp = 0x0c00;
	tmp2 = 0x1e47;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the noise gain in band one. SNR ration = Signal/(mic_ns_lowband_gain * noise). The 
	bigger the value, the more the noise suppression will be, but the worse the SNR. 0x800 is unit gain, 0x1200 = 
	2.25X*/
	//Write_by_Uart(0x1e48,0x1200);	//default
	tmp = 0x1200;
	tmp2 = 0x1e48;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the noise gain in band other than band one. SNR ration = Signal/(mic_ns_highband_gain 
	* noise). The bigger the value, the more the noise suppression, but the worse the SNR 0x800 is unit gain.*/
	//Write_by_Uart(0x1e49,0x0400);	//default	0x0800
	tmp = 0x0400;
	tmp2 = 0x1e49;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter specifies the max echo cancellation when has FE voice in frequency domain. The smaller the 
	value, the higher the limit of echo cancellation in frequency domain will be. Frequency domain echo 
	cancellation limit = 20 log (fevad_gain_limit/32768), the default value 0x0300 = -32 db. */
	//Write_by_Uart(0x1eca,0x0064); 	//default 0x0300
	tmp = 0x0064;
	tmp2 = 0x1eca;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the far end VAD gain of low frequency section used in frequency domain for echo 
	cancellation when there is FE voice. The bigger the gain, the more the echo cancellation in frequency domain 
	will be. 0x4000 is unit gain.*/
	//Write_by_Uart(0x1ecb,0x1200);	//default	0x1200
	tmp = 0x1200;
	tmp2 = 0x1ecb;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the far end VAD gain of high frequency section used in frequency domain for echo 
	cancellation when there is FE voice. The bigger  the gain, specifies the more echo cancellation. 0x4000 is unit 
	gain.*/
	//Write_by_Uart(0x1ecc,0x1000);  	//default	0x1000
	tmp = 0x1000;
	tmp2 = 0x1ecc;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This is a VAD0 detection factor at noise environment. Higher value make it harder to detect VAD0.
	When the voice is too loud, the microphone may be saturated and the pipe sound may be heard from 
	lineout. Then the echo cancellation will get into non-linear region. It requires special process to take care 
	of echo cancellation, and it is called big echo cancellation. And the echo at the time is called big echo*/
	//Write_by_Uart(0x1ee4,0x0700);	//default
	tmp = 0x0700;
	tmp2 = 0x1ee4;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}

	//Aoucstic echo cancellation
	/*This parameter defines the threshold of fe_vad_big. Higher values can increase VAD0 decision and decrease 
	echo suppression. The effect is less echo suppression and more full duplex.*/
	//Write_by_Uart(0x1e8b,0x2500);	//default 0x0500
	tmp = 0x2500;
	tmp2 = 0x1e8b;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter specified the threshold of fe_vad decision. Higher values render less adaptation on Linear AEC 
	and more pass-through on AEC post-filter.*/
	//Write_by_Uart(0x1e8c,0x0100);  	//default
	tmp = 0x0100;
	tmp2 = 0x1e8c;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines mic AGC level control when mic-in AGC is enabled (Bit[15] of sp_flag (0x1E45) is set 
	to “1”). Higher value increases the adjustable mic -in power range. The current mic AGC gain can be read back 
	on Mic_agc_gain (0x1F42 and 0x1F43).*/
	//Write_by_Uart(0x1e45,0xd7d5);	//*****验证
	tmp = 0xd7d5;
	tmp2 = 0x1e45;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This is the gain to multiply with speaker feedback to cancel the echo. If microphone and speaker are placed 
	closer, the amount of echo need to be cancelled would be bigger, then the gain  need to be bigger in order to 
	cancel the echo. 0x0100 is unit gain. */
	//Write_by_Uart(0x1e4d,0x0001); 	//default 0x0060
	tmp = 0x0001;
	tmp2 = 0x1e4d;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter defines the extra delay length of AEC reference in order to compensate the delay of ADC/DAC. 
	Minimum value is 0x1.*/
	//Write_by_Uart(0x1e63,0x0010);	//default	0x0010
	tmp = 0x0010;
	tmp2 = 0x1e63;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*his parameter is used to calculate the AEC mu. The larger the value, the faster the echo converges.*/
	//Write_by_Uart(0x1e90,0x0001);	//default
	tmp = 0x0001;
	tmp2 = 0x1e90;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*If far end voice is detected, this value is added to 0x1E90 to calculate the AEC mu. The larger the value, the 
	faster converge the echo is, but more unstable the system will be.*/
	//Write_by_Uart(0x1e91,0x0001);	//default
	tmp = 0x0001;
	tmp2 = 0x1e91;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}



	//non-linear AEC   POST-Filter ctrl by 0x1e45
	/*This parameter defines the VAD3 ratio threshold. The bigger the value, the harder the VAD3 will be detected*/
	//Write_by_Uart(0x1eef,0x0001);
	tmp = 0x0001;
	tmp2 = 0x1eef;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter is used for echo cancellation when there is no VAD. When there is no VAD3 (i.e. no near end 
	talk), more echo cancellation is applied. This parameter is also used in one microphone mode when no double 
	talk is detected.*/
	//Write_by_Uart(0x1e86,0x0006);
	tmp = 0x0006;
	tmp2 = 0x1e86;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	/*This parameter is used for echo cancellation when there is VAD. When VAD3 is detected (i.e. near end talk), 
	then less echo will be cancelled. This parameter is used in one microphone application w hen there is double 
	talk detected*/
	//Write_by_Uart(0x1e87,0x0002);
	tmp = 0x0002;
	tmp2 = 0x1e87;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	
	//ftf_ifft
	//Write_by_Uart(0x1e4f,0x0001);	//default
	tmp = 0x0001;
	tmp2 = 0x1e4f;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
	
	//Write_by_Uart(0x1e3a,0x0);	//Enable DSP "run" Mode
	tmp = 0x0;
	tmp2 = 0x1e3a;
	rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
	if (rs != 0) {
		return -1;
	}
#endif


#if 1

    
    tmp = DVENABLE_UART_ENABLE | DVENABLE_SER_ENABLE
            | DVENABLE_SPKOUT_ENABLE | DVENABLE_MIC0_ENABLE;

	//tmp = 0x55;
    tmp2 = FM11GE300_DVENABLE;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
    if (rs != 0) {
        return -1;
    }

	//delay_us(200);

	rt_kprintf("AAAA\n");
    

    tmp = 0x38;
    tmp2 = FM11GE300_MICPGAGAIN;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
    if (rs != 0) {
        return -1;
    }
	rt_kprintf("BBBB\n");
    tmp = 0x1D;
    tmp2 = FM11GE300_SPKPGAGAIN;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
    if (rs != 0) {
        return -1;
    }    

	rt_kprintf("CCCC\n");

    
    tmp = DIGICTRL_SERCLK_INT | DIGICTRL_SERSYN_INT| DIGICTRL_SERWORD_LEN16 |DIGICTRL_SERCLK_DLY;
    tmp2 = FM11GE300_DIGICTRL;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
    if (rs != 0) {
        return -1;
    }       
    rt_kprintf("DDDD\n");
    tmp = 0x02;
    tmp2 = FM11GE300_NUMOFMICS;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // choose 2 omni mic, may be modified
    if (rs != 0) {
        return -1;
    }
	rt_kprintf("EEEE\n");
    
    tmp = 0x13;
    tmp2 = FM11GE300_DSPMIPS;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // select mips
    if (rs != 0) {
        return -1;
    }
	rt_kprintf("FFFFF\n");
    
/*   
    tmp = 0x07DE;
    tmp2 = FM11GE300_SPFLAG;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // select mips
    if (rs != 0) {
        return -1;
    }
*/    
   
    tmp = 0x01;
    tmp2 = FM11GE300_FFTIFFT;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // mic0 bypass to lineout
    if (rs != 0) {
        return -1;
    }


    
    tmp = 0x200;
    tmp2 = FM11GE300_MICVOLUME;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // mic volume
    if (rs != 0) {
        return -1;
    }

    //tmp = 0x100;
    tmp = 0x200;
    tmp2 = FM11GE300_SPKVOLUME;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // speaker volume
    if (rs != 0) {
        return -1;
    }

    tmp = 0x2000;
    tmp2 = FM11GE300_LECREFPOWTH;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // better than default value 0x2000
    if (rs != 0) {
        return -1;
    }
    
/*    
    tmp = 0x54;
    tmp2 = FM11GE300_FTFLAG;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);  // choose 2 omni mic, may be modified
    if (rs != 0) {
        return -1;
    }
*/   
/*   
    tmp = 0xFFFF;
    tmp2 = FM11GE300_MICRVMODE;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1); // mic revert mode
    if (rs != 0) {
        return -1;
    }   
*/   
    
    // let configer go
    tmp = 0;    // DVFLAG_READY;
    tmp2 = FM11GE300_DVFLAG;
    rs = fm11ge300_write_memory(&tmp2, &tmp, 1);
    if (rs != 0) {
        return -1;
    }  
	rt_kprintf("GGGG\n");
    return 0;
	#endif
}


/*
SPI2_MOSI: PB15
SPI2_MISO: PB14
SPI2_SCK : PB13

CS0: PG10  SPI FLASH
CS1: PB12  TOUCH
CS2: PG7   WIFI
*/

/*---------------------------SPI3----------------------------------*/
#define SPIy                           SPI3
#define SPIy_CLK                       RCC_APB1Periph_SPI3
#define SPIy_IRQn                      SPI3_IRQn
#define SPIy_IRQHANDLER                SPI3_IRQHandler

#define SPIy_SCK_PIN                   GPIO_Pin_10
#define SPIy_SCK_GPIO_PORT             GPIOC
#define SPIy_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOC
#define SPIy_SCK_SOURCE                GPIO_PinSource10
#define SPIy_SCK_AF                    GPIO_AF_SPI3

#define SPIy_MISO_PIN                  GPIO_Pin_11
#define SPIy_MISO_GPIO_PORT            GPIOC
#define SPIy_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPIy_MISO_SOURCE               GPIO_PinSource11
#define SPIy_MISO_AF                   GPIO_AF_SPI3

#define SPIy_MOSI_PIN                  GPIO_Pin_12
#define SPIy_MOSI_GPIO_PORT            GPIOC
#define SPIy_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPIy_MOSI_SOURCE               GPIO_PinSource12
#define SPIy_MOSI_AF                   GPIO_AF_SPI3

void SPI3_Configuration(void)
{

	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE );//PORTB???? 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE );
	RCC_APB1PeriphClockCmd(SPIy_CLK,ENABLE);

	GPIO_DeInit(GPIOC);  
    
	
	GPIO_PinAFConfig(SPIy_SCK_GPIO_PORT, SPIy_SCK_SOURCE, SPIy_SCK_AF);
	GPIO_PinAFConfig(SPIy_MOSI_GPIO_PORT, SPIy_MOSI_SOURCE, SPIy_MOSI_AF);
	GPIO_PinAFConfig(SPIy_MISO_GPIO_PORT, SPIy_MISO_SOURCE, SPIy_MISO_AF);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPIy_SCK_PIN;
	GPIO_Init(SPIy_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* SPI	MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIy_MOSI_PIN;
	GPIO_Init(SPIy_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPIy_MISO_PIN;
	GPIO_Init(SPIy_MISO_GPIO_PORT, &GPIO_InitStructure);

	/* SPI NSS pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//SPI_I2S_DeInit(SPIy);
	SPI_I2S_DeInit(SPIy);

	
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High	SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPIy, &SPI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
   
	SPI_CalculateCRC(SPI3, DISABLE);
	//SPI_TIModeCmd(SPIx,DISABLE);
	// Enable SPI1
	SPI_Cmd(SPIy, ENABLE);
	SPI_I2S_ITConfig( SPI3, SPI_I2S_IT_RXNE, ENABLE );//相当于开启中断
	

}


#if 0

void SPI3_Configuration(void)
{
	

	SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE );//PORTB???? 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);

	
	rt_memset(SPI2_RX_Buf,0,20);
	/*NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);*/

	
	
	/* init gpio*/
	
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);

	//GPIO_PinAFConfig(GPIO_AF_SWJ,)

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* SPI SCK pin configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_PinSource10;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* SPI  MOSI pin configuration */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	 /* SPI NSS pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


	//SPI_I2S_DeInit(SPIy);
	//SPI_Cmd(SPI3, DISABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High   SPI_CPOL_Low
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
	//SPI_I2S_DeInit(SPI3);
    SPI_Init(SPI3, &SPI_InitStructure);
	SPI_TIModeCmd(SPI3,ENABLE);

	SPI_Cmd(SPI3, ENABLE);



	//Enable DMA1 channel IRQ Channel 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	//DMA_DeInit(DMA_Channel_0);
	//DMA_DeInit(DMA_Channel_5); 
	/* DMA Channel2 configuration ----------------------------------------------*/

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI3->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI2_RX_Buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 20;		
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//Normal
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream0, &DMA_InitStructure);


	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI3->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)SPI1_TX_Buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 10;		
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	

	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);
	//DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);

  
    // Enable SPI1

	SPI_Cmd(SPI3, ENABLE);
	SPI_CalculateCRC(SPI3, DISABLE);
	
	

	//SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	//SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);
    

	DMA_Cmd(DMA1_Stream0, ENABLE);  
	DMA_Cmd(DMA1_Stream5, ENABLE);
	//DMA_Cmd(DMA2_Stream0, ENABLE);  
	//DMA_Cmd(DMA2_Stream3, ENABLE);	


}

#endif


static  DMA_InitTypeDef    DMA_InitStructureTx;
static  DMA_InitTypeDef    DMA_InitStructureRx;


unsigned char SPI_Write(unsigned char dat)
{
	while (SPI_GetFlagStatus(SPI3, SPI_FLAG_TXE) == RESET);
	SPI_SendData(SPI3,dat);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_ReceiveData(SPI3); //????SPIx???????	
}


#if 0
void DMA_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	//发送通道	
	DMA_DeInit(DMA2_Stream3);
	DMA_InitStructureTx.DMA_Channel = DMA_Channel_3;
	DMA_InitStructureTx.DMA_PeripheralBaseAddr =(u32)&SPI3->DR;//SPI1的DR寄存器地址
	DMA_InitStructureTx.DMA_Memory0BaseAddr = (uint32_t)&SPI1_TX_Buf;
	DMA_InitStructureTx.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
	DMA_InitStructureTx.DMA_BufferSize = 10;
	DMA_InitStructureTx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructureTx.DMA_MemoryInc = DMA_MemoryInc_Enable;  //DMA内存地址自动增加模式
	DMA_InitStructureTx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructureTx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructureTx.DMA_Mode = DMA_Mode_Normal;    
	DMA_InitStructureTx.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructureTx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructureTx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructureTx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructureTx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;        
	DMA_Init(DMA2_Stream3, &DMA_InitStructureTx);

	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
	//DMA_Cmd(DMA2_Stream3, ENABLE);

	//接收通道
	DMA_DeInit(DMA2_Stream2);
	DMA_InitStructureRx.DMA_Channel = DMA_Channel_3;
	DMA_InitStructureRx.DMA_PeripheralBaseAddr =(u32)&SPI3->DR;//SPI2的DR寄存器地址
	DMA_InitStructureRx.DMA_Memory0BaseAddr = (uint32_t)&SPI2_RX_Buf;//0;//(u32)SPI2_RX_Buf;
	DMA_InitStructureRx.DMA_DIR = DMA_DIR_PeripheralToMemory;  
	DMA_InitStructureRx.DMA_BufferSize = 10;
	DMA_InitStructureRx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructureRx.DMA_MemoryInc = DMA_MemoryInc_Enable;  //DMA内存地址自动增加模式
	DMA_InitStructureRx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructureRx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructureRx.DMA_Mode = DMA_Mode_Normal;    
	DMA_InitStructureRx.DMA_Priority = DMA_Priority_High;
	DMA_InitStructureRx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructureRx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructureRx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructureRx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;                
	DMA_Init(DMA2_Stream2, &DMA_InitStructureRx);

	/* Enable SPI DMA Tx request */

	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);
	//DMA_Cmd(DMA2_Stream2, DISABLE);

	
	DMA_Cmd(DMA2_Stream3, ENABLE);
	DMA_Cmd(DMA2_Stream2, ENABLE);
	 
	DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);      //清DMA发送完成标志
	DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);      //清DMA发送完成标志

}
#endif

void DMA1_Stream0_IRQHandler(void)//tx   主机接收
{
    rt_kprintf(" spi1 tx done!\r\n");
	if (DMA_GetITStatus(DMA1_Stream0,DMA_FLAG_TCIF0)==SET)
	{
		DMA_ClearITPendingBit(DMA1_Stream0,DMA_FLAG_TCIF0|DMA_FLAG_TEIF0);
		rt_kprintf(" spi tx done!\r\n");

		DMA_Cmd(DMA1_Stream0, DISABLE);
	}
}

void DMA1_Stream5_IRQHandler(void)//rx  主机发送
{

  /* Check the interrupt source */
  rt_kprintf(" spi1 rx done!\r\n");
  if (DMA_GetITStatus(DMA1_Stream5,DMA_FLAG_TCIF5)==SET)
  {
      DMA_ClearITPendingBit(DMA1_Stream5,DMA_FLAG_TCIF5|DMA_FLAG_TEIF5);
    
      DMA_Cmd(DMA1_Stream5, DISABLE);
      rt_kprintf(" spi rx done!\r\n");
        
  }
}

void SPI3_IRQHandler(void)
{
	rt_uint32_t i;

	 rt_kprintf(" spi rx done!\r\n");
	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET)  
	{    
		SPI2_RX_Buf[0] = SPI_I2S_ReceiveData(SPI3);
		//SPI_I2S_ClearITPendingBit( SPI3 , SPI_I2S_IT_RXNE ); 
		
		/*for(i = 0;i < 20;i++)
		{
			while(SPI_I2S_GetFlagStatus(SPI3 , SPI_I2S_FLAG_RXNE)==RESET );
				{
				SPI2_RX_Buf[i] = SPI_I2S_ReceiveData(SPI3);
				SPI_I2S_ClearITPendingBit( SPI3 , SPI_I2S_IT_RXNE );  
				}
			
		}*/
	}
}


/*void SPI3_IRQHandler(void)  
{  
	rt_kprintf(" spi3 rx\r\n");

	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET)  
	{    
		SPI_I2S_ClearITPendingBit( SPI3 , SPI_I2S_IT_RXNE);
		
		rt_kprintf(" spi3 rx done!\r\n");
		//for(i = 0;i < SPI2_RxDataLength;i++)  
		//{  
		//while(SPI_I2S_GetFlagStatus(SPI3 , SPI_I2S_FLAG_RXNE)==RESET );  
		//SPI2_RxBuf[i] = SPI_I2S_ReceiveData(SPI2);  
		//}  
	}  
} */

static void rt_hw_spi2_init(void)
{
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*!< SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);

        stm32_spi_register(SPI2, &stm32_spi, "spi2");
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /* spi21: PG10 */
        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_9;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", (void*)&spi_cs);
    }
}

static void rt_hw_spi1_init(void)
{
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*!< SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

        stm32_spi_register(SPI1, &stm32_spi, "spi1");
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /* spi21: PG10 */
        spi_cs.GPIOx = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_4;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);
    }
}

#endif /* RT_USING_SPI */

void rt_platform_init(void)
{
#ifdef RT_USING_SPI
    rt_hw_spi2_init();
    rt_hw_spi1_init();
#ifdef RT_USING_DFS
    w25qxx_init("flash0", "spi20");
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    /* initilize touch panel */
    rtgui_touch_hw_init("spi21");
#endif /* RT_USING_RTGUI */
#endif /* RT_USING_SPI */

#ifdef RT_USING_USB_HOST
    /* register stm32 usb host controller driver */
    rt_hw_susb_init();
#endif

#ifdef RT_USING_DFS
    /* initilize sd card */
    //rt_hw_sdcard_init();
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    /* initilize ra8875 lcd controller */
    ra8875_init();

    /* initilize key module */
    rt_hw_key_init();
#endif /* RT_USING_RTGUI */

#ifdef RT_USING_RTC
    rt_hw_rtc_init();
#endif /* RT_USING_RTC */

#ifdef RT_USING_LWIP
    rt_hw_stm32_eth_init();
#endif

	SPI3_Configuration();
	//DMA_Config();
	
	
	while(1)
	{
		rt_kprintf("BBBB\n");

		
	 	rt_kprintf("%d,%d,%d,%d,%d,%d,%d,%d\n",SPI2_RX_Buf[0],SPI2_RX_Buf[1],SPI2_RX_Buf[2],SPI2_RX_Buf[3],SPI2_RX_Buf[4],SPI2_RX_Buf[5],SPI2_RX_Buf[6],SPI2_RX_Buf[7]);


		feed_watchdog();
		delay_us(900000);
	}

    rt_thread_delay(50);
    rt_device_init_all();
}

