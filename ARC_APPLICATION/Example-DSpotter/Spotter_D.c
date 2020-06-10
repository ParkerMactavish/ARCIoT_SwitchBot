#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include "DSpotterSDKApi.h"


#define k_nMaxTime			300
#define k_nMaxGroupSize 5
int level2RemainTime;
typedef struct
{
 HANDLE hDSpotter;
 int groupId;
 int cmdId;
 int groupList[k_nMaxGroupSize+1];
}Spotter;

static Spotter gSpotter= {0};
void Spotter_Release(void);
int Spotter_Init(BYTE *lpbyCYBase, BYTE *lppbyModel[], void* pMem, int memSize){
	int n;
	int nGroups, nCmds;
	int err;
	if (gSpotter.hDSpotter!= 0){
		Spotter_Release();
	}
	
	for(nGroups= 0, nCmds= 0; nGroups<= k_nMaxGroupSize; nGroups++){
		if(lppbyModel[nGroups]== 0) break;
		nCmds+= DSpotter_GetNumWord(lppbyModel[nGroups]);
		gSpotter.groupList[nGroups]= nCmds;
	}
	gSpotter.groupList[nGroups]= INT_MAX;
	// Check the memory usage
	n = DSpotter_GetMemoryUsage_Multi(lpbyCYBase , lppbyModel , nGroups, k_nMaxTime);
	if(n< 0){
		err= n;
		printf("get mem usage fail.\r\n");
		goto Error;
	}
	if( n > memSize) {
		printf("Spotter init fail: memory need %d\n", n);
		return -2;
	}
	
	gSpotter.hDSpotter = DSpotter_Init_Multi(lpbyCYBase , lppbyModel , nGroups, k_nMaxTime, (BYTE*)pMem , memSize , 0 , 0 , &err ) ;
	if( err != DSPOTTER_SUCCESS ) {
		goto Error;
	}
	return DSPOTTER_SUCCESS;	
Error:
	printf("DSpotter init fail:error code is %d\n", err);
	return err;
}

int Spotter_GetMemoryUsage(BYTE *lpbyCYBase, BYTE *lppbyModel[]){
	int nGroups;
	for(nGroups= 0;nGroups<= k_nMaxGroupSize; nGroups++){
		if(lppbyModel[nGroups]== 0) break;
	}	
	return DSpotter_GetMemoryUsage_Multi(lpbyCYBase , lppbyModel , nGroups, k_nMaxTime);
}

static void calResult(void){
	int cmd= DSpotter_GetResult(gSpotter.hDSpotter);
	int i;
	for(i= 0; ; i++){
		if(cmd< gSpotter.groupList[i]) {
			gSpotter.groupId= i;
			if(i> 0){
				gSpotter.cmdId= cmd- gSpotter.groupList[i- 1];
			}else{
				gSpotter.cmdId= cmd;
			}
			return;
		}
	}
}

int Spotter_AddSample(SHORT *lpsSample, INT nNumSample){
	int nRet= DSpotter_AddSample(gSpotter.hDSpotter, lpsSample, nNumSample);
	if(nRet== DSPOTTER_SUCCESS){
		calResult();
		if(gSpotter.groupId== 0){
			level2RemainTime= 5*16000; //goto level 2 (for 5s)
		}else if(level2RemainTime<= 0){
			nRet= DSPOTTER_ERR_NeedMoreSample; //false alarm
		}else{
			level2RemainTime= 0; //detect level 2 cmd, back to level 1
		}
	}else if(level2RemainTime> 0){
		level2RemainTime-= nNumSample;
		if(level2RemainTime<= 0){
			printf("Time Out !\n" );
		}
	}
	return nRet;
}


int Spotter_GetResultId(void){
	return gSpotter.cmdId;
}

int Spotter_GetResultGroupId(void){
	return gSpotter.groupId;
}

void Spotter_Reset(void){
	DSpotter_Reset(gSpotter.hDSpotter);
}

void Spotter_Release(void){
	if(!gSpotter.hDSpotter) return;
	DSpotter_Release(gSpotter.hDSpotter);
	gSpotter.hDSpotter= 0;		
}

const char* DSpotter_GetVersion(void);
const char* Spotter_GetVersion(void){
	return DSpotter_GetVersion();
}

int Spotter_VAD_Enable(int isEnabled){
	return -1; //not support
}

int Spotter_VAD_SetMinEnergyThreshd(int nThreshold){
	return -1; //not support
}

HANDLE Spotter_GetHandle(){
	return gSpotter.hDSpotter;
}