/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */

#include <ti/mcu/msp430/Grace.h>
		/// PROGRAM PARAMETERS (may vary for each unit)
#define COMM_SERIAL_NUMBER "C-AA0011"	// Unique serial number for each device.  Update before compiling for a new device
// REMEMBER TO CHECK THE FTP LOG-IN CREDENTIALS IN THE MESSAGES SECTION!!!!
/*
        ***************************************************************************************
        //
        // TURTLE SENSE COMMUNICATOR CONTROLLER v.0.2504
        // PHASE TWO -- Turtle M2M Interface -- Comm Head Master Controller
        // Programmed By Samuel Wantman cc 4.0
		//
        //      VERSION HISTORY OF PHASE TWO
        // 		V0.01 2/21/14 First operational version for use with Phase II board version 0.14
        //			  3/11/14 Comm head master controller code split from smart sensor code
        //			  5/2/14 Revised to work with Phase II board version 0.25
        //			  7/12/14 Timeout added if sensor does not report back after a day and 256 seconds.
        //	  v0.2504 8/6/14  Battery check on start-up to keep batteries from running down if there are hardware problems
		//
        // some routines based on code logic from PHASE ONE --Turtle Dialer Phone Interface
        // Programmed by Thomas Zimmerman  cc 4.0
		****************************************************************************************
        //////////////////////// Main logic for continuous operation ///////////////////////////
        ****************************************************************************************
        // Sensor buffers are read frequently (default is 8 times a second) in the smart sensor slave
        // Sensor reads are accumulated into records (8 K default is every 30 seconds or 6 minutes)
        // When all the records are complete, the smart sensor slave signals to the comm tower that it is ready to send the data.
        // 		TODO // Possible way to double memory since there is space in the comm tower (not yet implemented)
        // 			When there is room in memory in the comm tower CPU, the data is uploaded
        // 			If there is no more room, the record stays in the smart sensor and is just counted by the comm tower
    	// 			first send out the data already uploaded from the sensor. Then upload the rest and send it
        // 		TODO //	If major activity is detected increase frequency of reporting and histograms.
        //		TODO // Support for other Janus phone boards
        //		TODO // Improved timeout and recovery of data from sensor problems.


		Timings
		ODR = Ouput Data Rate (eg. 50hz = 50 samples.  Each sample = 3 readings (x, y, z) each reading is 2 bytes)

		ODR		Max Seconds/Record		8K capacity		10 K capacity	16K capacity	20K capacity
		------	------------------		-----------		-------------	------------	------------
		50 Hz	1280 (~20 minutes)		~85 hrs			~4 days			~7 days			~9 days
		100 Hz	640  (~10 minutes)		~43 hrs			~2 days			~3 days			~4 days
		200 Hz	320	 (~5 minutes)		~21 hrs			~25 hrs			~43 hrs			~2 days
		400 Hz	160	 (~2.6 minutes)		~10 hrs			~13 hrs			~21 hrs			~25 hrs

		Suggested Default rates
		Slow days -- 1 upload/day		50Hz - 6min		50Hz - 5min		50Hz - 3min		50 Hz - 150sec
		Fast days -- 6 uploads/day		50Hz - 60 sec	50 Hz -60 sec	50 Hz - 30sec	50 Hz - 30 sec



        // All unused pins should be set to Output Low to minimize current drain
         *
         * 			NEW SETTINGS version 0.25 (SMT)

           PIN HARDWARE USAGE			w/o phone				w/phone
           MSP430FR5739					-------------------     -------------------
           PIN CONNCECTION	    PORT	FUNC	HIGH	LOW		FUNC	HIGH	LOW		USAGE
        	1		XIN			J.4		---		---		---		---		---		---		Crystal
        	2		XOUT		J.5		---		---		---		---		---		---		Crystal
        	3		AVSS		---		---		---		---		---		---		---		Analog Ground
        	4		AVCC		---		---		---		---		---		---		---		Analog 3.3 V
        	5		Aux0		1.0		Opt		Opt		Opt		Opt		Opt		Opt		Optional (Digital I/O or ADC)
        	6		Aux1		1.1		Opt		Opt		Opt		Opt		Opt		Opt		Optional (Digital I/O or ADC)
        	7		Aux2		1.2		Opt		Opt		Opt		Opt		Opt		Opt		Optional (Digital I/O or ADC)
        	8		BattMon		3.0		OUT-HI	---		---		ADC A12 1.4V	0.9V	Battery Monitor
         	9		SnsrRst		3.1		OUT		Reset	Power	OUT		Reset	Power	Reset Power to smart sensor (powers up after reset)
        	10		TX/RX on	3.2		OUT		TX/RX	OFF		OUT		TX/RX	OFF		Sensor tx/rx enable
			11		Interupt	3.3		IN		COM-RQ	wait	OUT		COM-RQ	wait	Interrupt to request communication from sensor
        	12		Aux3		1.3		Opt		Opt		Opt		Opt		Opt		Opt		Optional (Digital I/O or ADC)
        	13		Aux4		1.4		Opt		Opt		Opt		Opt		Opt		Opt		Optional
        	14		Aux5		1.5		Opt		Opt		Opt		Opt		Opt		Opt		Optional
        	15		TDO			J.0		Opt		Opt		Opt		Opt		Opt		Opt		JTAG connector
        	16		TDI			J.1		Opt		Opt		Opt		Opt		Opt		Opt		JTAG connector
        	17		TMS			J.2   	Opt		Opt		Opt		Opt		Opt		Opt		JTAG connector
        	18		TCK			J.3  	Opt		Opt		Opt		Opt		Opt		Opt		JTAG connector
        	19		SensorTXD	2.5		UCA1TXD	DATA	DATA	UCA1TXD	DATA	DATA	Smart Sensor UART UCA1SIMO
        	20		SensorRXD	2.6 	UCA1RXD	DATA	DATA	UCA1RXD	DATA	DATA	Smart Sensor UART UCA1SOMI
        	21		TEST		TEST  	---		---		---		---		---		---		JTAG connector
        	22		#RST		#RST  	---		---		---		---		---		---		JTAG connector, Reset Button and Switch
        	23		U-OUT		2.0		OUT-HI	---		---		UCA0TXD	DATA	DATA	UART to Phone, UCA0TXD
        	24		U-IN		2.1		IN		---		Drain	UCA0RXD	DATA	DATA	UART from Phone, UCA0RXD
        	25		PhnMon		2.2		IN		???		OFF		IN		ON		OFF		Phone status indicator from phone board (can be monitored by LED)
        	26		TAMPER		3.4     IN		OK		Trouble	IN		OK		Trouble	Sensor cable trouble line (There is a problem if it goes low)
        	27		PhnOn/Off	3.5		OUT-HI	---		---		OUT		Hi-Z	LOW		Phone on/off request (hold low for 3 sec)
        	28		PhnPower	3.6		OUT-LO	ON		OFF		OUT-HI	ON		OFF		Turn on 5V Regulator to power phone card
        	29		LED			3.7		OUT		LED ON	LED	OFF	OUT		LED ON	LED	OFF	Phone power indicator LED (see individual board specs for meaning of blinks)
        	30		Uext6		1.6		Opt		Opt		Opt		Opt		Opt		Opt		Uext connector. UCB0SIMO
        	31		Uext5		1.7		Opt		Opt		Opt		Opt		Opt		Opt		Uext connector. UCB0SOMI
        	32		VCORE		VCORE	---		---		---		---		---		---		Internal power
        	33		DVSS		DVSS	---		---		---		---		---		---		Digital Ground
        	34		DVCC		DVCC	---		---		---		---		---		---		Digital Power
        	35		LED1		2.7		OUT		LED ON	LED OFF	OUT		LED ON	LED OFF	LED1
        	36		PhnReset	2.3		OUT-HI	---		---		OUT		Hi-Z	LOW		Phone Reset (hold low to reset)
        	37		PwrMon		2.4		IN		ON		OFF		IN		ON		OFF		Phone power indicator (Also UEXT pin 9)
        	38		AVSS		AVSS	---		---		---		---		---		---		Analog ground
         *
         *
         *
*/

	// Interrupt code resides in InterruptVectors_init.c

/////////// SMART SENSOR INSTRUCTION SET///////////////////////////////////
	#define SENSOR_OFF 0x00			// Hardware power cycle -- puts ADXL in low power standby with default settings
	#define SENSOR_ON 0x01			// turn on the ADXL sensor (does not change any settings)
	#define SENSOR_RESET			// does a hardware and software reset of the ADXL and puts it in standby and loads settings
	#define MSP_SLEEP 0x03			// put the smart sensor in a low power sleep mode
	#define CALIBRATE 0x04			// calibrate the ADXL chip
	#define REGISTER_SS 0x05		// send new GPS and time settings to register sensor
	#define NEW_PARAMETERS 0x06		// send new parameters for the ADXL chip to the smart sensor
	#define CLEAR_RECORD 0x07		// erase the last record on the smart sensor
	#define CLEAR_ALL 0x08			// clear the entire smart sensor memory
	#define UPLOAD_LAST 0x09		// upload the last record from the smart sensor
	#define UPLOAD_ALL 0x0A			// upload all records from the smart sensor
	#define MANUAL_OPERATION 0x0B	// Read or write data directly to and from the ADXL sensor without processing
	#define STREAM_DATA 0x0C		// Streams of data from the smart sensor
	#define CHECK_ID 0x0D			// upload the GPS and time settings stored in the sensor
	#define SEND_PARAMETERS 0x0E	// upload the old parameters from the smart sensor
	#define PROGRESS_REPORT 0x0F	// request the status of the sensor after the last command
	#define RESUME_RUN_BINS 0x10	// continues a run after an interruption
	#define START_RUN_BINS 0x11		// start up the smart sensor and collect data in bins

