#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <LabJackM.h>
#include "../LJM_Utilities.h"
int main()
{   
	int hours, minutes, seconds, day, month, year;
	int err;
	int handle;
	int i;
	int errorAddress = INITIAL_ERR_ADDRESS;
	int skippedIntervals;
	int deviceType;
	double dacVolt;
	int fioState;
	const int INTERVAL_HANDLE = 1;
	int it = 0;
	int msDelay = 500;
	enum { MAX_FRAMES = 2 };
	int numFrames;
	char * aNames[MAX_FRAMES];
	double aValues[MAX_FRAMES];
	err = LJM_Open(LJM_dtANY, LJM_ctANY, "LJM_idANY", &handle);
	ErrorCheck(err, "LJM_Open");
	PrintDeviceInfoFromHandle(handle);
	deviceType = GetDeviceType(handle);
	WriteNameOrDie(handle, "DIO_INHIBIT", 0xF9F);
	WriteNameOrDie(handle, "DIO_ANALOG_ENABLE", 0x000);
	WriteNameOrDie(handle, "AIN0_RESOLUTION_INDEX", 0);
	WriteNameOrDie(handle, "AIN0_SETTLING_US", 0);
	err = LJM_StartInterval(INTERVAL_HANDLE, msDelay * 1000);
	ErrorCheck(err, "LJM_StartInterval");
	printf("\n");
	while (1) {
		time_t now;
		time(&now);
		struct tm* local = localtime(&now);
		hours = local->tm_hour;
		minutes = local->tm_min;
		seconds = local->tm_sec;
		day = local->tm_mday;
		month = local->tm_mon + 1;
		year = local->tm_year + 1900;

		numFrames = 3;
		aNames[0] = "AIN0";
		aNames[1] = "AIN1";
		aNames[2] = "AIN2";
		err = LJM_eReadNames(handle, numFrames, (const char **)aNames, aValues,
			&errorAddress);
		ErrorCheckWithAddress(err, errorAddress, "LJM_eReadNames");
		printf("Date is : %02d/%02d/%d ", day, month, year);
		printf("Time is : %02d:%02d:%02d am ", hours, minutes, seconds);
		printf(" %s = %.4f  ", aNames[0], aValues[0]);
		printf(" %s = %.4f  ", aNames[1], aValues[1]);
		printf(" %s = %.4f  ", aNames[2], aValues[2]);
		err = LJM_WaitForNextInterval(INTERVAL_HANDLE, &skippedIntervals);
		ErrorCheck(err, "LJM_WaitForNextInterval");
		printf("\n");
	}

	err = LJM_CleanInterval(INTERVAL_HANDLE);
	ErrorCheck(err, "LJM_CleanInterval");

	err = LJM_Close(handle);
	ErrorCheck(err, "LJM_Close");

	WaitForUserIfWindows();

	return LJME_NOERROR;
}
