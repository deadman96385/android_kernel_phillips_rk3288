/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/irqdomain.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/wakelock.h>
#include <linux/slab.h>
#include <linux/rockchip/rockchip_sip.h>

#include "rockchip_pwm_remotectl.h"
struct rkxx_remotectl_button *remotectl_button;

void tpv_rc6_reset(struct rkxx_remotectl_drvdata *ddata){
	struct tpv_rc6_data* rc6 = &ddata->rc6;
	rc6->raw.bits = 0;
	rc6->bit_count = 0;
	rc6->keynum = 0;
	rc6->press = 0;
	rc6->keycode_ev = 0;
	ddata->protocol_state = RMC_PROTOCOL_NOTHING;
}

void tpv_rc6_remotectl_timer(unsigned long _data){
	struct rkxx_remotectl_drvdata *ddata;
	struct tpv_rc6_data *rc6;
	DBG_RC6("enter\n");
	ddata =  (struct rkxx_remotectl_drvdata *)_data;
 	rc6 = &ddata->rc6;
 	if (rc6->press == 1) {
		rc6->press = 0;
		DBG_RC6("report EV_KEY UP, code=0x%02x\n",rc6->keycode_ev);
		input_event(ddata->input, EV_KEY, rc6->keycode_ev, 0);
		input_sync(ddata->input);
		tpv_rc6_reset(ddata);
	}
}

int tpv_rc6_remotectl_keybd_num_lookup(struct rkxx_remotectl_drvdata *ddata){
	int i;
	int num;
	struct tpv_rc6_data* rc6 = &ddata->rc6;
	DBG_RC6("keybd num not fount.\n");
	num = ddata->maxkeybdnum;
	DBG_RC6("keybd num not fount.\n");
	for (i = 0; i < num; i++) {
	DBG_RC6("keybd num not fount.\n");
		if(remotectl_button[i].procotol == ddata->protocol_state){
	DBG_RC6("keybd num not fount.\n");
			if (remotectl_button[i].usercode == (rc6->keydata.dec_ctrl&0xFFFF)) {
	DBG_RC6("keybd num not fount.\n");
				rc6->keynum = i;
	DBG_RC6("keybd num not fount.\n");
				DBG_RC6("keynum:%d\n", rc6->keynum);
	DBG_RC6("keybd num not fount.\n");
				return 1;
			}
		}
	}
	DBG_RC6("keybd num not fount.\n");
	return 0;
}


int tpv_rc6_remotectl_keycode_lookup(struct rkxx_remotectl_drvdata *ddata){
	int i;
	struct tpv_rc6_data* rc6 = &ddata->rc6;
	DBG_RC6("enter\n");

	for (i = 0; i < remotectl_button[rc6->keynum].nbuttons; i++) {
		if (remotectl_button[rc6->keynum].key_table[i].scancode == 
				rc6->keydata.dec_info) {
			rc6->keycode_ev = remotectl_button[rc6->keynum].key_table[i].keycode;
			DBG_RC6("keycode_ev:0x%02x\n", rc6->keycode_ev);
			return 1;
		}
	}
	return 0;
}

