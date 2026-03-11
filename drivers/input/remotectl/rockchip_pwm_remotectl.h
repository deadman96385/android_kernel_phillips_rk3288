/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __RKXX_PWM_REMOTECTL_H__
#define __RKXX_PWM_REMOTECTL_H__
#include <linux/input.h>

#define MAX_NUM_KEYS  60

/* PWM0 registers  */
#define PWM_REG_CNTR                    0x00  /* Counter Register */
#define PWM_REG_HPR		                  0x04  /* Period Register */
#define PWM_REG_LPR                     0x08  /* Duty Cycle Register */
#define PWM_REG_CTRL                    0x0c  /* Control Register */
#define PWM3_REG_INTSTS                 0x10  /* Interrupt Status Refister For Pwm3*/
#define PWM2_REG_INTSTS                 0x20  /* Interrupt Status Refister For Pwm2*/
#define PWM1_REG_INTSTS                 0x30  /* Interrupt Status Refister For Pwm1*/
#define PWM0_REG_INTSTS                 0x40  /* Interrupt Status Refister For Pwm0*/
#define PWM3_REG_INT_EN                 0x14  /* Interrupt Enable Refister For Pwm3*/
#define PWM2_REG_INT_EN                 0x24  /* Interrupt Enable Refister For Pwm2*/
#define PWM1_REG_INT_EN                 0x34  /* Interrupt Enable Refister For Pwm1*/
#define PWM0_REG_INT_EN                 0x44  /* Interrupt Enable Refister For Pwm0*/

/*REG_CTRL bits definitions*/
#define PWM_ENABLE			            (1 << 0)
#define PWM_DISABLE			            (0 << 0)

/*operation mode*/
#define PWM_MODE_ONESHOT			     (0x00 << 1)
#define PWM_MODE_CONTINUMOUS 	     (0x01 << 1)
#define PWM_MODE_CAPTURE		        (0x02 << 1)

/*duty cycle output polarity*/
#define PWM_DUTY_POSTIVE	            (0x01 << 3)
#define PWM_DUTY_NEGATIVE	            (0x00 << 3)

/*incative state output polarity*/
#define PWM_INACTIVE_POSTIVE 		    (0x01 << 4)
#define PWM_INACTIVE_NEGATIVE		    (0x00 << 4)

/*clock source select*/
#define PWM_CLK_SCALE		            (1 << 9)
#define PWM_CLK_NON_SCALE 	            (0 << 9)

#define PWM_CH0_INT                     (1 << 0)
#define PWM_CH1_INT                     (1 << 1)
#define PWM_CH2_INT                     (1 << 2)
#define PWM_CH3_INT                     (1 << 3)

#define PWM_CH0_POL                     (1 << 8)
#define PWM_CH1_POL                     (1 << 9)
#define PWM_CH2_POL                     (1 << 10)
#define PWM_CH3_POL                     (1 << 11)

#define PWM_CH0_INT_ENABLE              (1 << 0)
#define PWM_CH0_INT_DISABLE             (0 << 0)

#define PWM_CH1_INT_ENABLE              (1 << 0)
#define PWM_CH1_INT_DISABLE             (0 << 1)

#define PWM_CH2_INT_ENABLE              (1 << 2)
#define PWM_CH2_INT_DISABLE             (0 << 2)

#define PWM_CH3_INT_ENABLE              (1 << 3)
#define PWM_CH3_INT_DISABLE             (0 << 3)

/*prescale factor*/
#define PWMCR_MIN_PRESCALE	            0x00
#define PWMCR_MAX_PRESCALE	            0x07

#define PWMDCR_MIN_DUTY	       	        0x0001
#define PWMDCR_MAX_DUTY		            0xFFFF

#define PWMPCR_MIN_PERIOD		        0x0001
#define PWMPCR_MAX_PERIOD		        0xFFFF

#define PWMPCR_MIN_PERIOD		        0x0001
#define PWMPCR_MAX_PERIOD		        0xFFFF

