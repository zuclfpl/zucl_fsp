#include "devices.h"
#include "bitman.h"

extern int CLKFrames;
extern int IOBFrames;
extern int IOIFrames;
extern int CLBFrames;
extern int RAMFrames;
extern int RI_Frames;				// number of BRAM interconnect frames
extern int MFrames;
extern int DFrames;
extern int GFrames;
extern int HWords;
extern int CLBBytesIO;				// amount of Bytes for the CLB-IO-blocks within each frame
extern int CLBBytes;
extern int NFrames;
extern int FLength;			// frame length can be set by writing a device ID
extern int NColCLB;
extern int NColRAM;
extern int NULLArea;
extern int NULLArea1;
extern int NULLArea2;
extern int NULLArea3;
extern int RowFrames;
extern int RowFrames2;
extern int RAMRowOff;
extern int RAMRowFrames;
extern int NRowRsc;
extern int LutBitOffset[8][16];		// frame offset values for all LUTs Slice3,G downto Slice0,F and Bit F downto 0
extern int LutShiftUsageOffset[8][4];		// for detecting reconfigurable select generator usage; we check up to 4 bit; set to -1 if unused
extern char *ResourceString;
extern char *ResourceString2;
extern char *Device;	// device identifier
extern char *Family;	// FPGA device family

extern int BA;   				// block address
extern int TB;				// top/bottom bit
extern int RA;				// row address
extern int MJA;  				// major address
extern int MNA;  				// minor address

extern int NColNULL;
extern int NColRI;
extern int NColBFG;
extern int NColM;
extern int NColD;

extern struct DeviceParameters {
	int NFrames;
	int FLength;
	int NColCLB;
	int NColRAM;
	int NRowRsc;
	char *ResourceString;
	char *ResourceString2;
	char *Family;
	int CLKFrames;
	int	IOBFrames;
	int	IOIFrames;
	int	CLBFrames;	// CLBL frames
	int	RAMFrames;	// Block RAM frames
	int	RI_Frames;	// Block RAM interconnect frames
	int MFrames;	// CLBM frames
	int DFrames;	// DSP frames
	int GFrames;	// BUFG frames
	int HWords;	// RCLK frames
	int	CLBBytesIO;
	int	CLBBytes;
	int LutBitOffset[8][16];
	int LutShiftUsageOffset[8][4];
	int NULLArea;
	int NULLArea1;
	int NULLArea2;
	int NULLArea3;
	int RowFrames;
	int RowFrames2;
	int RAMRowOff;
	int RAMRowFrames;
};

unsigned int *inData, *outData;

int DeviceID;

int calNewFAR(int island)
{
    unsigned int BA = 0, RA = 0, MJA = 0;
    unsigned int FARValue = 0x00000000;

    RA = island + 3;    // Fixed for Trenz board
    if  (strcmp(Device,"XCZU9EG") == 0) {
        FARValue = FARValue | (BA << 24) | (RA << 18) | (MJA << 8) | MNA;
    }
    return FARValue;
}

int ByteSwap(unsigned int *inData, unsigned int *outData, int inSize)
{
    int tmp, i, j;

    for (i = 0; i < inSize; i++) {
        tmp  = 0;
        for (j = 0; j <= 3; j++)
            tmp |= ((inData[i] >> (8*j)) & 0x000000FF) << 8*(3-j);
        outData[i] = tmp;
//        printf("outData 0x%08x\n", outData[i]);
    }

    return 0;
}
#if 0
int main()
{
    int tmp, i, j;
    int inSize = sizeof(inData)/sizeof(unsigned int);
    int island = 6;

    int returnOfCheckRM;
    outData = (unsigned int *)malloc(inSize * sizeof(int));
    if (!outData) {
        printf("Failed to allocate memory for outData\n");
        return -1;
    }
    printf("Device %s\n",Device);
    for (i = 0; i < inSize; i++) {
        printf("0x%08x\n", inData[i]);
        if (inData[i] == 0x30018001) {
            getDeviceInformation(inData[i+1]);
            printf("Device %s\n", Device);
        }
//        if (inData[i] == 0x30002001) {
//            printf("old FAR value 0x%08x\n", inData[i+1]);
//            tmp = calNewFAR(island);
//            if (tmp < 0)
//                printf("Failed to cal new FAR value\n");
//            else {
//                printf("new FAR value 0x%08x\n", tmp);
//                inData[i+1] = tmp;
//            }
//        }
    }
    SetGlobalDeviceParameters(Device);
    returnOfCheckRM = checkResourceMatching("CCC", 1);
    if (returnOfCheckRM)
        printf("Resource footprint mismatched!\n");
    returnOfCheckRM = checkResourceMatching("CCC", 2);
    if (returnOfCheckRM)
        printf("Resource footprint mismatched!\n");

//    printf("#####Byte Swap#####\n");
//
//    ByteSwap(inData, outData, inSize);
//
//    for (i = 0; i < inSize; i++)
//        printf("0x%08x\n", outData[i]);

    if (outData)
        free(outData);
    return 0;
}
#endif