/////////////////////DATA RECORDING PARAMETERS//////////////////////////////////
	// SAMPLE_SPEED sets the OUTPUT DATA RATES (ODR) for the ADXL sensor (below)
	// The sample speed is the rate of sensor reads (12.5, 25, 50, 100, 200 or 400 Hz)
	// 0 = 12.5 Hz, 1 = 25 Hz, 2 = 50 Hz, 3 = 100 Hz,  4 = 200 Hz and 5 = 400 Hz
	// Bits 0, 1 and 2  of FILTER_CTL select the Output Data Rate.
	// which also configures internal filters to a bandwidth using the QUARTER_BW or the HALF_BW bit setting.
    #define ODR_12			0x00	// 12.5 Hz ODR
    #define ODR_25	 		BIT0	// 25 Hz ODR
    #define ODR_50 			BIT1	// 50 Hz ODR (reset default)
    #define ODR_100    (BIT1|BIT0)	// 100 Hz ODR
    #define ODR_200	   		BIT2	// 200 Hz ODR
    #define ODR_400    (BIT2|BIT0)	// 400 Hz ODR


#define SAMPLE_SPEED	ODR_100	// Output Data Rate (see above)
#define READ_SPEED		1		// the number of sets of buffer reads per second.
#define SLEEP_INTERVALS 8		// Must be a power of 2.  REED_SPEED X SLEEP_INTERVALS should equal 8
	// The timer is set to have an interrupt every 1/8 of a second, so SLEEP_INTERVALS
	// is number of interrupts to wait before emptying the buffer.
	// Each set of sensor readings is 6 bytes.  2 bytes for each sensor read of X, Y and Z acceleration
	// Temperature is only read once for each record.
	// The ADXL FIFO buffer is set to hold a maximum of 440 readings (a maximum of 100 sensor reading sets of 4)
	// If the speed is 100 Hz or less the buffer can be emptied once per second (SLEEP_INTERVALS = 8)
	// For 200 reads, this should be set to 2 buffer downloads (SLEEP_INTERVALS = 4)
	// For 400 reads, this should be set to 4 buffer downloads. (SLEEP_INTERVALS = 2)
	// the number of samples per second times SLOWHOURSBINSEC can not be more than 64K or bins might overflow


////////////// PROGRAM TIMINGS///////////////////////////
	// Default timings  (You can change these)
#define HOURS 4 		// Default is 4  // Number of hours for data collection per call-in period once motion detected.  This must be a factor of 120.
#define SLOWHOURS 24		// Default is 24 // Number of hours in a slow day of data collection
#define SLOW_DAYS 40	// Number of days that should elapse until there is more frequent reporting

	// These are computed (don't change them)
#define BINSEC (15 * HOURS)		// The default is 60 // Number of seconds in each bin record.  Must be factor of 3600
#define SLOWHOURS_BIN_SEC (15 * SLOWHOURS)	// The default is 360 but it could be less if records are also stored in comm board or phone board.
								// Number of seconds in each bin record on a slow day (every 6 minutes)

	// These can be changed, but the program may need to be adjusted (change with caution)
#define MAX_BIN 10       		// number of histogram bins kept in each record (changing this might cause problems -- check thoroughly)
#define DATA_SIZE 16			// This is MAX_BIN plus other parameters (currently 6: Temp, X, Y, Z, Count, Max)
#define DATA_BYTES 32			// The number of bytes in one record (DATA_SIZE * 2)
#define MAX_RECORDS 240			// The default is 240 // The maximum number of Bin records assuming about 8 K and 32 bytes per record
#define CLOCKSPEED 8			// The number of  million cycles per second for the normal clockspeed

	// System parameters
#define REPORT_HEADINGS 1		// flags if headings are printed in the reports (1=YES, 0 = NO)
#define CABLE_LENGTH 50			// the maximum cable length in feet (we roughly need 1 microsecond to discharge each foot of cat5e cable )
								// if you want to be more precise the actual value is .75 microsecond per foot
#define CABLE_DISCHARGE_TIME (50 + CABLE_LENGTH) // The capacitor in the circuit takes 50 microseconds to discharge
#define SHUT_OFF_LEVEL 549		// Battery capacity left at shut-off (2%)
								// 12V NiMH (8 AA * 1.2V) = 549 which is about 8 volts
								// 9V  NiMH = (7 * 1.2V) = 480 which is about 7 volts

	//  Computed Default timings (Don't change these, change the ones above)
#define SECS_IN_DAY 86400		// Number of seconds in a day
#define TIME_OUT  256		 	// resets if no readings after this many seconds after expected
#define MAXRUN (SECS_IN_DAY-60)	// The maximum length of a run before an interrupt (a day minus a minute)
#define MAXRUN4H ((MAXRUN & 0xFF000000) / 0x01000000) // The highest byte of MAXRUN
#define MAXRUN3  ((MAXRUN & 0x00FF0000) / 0x00010000)
#define MAXRUN2  ((MAXRUN & 0x0000FF00) / 0x00000100)
#define MAXRUN1L  (MAXRUN & 0x000000FF)			// The lowest byte of MAXRUN
#define BINSEC_HI ((BINSEC & 0xFF00) / 0x0100)	// The high byte of BIN_SEC
#define BINSEC_LO (BINSEC & 0x00FF) 			// The low byte of BIN_SEC
#define SLOWBIN_HI ((SLOWHOURS_BIN_SEC & 0xFF00) / 0x0100)	// The high byte of SLOWHOURS_BIN_SEC
#define SLOWBIN_LO (SLOWHOURS_BIN_SEC & 0x00FF)	// The low byte of SLOWHOURS_BIN_SEC
#define MAX_RECORDS_HI ((MAX_RECORDS & 0xFF00) / 0x0100) 	// The high byte of MAX_RECORDS
#define MAX_RECORDS_LO  (MAX_RECORDS & 0x00FF) 	// The low byte of MAX_RECORDS


    #define CR 13					// Carriage return
	#define LF	10					// Line feed
	#define COMMA 44				// comma
    #define SPACE 32				// space character
    #define DATA_BUFFER_SIZE 127	// The size of the data buffer for receiving data from UART or phone


    //	TSB = Turtle Sense Board (prefix)


    // blinking speeds
    #define FAST 125		// length of, and msecs between fast blinks
    #define SLOW 500		// length of, and msecs between slow blinks
	#define PAUSE 1000
//    #define BLINK YES		// For testing yes -- for power measurements No


    #define TRUE 1
    #define FALSE 0
    #define YES 1
    #define NO 0
    #define HIGH 1
    #define LOW 0
	#define RECEIVE 0
	#define SEND 1


    // Port 1
    #define TSB_AUX0 BIT0
    #define TSB_AUX1 BIT1
    #define TSB_AUX2 BIT2
    #define TSB_AUX3 BIT3
    #define TSB_AUX4 BIT4
    #define TSB_AUX5 BIT5	// Green button on Hand Held Unit
    #define TSB_UEXT6 BIT6
    #define TSB_UEXT5 BIT7

	#define HH_RED    TSB_AUX0	// Red LED on Hand Held Unit
    #define HH_YELLOW TSB_AUX1	// Yellow LED on Hand Held Unit
    #define HH_GREEN  TSB_AUX2	// Green LED on Hand Held Unit
	#define HH_BUTTON TSB_AUX5 	// Green button on Hand Held Unit

    // Port 2
    #define TSB_UCA0TXD BIT0
    #define TSB_UCA0RXD BIT1
    #define TSB_PHNMON BIT2
    #define TSB_PHNRESET BIT3
    #define TSB_PWRMON BIT4
    #define TSB_SENSOR_TXD BIT5
    #define TSB_SENSOR_RXD BIT6
 	#define TSB_LED BIT7			//  LEDs are connected to pin 2.7 and 3.7

    // Port 3
    #define TSB_BATTMON BIT0
    #define TSB_SNSRRST BIT1
    #define TSB_TX_RX_ON BIT2		// Enable communication with smart sensor
    #define TSB_INTERRUPT BIT3 		// Interrupt line from smart sensor
    #define TSB_TAMPER BIT4			// Sensor cable tamper line (should always be high)
    #define TSB_PHNONOFF BIT5
    #define TSB_PHNPOWER BIT6
//    #define TSB_LED2 BIT7			// Also LED2



    // ADXL362 Instruction set for MANUAL OPERATION (command 0x0B0)

// The first byte sent after the command should be the instruction code for the ADXL (un-comment if using)
//#define ADXL_WRITE 0x0A			// Instruction to write to a register
//#define ADXL_READ  0x0B			// Instruction to read from a register
// DO NOT SEND THE following instruction, it will return an error.  To stream directly use command (0x0C)
//#define ADXL_FIFO  0x0D			// Instruction to read the FIFO stack
//
//	//	//	//	//	ADXL362 Registers	//	//	//	//	//	//	//
/*

    Table 11. Register Summary

    Reg		Name			Bits	 Bit 7	 Bit 6	  Bit 5		 Bit 4	   Bit 3		   Bit 2		    Bit 1	   Bit 0	Reset	RW
    0x00	DEVID_AD		[7:0]								DEVID_AD[7:0]													0xAD	R
	0x01	DEVID_MST		[7:0]								DEVID_MST[7:0]													0x1D	R
	0x02	PARTID			[7:0]								PARTID[7:0]														0xF2	R
	0x03	REVID			[7:0]								REVID[7:0]														0x01	R
	0x08	XDATA			[7:0]								XDATA[7:0]														0x00	R
	0x09	YDATA			[7:0]								YDATA[7:0]														0x00	R
	0x0A	ZDATA			[7:0]								ZDATA[7:0]														0x00	R
	0x0B	STATUS			[7:0]  ERR_USER_ REGS|AWAKE  |INACT|ACT		|FIFO_OVER-RUN	|FIFO_WATER-MARK|FIFO_READY	|DATA_READY	0x40	R
	0x0C	FIFO_ENTRIES_L	[7:0]												FIFO_ENTRIES_L[7:0]								0x00	R
	0x0D	FIFO_ENTRIES_H	[7:0]									UNUSED								   |FIFO_ENTRIES_H[1:0]	0x00	R
	0x0E	XDATA_L			[7:0]								XDATA_L[7:0]													0x00	R
	0x0F	XDATA_H			[7:0]							SX			|						XDATA_H[3:0]					0x00	R
	0x10	YDATA_L			[7:0]								YDATA_L[7:0]													0x00	R
	0x11	YDATA_H			[7:0]							SX			|						YDATA_H[3:0]					0x00	R
	0x12	ZDATA_L			[7:0]								ZDATA_L[7:0]													0x00	R
	0x13	ZDATA_H			[7:0]							SX			|						ZDATA_H[3:0]					0x00	R
	0x14	TEMP_L			[7:0]								TEMP_L[7:0]														0x00	R
	0x15	TEMP_H			[7:0]							SX			|						TEMP_H[3:0]						0x00	R
	0x20	THRESH_ACT_L	[7:0]								THRESH_ACT_L[7:0]												0x00	RW
    0x21	THRESH_ACT_H	[7:0]				UNUSED									| 			 THRESH_ACT_H[2:0]			0x00	RW
    0x22	TIME_ACT		[7:0]								TIME_ACT[7:0]													0x00	RW
    0x23	THRESH_INACT_L	[7:0]								THRESH_INACT_L[7:0]												0x00	RW
    0x24	THRESH_INACT_H	[7:0]				UNUSED									|  			THRESH_INACT_H[2:0]			0x00	RW
    0x25	TIME_INACT_L	[7:0]								TIME_INACT_L[7:0]												0x00	RW
    0x26	TIME_INACT_H	[7:0]								TIME_INACT_H[7:0]												0x00	RW
    0x27	ACT_INACT_CTL	[7:0]	RES					  | LINKLOOP 	|  INACT_REF 	|INACT_EN 		|ACT_REF 	|ACT_EN		0x00	RW
    0x28	FIFO_CONTROL	[7:0]				UNUSED					|	AH			|FIFO_TEMP		|		FIFO_MODE		0x00	RW
    0x29	FIFO_SAMPLES	[7:0]								FIFO_SAMPLES[7:0]												0x80	RW
    0x2A	INTMAP1			[7:0]	INT_LOW 	  |AWAKE  |INACT|ACT	|FIFO_OVER-RUN 	|FIFO_WATER-MARK|FIFO_READY	|DATA_READY	0x00	RW
    0x2B	INTMAP2			[7:0]	INT_LOW 	  |AWAKE  |INACT|ACT	|FIFO_OVER-RUN	|FIFO_WATER-MARK|FIFO_READY	|DATA_READY	0x00	RW
    0x2C	FILTER_CTL		[7:0]	     RANGE			  |RES  |HALF_BW|	EXT_SAMPLE	|		 		  ODR					0x13	RW
    0x2D	POWER_CTL		[7:0]	RES			  |EXT_CLK|  LOW_NOISE	|		WAKEUP	|	AUTOSLEEP	|	MEASURE				0x00	RW
*/


