#include "devices.h"

int	CLKFrames;
int	IOBFrames;
int	IOIFrames;
int	CLBFrames;
int	RAMFrames;
int	RI_Frames;				// number of BRAM interconnect frames
int MFrames;
int DFrames;
int GFrames;
int SwFrames;
int PFrames;
int IFrames;
int mFrames;
int FFrames;
int VFrames;
int HWords;
int	CLBBytesIO;				// amount of Bytes for the CLB-IO-blocks within each frame
int	CLBBytes;
int	NFrames;
int FLength;			// frame length can be set by writing a device ID
int NColCLB;
int NColRAM;
Xilinx_Lut_Bit_Offset Lut_Bit_Offset;
int NULLArea;
int NULLArea1;
int NULLArea2;
int NULLArea3;
int RowFrames;
int RowIOPad;
int RowFrames2;
int RAMRowOff;
int RAMRowFrames;
int NRowRsc;
//int LutBitOffset[8][16];		// frame offset values for all LUTs Slice3,G downto Slice0,F and Bit F downto 0
//int LutShiftUsageOffset[8][4];		// for detecting reconfigurable select generator usage; we check up to 4 bit; set to -1 if unused
char *ResourceString;
char *ResourceString2;
extern char *Device;	// device identifier
char *Family;	// FPGA device family

int BA;   				// block address
int TB;				// top/bottom bit
int RA;				// row address
int MJA;  				// major address
int MNA;  				// minor address

int NColNULL;
int NColRI;
int NColBFG;
int NColM;
int NColD;
int NColS;
int NColP;
int NColI;
int NColm;
int NColF;
int NColV;
struct DeviceParameters {
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
	int SwFrames;	// Switch matrix frames
	int PFrames;	// Primitive frames
	int IFrames;	// IO frames
	int mFrames;	// interconnect matrix frames
	int FFrames;	// FIFO out frames
	int VFrames;	// VFrame frames
	int HWords;	// RCLK frames
	int	CLBBytesIO;
	int	CLBBytes;
	Xilinx_Lut_Bit_Offset Lut_Bit_Offset;
//	int LutBitOffset[2][8][64][2];	// 2 types M and L, 8 LUT positions from bottom to top A6-1 B6-1 C6-1 D6-1 A6-2 B6-2 C6-2 D6-2, 64 bits of value, 2 needed info: frame number and bitoffset
	//int LutBitOffset[8][16];
	//int LutShiftUsageOffset[8][4];
	int NULLArea;
	int NULLArea1;
	int NULLArea2;
	int NULLArea3;
	int RowFrames;
	int RowIOPad;
	int RowFrames2;
	int RAMRowOff;
	int RAMRowFrames;
};

extern int DeviceID;

//const Xilinx_Lut_Bit_Offset XC7V_Lut_Bit_Offset =
const Xilinx_Lut_Bit_Offset Series_7_Lut_Bit_Offset =
{
};

int getDeviceInformation(int DeviceID)
{
	switch(DeviceID)
	{
// Zynq UltraScale+ family
		case 0x0484A093:	Device="XCZU9EG";			break;
		default :		printf("Sorry, can not handle this device ID: 0x%08X.\n",DeviceID);
					Device="unknown";
	}
	printf("%s\n",Device);
	return(0);
}