enum pwm_div {
        PWM_DIV1                 = (0x0 << 12),
        PWM_DIV2                 = (0x1 << 12),
        PWM_DIV4                 = (0x2 << 12),
        PWM_DIV8                 = (0x3 << 12),
        PWM_DIV16                = (0x4 << 12),
        PWM_DIV32                = (0x5 << 12),
        PWM_DIV64                = (0x6 << 12),
        PWM_DIV128   	         = (0x7 << 12),
}; 




/********************************************************************
**                            �궨��                                *
********************************************************************/
#define RK_PWM_TIME_PRE_MIN	4000
#define RK_PWM_TIME_PRE_MAX	5000

#define RK_PWM_TIME_BIT0_MIN	480
#define RK_PWM_TIME_BIT0_MAX	700

#define RK_PWM_TIME_BIT1_MIN	1300
#define RK_PWM_TIME_BIT1_MAX	2000

#define RK_PWM_TIME_RPT_MIN	2000
#define RK_PWM_TIME_RPT_MAX	2500

#define RK_PWM_TIME_SEQ1_MIN	95000
#define RK_PWM_TIME_SEQ1_MAX	98000

#define RK_PWM_TIME_SEQ2_MIN	30000
#define RK_PWM_TIME_SEQ2_MAX	55000


#define PWM_REG_INTSTS(n)       ((4 - (n)) * 0x10)
#define PWM_CH_INT(n)   BIT(n)
#define PWM_CH_POL(n)   BIT(n+8)

//#define CHECK_PWM_REGISTERS_PER_1S

/********************************************************************
**                          �ṹ����                                *
********************************************************************/
typedef enum _RMC_STATE
{
    RMC_IDLE,
    RMC_PRELOAD,
    RMC_USERCODE,
    RMC_GETDATA,
    RMC_SEQUENCE
}eRMC_STATE;


struct RKxx_remotectl_platform_data {
	//struct rkxx_remotectl_button *buttons;
	int nbuttons;
	int rep;
	int timer;
	int wakeup;
};

typedef enum _RMC_PROTOCOL_ING
{
    RMC_PROTOCOL_NOTHING    = 0xff,
    RMC_PROTOCOL_NEC    = 0x10,
    RMC_PROTOCOL_RC6    = 0x20,

}eRMC_PROTOCOL_ING;

#define RMC_START_MIN           50000
#define RMC_NEC_STARTER_MIN     8000
#define RMC_NEC_STARTER_MAX     10000
#define RMC_RC6_STARTER_MIN     2500
#define RMC_RC6_STARTER_MAX     3000


#define RC6_BIT_COUNT 52
#define RC6_RAW_LS_1        0b1100
#define RC6_RAW_LS_2    0b1111
#define RC6_RAW_SB      0b10
#define RC6_RAW_LS_2    0b1111
#define RC6_RAW_LS_2    0b1111
#define RC6_RAW_LS_2    0b1111


#define RAW_LS_H_4BITS        0b1111
#define RAW_LS_L_4BITS        0b1100
#define RAW_SB_BITS           0b10
#define RAW_MODE2_BITS        0b01
#define RAW_MODE1_BITS        0b01
#define RAW_MODE0_BITS        0b01

#define RAW_TR_0_BITS        0b0011
#define RAW_TR_1_BITS        0b1100

#define RAW_BIT0             0b01
#define RAW_BIT1             0b10

#define RAW_RC6_L_MEAN          450
#define RAW_RC6_H_MEAN          420

#define RAW_RC6_HIGH_POLARITY    0 

struct tpv_ir_rc6_raw {
	unsigned raw_info       :16;
	unsigned raw_ctrl       :16;

	unsigned raw_tr         :4; //1100 or 0011
	unsigned raw_mode_0     :2; //01
	unsigned raw_mode_1     :2; //01

	unsigned raw_mode_2     :2; //01
	unsigned raw_sb         :2; //10
	unsigned raw_ls_1       :4; //1100
	unsigned raw_ls_2       :4; //1111
};

union Rc6Raw_U
{
    struct tpv_ir_rc6_raw rawdata;
    unsigned long long bits;
};

 
struct tpv_ir_rc6_data {
	unsigned dec_ls;
	unsigned dec_sb;
	unsigned dec_mode;
	unsigned dec_tr;
	unsigned dec_ctrl;
	unsigned dec_info;
};