//---- ADXL Defines  // Refer to ADXL362 documentation for complete descriptions:  http://dlnmh9ip6v2uc.cloudfront.net/datasheets/BreakoutBoards/ADXL362.pdf

// ADXL bit definitions

   //	FIFO_CONTROL BITS
#define FIFO_AH				BIT3 	// This bit is the MSB of the FIFO_SAMPLES register, allowing FIFO samples a range of 0 to 511 bytes.
#define FIFO_TEMP_ON 		BIT2	// Store Temperature Data to FIFO. 1 = temperature data is stored in the FIFO together with x-, y-, and z-axis acceleration data.
#define FIFO_OFF 			0x00	// BIT0 and BIT1 combinations to Enable FIFO and Mode Selection.
#define FIFO_OLDEST_SAVED 	BIT0	// Only the oldest samples are saved.  Once the FIFO fills, nothing else is stored until the data is read, then it empties
#define FIFO_STREAM			BIT1	// The FIFO is filled and then overwritten with new readings
#define FIFO_TRIGGERED		(BIT0|BIT1)	// b01 = Oldest saved mode.  b10 = Stream mode. b11=Triggered mode.

    //	FILTER CONTROL BITS (select one range, one BW, one ODR)
#define RANGE_2G		0x00	// Default sensitivity setting of +/- 2g
#define RANGE_4G		BIT6	// +/- 4G
#define RANGE_8G		BIT7	// +/- 8G   (use only one range)
#define HALF_BW			0x00	// Default setting of half bandwidth antialiasing filtering
#define QUARTER_BW		BIT4	// More conservative antialiasing of quarter bandwidth
#define EXT_SAMPLE 		BIT3	// External Sampling Trigger. 1 = the INT2 pin is used for external conversion timing control.



    //	POWER CONTROL BITS
#define EXT_CLK			BIT6 	//External Clock. 1 = the accelerometer runs off the external clock provided on the INT1 pin.
								// EXTERNAL CLOCK IS NOT SUPPORTED IN THE CURRENT VERSION OF THE SMART SENSOR
#define INT_CLK 		0x00 	//External Clock. 0 = the accelerometer runs off internal clock (default)
#define NORMAL_NOISE  	0x00	// Bits 4 and 5 select LOW_NOISE Selects Power vs. Noise Tradeoff: 00=Normal (default and lower power consumption)
#define LOW_NOISE 	 	BIT4	// 01 = Low noise mode.
#define ULTRA_LOW_NOISE BIT5	// 10 = Ultralow noise mode.
#define WAKEUP_MODE		BIT3	// Wake-Up Mode.  1 = the part operates in wake-up mode.
#define AUTOSLEEP 		BIT2	// Autosleep. Activity and inactivity detection must be in linked mode or
    							// loop mode (LINK/LOOP bits in ACT_INACT_CTL register) to enable
    							// autosleep; otherwise, the bit is ignored. 1 = autosleep is enabled, and
    							//the device enters wake-up mode automatically upon detection of inactivity.
#define ADXL_STANDBY	0x00	// Bit 0 and Bit1 Selects Measurement Mode or Standby. Start up the chip in Standby, and then switch to Measurement
#define ADXL_ON			BIT1 	// Start recording data in measurement mode


#define	THRESH_ACT_L	0x00 // low byte of the activity threshold (default is 0x00) The sensor only records data when over the threshold
#define	THRESH_ACT_H	0x00 // high byte (lowest three bits) of the activity threshold (default is 0x00)
#define	TIME_ACT		0x00 // When this timer is used, only sustained motion can trigger activity detection (default is 0x00)
#define	THRESH_INACT_L	0x00 // low byte of inactivity threshold (default is 0x00) The sensor stops recording data when under the threshold
#define	THRESH_INACT_H	0x00 // high byte (lowest three bits) of the inactivity threshold (default is 0x00)
#define TIME_INACT_L	0x00 // low byte of inactivity timer (default is 0x00) The sensor goes to sleep after reading this many samples of inactivity
#define	TIME_INACT_H	0x00 // high byte (lowest three bits) of the inactivity timer (default is 0x00)
#define	ACT_INACT_CTL	0x00 // activity/inactivity control byte.  (default is 0x00 -- not activated)
#define	FIFO_CONTROL	(FIFO_AH + FIFO_OLDEST_SAVED) // more than 256 bytes, and oldest save mode.  No temperature readings in the FIFO (once a record is enough)
#define	FIFO_SAMPLES	44 	 //  300 FIFO samples (3 X 100) // Along with AH, this is the number of two byte samples to save in the buffer (subtract 256 if AH is set).
    						 // Should be multiple of 3 unless FIFO_TEMP_ON is added to FIFO_CONTRO, then it should be a multiple of 4.  511 maximum.
							 // Optimizing this so it is not much bigger than needed will make the sensor run on a little less power.
#define	INTMAP1	 		0x00 // no interrupts for now.  This interrupt is not connected to the MSP430
#define	INTMAP2			0x00 // This interrupt is not currently used.  It can send interrupts to the MSP430, or used to sync a clock from the MSP430
#define	FILTER_CTL 		(RANGE_2G | HALF_BW | SAMPLE_SPEED)	  		// 2g sensitivity, normal anti-aliasing and 100 Hz sample rate
#define	POWER_CTL_OFF	(INT_CLK | ULTRA_LOW_NOISE | ADXL_STANDBY) 	// Startup in standby, and then change the ADXL_ON bit when ready
#define	POWER_CTL_ON	(INT_CLK | ULTRA_LOW_NOISE | ADXL_ON)	 	// The byte to send when ready to read data

#define UART_TX_BUSY (!(UCA1IFG & UCTXIFG))      // Bit location to monitor while transmission is in process
#define UART_BUSY (UCA1STATW & UCBUSY)	 	   	 // Alternate bit location to monitor all UART activity
#define RX_BUFF_EMPTY (!(UCA1IFG & UCRXIFG))	 // Receive byte flag location
#define PHN_TX_BUSY	(!(UCA0IFG & UCTXIFG))    	 // Bit location to monitor while transmission is in process
#define PHN_RX_BUFF_EMPTY (!(UCA0IFG & UCRXIFG)) // Receive byte flag location

    		// SRAM variables

    // Non persistant variables -- these will be reset after a powerup
    volatile unsigned char new_settings = NO;		// flag for uploading new settings to smart sensor
    volatile unsigned char Python_available = NO;	// flag for knowing if phone boardd has Python available for data storage
    volatile unsigned char sensor_ready = NO;		// flag for interrupt by smart sensor when a record is ready to read
    volatile unsigned char wd_reset;				// flag for watchdog reset or power up reset
    volatile unsigned char tamper_count = 10;		// the count down until it is time to check for a tamper condition
    volatile unsigned char starting_up = YES;		// "Yes" after a new sensor plug in or power up, "No" otherwise
    volatile unsigned char force_shut_down = NO;	// Set to "yes" after a bad battery reading
    volatile unsigned int adc_read;					// The data read in the adc interrupt routine
    volatile unsigned int script_counter = 0;
    volatile unsigned char sensor_plugged_in = NO;	// The pull-downs are set assuming that the sensor starts out in an unplugged state
    volatile unsigned char sensor_error = NO;		// The reply from the smart sensor after a command or interrupt request
    volatile unsigned char eightCount=0;			// counts clock interrupts every eighth of a second, wakes up every 8 counts


    // FRAM data storage

    #pragma SET_DATA_SECTION(".fram_vars")

    char data_received[DATA_BUFFER_SIZE]; 	//phone data buffer (incoming data)
    unsigned char records[MAX_RECORDS][32];			// record data storage

    //	[MAX_RECORDS][0-1]   -- Temperature reading (the average of 16 readings)
    //	[MAX_RECORDS][2-3]   -- X Position
    //	[MAX_RECORDS][4-5]   -- Y Position
    //	[MAX_RECORDS][6-7]   -- Z Position
    //	[MAX_RECORDS][8-9]   -- Total number of accumulated readings for each record
    // 	[MAX_RECORDS][10-11] -- Highest Bin: the highest bin corresponds to bincount
    //							(plus one because zero readings are possible) of the highest bit
    //							triggered in any reading.  If there are high readings (>0x0A)
    //							the lowest bins are dropped.  EG:  0x0A = the highest is bin 0x0A,
    //							the lowest bin correspondsto the lowest readings;  0x10 = the highest is bin 0x10
    //							the lowest 6 bins are dropped.
    //  [MAX_RECORDS][22-31] --	The ten highest bin readings.  Bytes 30 and 31 are the highest bin.

    unsigned int battery_level;						// 0 = battery dead, 100 = fully charged
