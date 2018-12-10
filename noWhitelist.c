//NoWhitelist
//Disable PSM Whitelist  

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/kernel/sysmem.h>
#include <psp2kern/kernel/cpu.h>
#include <taihen.h>

static int hook = -1;
static tai_hook_ref_t ref_hook;

static int ksceRegMgrGetKeyInt_patched(const char* category, const char* name, int* buf) {
	int ret = TAI_CONTINUE(int,ref_hook,category,name,buf);
	if(strcmp(name,"launch_check") == 0)
	{
		//ksceDebugPrintf("Spoofing Launch Check\n",category,name,*buf,ret);
		*buf = 1; 
		return 0x00;
	}
	return ret;
}	
void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	hook = taiHookFunctionExportForKernel(KERNEL_PID,
		&ref_hook, 
		"SceRegistryMgr",
		TAI_ANY_LIBRARY,
		0x16DDF3DC, //ksceRegMgrGetKeyInt
		ksceRegMgrGetKeyInt_patched);
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	if (hook >= 0) taiHookReleaseForKernel(hook, ref_hook);   
	return SCE_KERNEL_STOP_SUCCESS;
}