typedef enum _RMC_RC6_RET
{
    RC6_GOOD,
    RC6_ERR_LS          = -1,
    RC6_ERR_SB          = -2,
    RC6_ERR_MODE        = -3,
    RC6_ERR_TR          = -4,
    RC6_ERR_CTRL        = -5,
    RC6_ERR_INFO        = -6,
    RC6_ERR_LENGTH      = -7,
    RC6_ERR_DATA      = 0xffff,
}eRMC_RC6_RET;

struct rkxx_remote_key_table {
	int scancode;
	int keycode;
};

struct rkxx_remotectl_button {
	int procotol;
	int usercode;
	int nbuttons;
	struct rkxx_remote_key_table key_table[MAX_NUM_KEYS];
};

struct tpv_rc6_data {
	union Rc6Raw_U raw;
	struct tpv_ir_rc6_data keydata;
	int bit_count;
	int keynum;
	int keycode_ev;
	int press;
	struct timer_list rc_timer;
};
struct rkxx_remotectl_drvdata {
	void __iomem *base;
	int state;
	int nbuttons;
	int result;
	int scandata;
	int count;
	int keynum;
	int maxkeybdnum;
	int keycode;
	int press;
	int pre_press;
	int irq;
	int remote_pwm_id;
	int handle_cpu_id;
	int wakeup;
	int clk_rate;
	int support_psci;
	unsigned long period;
	unsigned long temp_period;
	int pwm_freq_nstime;
	struct input_dev *input;
	struct timer_list timer;
	struct timer_list dbg_timer;
	struct tasklet_struct remote_tasklet;
	struct wake_lock remotectl_wake_lock;
	int protocol_state;
    struct tpv_rc6_data rc6;
};


extern int rk_remote_print_code;
#define DBG_CODE(fmt, args...) \
	do { \
		if (rk_remote_print_code) { \
			pr_info("MDC-DBG_CODE,M:%s,L:%d. "fmt,__FUNCTION__,__LINE__, ##args); \
		} \
	} while (0)

extern int rk_remote_pwm_dbg_level;
#define DBG(fmt, args...) \
	do { \
		if (rk_remote_pwm_dbg_level) { \
			pr_info("MDC-DBG,M:%s,L:%d. "fmt,__FUNCTION__,__LINE__, ##args); \
		} \
	} while (0)

extern int rk_remote_pwm_dbg_rc6;
#define DBG_RC6(fmt, args...) \
	do { \
		if (rk_remote_pwm_dbg_rc6) { \
			pr_info("MDC-RC6,M:%s,L:%d. "fmt,__FUNCTION__,__LINE__, ##args); \
		} \
	} while (0)


extern struct rkxx_remotectl_button *remotectl_button;


void tpv_rc6_remotectl_timer(unsigned long _data);
int tpv_rc6_remotectl_keybd_num_lookup(struct rkxx_remotectl_drvdata *ddata);
int tpv_rc6_remotectl_keycode_lookup(struct rkxx_remotectl_drvdata *ddata);
void tpv_rc6_raw_fill_bits(struct rkxx_remotectl_drvdata *ddata, int level, int number);
int tpv_rc6_raw16_to_code(int in);
void tpv_rc6_dump_keycode(struct rkxx_remotectl_drvdata *ddata);
int tpv_rc6_decode(struct rkxx_remotectl_drvdata *ddata);
void tpv_rc6_reset(struct rkxx_remotectl_drvdata *ddata);
void tpv_rc6_handler(struct rkxx_remotectl_drvdata *ddata, unsigned h_time, unsigned l_time);

static inline int isHighStarter(unsigned h){
    return h > RMC_START_MIN;
}
static inline int isRC6LowStarter(unsigned l){
    return l >RMC_RC6_STARTER_MIN && l < RMC_RC6_STARTER_MAX;
}
static inline int isNECLowStarter(unsigned l){
    return l >RMC_NEC_STARTER_MIN && l < RMC_NEC_STARTER_MAX;
}
#endif