//    unsigned int recUploads = 0;					// The number of records that have been uploaded
//    unsigned long int recReadings = 0;			// The number of readings that have been processed in the current record
    unsigned int recCount = 0;						// The count of how many records have been collected since last upload
//    unsigned int recCount_here = 0;				// The number of records stored on this device
//    unsigned int sensor_recCount = 0;				// The number of records stored on the smart sensor
//    unsigned int recCount_phone = 0;				// The number of records stored on the phone module
//    unsigned int room_here = MAX_RECORDS;			// The room remaining on this device for records
    unsigned long int secCount = SECS_IN_DAY + TIME_OUT; // time out counter -- reset after each data upload
    unsigned long int maxTime = SECS_IN_DAY;		 // default time out is a day and 256 seconds
    unsigned int day_count = 0;						// Number of days the nest has been active
    unsigned int last_day_count = 0; 				// Number of days the nest was active when last reported
//    unsigned int recSecs = 0;						// The number of seconds that the current record has been active
//    unsigned const char active = NO;				// "Yes" if the turtles are starting to move,  "No" if they are not
    unsigned char error_code = 0x00;				// for future error handling

	// SENSOR VALUES

	// TEMPERATURE CALIBRATION SETTINGS
	// During Calibration, the reading at 0 degrees C becomes the temperature offset, and the ratio of 0x0200
	// divided by the reading at room temperature (minus the offset) becomes the temperature ratio.
#define CALIBRATE_TEMP YES					// The default is to calibrate the sensors


	// NEST IDENTIFICATION and SENSOR ID
	// This is the data that results from a GPS read from the phone boards
	// The first 32 bytes returned by the AT$GPSACP command is stored just as received
	// This has the UTC time, Latitude and longitude of the reading
	// This is followed by the date of activation and serial number
	// See the top of the program to set the serial number.
#define NEST_ID "000000.000,0000.0000N,00000.0000W,000000,000000"
char nest_ID[] = NEST_ID;			// The unique ID of each nest based on time, date, location and sensor ID
	// NEST_ID is defined as "122330.000,4542.8106N,01344.2720E,240613,AA001"
	// it breaks down as follows:
#define NEST_ID_BYTES 47  	// the number of bytes in the nest_ID above
	// Starts with the UTC time, and GPS location copied directly from the GPS output.
	// The UTC is in the format: hhmmss.sss
#define UTC 0				// nest_ID[UTC] - The offset for the GPS UTC time code
#define UTC_BYTES 6			// The last byte is nest_ID[9], but the last 3 digits always seem to be zeros
	// LATITUDE is in the format: ddmm.mmmm N/S  where:
	//	dd - degrees 00..90
	//	mm.mmmm - minutes 00.0000..59.9999
	//  N/S: North / South
#define LATITUDE 11			// nest_ID[LATITUDE] - The offset for the GPS latitude
#define LATITUDE_BYTES 10 	// The last byte is nest_ID[20]
	// LOGITUDE is similar: dddmm.mmmm E/W   with the differences:
	 	// ddd - degrees 000..180
	// E/W: East / West
#define LONGITUDE  22		// nest_ID[LONGITUDE] - The offset for the GPS longitude
#define LONGITUDE_BYTES 11 	// The last byte is nest_ID[32]
	// The DATE is the date the sensor was activated in the format: ddmmyy  where:
	// dd - day 01..31
	// mm - month 01..12
	// yy - year 00..99 - 2000 to 2099
#define DATE 34				// nest_ID[DATE] - The date the sensor was activated
#define DATE_BYTES 6		// The last byte is nest_ID[39]
#define DAY 34
#define DAY_BYTES 2
#define MONTH 36
#define MONTH_BYTES 2
#define YEAR 38
#define YEAR_BYTES 2
	// The SERIAL NUMBER is a unique id for each Smart Sensor produced
	// The first letter is the hardware version
	// The second letter is the software version
	// The next 4 digits is a unique production number for each device
#define SERIAL 41		// nest_ID[SERIAL_START] - The offset for the sensor serial number nest_ID[SERIAL_START]
#define SERIAL_BYTES 6		// The last byte is nest_ID[46]

	// Smart Sensor Interrupt codes
	// These codes are returned after a command 0x0F is received
	// thet explain the reason for generating an interrupt
	// All codes less than 0x80 are not errors.
#define NO_ERROR  0x00 				// = No interrupt was generated -- operation in progress
#define NO_ERROR_INT1 0x01 			// = INT1 generated an interrupt
#define NO_ERROR_INT2 0x02			// = INT2 generated an interrupt
#define NO_ERROR_DATA_READY 0x04 	// = operation completed successfully, data ready to report
#define NO_ERROR_COMPLETE 0x08		// = operation completed successfully, no data to report
		// >= 0x80 = an error condition exists.  The error code is returned:
#define ERROR_SENSOR_TIMEOUT  0x81	// = Sensor Start up failure (timed out)
#define ERROR_NO_COMMAND 0x82		// = Unknown command received
#define ERROR_BAD_DATA 0x84			// = Bad Data received
#define ERROR_TX_TIMEOUT 0x88		// = Data transmission timed out
#define ERROR_OUT_OF_RANGE 0x90		//F = Parameters out of range


	// SMART SENSOR PARAMETERS
	// These 40 bytes can be downloaded from the comm tower to change the behavior of the sensor.
 char parameters[40] = {
		THRESH_ACT_L, THRESH_ACT_H, TIME_ACT, THRESH_INACT_L,
		THRESH_INACT_H, TIME_INACT_L, TIME_INACT_H,
		ACT_INACT_CTL, FIFO_CONTROL, FIFO_SAMPLES,
		INTMAP1, INTMAP2, FILTER_CTL, POWER_CTL_OFF, // parameters[0] to [13] - the first 14 are all the ADXL reset parameters.
		SLEEP_INTERVALS,				// parameters[14] - The number of clock interrupts between each FIFO buffer read.
		READ_SPEED,						// parameters[15] - The number of interrupts per second (default is 8 which equals 1/8 second)
		SLOWBIN_LO, SLOWBIN_HI,			// parameters[16] and [17] - The current number of seconds to accumulate readings in each record (set of bins)
		MAXRUN1L, MAXRUN2, MAXRUN3, MAXRUN4H,	// parameters[18] to [21] - The maximum time the sensor can run without reporting back to comm head.
		MAX_RECORDS_LO, MAX_RECORDS_HI,	// parameters[22] and [23] - The maximum number of records to collect
		CALIBRATE_TEMP,					// parameters[24] - Is the temperature calibration active (YES=calibrate)
		REPORT_HEADINGS,				// parameters[25] - (comm board) Flag if we are sending headings in report (YES=headings)
		BINSEC_LO, BINSEC_HI,			// parameters[26] and [27] timing settings for active days
		SLOW_DAYS,						// parameters[28] -	Number of days of low activity
		SLOWBIN_LO, SLOWBIN_HI,			// parameters[29] and [30] - The number of seconds to accumulate readings in each record (set of bins)
		0,0,0,0,0,0,0,0,0 };			// parameters[31] to [39] reserved for future use
#define PARAM_BYTES 40 // the number of bytes in the parameters structure above
 	 // new parameters can be ftp'd into the unit from the web
 char new_parameters[40]= {
		THRESH_ACT_L, THRESH_ACT_H, TIME_ACT, THRESH_INACT_L,
		THRESH_INACT_H, TIME_INACT_L, TIME_INACT_H,
		ACT_INACT_CTL, FIFO_CONTROL, FIFO_SAMPLES,
		INTMAP1, INTMAP2, FILTER_CTL, POWER_CTL_OFF,
		SLEEP_INTERVALS,
		READ_SPEED,
		SLOWBIN_LO, SLOWBIN_HI,
		MAXRUN1L, MAXRUN2, MAXRUN3, MAXRUN4H,
		MAX_RECORDS_LO, MAX_RECORDS_HI,
		CALIBRATE_TEMP,
		REPORT_HEADINGS,
		BINSEC_LO, BINSEC_HI,
		SLOW_DAYS,
		SLOWBIN_LO, SLOWBIN_HI,
		0,0,0,0,0,0,0,0,0 };