int SetDeviceParameters(char *Device, struct DeviceParameters *Parameter)
{
	if	(strcmp(Device,"XCZU9EG") == 0)		{Parameter->NFrames=71260;	Parameter->FLength=93;	Parameter->NColCLB=199;	Parameter->NColRAM=13;
	Parameter->ResourceString="ImsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsLRsLMsmPmsLMsDMsLMsDMsLMsLRsLMsN";
	Parameter->NULLArea1=0;
	Parameter->NULLArea2=0;
	Parameter->NULLArea3=0;
	Parameter->NRowRsc=7;
	Parameter->RowFrames=6850;
	Parameter->RowIOPad=29;
	Parameter->RowFrames2=0;
	Parameter->RAMRowOff=47950;
	Parameter->RAMRowFrames=3330;
	}
	else	{	printf("Warning: Device '%s' not supported\n",Device);
				return(-1);
	}
	

	if(strncmp(Device,"XCZU",4)==0) {
		Parameter->Family="Zynq Ultrascale+";
		printf("found a Zynq Ultrascale+ device\n");

		Parameter->CLBFrames=16;
		Parameter->CLBBytes=6;
		Parameter->RAMFrames=256;
		Parameter->CLBBytesIO=0;
		Parameter->CLKFrames=0;
		Parameter->IOBFrames=0;
		Parameter->IOIFrames=0;
		Parameter->RI_Frames=6;
		Parameter->MFrames=16;
		Parameter->DFrames=8;
		Parameter->GFrames=0;
		Parameter->HWords=3;
		Parameter->SwFrames=76;
		Parameter->PFrames=10;
		Parameter->IFrames=9;
		Parameter->mFrames=4;
	}
	return(0);
}

int SetGlobalDeviceParameters(char *Device)
{
	struct DeviceParameters GlobalParameters;
	int returnFromSetDeviceParameters;
	returnFromSetDeviceParameters = SetDeviceParameters(Device, &GlobalParameters);

	if (returnFromSetDeviceParameters < 0)
		return (-1);
	NFrames = GlobalParameters.NFrames;
	FLength = GlobalParameters.FLength;
	NColCLB = GlobalParameters.NColCLB;
	NColRAM = GlobalParameters.NColRAM;
	NRowRsc = GlobalParameters.NRowRsc;
	ResourceString = GlobalParameters.ResourceString;
	ResourceString2= GlobalParameters.ResourceString2;
	Family = GlobalParameters.Family;
	CLKFrames = GlobalParameters.CLKFrames;
	IOBFrames = GlobalParameters.IOBFrames;
	IOIFrames = GlobalParameters.IOIFrames;
	CLBFrames = GlobalParameters.CLBFrames;
	RAMFrames = GlobalParameters.RAMFrames;
	RI_Frames = GlobalParameters.RI_Frames;
	MFrames = GlobalParameters.MFrames;
	DFrames = GlobalParameters.DFrames;
	GFrames = GlobalParameters.GFrames;
	SwFrames= GlobalParameters.SwFrames;
	PFrames = GlobalParameters.PFrames;
	IFrames = GlobalParameters.IFrames;
	mFrames = GlobalParameters.mFrames;
	FFrames = GlobalParameters.FFrames;
	VFrames = GlobalParameters.VFrames;
	HWords = GlobalParameters.HWords;
	CLBBytesIO = GlobalParameters.CLBBytesIO;
	CLBBytes = GlobalParameters.CLBBytes;
	NULLArea = GlobalParameters.NULLArea;
	NULLArea1 = GlobalParameters.NULLArea1;
	NULLArea2 = GlobalParameters.NULLArea2;
	NULLArea3 = GlobalParameters.NULLArea3;
	RowFrames = GlobalParameters.RowFrames;
	RowIOPad  = GlobalParameters.RowIOPad;
	RowFrames2 = GlobalParameters.RowFrames2;
	RAMRowOff = GlobalParameters.RAMRowOff;
	RAMRowFrames = GlobalParameters.RAMRowFrames;
	Lut_Bit_Offset = GlobalParameters.Lut_Bit_Offset;
	return(0);
}

