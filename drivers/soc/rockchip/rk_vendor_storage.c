/*
 * Copyright (c) 2016, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */

#include <linux/stmmac.h>
#include <linux/kernel.h>
#include <linux/soc/rockchip/rk_vendor_storage.h>
#include <linux/etherdevice.h>

#if 1
#define DBG(x...)   printk(KERN_INFO "[MDC-WIFI-MAC-TO-OTHER]: "x)
#else
#define DBG(x...)
#endif
#define LOG(x...)   printk(KERN_INFO "[MDC-WIFI-MAC-TO-OTHER]: "x)

int (*_vendor_read)(u32 id, void *pbuf, u32 size) = NULL;
int (*_vendor_write)(u32 id, void *pbuf, u32 size) = NULL;

int rk_vendor_read(u32 id, void *pbuf, u32 size)
{
	if (_vendor_read)
		return _vendor_read(id, pbuf, size);
	return -1;
}
EXPORT_SYMBOL(rk_vendor_read);

int rk_vendor_write(u32 id, void *pbuf, u32 size)
{
	if (_vendor_write)
		return _vendor_write(id, pbuf, size);
	return -1;
}
EXPORT_SYMBOL(rk_vendor_write);

int mac_base_on_wifi_mac_plus(unsigned char *addr, int add){
	int ret;
	unsigned char wifi_addr[6]={0};
	ret = rk_vendor_read(WIFI_MAC_ID, wifi_addr, 6);
	if (ret == 6 && is_valid_ether_addr(wifi_addr)) {
		int i = 0,plus=add,value=0;
		for(i=5;i>=0;i--){
			value = wifi_addr[i] + plus;
			if(value>0xff){
				addr[i]=value-0x100;
				plus=1;
			}else{
				addr[i]=wifi_addr[i] + plus;
				plus=0;
			}
		}
		if(is_valid_ether_addr(addr))
			return 0;
	}
	return -1;
}
#define FORCE_OVERIDE_BT_ADDR
#define BT_EQUAL_WIFI_ADD  1


#define FORCE_OVERIDE_ETH_ADDR 
#define LAN_EQUAL_WIFI_ADD  2


bool isSameAddress(unsigned char *addr1,unsigned char *addr2){
	int i = 0;
	for(i=0;i<6;i++){
		if(addr1[i]!=addr2[i])
		return false;
	}
	return true;
}
void update_mac_baseon_wifi_mac(void){
	int ret;
    unsigned char wifiaddr[6]={0};
    unsigned char btaddr[6]={0};
    unsigned char ethaddr[6]={0};
	unsigned char gen_addr[6]={0};


	ret = rk_vendor_read(WIFI_MAC_ID, wifiaddr, 6);
	if (ret != 6 || is_zero_ether_addr(wifiaddr)) {
		LOG("no valid mac address found. do nothing\n");
		return;
	}else{
		DBG("%s: rk_vendor_read wifi mac from wifi mac address: "
			"%02x:%02x:%02x:%02x:%02x:%02x\n",
			__func__, wifiaddr[0], wifiaddr[1], wifiaddr[2],
			wifiaddr[3], wifiaddr[4], wifiaddr[5]);

#ifdef FORCE_OVERIDE_BT_ADDR
		//MDC for bt mac 
		ret = rk_vendor_read(BT_MAC_ID, btaddr, 6);
		if(mac_base_on_wifi_mac_plus(gen_addr,BT_EQUAL_WIFI_ADD)==0){
			if(!isSameAddress(btaddr,gen_addr)){
				ret = rk_vendor_write(BT_MAC_ID, gen_addr, 6);
				DBG("%s: rk_vendor_read bt mac from wifi mac address: "
                    "%02x:%02x:%02x:%02x:%02x:%02x\n",
                    __func__, gen_addr[0], gen_addr[1], gen_addr[2],
                    gen_addr[3], gen_addr[4], gen_addr[5]);
				LOG("%s: rk_vendor_write override bt mac address (%d)\n",
                        __func__, ret);
			}else{
				LOG("%s: get bt mac address: "
                    "%02x:%02x:%02x:%02x:%02x:%02x\n",
                    __func__, btaddr[0], btaddr[1], btaddr[2],
                    btaddr[3], btaddr[4], btaddr[5]);
			}
		}
#endif

#ifdef FORCE_OVERIDE_ETH_ADDR
		//MDC for eth mac 
		ret = rk_vendor_read(LAN_MAC_ID, ethaddr, 6);
		if(mac_base_on_wifi_mac_plus(gen_addr,LAN_EQUAL_WIFI_ADD)==0){
			if(!isSameAddress(ethaddr,gen_addr)){
				ret = rk_vendor_write(LAN_MAC_ID, gen_addr, 6);
				DBG("%s: rk_vendor_read ethernet mac from wifi mac address: "
                    "%02x:%02x:%02x:%02x:%02x:%02x\n",
                    __func__, gen_addr[0], gen_addr[1], gen_addr[2],
                    gen_addr[3], gen_addr[4], gen_addr[5]);
				LOG("%s: rk_vendor_write override ethernet mac address (%d)\n",
                        __func__, ret);
			}else{
				LOG("%s: get thernet mac address: "
                    "%02x:%02x:%02x:%02x:%02x:%02x\n",
                    __func__, ethaddr[0], ethaddr[1], ethaddr[2],
                    ethaddr[3], ethaddr[4], ethaddr[5]);
			}
		}
#endif
	}

}

int rk_vendor_register(void *read, void *write)
{
	if (!_vendor_read) {
		_vendor_read = read;
		_vendor_write =  write;
		update_mac_baseon_wifi_mac();
		return 0;
	}
	return -1;
}
EXPORT_SYMBOL(rk_vendor_register);

bool is_rk_vendor_ready(void)
{
	if (_vendor_read && _vendor_write)
		return true;
	return false;
}
EXPORT_SYMBOL(is_rk_vendor_ready);
