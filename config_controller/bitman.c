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

int WriteNOOP(int times, FILE *OutBitfilePtr)
{
	unsigned char NOOPCommand[4] = {0x20, 0x00, 0x00, 0x00};
	int CommandSize = sizeof(NOOPCommand)/sizeof(unsigned char);
	unsigned char NextByte;
	int tmp, i, j;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(j=0; j< times; j++)
	{
		for(i=0; i<CommandSize; i++)
		{
			NextByte=NOOPCommand[i];
			tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
		}
	}

	return 0;
}
int WriteNoOfWords(int Words, bool AddFrame, FILE *OutBitfilePtr)
{
	unsigned char WriteFDRIReg[4] = {0x30, 0x00, 0x40, 0x00};
	int tmp, i, j;
	int CommandSize = sizeof(WriteFDRIReg)/sizeof(unsigned char);
	unsigned char NextByte;

	unsigned int NumberOfWords, WriteCommand;
	unsigned int AddedFrames;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}
	if (Words < 3840)
	{
		WriteCommand = 0x30004000;
		WriteCommand = WriteCommand | Words;
		for(i=3; i>=0; i--)										// # frames to write
		{
			NextByte = (char)((WriteCommand >> (8*i)) & 0x000000FF);
			fwrite(&NextByte, 1, 1, OutBitfilePtr);
		}
	}
	else
	{
		for(i=0; i<CommandSize; i++)
		{
			NextByte=WriteFDRIReg[i];
			tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
		}

		if (AddFrame)
			AddedFrames = FLength;
		else
			AddedFrames = 0;

		NumberOfWords = 0x50000000;								// packet 2 type write operation
		NumberOfWords = NumberOfWords + Words + AddedFrames;
		printf("NumberOfFrames 0x%08X\n", NumberOfWords);
		for(i=3; i>=0; i--)										// # frames to write
		{
			NextByte = (char)((NumberOfWords >> (8*i)) & 0x000000FF);
			fwrite(&NextByte, 1, 1, OutBitfilePtr);
		}
	}
	return 0;
}
int WriteFAR(int FARValue, FILE *OutBitfilePtr)
{
	unsigned char WriteFARReg[4] = {0x30, 0x00, 0x20, 0x01};
	int tmp, i, j;
	int CommandSize = sizeof(WriteFARReg)/sizeof(unsigned char);
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(i=0; i<CommandSize; i++)
	{
		NextByte=WriteFARReg[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	printf("FARValue 0x%08X\n", FARValue);
	for(i=3; i>=0; i--)										// # frames to write
	{
		NextByte = (char)((FARValue >> (8*i)) & 0x000000FF);
		fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	return 0;
}
int getFAR(int RscCl1, int RscRw1)
{
	unsigned int BA = 0, TB = 0, RA = 0, MJA = 0;
	unsigned int FARValue = 0x00000000;

	if	(strcmp(Device,"XC7A35T") == 0)
		MJA = RscCl1 + 2;
	else if ((strcmp(Device,"XC7Z010") == 0)
			  || (strcmp(Device,"XC7Z020") == 0))
		MJA = RscCl1 + 19;

	FARValue = FARValue | (BA << 23) | (TB << 22) | (RA << 17) | (MJA << 7) | MNA;
	return FARValue;
}
int WriteRemaniningHeader(FILE *OutBitfilePtr)
{
	unsigned char RemainingHeader[32] = {
		0x30, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x30, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x04, 0x00,
		0x30, 0x00, 0xA0, 0x01, 0x00, 0x00, 0x04, 0x00,
		0x30, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x01
	};
	int tmp, i;
	int RemainingHeaderSize = sizeof(RemainingHeader)/sizeof(unsigned char);
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(i=0; i<RemainingHeaderSize; i++)
	{
		NextByte=RemainingHeader[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	return 0;
}
int WriteIDCODE(FILE *OutBitfilePtr)
{
	unsigned char WriteIDCommand[4] = {0x30, 0x01, 0x80, 0x01};
	int tmp, i;
	int CommandSize = sizeof(WriteIDCommand)/sizeof(unsigned char);
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(i=0; i<CommandSize; i++)
	{
		NextByte=WriteIDCommand[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	printf("DeviceID 0x%08X\n", DeviceID);
	for(i=3; i>=0; i--)										// # frames to write
	{
		NextByte = (char)((DeviceID >> (8*i)) & 0x000000FF);
		fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	return 0;
}
int WriteCMDReg(int CMDValue, FILE *OutBitfilePtr)
{
	unsigned char WriteCMDCommand[4] = {0x30, 0x00, 0x80, 0x01};
	int tmp, i;
	int CommandSize = sizeof(WriteCMDCommand)/sizeof(unsigned char);
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(i=0; i<CommandSize; i++)
	{
		NextByte=WriteCMDCommand[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	for(i=3; i>=0; i--)										// # frames to write
	{
		NextByte = (char)((CMDValue >> (8*i)) & 0x000000FF);
		fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	return 0;
}
int WriteCTL0Reg(int CTL0Value, FILE *OutBitfilePtr)
{
	unsigned char WriteCTL0Command[4] = {0x30, 0x00, 0xA0, 0x01};
	int tmp, i;
	int CommandSize = sizeof(WriteCTL0Command)/sizeof(unsigned char);
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(i=0; i<CommandSize; i++)
	{
		NextByte=WriteCTL0Command[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	for(i=3; i>=0; i--)										// # frames to write
	{
		NextByte = (char)((CTL0Value >> (8*i)) & 0x000000FF);
		fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	return 0;
}
int WriteMASKReg(int MASKValue, FILE *OutBitfilePtr)
{
	unsigned char WriteMASKCommand[4] = {0x30, 0x00, 0xC0, 0x01};
	int tmp, i;
	int CommandSize = sizeof(WriteMASKCommand)/sizeof(unsigned char);
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("The file is not opened yet.\n");
		return(-1);
	}

	for(i=0; i<CommandSize; i++)
	{
		NextByte=WriteMASKCommand[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	for(i=3; i>=0; i--)										// # frames to write
	{
		NextByte = (char)((MASKValue >> (8*i)) & 0x000000FF);
		fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}

	return 0;
}
int WriteSYNC(FILE *OutBitfilePtr)
{
	unsigned char PartialBitfileHeader[] = {
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xBB,
		0x11, 0x22, 0x00, 0x44, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xAA, 0x99, 0x55, 0x66
	};
	int HeaderSize = sizeof(PartialBitfileHeader)/sizeof(unsigned char);
	int tmp, i;
	unsigned char NextByte;

	if(OutBitfilePtr == NULL)
	{
		printf("Unable to open output bitstream file %s.\n",PartialBitstreamFileName);
		return(-1);
	}

	for(i=0; i<HeaderSize; i++)
	{
		NextByte=PartialBitfileHeader[i];
		tmp=fwrite(&NextByte, 1, 1, OutBitfilePtr);
	}
}

int calNewFAR(int island)
{
    unsigned int BA = 0, RA = 0, MJA = 0;
    unsigned int FARValue = 0x00000000;

    RA = island + 3;    // Fixed for Trenz board
    if  (strcmp(Device,"XCZU9EG") == 0) {
        if ((RscCl1 >= 0) && (RscCl1 < NColCLB))
            MJA = RscCl1;
        else {
            printf("RscCl1 is out of range\n");
            return -1;
        }
        FARValue = FARValue | (BA << 24) | (RA << 18) | (MJA << 8) | MNA;
    }
    return FARValue;
}

int getBufferByte(int BufferIndex, int Frame, int ByteIndex)
{
     unsigned word,masked;
     word=FrameBuffer[BufferIndex][Frame][ByteIndex/4];
     masked=word&(0xFF000000>>((ByteIndex%4)*8));
     return(masked>>(24-(ByteIndex%4)*8));
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