int getFrameIndexGlobal(void)
{
	int FrameOffset = 0;
	if (BA == 0) {
		if (NColNULL == 1)
			FrameOffset = NULLArea1;
		else if (NColNULL == 2)
			FrameOffset = NULLArea1 + NULLArea2;
		else if (NColNULL == 3)
			FrameOffset = NULLArea1 + NULLArea2 + NULLArea3;

		if (RowFrames2 == 0)
			return (MNA + NColRI*RI_Frames + NColBFG*GFrames + NColM*MFrames + NColD*DFrames + NColS*SwFrames + NColP*PFrames + NColI*IFrames + NColm*mFrames + NColF*FFrames + NColV*VFrames + (MJA - (NColRI + NColBFG + NColM + NColD + NColS + NColP + NColI + NColm + NColF + NColV))*CLBFrames + FrameOffset + (TB*NRowRsc/2 + RA)*RowFrames);
		else  // be careful with this case
			return (MNA + NColRI*RI_Frames + NColBFG*GFrames + NColM*MFrames + NColD*DFrames + NColS*SwFrames + NColP*PFrames + NColI*IFrames + NColm*mFrames + NColF*FFrames + NColV*VFrames + (MJA - (NColRI + NColBFG + NColM + NColD + NColS + NColP + NColI + NColm + NColF + NColV))*CLBFrames + FrameOffset + (TB*NRowRsc/2 + RA)*RowFrames + TB*RowFrames2);
	}
	else if (BA == 1)
		return (MNA + MJA*RAMFrames + RAMRowOff + (TB*NRowRsc/2 + RA)*RAMRowFrames);

	printf("No valid base address - trying to continue.\n");
	return(-1);
}

int getFrameIndex(int BA, int MJA, int MNA, int TB_RA)
{
	int FrameOffset = 0;
	int tmp;
	int tmpNColNULL=0, tmpNColRI=0, tmpNColBFG=0, tmpNColM=0, tmpNColD=0, tmpNColS=0, tmpNColP=0, tmpNColI=0, tmpNColm=0, tmpNColF=0, tmpNColV=0;
	if (BA == 0) {
// Scan ResourceString for how many columns of different resource types
		for (tmp=0; tmp<=MJA; tmp++) {
			if (ResourceString[tmp + tmpNColNULL] == 'N')
				tmpNColNULL++;
			if (tmp < MJA) {
				if (ResourceString[tmp + tmpNColNULL] == 'R')
					tmpNColRI++;
				else if (ResourceString[tmp + tmpNColNULL] == 'B')
					tmpNColBFG++;
				else if (ResourceString[tmp + tmpNColNULL] == 'M')
					tmpNColM++;
				else if (ResourceString[tmp + tmpNColNULL] == 'D')
					tmpNColD++;
				else if (ResourceString[tmp + tmpNColNULL] == 's')
					tmpNColS++;
				else if (ResourceString[tmp + tmpNColNULL] == 'P')
					tmpNColP++;
				else if (ResourceString[tmp + tmpNColNULL] == 'I')
					tmpNColI++;
				else if (ResourceString[tmp + tmpNColNULL] == 'm')
					tmpNColm++;
				else if (ResourceString[tmp + tmpNColNULL] == 'F')
					tmpNColF++;
				else if (ResourceString[tmp + tmpNColNULL] == 'V')
					tmpNColV++;
			}
		}

		if (tmpNColNULL == 1)
			FrameOffset = NULLArea1;
		else if (tmpNColNULL == 2)
			FrameOffset = NULLArea1 + NULLArea2;
		else if (NColNULL == 3)
			FrameOffset = NULLArea1 + NULLArea2 + NULLArea3;

		//if	((strncmp(Device,"XC7Z",4)==0) && (TB_RA > 1))
		//	FrameOffset = NULLArea;
		if (RowFrames2 == 0)
			return (MNA + tmpNColRI*RI_Frames + tmpNColBFG*GFrames + tmpNColM*MFrames + tmpNColD*DFrames + tmpNColS*SwFrames + tmpNColP*PFrames + tmpNColI*IFrames + tmpNColm*mFrames + tmpNColF*FFrames + tmpNColV*VFrames + (MJA - (tmpNColRI + tmpNColBFG + tmpNColM + tmpNColD + tmpNColS + tmpNColP + tmpNColI + tmpNColm + tmpNColF + tmpNColV))*CLBFrames + FrameOffset + TB_RA*RowFrames);
		else  // be careful with this case
			// dedicated for the Artix-7 XC7A35T device
			if	(strcmp(Device,"XC7A35T") == 0) {
				if (TB_RA < 2)
					return (MNA + tmpNColRI*RI_Frames + tmpNColBFG*GFrames + tmpNColM*MFrames + tmpNColD*DFrames + tmpNColS*SwFrames + tmpNColP*PFrames + tmpNColI*IFrames + tmpNColm*mFrames + tmpNColF*FFrames + tmpNColV*VFrames + (MJA - (tmpNColRI + tmpNColBFG + tmpNColM + tmpNColD + tmpNColS + tmpNColP + tmpNColI + tmpNColm + tmpNColF + tmpNColV))*CLBFrames + FrameOffset + TB_RA*RowFrames);
				else
					return (MNA + tmpNColRI*RI_Frames + tmpNColBFG*GFrames + tmpNColM*MFrames + tmpNColD*DFrames + tmpNColS*SwFrames + tmpNColP*PFrames + tmpNColI*IFrames + tmpNColm*mFrames + tmpNColF*FFrames + tmpNColV*VFrames + (MJA - (tmpNColRI + tmpNColBFG + tmpNColM + tmpNColD + tmpNColS + tmpNColP + tmpNColI + tmpNColm + tmpNColF + tmpNColV))*CLBFrames + FrameOffset + (TB_RA - 1)*RowFrames + RowFrames2);
			}
	}
	else if (BA == 1)
		return (MNA + MJA*RAMFrames + RAMRowOff + TB_RA*RAMRowFrames);
	//printf("BA %d, MJA %d, TB_RA %d \n", BA, MJA, TB_RA);
	printf("No valid base address - trying to continue.\n");
	return(-1);
}