unsigned char new_parameters_loaded = NO;		// Yes when loaded, reset after first report.
#pragma SET_DATA_SECTION()					// end of FRAM data section



    // AT codes and strings for phone  -- the back slash is for adding quotes \r = CR


     char *messages[] = {
    		"\n",				// messages[0]  not used (marks end of script)
    		"ATE0 V0\r",		// messages[1] turn off echo, return error codes only 0=OK, 4=error
    		"AT+CGDCONT=1,\"IP\",\"ISP-Provider.net\"\r",	// messages[2] Open internet connection UPDATE WITH YOUR CREDENTIALS
    		"AT#SGACT=1,1\r",	// messages[3] Activate context
    		"AT#FTPOPEN=\"yourwebsite.org\",\"ftp-username\",\"ftp-pasword\"\r",	// messages[4] FTP login UPDATE WITH YOUR CREDENTIALS
    		"AT#FTPTYPE=0\r",	// messages[5] File type = Binary
    		"AT#FTPTYPE=1\r",	// messages[6] File type = ASCII
    		" ",	// messages[7] Read GPS position
    		"AT#FTPAPP=\"",		// messages[8] Append to existing file and upload (file name must be added)
    		"+++",				// messages[9] Upload escape sequence
    		"AT#FTPCLOSE\r",	// messages[10] Close FTP session
    		"AT#FTPGET",		// messages[11] Read parameter  file
    		"AT#FTPDELE",		// messages[12] Delete parameter file
    		" ",				// messages[13]
    		" ",				// messages[14]
    		"AT+CREG?\r",		// messages[15] Registration report (will return "+CREG: 0,1" when successful.)
    							// 		CREG codes
    		    				// 		0 - not registered, ME is not currently searching a new operator to
    		    				// 	   		register to (bad reception?  antenna disconnected?
    		    				// 		1 - registered, home network
    		     	 	 	 	// 		2 - not registered, but ME is currently searching a new operator to
    		    				//    		 register to
    		     	 	 	 	// 		3 - registration denied
    		     	 	 	 	// 		4 - unknown
    		    				//		5 - registered, roaming
        	"AT#SERVINFO\r",	// messages[16]  get cellular service info
        	"AT#NITZ=1,0\r",	// messages[17]  set RTC from network time automatically
        	"AT#CCLK?\r",		// messages[18]  get time from RTC
        	"AT$GPSP=0\r ",		// messages[19] GPS off
        	" ",				//AT$GPSP=1\r",	// messages[20] power-up GPS
        	" ",				// messages[21]
        	" ",				// messages[22]
        	" ",				// messages[23]
        	" ",				// messages[24]
        	" ",				// messages[25] LAST AT command
    		"\"\r",				// messages[26] end quotes and CR (end of file name -- special case for error checking)
    		"\r",				// messages[27] CR
    		"Report: ",			// messages[28]
    		"Sensor ID#: ",				// messages[29]
    		"000000",					// messages[30] The serial number of the smart sensor
    		"Installed: ",				// messages[31]
        	"2014-05-01",				// messages[32]  The day the sensor was installed
    		"Days active & report #: ",	// messages[33]
        	"000-01",					// messages[34]  How many days have elapsed since installation day
        								//   		    and The number of reports sent today
    		" ",						//"000-00 ",	// messages[35] was previous report sent
        	"Comm ID#: ",				// messages[36]
        	COMM_SERIAL_NUMBER,			// messages[37]
    		"Nest location: ",		// messages[38]
        	"0000.0000N",  				// messages[39]  Nest GPS latitude
        	"00000.0000W",				// messages[40]  Nest GPS longitude
    		"Start date/time: ",		// messages[41]
        	"2000-00-00,00:00:00",		// messages[42]  Time of previous report from GPS
    		"Report date/time: ",		// messages[43]
        	"2001-01-01,01:01:00",		// messages[44]  Time of current report from GPS
    		"Secs per rec: ",			// messages[45]
    		"\r# of recs: ",			// messages[46]
    		"\"\r",						// messages[47] like #26, but without waiting for a response
    		"\rBattery level: ",		// messages[48]
    		"Rec#,Temp,   X,   Y,   Z, Cnt, Max,Bins: (Low) to (High)\r",	// messages[49]
    		",",						// messages[50] just a comma
    		// File type codes:
    		//	 		Used in file names to distinguish the different types of files generated
    	    // 			TODO // finish creating header files, log files and alert files.
    		"_r-",					// messages[51]  "r-" -- a standard report
    		"=\"parameters",		// messages[52] parameter file name beginning
    		".ts",					// messages[53] parameter file name ending
    		"TurtleSense 0.2504 -- CC 4.0 BY-SA NerdsWithoutBorders.Net\r",	// messages[54] COPYLEFT NOTICE
    		"--end of report--\r",	// messages[55]
    		",  ",					// messages[56] comma and a space
    		"_",					// messages[57] underscore
    		".txt",					// messages[58] text file extension
    		"NEW SETTINGS LOADED. STARTS AFTER THIS REPORT\r",		// messages[59]
    		"LOW BATTERY -- SHUTTING DOWN\r",				// messages[60]
    		"RESET OCCURRED\r",							// messages[61]
    		"POWERING UP - MONITORING NEST\r",	// messages[62]
    		"POWERING DOWN - SENSOR UNPLUGGED\r"	// messages[63]
    		};
    								// If the command is complete it ends with a CR (\r)


    /// SCRIPTS
    static const char report_script_noheading[] = {
    		 2,  3,  4,  5,  8,   			// Open ftp and append information into a file
    		32, 57, 30, 51, 34, 58, 26, 	// File name
    		32, 57, 30, 51, 34, 58, 27,		// Start Report heading
    		30, 27, 32, 27, 37, 27,			// Report Heading (IDs and Installation date)
    		34, 27, 						// Report Heading (report version)
//    		35, 36, 50, 37, 27,  			// Report Heading (current GPS location)
    		39, 50, 40, 27,  				// Report Heading (nest GPS location)
    		42, 27, 44, 27,  0				// Report Heading (Timings: start, end, and interval)
    		};

    static const char report_script[] = {
        		 2,  3,  4,  5,  8,   				// Open ftp and append information into a file
        		32, 57, 30, 51, 34, 58, 26, 		// File name
        		28, 32, 57, 30, 51, 34, 58, 27,		// Start Report heading
        		29, 30, 27, 31, 32, 27, 36, 37, 27,	// Report Heading (IDs and Installation date)
        		33, 34, 27, 						// Report Heading (report version)
    //    		35, 36, 50, 37, 27,  			    // Report Heading (current GPS location)
        		38, 39, 50, 40, 27,  				// Report Heading (nest GPS location)
        		41, 42, 27, 43, 44, 27, 45, 0		// Report Heading (Timings: start, end, and interval)
        		};


    static const char end_report[] = {
    			27, 54, 55, 0};	// end of report text

    static const char read_parameters[] = {
		   	   	 4,  5, 11, 52, 30, 53, 47, 0	// Read in paramtetersXX####.txt
   				};

    static const char delete_parameters[] = {
    		   	 4, 12, 52, 30, 53, 26, 10, 0	// Delete paramtetersXX####.txt
       			};

    // Error codes
    //------------
    // 	0 OK
    //	1 CONNECT
    //	2 RING
    //	3 NO CARRIER
    //	4 ERROR

    // Counters
    unsigned long int x;		//all purpose 4 byte
    unsigned int y;				//all purpose 2 byte
    unsigned int z;				//all purpose 2 byte

    static const char days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    							// used to calculate elapsed days
	static const unsigned int powers_of_10[] = { 1, 10, 100, 1000, 10000};
								// used for coverting HEX to ASCII



void watchdog_reset(void)
{	WDTCTL = ((WDTCTL &  0x007F) | 0x5A08);			// reset the watchdog timer
	// mask off the high byte, check that the WDT is not on hold, and put in the password
	// password is 5A hex in the upper byte.  8 in the lower byte resets the counter
}

    	// Wait a specified number of milliseconds (long int)
void wait(unsigned long int wait_millisecs)
{ unsigned int cycles = CLOCKSPEED;		// be sure that CLOCKSPEED is set correctly for the timing of this to work
		while (wait_millisecs > 0) {	// count off the milliseconds
			for (cycles = CLOCKSPEED; cycles>0; cycles--) {  // wait about a millisecond
				__delay_cycles(982) ;
				}
			wait_millisecs-- ;
		}
}



			// Sleep for a specified number of clock interrupt (they happen once a second)
void sleep(unsigned int secs)
{
	while (secs) {
		__bis_SR_register(LPM3_bits + GIE); // Enter low power mode until the next tick of the clock
		secs-- ;
	}
}


		// turn on yellow LED, wait specified amount of time, turn off LED
void blink (unsigned long int millisecs)
{	//if (BLINK)					// Blinking is just for testing and the hand-held unit.  We can turn it off to save power
//	{
		P3OUT |= TSB_LED;		// LED  at pin 3.7 on
		wait(millisecs);		// wait
		P3OUT &= ~TSB_LED;		// LED at pin 3.7 off
//	}
}

/*		// multi blinks are used for the hand-held unit
void multi_blink(unsigned char blinks, unsigned int speed)
{
	while (blinks> 0) {
		blink(speed);
		wait(speed);
		blinks--;
	}
	wait(speed+speed);
}
*/

unsigned int readBattery(void)
{	unsigned int ADC_Result=0;
	REFCTL0 |= REFON; 			// Turn on 1.5V internal reference
	wait(10);					// Let ref voltage stabilize for 10 mSec
	ADC10CTL0 &= ~ADC10ENC ;  	// ADC10_B disabled
	ADC10CTL0 |= ADC10ON ;    	// ADC10_B module turned on
	ADC10CTL1 &= ~(ADC10SHS0 | ADC10SHS1) ;     // Trigger on ADC10SC

				//Enable and Start the conversion in Single-Channel, Single Conversion Mode
	for(x = 16; x>0; x--)
	{			// we'll take sixteen readings and average them
         ADC10CTL0 |= (ADC10ENC | ADC10SC) ;    // Enable ADC10_B and start sample and conversion
         while (!(ADC10IFG & ADC10IFG0) ); 		// wait until conversion is finished
                // Accumulate ADC10_B conversion results
         ADC_Result += ADC10MEM0;   			// add the 10 bit result into the total
    }
	ADC10CTL0 &= ~ADC10ON ; 	// ADC10_B module turned off
	REFCTL0 &= ~REFON;			// Turn off 1.5V reference
	battery_level = (ADC_Result >>4) ; 		// Make it into a 10 bit number again
	return (battery_level);
}


// TRANSFER A STRING
	// sends or receives a character array of known length using the UART
	// 1st parameter is the pointer to the origin or destination string,
	// the second is the length of that array
	// the third is starting byte offset of the first character to move
	// the fourth is the number of bytes to transfer
	// the fifth is whether sending or receiving 0 = RECEIVE, 1 = SEND
void transfer_string( char *string_ptr, unsigned char string_cnt, unsigned char start_byte, unsigned char bytes2move, unsigned char sending)
{	unsigned char temp_count;		// counter
	while (UART_TX_BUSY);			// wait for no UART activity
	UCA1TXBUF = start_byte;			// the first byte sent is the starting byte
	for (temp_count = 0; temp_count < start_byte; temp_count++) string_ptr++; // adjust pointer to correct location
	while (UART_TX_BUSY);			// wait for an empty buffer
	UCA1TXBUF = bytes2move;			// the second byte sent is how many bytes to transfer
	if (!sending)
	{	for (; bytes2move; bytes2move--)	// count off how many we've gotten
		{		while (RX_BUFF_EMPTY) ;		// wait for a byte
				*string_ptr  = UCA1RXBUF;	// store the parameter
				string_ptr++;				// increment the pointer
		}
		return;
	}
	for (; bytes2move; bytes2move--)// count off how many we're sending
	{	while (UART_TX_BUSY);		// wait for an empty buffer
		UCA1TXBUF= *string_ptr;		// send the parameter
		string_ptr++;				// increment the pointer
	}
	return;
}


	// MOVE_STRING
	// moves characters from one string (origin) to another (destination), overwriting the destination
	// the first parameter is the pointer to the string that is the origin of the characters
	// the second parameter  is the offset into the offset into that string for the first byte to move
	// the third parameter is the number of characters to move
	// the fourth parameter is the pointer to the string that is the destination of the characters
	// the fifth parameter is the offset into that string where the first byte gets put
	// CAUTION!  There is no error checking in this routine.  BE CAREFUL!

