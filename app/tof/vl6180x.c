#include "vl6180x.h"
#include "misc.h"
#include "elog.h"
#include "gpio.h"
#include "i2c.h"

#undef LOG_TAG
#define LOG_TAG    "ToF"

#define VL6180x_ADDR 0x52

/* Definitions of VL6180x internal register  */
#define IDENTIFICATION_MODEL_ID 		0x001
#define IDENTIFICATION_MODEL_REV_MAJOR	0x002
#define IDENTIFICATION_MODEL_REV_MINOR	0x003
#define IDENTIFICATION_MODULE_REV_MAJOR 0x004
#define IDENTIFICATION_MODULE_REV_MINOR 0x005
#define IDENTIFICATION_DATE_HI	0x006
#define IDENTIFICATION_DATE_LO 	0x007
#define IDENTIFICATION_TIME	0x008
#define SYSTEM_MODE_GPIOO	0x010
#define SYSTEM_MODE_GPIO1	0x011
#define SYSTEM_HISTORY_CTRL	0x012
#define SYSTEM_INTERRUPT_CONFIG_GPIO    0x014
#define SYSTEM_INTERRUPT_CLEAR          0x015
#define SYSTEM_FRESH_OUT_OF_RESET	    0x016
#define SYSTEM_GROUPED_PARAMETER_HOLD   0x017
#define SYSRANGE_START  			    0x018
#define SYSRANGE_THRESH_HIGH
#define SYSRANGE_THRESH_LOW
#define SYSRANGE_INTERMEASUREMENT_PERIOD
#define SYSRANGE_MAX_CONVERGENCE_TIME
#define SYSRANGE_CROSSTALK_COMPENSATION_RATE
#define SYSRANGE_CROSSTALK_VALID_HEIGHT
#define SYSRANGE_EARLY_CONVERGENCE_ESTIMATE
#define SYSRANGE_PART_TO_PART_RANGE_OFFSET
#define SYSRANGE_RANGE_IGNORE_VALID_HEIGHT
#define SYSRANGE_RANGE_IGNORE_THRESHOLD
#define SYSRANGE_MAX_AMBIENT_LEVEL_MULT
#define SYSRANGE_RANGE_CHECK_ENABLES
#define SYSRANGE_VHV_RECALIBRATE
#define SYSRANGEVHV_REPEAT_RATE
#define SYSALS_START
#define SYSALS_THRESH_HIGH
#define SYSALS_THRESH_LOW
#define SYSALS_INTERMEASUREMENT_PERIOD
#define SYSALS_ANALOGUE_GAIN
#define SYSALS_INTEGRATION_PERIOD
#define RESULTRANGE_STATUS
#define RESULT_ALS_STATUS
#define RESULT_INTERRUPT_STATUS_GPIO
#define RESULT_ALS_VAL
#define RESULT_HISTORY_BUFFER_X
#define RESULT_RANGE_VAL	0x062
#define RESULT_RANGE_RAW	0x063
#define RESULT_RANGE_RETURN_RATE
#define RESULT_RANGE_REFERENCE_RATE
#define RESULT_RANGE_RETURN_SIGNAL_COUNT
#define RESULT_RANGE_REFERENCE_SIGNAL_COUNT
#define RESULT_RANGE_RETURN_AMB_COUNT
#define RESULT_RANGE_REFERENCE_AMB_COUNT
#define RESULT_RANGE_RETURN_CONV_TIME
#define RESULT_RANGE_REFERENCE_CONV_TIME
#define READOUT_AVERAGING_SAMPLE_PERIOD
#define FIRMWAREBOOTUP
#define FIRMWARE_RESULT_SCALER
#define I2C_SLAVEDEVICE_ADDRESS
#define INTERLEAVED_MODE_ENABLE


struct add_val {
	uint16_t addr;
	uint8_t  val;
};

static struct add_val vl6180x_init_sequence[] = {
	{0x0207,0x01},
	{0x0208,0x01},
	{0x0096,0x00},
	{0x0097,0xFD},
	{0x00E3,0x00},
	{0x00E4,0x04},
	{0x00E5,0x02},
	{0x00E6,0x01},
	{0x00E7,0x03},
	{0x00F5,0x02},
	{0x00D9,0x05},
	{0x00DB,0xCE},
	{0x02DC,0x03},
	{0x00DD,0xF8},
	{0x009F,0x00},
	{0x00A3,0x3C},
	{0x00B7,0x00},
	{0x00BB,0x3C},
	{0x00B2,0x09},
	{0x00CA,0x09},
	{0x0198,0x01},
	{0x01B0,0x17},
	{0x01AD,0x00},
	{0x00FF,0x05},
	{0x0100,0x05},
	{0x0199,0x05},
	{0x01A6,0x1B},
	{0x01AC,0x3E},
	{0x01A7,0x1F},
	{0x0030,0x00},

	{0x0011,0x10},
	{0x010A,0x30},
	{0x003F,0x46},
	{0x0031,0xFF},
	{0x0040,0x63},
	{0x002E,0x01},
	{0x001B,0x09},
	{0x003E,0x31},
	{0x0014,0x24},

	{0x0016,0x00},
};

/*
 *  @brief Writing a byte to the memory of a IIC device.
 *  @reg The destination memory address.
 *  @date The data to be written.
 */
static void WriteByte(uint16_t reg, uint8_t data)
{
   uint8_t data_write[3];
   data_write[0]=(reg>>8)&0xff;
   data_write[1]=reg&0xff;
   data_write[2]=data&0xff;
   while(HAL_I2C_Master_Transmit(&hi2c1,VL6180x_ADDR,(uint8_t*)data_write,3,1000) != HAL_OK);
   if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) {
     log_e("i2c tx error!");
   }
}

/*
 * @brief Reading a byte from the memory of a IIC device.
 * @reg The destination memory address to be read.
 * @return The value aimed to be read.
*/
static uint8_t ReadByte(uint16_t reg)
{
	uint8_t receive_data=0;

    if (HAL_I2C_Mem_Read(&hi2c1,VL6180x_ADDR,reg,
    	     I2C_MEMADD_SIZE_16BIT, (uint8_t *)&receive_data, 1, 1000) != HAL_OK) {
    	log_e("i2c rx error!");
    }

	return receive_data;
}


/*
 *  @brief Initialize VL6180x
 */
int VL6180X_Init(void)
{
     uint8_t reset;
     int i;

     HAL_GPIO_WritePin(SHUT_GPIO_Port, SHUT_Pin, GPIO_PIN_RESET);
     delay_ms(10);
     HAL_GPIO_WritePin(SHUT_GPIO_Port, SHUT_Pin, GPIO_PIN_SET);
     delay_ms(1);
	  
     /* check if reset*/
	 reset = ReadByte(SYSTEM_FRESH_OUT_OF_RESET);
	 if(reset==1) {
		 for(i=0; i< ARRAY_SIZE(vl6180x_init_sequence); ++i) {
	         WriteByte(vl6180x_init_sequence[i].addr, vl6180x_init_sequence[i].val);
		 }
	 }

	 return 0;
}
/*
 * @brief Start to ranging, one-shot mode.
 */
void VL6180x_Ranging(void)
{
     WriteByte(SYSRANGE_START,0x01);
}
  
/*
 * @brief Read range result (unit:mm).
 * @return Distance result.
 */
uint8_t VL6180_Read_Range(void)
{
    int range;
	range=ReadByte(RESULT_RANGE_VAL);
	return range;
}
  
/*clear interrupt*/
void VL6180x_Clear_Interrupt(void)
{
     WriteByte(SYSTEM_INTERRUPT_CLEAR,0x07);
}