int incFrameAddress(void)
{
	if((BA==-1) || (MJA==-1) || (MNA==-1))
	{
		printf("Base frame address not found - cannot compute frame address.\n");
		return(-1);
	}
	else if (BA == 0)	// in the CLB, I/O, CLK, interconnects section
	{
//		printf("MJA %d NColNULL %d RSC %c \n", MJA, NColNULL, ResourceString[MJA + NColNULL]);
		if ((ResourceString[MJA + NColNULL] == 'C')
			|| (ResourceString[MJA + NColNULL] == 'L'))
			if (MNA < (CLBFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
			}
		else if (ResourceString[MJA + NColNULL] == 'R')
			if (MNA < (RI_Frames - 1))	// BRAM/DSP column has 28 frames for interconnect
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColRI++;
			}
		else if (ResourceString[MJA + NColNULL] == 'B')
			if (MNA < (GFrames - 1))	// 30 frames for unknown resources when passes clock region boundary
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColBFG++;
			}
		else if (ResourceString[MJA + NColNULL] == 'M')
			if (MNA < (MFrames - 1))	// CLBM column has 70 frames
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColM++;
			}
		else if (ResourceString[MJA + NColNULL] == 'D')
			if (MNA < (DFrames - 1))	// BRAM/DSP column has 28 frames for interconnect
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColD++;
			}
		else if (ResourceString[MJA + NColNULL] == 's')
			if (MNA < (SwFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColS++;
			}
		else if (ResourceString[MJA + NColNULL] == 'P')
			if (MNA < (PFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColP++;
			}
		else if (ResourceString[MJA + NColNULL] == 'I')
			if (MNA < (IFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColI++;
			}
		else if (ResourceString[MJA + NColNULL] == 'm')
			if (MNA < (mFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColm++;
			}
		else if (ResourceString[MJA + NColNULL] == 'F')
			if (MNA < (FFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColF++;
			}
		else if (ResourceString[MJA + NColNULL] == 'V')
			if (MNA < (VFrames - 1))
				MNA++;
			else {
				MJA++;
				MNA = 0;
				NColV++;
			}
		return 0;
	}
	else if (BA == 1)
		if (MNA < (RAMFrames - 1))
			MNA++;
		else {
			MJA++;
			MNA = 0;
		}
	else
	{
		printf("Internal error: invalid frame address\n");
		exit(13);
	}

	return 0;
}
