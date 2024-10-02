#ifndef IO_MAP_H
#define IO_MAP_H

#ifndef CLK_FREQ
#ifndef _MSC_VER
#warning("CLKFREQ = 95000000")
#endif
#define CLK_FREQ          (100000000)
#endif

//Bus address mapping
// Here V4K_50T means new MIPS
//#ifdef V4K_50T

#define AXILITE_IO        ((0x1<<31)+(0x00<<24))   //previous: HWA
#define UART_IO           ((0x1<<31)+(0x01<<24))
#define XMII_IO           ((0x1<<31)+(0x02<<24))
#define I2C_IO            ((0x1<<31)+(0x03<<24))
#define VIDEO_IN          ((0x1<<31)+(0x04<<24))   //same!!
#define VIDEO_OUT         ((0x1<<31)+(0x04<<24))   //same!!
#define GPIO_IO           ((0x1<<31)+(0x05<<24))
#define DDR_BUF0          ((0x1<<31)+(0x06<<24))
#define SPI_IO            ((0x1<<31)+(0x07<<24))
#define IR_IO	          ((0x1<<31)+(0x08<<24))
#define MUTEX_IO          ((0x1<<31)+(0x09<<24))   //previous: DDR3_IO
#define CMDR_IO           ((0x1<<31)+(0x0a<<24))   //previous: VIN_FORMAT_IO
#define VCLK_IO           ((0x1<<31)+(0x0b<<24))
#define VCLK2_IO          ((0x1<<31)+(0x0c<<24))
#define VESA_IO    		  ((0x1<<31)+(0x0d<<24))   //previous: RESERVED_13_IO
#define RESERVED_14_IO    ((0x1<<31)+(0x0e<<24))   //previous: CTRL_RISC_IO
#define DMA_IO            ((0x1<<31)+(0x0f<<24))   //VESA DMA
#define AUDIO_IO          ((0x1<<31)+(0x10<<24))
#define RESERVED_17_IO    ((0x1<<31)+(0x11<<24))   //previous: XMII_DRP_IO
#define I2C_IO1           ((0x1<<31)+(0x12<<24))   //previous: RESERVED_18_IO
#define VIDEO_MUX         ((0x1<<31)+(0x13<<24))
#define PROFILER_IO    	  ((0x1<<31)+(0x14<<24))
#define DISCARD_FIFO      ((0x1<<31)+(0x15<<24))   //previous: APB0_IO
#define BITMEM            ((0x1<<31)+(0x16<<24))   //previous: APB1_IO
#define RESERVED_23_IO    ((0x1<<31)+(0x17<<24))   //previous: DDRCLK_IO
#define MTDMA_IO          ((0x1<<31)+(0x18<<24))   //previous: SLAVE_IO
#define LONGTAIL_IO       ((0x1<<31)+(0x19<<24))   //previous: UVFDDR
//#define NETCNT_IO	  	  ((0x1<<31)+(0x19<<24))   //For decoder to check the input clock frequency
#define CABAC_BIN_IO      ((0x1<<31)+(0x1a<<24))   //previous: SDI_IO
#define BSDMA_IO	      ((0x1<<31)+(0x1b<<24))   //previous: AXI_CFG_IO
#define DEBUGGER          ((0x1<<31)+(0x1c<<24))   //previous: COLORBAR_VOUT_IO,temporary add to test colorbar module
#define MPU_PROTECT       ((0x1<<31)+(0x1d<<24))
#define DFLOW_IO          ((0x1<<31)+(0x1e<<24))
#define HW_VERSION        ((0x1<<31)+(0x1f<<24))

//#define VIDEO_OUT         VOUT_IO

//Not used now
#define MAIL_BOX          0
#define GMII_IO           0
#define GMII_DFM0         0
#define GMII_DFM1         0
#define VOUT_DFM0         0
#define VOUT_DFM1         0
#define DDR_BUF1          0
#define VOUT_MUX          0
#define ITF60G            0

//#endif

//DFM command
#define DFM_NO_CONFIG     0
#define DFM_SIZE          1
#define DFM_MIN           2
#define DFM_WBINC         3
#define DFM_RBINC         4
#define DFM_PUSH          5
#define DFM_POP           6
#define DFM_BUFCNT        7
#define DFM_SPACE         8

#endif