void move_string(char *origin_ptr,   unsigned char origin_offset,  unsigned char bytes2move,
									 char *destination_ptr,  unsigned char destination_offset)
{	unsigned char temp_count;		// counter
	for (temp_count=0; temp_count < destination_offset; temp_count++) destination_ptr++;
							// adjust the destination pointer to its actual destination
	for (temp_count=0; bytes2move; temp_count++)	// count through all of them
	{	if (temp_count >= origin_offset)			//only move the data once we're at the offset
		{	bytes2move--;							// decrement the number of bytes left to move
			*destination_ptr =  *origin_ptr;		// move a character from the origin string to the destination string
			destination_ptr++ ;						// increment the destination pointer only after a move
		}
		origin_ptr++;								// always increment the pointer to the origin
	}
}


////////// SS_command -- Send SMART SENSOR COMMANDS
		// send a command byte over the UART to the smart sensor
		// Smart Sensor Interrupt codes
		// These codes are returned after a command 0x0F is received
		// thet explain the reason for generating an interrupt
		// All codes less than 0x80 are not errors.
		#define NO_ERROR  0x00 				// = No interrupt was generated -- operation in progress
		#define NO_ERROR_INT1 0x01 			// = INT1 generated an interrupt
		#define NO_ERROR_INT2 0x02			// = INT2 generated an interrupt
		#define NO_ERROR_DATA_READY 0x04 	// = operation completed successfully, data ready to report
		#define NO_ERROR_COMPLETE 0x08		// = operation completed successfully, no data to report
		#define NO_ERROR_START_UP 0x10		// = Normal start-up
		#define NO_ERROR_SIGN_OFF 0x20		// = processing beginning, communication over
		#define NO_ERROR_READY_TO_RECEIVE 0x40 // = sensor is ready to receive data

				// >= 0x80 = an error condition exists.  The error code is returned:
		#define ERROR_SENSOR_TIMEOUT  0x81	// = Sensor Start up failure (timed out)
		#define ERROR_NO_COMMAND 0x82		// = Unknown command received
		#define ERROR_BAD_DATA 0x84			// = Bad Data received
		#define ERROR_TX_TIMEOUT 0x88		// = Data transmission timed out
		#define ERROR_OUT_OF_RANGE 0x90		// = Parameters out of range (bug in code)
void SS_command(char command2send)
{	unsigned char command_check;
	wait(1);
	UCA1CTL1 |= UCSWRST;			// Reset the UART;
	__no_operation();
	UCA1CTL1 &= ~UCSWRST;			// Start the UART
	wait(2);						// give the sensor a chance to finish whatever it is doing
	while(UART_TX_BUSY);			// wait for the UART to finish
	UCA1TXBUF = command2send;		// Send the command byte
	if (command2send == MSP_SLEEP)	// if the command is to power down the sensor
	{	P3OUT &= ~TSB_TX_RX_ON;		// turn off the transceivers
		return;						// The sensor won't respond because it is asleep
	}
	while (RX_BUFF_EMPTY) ;
	command_check = UCA1RXBUF;
	while (RX_BUFF_EMPTY) ;
	sensor_error = UCA1RXBUF;
	if (sensor_error == NO_ERROR_SIGN_OFF) P3OUT &= ~TSB_TX_RX_ON ;
									// Turn off RS485 -- communication is finished for the time being
//	if (sensor_error & 0x80) multi_blink(25,FAST);	// for now just blink //TODO// more robust error handling
//	if (command2send != command_check) multi_blink(25,SLOW);
}


////// SS_receive_records -- RECEIVE RECORDS FROM THE SMART SENSOR
	// receive data over the UART from the RS485
	// can receive 64K records of 255 bytes maximum
	// records[][] needs to be declared to be large enough to handle whatever comes in here.
	// probably no more than 240 records of 32 bytes (about 8K) on the MSP430FR5739
int SS_receive_records(void)
{	unsigned char data_count;
	unsigned int record_count = 0;
	unsigned char data_size;
	unsigned int records_coming;
	while(RX_BUFF_EMPTY);			// wait for the first byte
	records_coming = UCA1RXBUF;		// the first byte sent will be the high byte of the number of blocks to expect
	records_coming <<= 8;			// move it to the high byte
	while(RX_BUFF_EMPTY);			// wait for the second byte
	records_coming += UCA1RXBUF;	// the second byte sent will be the low byte of the number of blocks to expect
	if (records_coming)				// if no data, the reply is zero
	{	while(RX_BUFF_EMPTY);		// wait for another byte
		data_size = UCA1RXBUF;		// the third byte sent will be the count of how many bytes to receive in each record
		while (record_count < records_coming)		//loop for each record
		{	data_count = 0;							// start the data count at zero
			while (data_count < data_size)			//loop for each byte
			{	while(RX_BUFF_EMPTY);		// wait for a byte
				records[record_count][data_count] = UCA1RXBUF;	// Store the byte in the buffer
				data_count++;				// increment data count
			}
			record_count++;
		}
	}
	return(record_count);			// return the number of records received
}


void reset_phone_UART(void)
{		wait(10);
		UCA0CTL1 |= UCSWRST;			// Reset the UART for the phone
		__no_operation();
		UCA0CTL1 &= ~UCSWRST; 			// enable the UART for the phone
		wait(10);
}


	// Receive a response from the phone
	// The error code will be the penultimate character
unsigned int ATresponse(unsigned char CRs_expected)	// Pass the number of lines expected (additional lines are ignored)
{	unsigned int data_count = 0;				// how many bytes are received
	if(!CRs_expected) return(0);				// don't do anything if no response is expected
	while (CRs_expected)						// get whatever else comes in
	{ 	while(PHN_RX_BUFF_EMPTY);				// wait for a byte
		data_received[data_count] = UCA0RXBUF;	// store the byte received in a buffer
		if (data_received[data_count] == CR) CRs_expected--;	// count off the CRs received
		data_count++;							// increment the number of bytes received
	}
	if (data_count>1) return(data_received[data_count-2]-0x30) ;	// return error state:  0 = OK, 4=Error
	return(0);
}

		// Sends an AT code or string to the phone
void sendMessage(unsigned char message_code)	// the array number of the pointers is passed
{ 	unsigned char AT_tx_char;					// The character we will send out
	unsigned char last_char;					// used to check if the last char is a CR
	const char *string2send;					// we'll put the pointer here
	string2send = messages[message_code];		// get the pointer out of the array
	while (AT_tx_char = *string2send)			// get the next character and also continue looping if it is non-zero
	{ 	while(PHN_TX_BUSY);						// wait for the UART to finish
		last_char = AT_tx_char;					// save the last char
		string2send++;							// increment the pointer so it points to the next char
		UCA0TXBUF = AT_tx_char;					// Send the byte
	}
	error_code = 0;								// no error in the middle of lines
//	if (message_code==3) error_code = ATresponse(1);		// There are two CRs for this command
	if (last_char == CR)						// things to do at the end of lines
	{	if (message_code<27)
		{	error_code = ATresponse(1);			// wait for a reply for FTP commands
			wait(980); 							//wait a second between AT commands
		}
		blink(1);									// blink as we process messages
	}
}


		// process a script for the phone
void do_script(const char *script)	//pass the pointer to the script array
{	unsigned char code;				// the command codes in the script
	const char *script_ptr;			// This pointer we can change
	script_ptr = script;			// set it to point to the script
	reset_phone_UART();				// clear out the buffers before starting
	while (code = *script_ptr)		// get the code pointed to in the script array
	{ 	sendMessage(code);			// send the message with that code
		script_ptr++;				// point to the next character in the script
		if ((code == 26) && (error_code == 4)) script_ptr = script;	// reset the pointer and restart the script if there is a problem opening files
	}
}


		// get an internet connection with the phone board
void makeConnection(void)
{	reset_phone_UART();
	sendMessage(1);							// Send the AT command to turn off echo and verbose responses
				// TODO // Modify so that this will work with all Janus boards automatically.
				// The following line is necessary for an HE910.
	while (data_received[1] != 0x2B ) error_code = ATresponse(1);  	// Wait for board to say "+PACSP0" (don't know why this happens)

	sendMessage(19);		// Turn off GPS if comm tower, turn on if hand-held unit
	sendMessage(17);						// Set phones RTC from network time automatically
	sendMessage(15);						// Send AT code to request a registration report
	while (error_code != 1)					// Keep trying until there is a connection
	{	sleep(1);							// wait a second before trying again
		reset_phone_UART();
		sendMessage(15);					// Send AT code to request a registration report
	}										// TODO // better error handling!
}

	// convert an integer to an ascii string
void int2ascii(unsigned int number, char *char_string, unsigned char offset, unsigned char string_length) // string_length can be 1, 2, 3 or 4
{	char test_chars[4];									// an easier to reference array to create the result
	unsigned char power_count, string_count;			// keep track of powers of ten
	power_count = string_length - 1;					// start with the highest power of ten
	if (number >= powers_of_10[string_length]) return;	// a 4 character int must be less than 10 to the 4th power.
	for (string_count = 0; string_count < string_length; string_count++ )	// start with the leftmost character
	{	test_chars[string_count] = 0x30;
		while(number >= powers_of_10[power_count])
		{	test_chars[string_count]++ ;
			number -= powers_of_10[power_count];		//start the char at hex30, increment it for every
		}												// power of ten subtracted from the number
		power_count--;									// do this for all the digits
	}
	move_string(test_chars, 0, string_length, char_string, offset);  // move temporary string back to substring specified;
}

	// convert an ascii string to an integer
