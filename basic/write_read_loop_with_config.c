#include <stdio.h>
#include <LabJackM.h>
#include "../LJM_Utilities.h"
int main()
{
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
	int msDelay = 1000;
	enum { MAX_FRAMES = 2 };
	int numFrames;
	char * aNames[MAX_FRAMES];
	double aValues[MAX_FRAMES];
	err = LJM_Open(LJM_dtANY, LJM_ctANY, "LJM_idANY", &handle);
	ErrorCheck(err, "LJM_Open");
	PrintDeviceInfoFromHandle(handle);
	printf("\n");
	deviceType = GetDeviceType(handle);
	printf("Configuring...\n");
	if (deviceType == LJM_dtT4) {
		WriteNameOrDie(handle, "DIO_INHIBIT", 0xF9F);
		printf("    DIO_INHIBIT : 0x%x\n", 0xF9F);
		WriteNameOrDie(handle, "DIO_ANALOG_ENABLE", 0x000);
		printf("    DIO_ANALOG_ENABLE : 0x%x\n", 0x000);
	}
	else {
		WriteNameOrDie(handle, "AIN0_NEGATIVE_CH", 199);
		printf("    AIN0_NEGATIVE_CH : %d\n", 199);
		WriteNameOrDie(handle, "AIN0_RANGE", 10);
		printf("    AIN0_RANGE : %d\n", 10);
	}
	WriteNameOrDie(handle, "AIN0_RESOLUTION_INDEX", 0);
	printf("    AIN0_RESOLUTION_INDEX : %d\n", 0);
	WriteNameOrDie(handle, "AIN0_SETTLING_US", 0);
	printf("    AIN0_SETTLING_US : %d\n", 0);

	printf("\n");
	printf("Starting loop.  Press Ctrl+c to stop.\n\n");
	err = LJM_StartInterval(INTERVAL_HANDLE, msDelay * 1000);
	ErrorCheck(err, "LJM_StartInterval");

	while (1) {
		numFrames = 2;
		aNames[0] = "DAC0";
		if (deviceType == LJM_dtT4) {
			aNames[1] = "FIO5";
		}
		else {
			aNames[1] = "FIO1";
		}
		dacVolt = it % 6; // 0-5
		fioState = it % 2; // 0 or 1
		aValues[0] = dacVolt;
		aValues[1] = (double)fioState;
		err = LJM_eWriteNames(handle, numFrames, (const char **)aNames, aValues,
			&errorAddress);
		ErrorCheckWithAddress(err, errorAddress, "LJM_eWriteNames");
		printf("eWriteNames :");
		for (i = 0; i < numFrames; i++) {
			printf(" %s = %.4f  ", aNames[i], aValues[i]);
		}
		printf("\n");
		numFrames = 2;
		aNames[0] = "AIN0";
		if (deviceType == LJM_dtT4) {
			aNames[1] = "FIO6";
		}
		else {
			aNames[1] = "FIO2";
		}
		aValues[0] = dacVolt;
		aValues[1] = (double)fioState;
		err = LJM_eReadNames(handle, numFrames, (const char **)aNames, aValues,
			&errorAddress);
		ErrorCheckWithAddress(err, errorAddress, "LJM_eReadNames");
		printf("eReadNames  :");
		for (i = 0; i < numFrames; i++) {
			printf(" %s = %.4f  ", aNames[i], aValues[i]);
		}
		printf("\n");

		++it;

		err = LJM_WaitForNextInterval(INTERVAL_HANDLE, &skippedIntervals);
		ErrorCheck(err, "LJM_WaitForNextInterval");
		if (skippedIntervals > 0) {
			printf("SkippedIntervals: %d\n", skippedIntervals);
		}
		printf("\n");
	}

	err = LJM_CleanInterval(INTERVAL_HANDLE);
	ErrorCheck(err, "LJM_CleanInterval");

	err = LJM_Close(handle);
	ErrorCheck(err, "LJM_Close");

	WaitForUserIfWindows();

	return LJME_NOERROR;
}