void tpv_rc6_raw_fill_bits(struct rkxx_remotectl_drvdata *ddata, int level, int number){
	int i = 0;
	int bitOffset = 0;
	struct tpv_rc6_data* rc6 = &ddata->rc6;

	//DBG("fill %d for %d bits START FROM %d\n", level,number,rc6->bit_count);
	if(RAW_RC6_HIGH_POLARITY == level){
		for(i=0;i<number;i++){
			bitOffset = RC6_BIT_COUNT - rc6->bit_count -1;
			rc6->raw.bits = rc6->raw.bits | (long long)1 << (bitOffset);
			rc6->bit_count++;
		}
		if(rc6->bit_count == RC6_BIT_COUNT -1){
			rc6->bit_count++;
		}
	}else{
		rc6->bit_count += number;
	}
	//DBG("totally %d bits, raw_bits=0x%016llx\n",rc6->bit_count, rc6->raw.bits);

}
int tpv_rc6_raw16_to_code(int in){
	int out = 0;
	int i = 0;
	int bit = 0;

	DBG_RC6("enter\n");
	for(i=0;i<8;i++){
		bit = (in >>(2*i))&0x03;
		if(bit==RAW_BIT1){
			out |= (1<<i);
		}else if(bit==RAW_BIT0){
			//nothing
		}else{
			return RC6_ERR_DATA;
		}
	}
	return out;
}
void tpv_rc6_dump_keycode(struct rkxx_remotectl_drvdata *ddata){

	struct tpv_rc6_data* rc6 = &ddata->rc6;
	DBG("RC6 KEYCODE:ls=%d,sb=%d,mode=%d,tr=%d,ctrl=0x%04x,info=0x%04x\n",
		rc6->keydata.dec_ls,
		rc6->keydata.dec_sb,
		rc6->keydata.dec_mode,
		rc6->keydata.dec_tr,
		rc6->keydata.dec_ctrl,
		rc6->keydata.dec_info);
	DBG_CODE("RC6 KEYCODE:mode:0x%x,usercode=0x%04x,keycode=0x%04x\n",
		rc6->keydata.dec_mode,
		rc6->keydata.dec_ctrl,
		rc6->keydata.dec_info);
}
int tpv_rc6_decode(struct rkxx_remotectl_drvdata *ddata){
	struct tpv_rc6_data* rc6 = &ddata->rc6;
	struct tpv_ir_rc6_raw raw = rc6->raw.rawdata;
	int data = 0;
	if(rc6->bit_count != RC6_BIT_COUNT)
		return RC6_ERR_LENGTH;

	if(raw.raw_ls_2 != RAW_LS_H_4BITS || raw.raw_ls_1 != RAW_LS_L_4BITS){
		DBG("wrong ls info: 0x%x-%x\n", raw.raw_ls_2,raw.raw_ls_1);
		return RC6_ERR_LS;
	}else{
		rc6->keydata.dec_ls = 1;
	}
	
	if(raw.raw_sb != RAW_SB_BITS){
		DBG("wrong sb info: 0x%x\n", raw.raw_sb);
		return RC6_ERR_SB;
	}else{
		rc6->keydata.dec_sb = 1;
	}
	
	if(raw.raw_mode_0 != RAW_MODE0_BITS ||raw.raw_mode_1 != RAW_MODE1_BITS ||raw.raw_mode_2 != RAW_MODE2_BITS){
		DBG("wrong mode info: 0x%x-%x-%x\n", raw.raw_mode_2, raw.raw_mode_1, raw.raw_mode_0);
		return RC6_ERR_MODE;
	}else{
		rc6->keydata.dec_mode = 0;
	}
	
	if(raw.raw_tr == RAW_TR_0_BITS){
		rc6->keydata.dec_tr = 0;
	}else if (raw.raw_tr == RAW_TR_1_BITS){
		rc6->keydata.dec_tr = 1;
	}else{
		DBG("wrong tr info: 0x%x\n", raw.raw_tr);
		return RC6_ERR_TR;
	}

	data = tpv_rc6_raw16_to_code(raw.raw_ctrl);
	if(data == RC6_ERR_DATA){
		DBG("wrong CTRL info: 0x%04x\n", raw.raw_ctrl);
		return RC6_ERR_CTRL;
	}else{
		rc6->keydata.dec_ctrl = data;
	}

	data = tpv_rc6_raw16_to_code(raw.raw_info);
	if(data == RC6_ERR_DATA){
		DBG("wrong INFO info: 0x%04x\n", raw.raw_info);
		return RC6_ERR_INFO;
	}else{
		rc6->keydata.dec_info = data;
	}
	return RC6_GOOD;
}

void tpv_rc6_handler(struct rkxx_remotectl_drvdata *ddata, unsigned h_time, unsigned l_time){
	struct tpv_rc6_data* rc6 = &ddata->rc6;
	unsigned bit_num;
	if(h_time < RMC_START_MIN){
		bit_num = (h_time + RAW_RC6_H_MEAN/2)/RAW_RC6_H_MEAN;
		tpv_rc6_raw_fill_bits(ddata,1,bit_num);
	}
	bit_num = (l_time + RAW_RC6_L_MEAN/2)/RAW_RC6_L_MEAN;
	tpv_rc6_raw_fill_bits(ddata,0,bit_num);
	if(rc6->bit_count == RC6_BIT_COUNT){
		if(RC6_GOOD == tpv_rc6_decode(ddata)){

			if (tpv_rc6_remotectl_keybd_num_lookup(ddata)) {
				if (tpv_rc6_remotectl_keycode_lookup(ddata)) {
						rc6->press = 1;
						input_event(ddata->input, EV_KEY,
										rc6->keycode_ev, 1);
						input_sync(ddata->input);
						mod_timer(&rc6->rc_timer, jiffies + msecs_to_jiffies(130));
				}else{
					//tpv_rc6_reset(ddata);
				}
			}
			tpv_rc6_dump_keycode(ddata);
		}
	}
}
