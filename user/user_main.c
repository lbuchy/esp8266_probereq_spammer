#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "espconn.h"

#define procTaskPrio        0
#define procTaskQueueLen    1

#define IEEE80211_ADDR_LEN 6

static volatile os_timer_t some_timer;

// Declare some functions that are found in the precompiled libraries
extern struct netif* eagle_lwip_getif(int id);
extern int ieee80211_send_probereq(struct netif* pIf,
	const uint8_t sa[IEEE80211_ADDR_LEN],
	const uint8_t da[IEEE80211_ADDR_LEN],
	const uint8_t bssid[IEEE80211_ADDR_LEN],
	const uint8_t* ssid, size_t ssidlen);

//Tasks that happen all the time.
static const uint8_t g_sa[IEEE80211_ADDR_LEN] = {0xde,0xad,0xbe,0xef,0xde,0xad};
static const uint8_t g_da[IEEE80211_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
static const uint8_t g_bssid[IEEE80211_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
static const uint8_t g_ssid = 0x0;
static const size_t g_ssidlen = 0;

void at_recvTask() {}

os_event_t    procTaskQueue[procTaskQueueLen];
static void ICACHE_FLASH_ATTR
procTask(os_event_t *events)
{
	//system_os_post(procTaskPrio, 0, 0 );
	if( events->sig == 0 && events->par == 0 )
	{
		//Idle Event.
	}
}
static void ICACHE_FLASH_ATTR
 myProber(void *arg)
{
	struct netif* pIf = eagle_lwip_getif(0);
	ieee80211_send_probereq(pIf,g_sa,g_da,g_bssid,&g_ssid,g_ssidlen);
}

void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	//Add a process
	uart0_sendStr("Registering idle task...\r\n");
	system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);

	//Timer example
	uart0_sendStr("Starting timer task...\r\n");
	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)myProber, NULL);
	os_timer_arm(&some_timer, 50, 1); // Do this every 50ms
 
	uart0_sendStr("Starting idle task...\r\n");
	system_os_post(procTaskPrio, 0, 0 );
}