unsigned int ascii2int(char *char_string, unsigned char offset, unsigned char string_length) // string_length can be 1, 2, 3 or 4
{	char test_chars[5];					// an easier to reference array to work with
	unsigned char result = 0;			// accumulate the result here
	unsigned char string_count;			// go character by character
	unsigned char power_count = 0;		// keep track of powers of ten
	move_string(char_string, offset, string_length, test_chars, 1);  // move substring to temporary location
	for (; string_length; string_length-- )
	{	for	(string_count = test_chars[string_length]- 0x30; string_count; string_count--)
				result += powers_of_10[power_count];
		power_count++;
	}
	return(result);
}

	// set the RTC from the cell tower
	// TODO // Save day and report count on sensor so that the numbers continue if the comm tower is swapped
void set_time(void)
{	unsigned int start_day, current_day;
	unsigned int start_month, current_month;
	move_string(messages[44], 0, 19, messages[42], 0);  // move old report time to start time
	reset_phone_UART();									// empty the buffers
	sendMessage(18);									// request network time
	move_string(data_received, 8, 17, messages[44], 2); // move the date and time
	start_month =  ascii2int(messages[32], 5, 2);		// convert 2 ASCII month chars to a single byte
	start_day =  ascii2int(messages[32], 8, 2);			// convert 2 ASCII day chars to a single byte
	current_month = ascii2int(messages[44], 5, 2);		// convert 2 ASCII month chars to a single byte
	current_day = ascii2int(messages[44], 8, 2);		// convert 2 ASCII day chars to a single byte

	if (start_month == current_month) day_count = current_day-start_day+1;	// the case of the same month
	else
	{	day_count = days_in_month[start_month] - start_day + 1;
		start_month++;
		while(start_month<current_month)
		{	day_count += days_in_month[start_month];
			start_month++;
		}
		day_count+= current_day;
	}
	int2ascii(day_count, messages[34], 0, 3);			//put the count into the string
}



		// Wait for the sensor to finish whatever it was doing
void contact_sensor(void)
{	wait(1);						// allow RS485 to finish last send
	P3OUT &= ~TSB_TX_RX_ON ;		// Turn off RS485
	__no_operation();
	while (!(P3IN & TSB_INTERRUPT));// Wait for Sensor to indicate that it is ready to talk
			// Turn on the RS485 chips and confirm everything is allright
	UCA1CTL1 |= UCSWRST;			// Reset the UART;
	__no_operation();				// very quick pause
	UCA1CTL1 &= ~UCSWRST;			// Make sure the UART is on
	P3OUT |= TSB_TX_RX_ON ;			// Turn on RS485
	while (P3IN & TSB_INTERRUPT);	// wait for sensor to acknowledge RS485 is on by setting the interrupt line low
	wait(5);						// The interrupt line sometimes goes low even without the acknowledgment.
	SS_command(PROGRESS_REPORT);	// send SS the command to see if all is good
}

		// download a new set of parameters from the web if they are available for this sensor
void get_new_parameters(void)
{	unsigned int tries, checksum;					// use a checksum to verify a good load
	char *message_ptr;								// pointer to sensor ID
	unsigned char datacount;						// counter for file data
	unsigned char bad_checksum = TRUE;				// flag for checksum results
	for (tries = 1; bad_checksum && (tries<3) ; tries++) // try 2 times for a good load
	{	wait(980);	// pause between tries
		message_ptr = messages[30];					// point to the sensor ID
		do_script(read_parameters);					// download the file
		while(PHN_RX_BUFF_EMPTY);					// wait for a byte
		data_received[0] = UCA0RXBUF;				// store the byte received in a buffer
		if (data_received[0] != 0x31) continue;		//first byte should be a "1"
		for(datacount=1; datacount < 0x33; datacount++)	// read in the file
	 	{	while(PHN_RX_BUFF_EMPTY);				// wait for a byte
			data_received[datacount] = UCA0RXBUF;	// store the byte received in a buffer
	 	}
		checksum=0;									// start the checksum at zero
		for(datacount=2; datacount < 0x30; datacount++) // verify the file
		{	if (datacount < 0x08) if (data_received[datacount] != *message_ptr++) continue; // verify the sensor ID
			checksum += data_received[datacount];	// add to the checksum
		}
		if (checksum == ((data_received[0x30]<<8) + data_received[0x31])) bad_checksum = FALSE;
													// confirm the check-sum
	}												// try again if there is an error.  Give up after 2 tries.
	wait(980);										// pause before delete
	if (!bad_checksum) 								// double negative if a good file load
	{	move_string(data_received, 8, PARAM_BYTES, new_parameters, 0);	// save the downloaded parameters
		do_script(delete_parameters);				// delete file if successful
		new_parameters_loaded = YES;				//set flag for report message
	}
	sendMessage(10);		// close FTP connection
}

		// decide if reports should be daily or more frequent and adjust timings
void set_report_frequency(void)
{	// unsigned char byteCount;					// counter for counting the 4 bytes of MAXRUN
		// calculate the new maxTime
//	maxTime = parameters[21]; 					// maxTime starts with the highest byte of MAXRUN
//	for (byteCount = 3; byteCount; byteCount--)	// set maxTime to catch sensor errors
//	{	maxTime <<= 8;							// Shift the bits up to get ready for the next byte
//		maxTime += parameters[17 + byteCount];  // combine MAXRUN1L, MAXRUN2, MAXRUN3 and MAXRUN4H (parameters[18] to [21])
//	}
		// adjust the new parameters and send them out
	new_parameters[16] = new_parameters[29];	// reset parameters[16] and [17] to the slow day settings
	new_parameters[17] =new_parameters[30];
	if (day_count >= new_parameters[28])	// first approximation, wait 40 days and then go fast
	{	new_parameters[16] = new_parameters[26];	// reset parameters[16] and [17] to the slow day settings
		new_parameters[17] = new_parameters[27];
	}
	SS_command(NEW_PARAMETERS);		// send the parameters to the sensor
	wait(1);						// don't get ahead of the sensor
	transfer_string(new_parameters, PARAM_BYTES, 0, PARAM_BYTES, SEND);	// send all the parameters
	contact_sensor();				// wait for the sensor to respond and turn on RS485
}


		// send out a nibble as an ASCII hexadecimal number from 0 to F
void send_nibble(unsigned char nibble)		// pass the nibble and we'll convert it
{	if (nibble > 0x09) nibble += 7;			// add 7 if the nibble is ten or more
	while(PHN_TX_BUSY);						// wait for the UART to finish
	UCA0TXBUF = (nibble + 0x30);			// Send the byte converted to ASCII
}

		// send out a byte in two nibbles
void send_byte(unsigned char data_byte)		// pass the byte to send
{		send_nibble(data_byte>>4);			// send high nibble of byte
		send_nibble(data_byte & 0x0F);		// send low nibble of byte
}

		// sends out an integer in four nibbles
void send_integer(unsigned int data_integer)
{
	send_byte((data_integer & 0xFF00)>>8); // print the high byte of recCount
	send_byte(data_integer & 0x00FF);	// print the low byte of recCount
}
		// send out records in the report
void report_record_data(void)
{ 	unsigned int report_count;
	unsigned char char_count;
	for (report_count = 0; report_count < recCount; report_count++)
	{	if (parameters[25])				// if report headings are desired
		{	send_integer(report_count);		// send a count
			sendMessage(50);				// send a comma and a space
		}
		for (char_count = 0; char_count < 32; char_count +=2)// go through them two by two
		{	send_byte(records[report_count][char_count]);	// send first byte of data
			send_byte(records[report_count][char_count+1]);	// send second byte of data
			if (char_count < 30 ) sendMessage(50);			// send a comma and a space except at the end of the line
		}
		sendMessage(27);			// send a carriage return
	}
	wait(2);						// make certain there is enough time to finish transmission.
}


void send_report(void)
{	if (parameters[25])				// contains REPORT_HEADINGS
	{	do_script(report_script);	// send command to open FTP and create a file
									// also creates most of the file header
		send_byte(parameters[17]);		// High byte of seconds per record
		send_byte(parameters[16]);		// Low byte of seconds per record
		sendMessage(46);				// # of records
		send_integer(recCount); 		// print the recCount
		sendMessage(48);				// battery level
		send_integer(battery_level ); 	// print the battery level
		sendMessage(27);				// send a carriage return
		sendMessage(27);				// send a carriage return
		sendMessage(49);				// headings for record data
	}
	else
	{	do_script(report_script_noheading);
		send_byte(parameters[17]);		// High byte of seconds per record
		send_byte(parameters[16]);		// Low byte of seconds per record
		sendMessage(27);				// send a carriage return
		send_integer(recCount); 		// print the recCount
		sendMessage(27);				// send a carriage return
		send_integer(battery_level ); 	// print the battery level
		sendMessage(27);				// send a carriage return
		sendMessage(27);				// send a carriage return
		sendMessage(27);				// send a carriage return
	}
	report_record_data();						// send all the data
	if (new_parameters_loaded) sendMessage(59); // send new parameters loaded message
	if (force_shut_down) sendMessage(60); 		// send shutdown warning
	if (wd_reset) sendMessage(61);				// send reset message
	if (starting_up) sendMessage(62);			// send startup message
	if (!sensor_plugged_in) sendMessage(63);	// send sensor disconnect message
	if (parameters[25])	do_script(end_report);	// closing messages
	new_parameters_loaded = NO;					// clear the new parameters flag
	wd_reset = NO;								// clear the reset flag
	starting_up = NO;							// clear the starting up flag
	wait(2000); 								// add 2 seconds before and after escape routine (code 9)
	sendMessage(9);								// send the escape message
	wait(2000); 								//  2 seconds after escape routine
	sendMessage(10);							// close ftp connection
}


		// Check for a sensor connection
		// The default setting before connection is for P3.3 (INTERRUPT) to have a pull-down on the input, and the default
		// for P.3.4 (TAMPER) is to have a pull-up so that the inputs do not float.
		// Once the sensor is connected the pull-up and pull-down is removed so there is no current drain.
		// sensor_plugged_in is set when the sensor is connected.
		// This routine polls these the TAMPER line to see if it is high.  If sensor_plugged_in is not set, then
		// a pull-down resistor is turned on before polling.  If the line is low,
unsigned char sensor_connection(void)
{	P3REN = (BIT3 | BIT4); // enable the pull down resistor on the TAMPER line
	if (sensor_plugged_in) wait(CABLE_DISCHARGE_TIME); // wait long enough to discharge the capacitors filtering the line
	if (P3IN & TSB_TAMPER)
	{	// there is no tamper condition if the pin is high
		if (!sensor_plugged_in)
		{	//  the pull-down resistors are no longer needed because the sensor was just plugged in
		    P3REN = 0;	 // Port 3 Resistor Enable Register
			sensor_plugged_in = YES;	// remember for next time
		}
		return(YES);
	}	// there is a tamper condition if the pin is low -- the pull-down down resistors are still needed
	sensor_plugged_in = NO;				// remember for next time
	return(NO);
}


		// Turn on the phone
void phone_on(void)
{		// Enable REF Module and prepare to turn on the phone
	unsigned int tries=0;
	REFCTL0 |= REFON;				// Turn on 1.5V internal reference
	P2OUT |= TSB_PHNRESET;			// Set phone reset bit high  to set the phone pins to hi-z
	P3OUT|= TSB_PHNONOFF;			// Set on/off bit high to set the phone pins to hi-z
	P2OUT &= ~TSB_UCA0TXD;			// Set UART transmit bit low
	P3OUT |= TSB_PHNPOWER;			// Turn on the phone power
	wait(10);						// wait for 10 milliseconds
		// then make sure the phone is off, but the power supply is on
	while (P2IN & TSB_PWRMON) 		// check if the power monitor pin (P3.3) is high
	{	P3OUT &= ~TSB_PHNPOWER;		// while it is, turn off the phone board power supply
		while (P2IN & TSB_PWRMON) wait(1);
			// loop until the power monitor pin (P3.3) is low.  Pause a millisecond before checking again
		P3OUT |= TSB_PHNPOWER;		// Turn on the phone power
		wait(10);					// wait for 10 milliseconds for the capacitor to charge up
	}
		// actually turn on the phone
	while (!(P2IN & TSB_PWRMON)) 	// While the power monitor indicator bit is off
	{	P3OUT &= ~TSB_PHNONOFF;		// switch the phone power on/off by driving the pin low by setting P3.5 low
		force_shut_down = (readBattery() < SHUT_OFF_LEVEL) ;	// if the battery is low, set a flag to shut down after uploading data
		wait(3000);					// with the battery reads this will be over 3 seconds
		P3OUT |= TSB_PHNONOFF;		// return the pin to hi-z by setting 3.5 high
		wait(4000);
	}
}


	// turn off the phone
void phone_off(void)
{
	P2OUT &= ~TSB_UCA0TXD;		// Set UART transmit bit low
	P2OUT|= TSB_PHNRESET;		// Make certain that the reset bit is off
	wait(1);					// wait for the chips to settle?
	while (P2IN & TSB_PWRMON)	// keep doing this while the phone power monitor stays high
	{	P3OUT &= ~TSB_PHNONOFF;	// switch the phone power on/off by driving the pin low by setting P3.5 low
		wait(3000);				// wait at least 3 seconds
		P3OUT |= TSB_PHNONOFF;	// return the pin to hi-z by setting 3.5 high
		wait(3000);				// wait at least 3 seconds
	}
	P3OUT &= ~TSB_PHNPOWER;				// Turn off the phone power
}

		// AWAIT CONNECTION
		// Check to see if the smart sensor is plugged in.
		// go to sleep when the sensor is disconnected to save power.
		// check for a connection every 10 seconds.
void await_connection()
{	while (!sensor_connection())
	{	sleep(10);
		blink(1);
		watchdog_reset();
	}
sleep(1);			// let the smart sensor power up for a moment longer
}

		// POWER DOWN
		// enter sleep mode 4 if the batteries are low
void power_down()
{
		P3OUT |= TSB_SNSRRST;		// Power down the sensor to reset it (TODO this needs fixing because it causes 70uA drain)
	// TODO // this may ultimately be improved by sending an instruction to the microprocessor to go to sleep.
		WDTCTL = ((WDTCTL &  0x00FF) | 0x5A88);		// reset the watchdog timer on hold
//	await_connection();				// wait in low power mode until the smart sensor is plugged in.
//	contact_sensor();				// wait for the sensor to respond and turn on RS485
//	SS_command(MSP_SLEEP);			// send SS the command to sleep
		// this causes strange power consumption problems
	while (TRUE) __bis_SR_register(LPM3_bits + GIE); 	// Enter low power mode 3
}

		// PHONE IT IN
		// Turn on the phone and create a report
void phone_it_in(void)
{	phone_on();
	makeConnection();				// wait for the phone to establish an internet connection
	sleep(1);
	set_time();						// reads RTC from the phone and saves it in messages
		// increment the report count, start over if it is a new day
	if (day_count!= last_day_count) move_string("00", 0, 2, messages[34], 4);	// move the two zeros to the string
	int2ascii(ascii2int(messages[34], 4, 2) + 1, messages[34], 4, 2);	// increment report count
	int2ascii(day_count, messages[34], 0, 3);	//put the count into the string
	last_day_count = day_count;		// save the count for next time
	send_report();					// generate a report
	move_string(new_parameters, 0, PARAM_BYTES, parameters, 0); // save the current parameters to print out later
	get_new_parameters();			// upload a new set of paramters if they are available
	phone_off();					// turn the phone module off
	watchdog_reset();  				// reset the watchdog timer
		// erase the previously uploaded data
	for (y = 0; y < MAX_RECORDS; y++) for (z = 0; z < 32; z++) records[y][z] = 0;  // Erase the entire array
	recCount = 0;					// The count of how many records have been collected since last upload
	secCount = maxTime + TIME_OUT;	// Reset the secCount countdown counter
	if (force_shut_down) power_down();
}


	// START UP routine -- only runs after a reset or cold start
void start_up(void)
{	if (readBattery() < SHUT_OFF_LEVEL) power_down(); 	// Check for sufficient battery power
	watchdog_reset();				// Remove hold from watchdog
	P3OUT |= TSB_SNSRRST;			// Power down the sensor to reset it
	wait(1000);						// Wait a second
	P3OUT &= ~TSB_SNSRRST;			// Turn the sensor back on
	if (recCount) phone_it_in();	// send old data if there is any after WD reset
	await_connection();				// wait in low power mode until the smart sensor is plugged in.
	contact_sensor();				// wait for the sensor to respond and turn on RS485
	SS_command(PROGRESS_REPORT);	// send SS the command to see if all is good
	SS_command(CHECK_ID);			// ask sensor to send the nest_ID
	transfer_string(nest_ID, NEST_ID_BYTES, 0, NEST_ID_BYTES, RECEIVE);	// receive the nest_ID from the sensor
	move_string(nest_ID,  SERIAL,SERIAL_BYTES, messages[30], 0);		// store the serial number
	move_string(nest_ID,  DAY, DAY_BYTES, messages[32], 8);				// store the date installed
	move_string(nest_ID,  MONTH, MONTH_BYTES, messages[32], 5);
	move_string(nest_ID,  YEAR, YEAR_BYTES, messages[32], 2);
	move_string(nest_ID,  LATITUDE, LATITUDE_BYTES, messages[39], 0);	// store the nest location
	move_string(nest_ID,  LONGITUDE, LONGITUDE_BYTES, messages[40], 0);
	move_string("00", 0, 2, messages[34], 4); // reset the report counter
	starting_up = YES;				// flag the sensor power up
	phone_it_in();					// report that the new sensor was plugged in
	set_report_frequency();			// decide how often to report
	SS_command(START_RUN_BINS);		// send SS the command to collect data and process

}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////// MAIN ROUTINE ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
		// sends commands to the smart sensor and controls reporting via
		// Janus Plug-in terminus boards
void main(void)
{	Grace_init();       // Activate Grace-generated configuration
	wd_reset = YES;		// flag a power-up reset
	blink(500);			// show that it is running
	start_up();			// initialization after cold-start or reset
	while (TRUE)		// Main loop -- never stops
	{	__bis_SR_register(LPM3_bits + GIE); 	// Enter low power mode until the next tick of the clock (every second)
		if (P3IN & TSB_INTERRUPT)				// The smart sensor will set this pin high when all recording is completed
		{	contact_sensor();					// set up communications
			SS_command(UPLOAD_ALL);				// send SS the command to upload all the records
			recCount = SS_receive_records();	// upload what is sent
			wait(5);							// wait 5 ms.
			set_report_frequency();				// set timings for daily or more frequent reports
			SS_command(START_RUN_BINS);			// send SS the command to collect data and process the data into records
			phone_it_in();						// turn on the phone and send a report with the uploaded data
		}
		blink(1);								// one millisecond blink
		if(!secCount) WDTCTL = 0;				// force a reset if we time out
		if (!tamper_count) if (!sensor_connection()) // Every 10 seconds check to see if the sensor is still connected
				{	phone_it_in();				// turn on the phone and send a report of the disconnect
					starting_up = YES;			// If it is not connected there is a tamper condition possible
					await_connection();			// Await a reconnection and then resume operation
					start_up();					// restart everything
				}
		watchdog_reset();  // reset watchdog timer every second.  If something hangs, the timer will cause a reset in about 4 minutes.
	}
	// we'll never get here!
}


/*	  Some possible code if we get python working in the Phone module

			if (Python_available) {
					store_python_record() ;  //If the m2m module supports python, store the record there.
					recCount_phone++ ;		 // Increment the number of records stored on the phone
					erase_record() ;
					SS_delete_record() ; // Tell the sensor that the record has been saved so it can delete it there
				}
				else if (room_here) {
						keep_record_here();
						SS_delete_record();
						recCount_here++ ;		// Increment the number of records stored here
						room_here-- ;			// Room for one less
				}
				else sensor_recCount++;	// Increment the number of records stored on the sensor
				recCount++ ;			// Increment counter of records stored everywhere


void evaluate_activity(void) 	// Decide if there is enough activity to warrant frequent reporting
{
}


void store_python_record(void)  //If the m2m module supports python, store the record there.
{
}


void erase_record(void) 		// erase temp record stored here once it has be moved to the phone
{
}


void SS_delete_record(void)  // Tell the sensor that the record has been saved so it can delete it there
{
}


void keep_record_here(void)		// make temp record into record stored here
{
}


*/
