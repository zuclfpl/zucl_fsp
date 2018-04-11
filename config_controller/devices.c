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

//const Xilinx_Lut_Bit_Offset XC7V_Lut_Bit_Offset =
const Xilinx_Lut_Bit_Offset Series_7_Lut_Bit_Offset =
{
	{
		// CLBM
		{
			// SLICEM LUT A6-1 : lut_pos 0
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{33, 16}, {34, 16}, {33, 17}, {34, 17}, {33, 18}, {34, 18}, {33, 19}, {34, 19},
				{31, 16}, {32, 16}, {31, 17}, {32, 17}, {31, 18}, {32, 18}, {31, 19}, {32, 19},
				{33, 20}, {34, 20}, {33, 21}, {34, 21}, {33, 22}, {34, 22}, {33, 23}, {34, 23},
				{31, 20}, {32, 20}, {31, 21}, {32, 21}, {31, 22}, {32, 22}, {31, 23}, {32, 23},
				{33, 24}, {34, 24}, {33, 25}, {34, 25}, {33, 26}, {34, 26}, {33, 27}, {34, 27},
				{31, 24}, {32, 24}, {31, 25}, {32, 25}, {31, 26}, {32, 26}, {31, 27}, {32, 27},
				{33, 28}, {34, 28}, {33, 29}, {34, 29}, {33, 30}, {34, 30}, {33, 31}, {34, 31},
				{31, 28}, {32, 28}, {31, 29}, {32, 29}, {31, 30}, {32, 30}, {31, 31}, {32, 31}
			},
			// SLICEM LUT B6-1 : lut_pos 1
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{33, 0}, {34, 0}, {33, 1}, {34, 1}, {33, 2}, {34, 2}, {33, 3}, {34, 3},
				{31, 0}, {32, 0}, {31, 1}, {32, 1}, {31, 2}, {32, 2}, {31, 3}, {32, 3},
				{33, 4}, {34, 4}, {33, 5}, {34, 5}, {33, 6}, {34, 6}, {33, 7}, {34, 7},
				{31, 4}, {32, 4}, {31, 5}, {32, 5}, {31, 6}, {32, 6}, {31, 7}, {32, 7},
				{33, 8}, {34, 8}, {33, 9}, {34, 9}, {33, 10}, {34, 10}, {33, 11}, {34, 11},
				{31, 8}, {32, 8}, {31, 9}, {32, 9}, {31, 10}, {32, 10}, {31, 11}, {32, 11},
				{33, 12}, {34, 12}, {33, 13}, {34, 13}, {33, 14}, {34, 14}, {33, 15}, {34, 15},
				{31, 12}, {32, 12}, {31, 13}, {32, 13}, {31, 14}, {32, 14}, {31, 15}, {32, 15}
			},
			// SLICEM LUT C6-1 : lut_pos 2
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{33, 48}, {34, 48}, {33, 49}, {34, 49}, {33, 50}, {34, 50}, {33, 51}, {34, 51},
				{31, 48}, {32, 48}, {31, 49}, {32, 49}, {31, 50}, {32, 50}, {31, 51}, {32, 51},
				{33, 52}, {34, 52}, {33, 53}, {34, 53}, {33, 54}, {34, 54}, {33, 55}, {34, 55},
				{31, 52}, {32, 52}, {31, 53}, {32, 53}, {31, 54}, {32, 54}, {31, 55}, {32, 55},
				{33, 56}, {34, 56}, {33, 57}, {34, 57}, {33, 58}, {34, 58}, {33, 59}, {34, 59},
				{31, 56}, {32, 56}, {31, 57}, {32, 57}, {31, 58}, {32, 58}, {31, 59}, {32, 59},
				{33, 60}, {34, 60}, {33, 61}, {34, 61}, {33, 62}, {34, 62}, {33, 63}, {34, 63},
				{31, 60}, {32, 60}, {31, 61}, {32, 61}, {31, 62}, {32, 62}, {31, 63}, {32, 63}
			},
			// SLICEM LUT D6-1 : lut_pos 3
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{33, 32}, {34, 32}, {33, 33}, {34, 33}, {33, 34}, {34, 34}, {33, 35}, {34, 35},
				{31, 32}, {32, 32}, {31, 33}, {32, 33}, {31, 34}, {32, 34}, {31, 35}, {32, 35},
				{33, 36}, {34, 36}, {33, 37}, {34, 37}, {33, 38}, {34, 38}, {33, 39}, {34, 39},
				{31, 36}, {32, 36}, {31, 37}, {32, 37}, {31, 38}, {32, 38}, {31, 39}, {32, 39},
				{33, 40}, {34, 40}, {33, 41}, {34, 41}, {33, 42}, {34, 42}, {33, 43}, {34, 43},
				{31, 40}, {32, 40}, {31, 41}, {32, 41}, {31, 42}, {32, 42}, {31, 43}, {32, 43},
				{33, 44}, {34, 44}, {33, 45}, {34, 45}, {33, 46}, {34, 46}, {33, 47}, {34, 47},
				{31, 44}, {32, 44}, {31, 45}, {32, 45}, {31, 46}, {32, 46}, {31, 47}, {32, 47}
			},
			// SLICEL LUT A6-2 : lut_pos 4
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 16}, {26, 16}, {25, 17}, {26, 17}, {25, 18}, {26, 18}, {25, 19}, {26, 19},
				{28, 16}, {27, 16}, {28, 17}, {27, 17}, {28, 18}, {27, 18}, {28, 19}, {27, 19},
				{25, 20}, {26, 20}, {25, 21}, {26, 21}, {25, 22}, {26, 22}, {25, 23}, {26, 23},
				{28, 20}, {27, 20}, {28, 21}, {27, 21}, {28, 22}, {27, 22}, {28, 23}, {27, 23},
				{25, 24}, {26, 24}, {25, 25}, {26, 25}, {25, 26}, {26, 26}, {25, 27}, {26, 27},
				{28, 24}, {27, 24}, {28, 25}, {27, 25}, {28, 26}, {27, 26}, {28, 27}, {27, 27},
				{25, 28}, {26, 28}, {25, 29}, {26, 29}, {25, 30}, {26, 30}, {25, 31}, {26, 31},
				{28, 28}, {27, 28}, {28, 29}, {27, 29}, {28, 30}, {27, 30}, {28, 31}, {27, 31}
			},
			// SLICEL LUT B6-2 : lut_pos 5
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 0}, {26, 0}, {25, 1}, {26, 1}, {25, 2}, {26, 2}, {25, 3}, {26, 3},
				{28, 0}, {27, 0}, {28, 1}, {27, 1}, {28, 2}, {27, 2}, {28, 3}, {27, 3},
				{25, 4}, {26, 4}, {25, 5}, {26, 5}, {25, 6}, {26, 6}, {25, 6}, {26, 7},
				{28, 4}, {27, 4}, {28, 5}, {27, 5}, {28, 6}, {27, 6}, {28, 7}, {27, 7},
				{25, 8}, {26, 8}, {25, 9}, {26, 9}, {25, 10}, {26, 10}, {25, 11}, {26, 11},
				{28, 8}, {27, 8}, {28, 9}, {27, 9}, {28, 10}, {27, 10}, {28, 11}, {27, 11},
				{25, 12}, {26, 12}, {25, 13}, {26, 13}, {25, 14}, {26, 14}, {25, 15}, {26, 15},
				{28, 12}, {27, 12}, {28, 13}, {27, 13}, {28, 14}, {27, 14}, {28, 15}, {27, 15}
			},
			// SLICEL LUT C6-2 : lut_pos 6
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 48}, {26, 48}, {25, 49}, {26, 49}, {25, 50}, {26, 60}, {25, 51}, {26, 51},
				{28, 48}, {27, 48}, {28, 49}, {27, 49}, {28, 50}, {27, 50}, {28, 51}, {27, 51},
				{25, 52}, {26, 52}, {25, 53}, {26, 53}, {25, 54}, {26, 54}, {25, 55}, {26, 55},
				{28, 52}, {27, 52}, {28, 53}, {27, 53}, {28, 54}, {27, 54}, {28, 55}, {27, 55},
				{25, 56}, {26, 56}, {25, 57}, {26, 57}, {25, 58}, {26, 58}, {25, 59}, {26, 59},
				{28, 56}, {27, 56}, {28, 57}, {27, 57}, {28, 58}, {27, 58}, {28, 59}, {27, 59},
				{25, 60}, {26, 60}, {25, 61}, {26, 61}, {25, 62}, {26, 62}, {25, 63}, {26, 63},
				{28, 60}, {27, 60}, {28, 61}, {27, 61}, {28, 62}, {27, 62}, {28, 63}, {27, 63}
			},
			// SLICEL LUT D6-2 : lut_pos 7
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 32}, {26, 32}, {25, 33}, {26, 33}, {25, 34}, {26, 34}, {25, 35}, {26, 35},
				{28, 32}, {27, 32}, {28, 33}, {27, 33}, {28, 34}, {27, 34}, {28, 35}, {27, 35},
				{25, 36}, {26, 36}, {25, 37}, {26, 37}, {25, 38}, {26, 38}, {25, 39}, {26, 39},
				{28, 36}, {27, 36}, {28, 37}, {27, 37}, {28, 38}, {27, 38}, {28, 39}, {27, 39},
				{25, 40}, {26, 40}, {25, 41}, {26, 41}, {25, 42}, {26, 42}, {25, 43}, {26, 43},
				{28, 40}, {27, 40}, {28, 41}, {27, 41}, {28, 42}, {27, 42}, {28, 43}, {27, 43},
				{25, 44}, {26, 44}, {25, 45}, {26, 45}, {25, 46}, {26, 46}, {25, 47}, {26, 47},
				{28, 44}, {27, 44}, {28, 45}, {27, 45}, {28, 46}, {27, 46}, {28, 47}, {27, 47}
			}
		},
		// CLBL
		{
			// SLICEL LUT A6-1 : lut_pos 0
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{31, 16}, {32, 16}, {31, 17}, {32, 17}, {31, 18}, {32, 18}, {31, 19}, {32, 19},
				{34, 16}, {33, 16}, {34, 17}, {33, 17}, {34, 18}, {33, 18}, {34, 19}, {33, 19},
				{31, 20}, {32, 20}, {31, 21}, {32, 21}, {31, 22}, {32, 22}, {31, 23}, {32, 23},
				{34, 20}, {33, 20}, {34, 21}, {33, 21}, {34, 22}, {33, 22}, {34, 23}, {33, 23},
				{31, 24}, {32, 24}, {31, 25}, {32, 25}, {31, 26}, {32, 26}, {31, 27}, {32, 27},
				{34, 24}, {33, 24}, {34, 25}, {33, 25}, {34, 26}, {33, 26}, {34, 27}, {33, 27},
				{31, 28}, {32, 28}, {31, 29}, {32, 29}, {31, 30}, {32, 30}, {31, 31}, {32, 31},
				{34, 28}, {33, 28}, {34, 29}, {33, 29}, {34, 30}, {33, 30}, {34, 31}, {33, 31}
			},
			// SLICEL LUT B6-1 : lut_pos 1
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{31, 0}, {32, 0}, {31, 1}, {32, 1}, {31, 2}, {32, 2}, {31, 3}, {32, 3},
				{34, 0}, {33, 0}, {34, 1}, {33, 1}, {34, 2}, {33, 2}, {34, 3}, {33, 3},
				{31, 4}, {32, 4}, {31, 5}, {32, 5}, {31, 6}, {32, 6}, {31, 7}, {32, 7},
				{34, 4}, {33, 4}, {34, 5}, {33, 5}, {34, 6}, {33, 6}, {34, 7}, {33, 7},
				{31, 8}, {32, 8}, {31, 9}, {32, 9}, {31, 10}, {32, 10}, {31, 11}, {32, 11},
				{34, 8}, {33, 8}, {34, 9}, {33, 9}, {34, 10}, {33, 10}, {34, 11}, {33, 11},
				{31, 12}, {32, 12}, {31, 13}, {32, 13}, {31, 14}, {32, 14}, {31, 15}, {32, 15},
				{34, 12}, {33, 12}, {34, 13}, {33, 13}, {34, 14}, {33, 14}, {34, 15}, {33, 15}
			},
			// SLICEL LUT C6-1 : lut_pos 2
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{31, 48}, {32, 48}, {31, 49}, {32, 49}, {31, 50}, {32, 50}, {31, 51}, {32, 51},
				{34, 48}, {33, 48}, {34, 49}, {33, 49}, {34, 50}, {33, 50}, {34, 51}, {33, 51},
				{31, 52}, {32, 52}, {31, 53}, {32, 53}, {31, 54}, {32, 54}, {31, 55}, {32, 55},
				{34, 52}, {33, 52}, {34, 53}, {33, 53}, {34, 54}, {33, 54}, {34, 55}, {33, 55},
				{31, 56}, {32, 56}, {31, 57}, {32, 57}, {31, 58}, {32, 58}, {31, 59}, {32, 59},
				{34, 56}, {33, 56}, {34, 57}, {33, 57}, {34, 58}, {33, 58}, {34, 59}, {33, 59},
				{31, 60}, {32, 60}, {31, 61}, {32, 61}, {31, 62}, {32, 62}, {31, 63}, {32, 63},
				{34, 60}, {33, 60}, {34, 61}, {33, 61}, {34, 62}, {33, 62}, {34, 63}, {33, 63}
			},
			// SLICEL LUT D6-1 : lut_pos 3
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{31, 32}, {32, 32}, {31, 33}, {32, 33}, {31, 34}, {32, 34}, {31, 35}, {32, 35},
				{34, 32}, {33, 32}, {34, 33}, {33, 33}, {34, 34}, {33, 34}, {34, 35}, {33, 35},
				{31, 36}, {32, 36}, {31, 37}, {32, 37}, {31, 38}, {32, 38}, {31, 39}, {32, 39},
				{34, 36}, {33, 36}, {34, 37}, {33, 37}, {34, 38}, {33, 38}, {34, 39}, {33, 39},
				{31, 40}, {32, 40}, {31, 41}, {32, 41}, {31, 42}, {32, 42}, {31, 43}, {32, 43},
				{34, 40}, {33, 40}, {34, 41}, {33, 41}, {34, 42}, {33, 42}, {34, 43}, {33, 43},
				{31, 44}, {32, 44}, {31, 45}, {32, 45}, {31, 46}, {32, 46}, {31, 47}, {32, 47},
				{34, 44}, {33, 44}, {34, 45}, {33, 45}, {34, 46}, {33, 46}, {34, 47}, {33, 47}
			},
			// SLICEL LUT A6-2 : lut_pos 4
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 16}, {26, 16}, {25, 17}, {26, 17}, {25, 18}, {26, 18}, {25, 19}, {26, 19},
				{28, 16}, {27, 16}, {28, 17}, {27, 17}, {28, 18}, {27, 18}, {28, 19}, {27, 19},
				{25, 20}, {26, 20}, {25, 21}, {26, 21}, {25, 22}, {26, 22}, {25, 23}, {26, 23},
				{28, 20}, {27, 20}, {28, 21}, {27, 21}, {28, 22}, {27, 22}, {28, 23}, {27, 23},
				{25, 24}, {26, 24}, {25, 25}, {26, 25}, {25, 26}, {26, 26}, {25, 27}, {26, 27},
				{28, 24}, {27, 24}, {28, 25}, {27, 25}, {28, 26}, {27, 26}, {28, 27}, {27, 27},
				{25, 28}, {26, 28}, {25, 29}, {26, 29}, {25, 30}, {26, 30}, {25, 31}, {26, 31},
				{28, 28}, {27, 28}, {28, 29}, {27, 29}, {28, 30}, {27, 30}, {28, 31}, {27, 31}
			},
			// SLICEL LUT B6-2 : lut_pos 5
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 0}, {26, 0}, {25, 1}, {26, 1}, {25, 2}, {26, 2}, {25, 3}, {26, 3},
				{28, 0}, {27, 0}, {28, 1}, {27, 1}, {28, 2}, {27, 2}, {28, 3}, {27, 3},
				{25, 4}, {26, 4}, {25, 5}, {26, 5}, {25, 6}, {26, 6}, {25, 6}, {26, 7},
				{28, 4}, {27, 4}, {28, 5}, {27, 5}, {28, 6}, {27, 6}, {28, 7}, {27, 7},
				{25, 8}, {26, 8}, {25, 9}, {26, 9}, {25, 10}, {26, 10}, {25, 11}, {26, 11},
				{28, 8}, {27, 8}, {28, 9}, {27, 9}, {28, 10}, {27, 10}, {28, 11}, {27, 11},
				{25, 12}, {26, 12}, {25, 13}, {26, 13}, {25, 14}, {26, 14}, {25, 15}, {26, 15},
				{28, 12}, {27, 12}, {28, 13}, {27, 13}, {28, 14}, {27, 14}, {28, 15}, {27, 15}
			},
			// SLICEL LUT C6-2 : lut_pos 6
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 48}, {26, 48}, {25, 49}, {26, 49}, {25, 50}, {26, 60}, {25, 51}, {26, 51},
				{28, 48}, {27, 48}, {28, 49}, {27, 49}, {28, 50}, {27, 50}, {28, 51}, {27, 51},
				{25, 52}, {26, 52}, {25, 53}, {26, 53}, {25, 54}, {26, 54}, {25, 55}, {26, 55},
				{28, 52}, {27, 52}, {28, 53}, {27, 53}, {28, 54}, {27, 54}, {28, 55}, {27, 55},
				{25, 56}, {26, 56}, {25, 57}, {26, 57}, {25, 58}, {26, 58}, {25, 59}, {26, 59},
				{28, 56}, {27, 56}, {28, 57}, {27, 57}, {28, 58}, {27, 58}, {28, 59}, {27, 59},
				{25, 60}, {26, 60}, {25, 61}, {26, 61}, {25, 62}, {26, 62}, {25, 63}, {26, 63},
				{28, 60}, {27, 60}, {28, 61}, {27, 61}, {28, 62}, {27, 62}, {28, 63}, {27, 63}
			},
			// SLICEL LUT D6-2 : lut_pos 7
			{
				// LSB to MSB (0->63): {frame number, bit offset}
				{25, 32}, {26, 32}, {25, 33}, {26, 33}, {25, 34}, {26, 34}, {25, 35}, {26, 35},
				{28, 32}, {27, 32}, {28, 33}, {27, 33}, {28, 34}, {27, 34}, {28, 35}, {27, 35},
				{25, 36}, {26, 36}, {25, 37}, {26, 37}, {25, 38}, {26, 38}, {25, 39}, {26, 39},
				{28, 36}, {27, 36}, {28, 37}, {27, 37}, {28, 38}, {27, 38}, {28, 39}, {27, 39},
				{25, 40}, {26, 40}, {25, 41}, {26, 41}, {25, 42}, {26, 42}, {25, 43}, {26, 43},
				{28, 40}, {27, 40}, {28, 41}, {27, 41}, {28, 42}, {27, 42}, {28, 43}, {27, 43},
				{25, 44}, {26, 44}, {25, 45}, {26, 45}, {25, 46}, {26, 46}, {25, 47}, {26, 47},
				{28, 44}, {27, 44}, {28, 45}, {27, 45}, {28, 46}, {27, 46}, {28, 47}, {27, 47}
			}
		}
	}
};
int getDeviceInformation(int DeviceID)
{
	switch(DeviceID)
	{
		case 0x01008093:	Device="XC2V40";			break;
		case 0x01010093:	Device="XC2V80";			break;
		case 0x01018093:	Device="XC2V250";			break;
		case 0x01020093:	Device="XC2V500";			break;
		case 0x01028093:	Device="XC2V1000";			break;
		case 0x01030093:	Device="XC2V1500";			break;
		case 0x01038093:	Device="XC2V2000";			break;
		case 0x01040093:	Device="XC2V3000";			break;
		case 0x01050093:	Device="XC2V4000";			break;
		case 0x01060093:	Device="XC2V6000";			break;
		case 0x01070093:	Device="XC2V8000";			break;
		
		case 0x01226093:	Device="XC2VP2";			break;
		case 0x0123E093:	Device="XC2VP4";			break;
		case 0x0124A093:	Device="XC2VP7";			break;
		case 0x01266093:	Device="XC2VP20";			break;
		case 0x0127E093:	Device="XC2VP30";			break;
		case 0x01292093:	Device="XC2VP40";			break;
		case 0x0129E093:	Device="XC2VP50";			break;
		case 0x012BA093:	Device="XC2VP70";			break;
		case 0x012D6093:	Device="XC2VP100";			break;

		case 0x0140D093:	Device="XC3S50";			break;
		case 0x01414093:	Device="XC3S200";			break;
		case 0x0141C093:	Device="XC3S400";			break;
		case 0x11428093:	Device="XC3S1000";			break;
		case 0x01434093:	Device="XC3S1500";			break;
		case 0x01440093:	Device="XC3S2000";			break;
		case 0x01448093:	Device="XC3S4000";			break;
		case 0x01450093:	Device="XC3S5000";			break;

		case 0x01C10093:	Device="XC3S100E";			break;
		case 0x01C1A093:	Device="XC3S250E";			break;
		case 0x01C22093:	Device="XC3S500E";			break;
		case 0x01C2E093:	Device="XC3S1200E";			break;
		case 0x01C3A093:	Device="XC3S1600E";			break;

		case 0x04250093:	Device="XC6VLX240T";		break;
// Artix-7 family
		case 0x0362D093:	Device="XC7A35T";			break;
		case 0x0362C093:	Device="XC7A50T";			break;
		case 0x03622093:	Device="XC7A75T";			break;
		case 0x03631093:	Device="XC7A100T";			break;
		case 0x03626093:	Device="XC7A200T";			break;
// Kintex-7 family
		case 0x03647093:	Device="XC7K70T";			break;
		case 0x0364C093:	Device="XC7K160T";			break;
		case 0x03651093:	Device="XC7K325T";			break;
		case 0x03747093:	Device="XC7K355T";			break;
		case 0x03656093:	Device="XC7K410T";			break;
		case 0x03752093:	Device="XC7K420T";			break;
		case 0x03751093:	Device="XC7K480T";			break;
// Virtex-7 family
		case 0x03671093:	Device="XC7V585T";			break;
		case 0x03667093:	Device="XC7VX330T";			break;
		case 0x03682093:	Device="XC7VX415T";			break;
		case 0x03687093:	Device="XC7VX485T";			break;
		case 0x03692093:	Device="XC7VX550T";			break;
		case 0x03691093:	Device="XC7VX690T";			break;
		case 0x03696093:	Device="XC7VX980T";			break;
		case 0x036D5093:	Device="XC7VX1140T";			break;
		case 0x036D9093:	Device="XC7VH580T";			break;
		case 0x036DB093:	Device="XC7VH870T";			break;
// Zynq-7000 family
		case 0x03722093:	Device="XC7Z010";			break;
		case 0x03727093:	Device="XC7Z020";			break;
// Kintex UltraScale family
		case 0x03824093:	Device="KU025";				break;
		case 0x03823093:	Device="KU035";				break;
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
	if		(strcmp(Device,"XC2V40") == 0)		{Parameter->NFrames=404;	Parameter->FLength=26;	Parameter->NColCLB=8;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCRCC";}
	else if	(strcmp(Device,"XC2V80") == 0)		{Parameter->NFrames=404;	Parameter->FLength=46;	Parameter->NColCLB=8;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCRCC";}
	else if	(strcmp(Device,"XC2V250") == 0)		{Parameter->NFrames=752;	Parameter->FLength=66;	Parameter->NColCLB=16;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCRCCCCRCCCCRCC";}
	else if	(strcmp(Device,"XC2V500") == 0)		{Parameter->NFrames=928;	Parameter->FLength=86;	Parameter->NColCLB=24;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCCCRCCCCRCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V1000") == 0)	{Parameter->NFrames=1104;	Parameter->FLength=106;	Parameter->NColCLB=32;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V1500") == 0)	{Parameter->NFrames=1280;	Parameter->FLength=126;	Parameter->NColCLB=40;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V2000") == 0)	{Parameter->NFrames=1456;	Parameter->FLength=146;	Parameter->NColCLB=48;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V3000") == 0)	{Parameter->NFrames=1804;	Parameter->FLength=166;	Parameter->NColCLB=56;	Parameter->NColRAM=6;	Parameter->ResourceString="CCRCCCCCCCCCCCCRCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCRCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V4000") == 0)	{Parameter->NFrames=2156;	Parameter->FLength=206;	Parameter->NColCLB=72;	Parameter->NColRAM=6;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V6000") == 0)	{Parameter->NFrames=2508;	Parameter->FLength=246;	Parameter->NColCLB=88;	Parameter->NColRAM=6;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC2V8000") == 0)	{Parameter->NFrames=2860;	Parameter->FLength=286;	Parameter->NColCLB=104;	Parameter->NColRAM=6;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCCCCCRCCCCRCCCCCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCCCCCRCC";}

	else if	(strcmp(Device,"XC2VP2") == 0)		{Parameter->NFrames=884;	Parameter->FLength=46;	Parameter->NColCLB=22;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP4") == 0)		{Parameter->NFrames=884;	Parameter->FLength=106;	Parameter->NColCLB=22;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP7") == 0)		{Parameter->NFrames=1320;	Parameter->FLength=106;	Parameter->NColCLB=34;	Parameter->NColRAM=6;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP20") == 0)		{Parameter->NFrames=1756;	Parameter->FLength=146;	Parameter->NColCLB=46;	Parameter->NColRAM=8;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VPX20") == 0)	{Parameter->NFrames=1756;	Parameter->FLength=146;	Parameter->NColCLB=46;	Parameter->NColRAM=8;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP30") == 0)		{Parameter->NFrames=1756;	Parameter->FLength=206;	Parameter->NColCLB=46;	Parameter->NColRAM=8;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP40") == 0)		{Parameter->NFrames=2192;	Parameter->FLength=226;	Parameter->NColCLB=58;	Parameter->NColRAM=10;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP50") == 0)		{Parameter->NFrames=2628;	Parameter->FLength=226;	Parameter->NColCLB=70;	Parameter->NColRAM=12;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP70") == 0)		{Parameter->NFrames=3064;	Parameter->FLength=266;	Parameter->NColCLB=82;	Parameter->NColRAM=14;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VPX70") == 0)	{Parameter->NFrames=3064;	Parameter->FLength=266;	Parameter->NColCLB=82;	Parameter->NColRAM=14;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}
	else if	(strcmp(Device,"XC2VP100") == 0)	{Parameter->NFrames=3500;	Parameter->FLength=306;	Parameter->NColCLB=94;	Parameter->NColRAM=16;	Parameter->ResourceString="CCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCCCCCCRCC";}

	else if	(strcmp(Device,"XC3S50") == 0)		{Parameter->NFrames=368;	Parameter->FLength=37;	Parameter->NColCLB=12;	Parameter->NColRAM=1;	Parameter->ResourceString="CCRCCCCCCCCCC";}
	else if	(strcmp(Device,"XC3S200") == 0)		{Parameter->NFrames=615;	Parameter->FLength=53;	Parameter->NColCLB=20;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC3S400") == 0)		{Parameter->NFrames=767;	Parameter->FLength=69;	Parameter->NColCLB=28;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC3S1000") == 0)	{Parameter->NFrames=995;	Parameter->FLength=101;	Parameter->NColCLB=40;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC3S1500") == 0)	{Parameter->NFrames=1223;	Parameter->FLength=137;	Parameter->NColCLB=52;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC3S2000") == 0)	{Parameter->NFrames=1451;	Parameter->FLength=165;	Parameter->NColCLB=64;	Parameter->NColRAM=2;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC3S4000") == 0)	{Parameter->NFrames=1793;	Parameter->FLength=197;	Parameter->NColCLB=72;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCCCRCC";}
	else if	(strcmp(Device,"XC3S5000") == 0)	{Parameter->NFrames=1945;	Parameter->FLength=213;	Parameter->NColCLB=80;	Parameter->NColRAM=4;	Parameter->ResourceString="CCRCCCCCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCCCCCCCCCRCCCCCCCCCCCCCCCCCCCCCCCCRCC";}

	else if	(strcmp(Device,"XC3S100E") == 0)	{Parameter->NFrames=368;	Parameter->FLength=196;	Parameter->NColCLB=16;	Parameter->NColRAM=0;	Parameter->ResourceString="CCCCCCCCCCCCCCCC";}
	else if	(strcmp(Device,"XC3S250E") == 0)	{Parameter->NFrames=577;	Parameter->FLength=292;	Parameter->NColCLB=26;	Parameter->NColRAM=0;	Parameter->ResourceString="CCCCCCCCCCCCCCCCCCCCCCCCCC";}
	else if	(strcmp(Device,"XC3S500E") == 0)	{Parameter->NFrames=729;	Parameter->FLength=388;	Parameter->NColCLB=34;	Parameter->NColRAM=0;	Parameter->ResourceString="CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";}
	else if	(strcmp(Device,"XC3S1200E") == 0)	{Parameter->NFrames=958;	Parameter->FLength=500;	Parameter->NColCLB=46;	Parameter->NColRAM=0;	Parameter->ResourceString="CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";}
	else if	(strcmp(Device,"XC3S1600E") == 0)	{Parameter->NFrames=1186;	Parameter->FLength=628;	Parameter->NColCLB=58;	Parameter->NColRAM=0;	Parameter->ResourceString="CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";}

	else if	(strcmp(Device,"XC6VLX240T") == 0)	{Parameter->NFrames=28488;	Parameter->FLength=81;	Parameter->NColCLB=80;	Parameter->NColRAM=8;	Parameter->ResourceString="NMLMLRMMDMMMMDMMRMMMMMMMMRMMDMMMMDMMRMLMLNMLMLRMMDMMMMDMMRMMMMMMMMRMMDMMMMDMMRMLML";
	Parameter->NULLArea1=44;
	Parameter->NULLArea2=630;
	Parameter->NRowRsc=6;
	Parameter->RowFrames=3594;
	Parameter->RowIOPad=48;
//	Parameter->ResourceString2="NCCCCRCCRCCCCRCCRCCCCRCCRCCCCCCCBCCRCCCCCCCCCCCCCRCCCCCRCCRCCCCRCCRCCCC";
//	Parameter->NULLArea=44;
	}
	else if	(strcmp(Device,"XC7A35T") == 0)		{Parameter->NFrames=5420;	Parameter->FLength=101;	Parameter->NColCLB=32;	Parameter->NColRAM=3;
	Parameter->ResourceString="NLMLMRMMDMMNLMLMBLMLMLMRLMMDMMRLMLM";
	Parameter->NULLArea1=73;
	Parameter->NULLArea2=246;	//282;
	//Parameter->NULLArea3=124;
	Parameter->NRowRsc=3;
	Parameter->RowFrames=1534;
	Parameter->RowIOPad=72;
	Parameter->RowFrames2=1322;
	}
	else if	(strcmp(Device,"XC7Z010") == 0)		{Parameter->NFrames=5152;	Parameter->FLength=101;	
	Parameter->NColCLB=28;
//	Parameter->NColCLB=35;
	Parameter->NColRAM=3;
//	Parameter->ResourceString="NCCCRCCDCCNCCCCBCCRCCCDCCRCCCC";
	Parameter->ResourceString="NMLMRMMDMMNLMLMBLMRLMMDMMRLMLM";
//	Parameter->ResourceString="NMLMRMMDMMMMMMMMFLMLMBLMRLMMDMMRLMLM";
	Parameter->NULLArea1=653;
	Parameter->NULLArea2=246;
	//Parameter->NULLArea2=0;
	Parameter->NRowRsc=2;
	Parameter->RowFrames=1934;
	Parameter->RowIOPad=73;
	Parameter->RowFrames2=0;
	Parameter->RAMRowOff=4124;
	Parameter->RAMRowFrames=642;}
	else if	(strcmp(Device,"XC7Z020") == 0)		{Parameter->NFrames=10008;	Parameter->FLength=101;	Parameter->NColCLB=70;	Parameter->NColRAM=6;	
	Parameter->ResourceString="IFMMMMRMMDMMMMDMMRLMLMRMMDMMLMLMLBMLRMLMLMLMLLLLLLLMLMLRMMDMMMMDMMRLMLM";
	Parameter->NULLArea1=0;
	Parameter->NULLArea2=0;
	Parameter->NRowRsc=3;
	Parameter->RowFrames=2566;
	Parameter->RowIOPad=73;
	Parameter->RowFrames2=0;
	Parameter->RAMRowOff=7698;
	Parameter->RAMRowFrames=770;
	}
	else if	(strcmp(Device,"XC7VX690T") == 0)		{Parameter->NFrames=71120;	Parameter->FLength=101;	Parameter->NColCLB=151;	Parameter->NColRAM=15;
	Parameter->ResourceString="ILMLMRMMDMMRLMLMMMLRMMDMMLMMDMMRLMPFMMDMMRMMDMMMMDMMRLLLLLLLVLMLMRMMDMMBMMDMMRMMDMMMMDMMRMMDMMMMDMMRMMDMMMMDMMRMMDMMFPLMRMMDMMLMMDMMRMLMLMLMRMMDMMRLMLM";
	Parameter->NULLArea1=0;
	Parameter->NULLArea2=0;
	Parameter->NULLArea3=0;
	Parameter->NRowRsc=10;
	Parameter->RowFrames=5190;
	Parameter->RowIOPad=33;
	Parameter->RowFrames2=0;
	Parameter->RAMRowOff=51900;
	Parameter->RAMRowFrames=1922;
	}
	else if	(strcmp(Device,"KU025") == 0)		{Parameter->NFrames=32530;	Parameter->FLength=123;	Parameter->NColCLB=130;	Parameter->NColRAM=10;
	Parameter->ResourceString="NLMLRLMDMLMLRLMDMLMLMLMDMLMLRLMDBMLMLRLMDMLMLRLMDMLMLMLMDMLMLRLMDBNLMLMDMLMLMDMLRLMDMLMLMDMLMLMDMLRLMDBMLMDMLMDMLMLRLLNLMLRLMLMLMLMLM";
	Parameter->NULLArea1=84;
	Parameter->NULLArea2=84;
	Parameter->NULLArea3=124;
	Parameter->NRowRsc=3;
	Parameter->RowFrames=5224;
	Parameter->RowIOPad=48;
	}
	else if	(strcmp(Device,"XCZU9EG") == 0)		{Parameter->NFrames=71260;	Parameter->FLength=93;	Parameter->NColCLB=199;	Parameter->NColRAM=13;
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
	
	if(strncmp(Device,"XC2V",4)==0)
	{
		Parameter->Family="Virtex2";

		printf("found a VirtexII or a VirtexIIPro device\n");
		Parameter->CLKFrames=4;
		Parameter->IOBFrames=4;
		Parameter->IOIFrames=22;
		Parameter->CLBFrames=22;
		Parameter->RAMFrames=64;
		Parameter->RI_Frames=22;
		Parameter->CLBBytesIO=12;
		Parameter->CLBBytes=10;
	}
	if(strncmp(Device,"XC2VP",5)==0)
		Parameter->Family="Virtex2Pro";

	if(strncmp(Device,"XC3S",4)==0)
	{
		Parameter->Family="Spartan3";
//		if(verbose>=2)
			printf("found a Spartan3 device\n");
		Parameter->CLKFrames=3;
		Parameter->IOBFrames=2;
		Parameter->IOIFrames=19;
		Parameter->CLBFrames=19;
		Parameter->RAMFrames=76;
		Parameter->RI_Frames=19;
		Parameter->CLBBytesIO=10;
		Parameter->CLBBytes=8;
	}

	if(strncmp(Device,"XC6V",4)==0) {
		Parameter->Family="Virtex 6";
		printf("found a Virtex 6 device\n");

		Parameter->CLBFrames=36;
		Parameter->CLBBytes=8;
		Parameter->RAMFrames=128;
		Parameter->CLBBytesIO=0;
		Parameter->CLKFrames=0;
		Parameter->IOBFrames=0;
		Parameter->IOIFrames=0;
		Parameter->RI_Frames=28;
		Parameter->MFrames=36;
		Parameter->DFrames=28;
		Parameter->GFrames=30;
		Parameter->HWords=1;
	}

	if(strncmp(Device,"XC7A",4)==0) {
		Parameter->Family="Artix 7";
		printf("found a Artix 7 device\n");

		Parameter->CLBFrames=36;
		Parameter->CLBBytes=8;
		Parameter->RAMFrames=128;
		Parameter->CLBBytesIO=0;
		Parameter->CLKFrames=0;
		Parameter->IOBFrames=0;
		Parameter->IOIFrames=0;
		Parameter->RI_Frames=28;
		Parameter->MFrames=36;
		Parameter->DFrames=28;
		Parameter->GFrames=30;
		Parameter->HWords=1;
	}

	if(strncmp(Device,"XC7Z",4)==0) {
		Parameter->Family="Zynq 7000";
		printf("found a Zynq 7000 device\n");

		Parameter->CLBFrames=36;
		Parameter->CLBBytes=8;
		Parameter->RAMFrames=128;
		Parameter->CLBBytesIO=0;
		Parameter->CLKFrames=0;
		Parameter->IOBFrames=0;
		Parameter->IOIFrames=0;
		Parameter->RI_Frames=28;
		Parameter->MFrames=36;
		Parameter->DFrames=28;
		Parameter->GFrames=30;
		Parameter->HWords=1;
//		Parameter->PFrames=42;
		Parameter->IFrames=43;
		Parameter->FFrames=30;
//		Parameter->VFrames=30;
		Parameter->Lut_Bit_Offset = Series_7_Lut_Bit_Offset;
	}

	if(strncmp(Device,"XC7V",4)==0) {
		Parameter->Family="Virtex 7";
		printf("found a Virtex 7 device\n");

		Parameter->CLBFrames=36;
		Parameter->CLBBytes=8;
		Parameter->RAMFrames=128;
		Parameter->CLBBytesIO=0;
		Parameter->CLKFrames=0;
		Parameter->IOBFrames=0;
		Parameter->IOIFrames=0;
		Parameter->RI_Frames=28;
		Parameter->MFrames=36;
		Parameter->DFrames=28;
		Parameter->GFrames=30;
		Parameter->HWords=1;
		Parameter->PFrames=42;
		Parameter->IFrames=33;
		Parameter->FFrames=30;
		Parameter->VFrames=30;
		Parameter->Lut_Bit_Offset = Series_7_Lut_Bit_Offset;
	}

	if(strncmp(Device,"KU0",3)==0) {
		Parameter->Family="Kintex Ultrascale";
		printf("found a Kintex Ultrascale device\n");

		Parameter->CLBFrames=12;
		Parameter->CLBBytes=8;
		Parameter->RAMFrames=128;
		Parameter->CLBBytesIO=0;
		Parameter->CLKFrames=0;
		Parameter->IOBFrames=0;
		Parameter->IOIFrames=0;
		Parameter->RI_Frames=62;
		Parameter->MFrames=70;
		Parameter->DFrames=4;
		Parameter->GFrames=2;
		Parameter->HWords=3;
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
	//for(int i=0; i<8; i++)
	//	for(int j=0; j<16; j++)
	//		LutBitOffset[i][j] = GlobalParameters.LutBitOffset[i][j];
	//for(int i=0; i<8; i++)
	//	for(int j=0; j<4; j++)
	//		LutShiftUsageOffset[i][j] = GlobalParameters.LutShiftUsageOffset[i][j];
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

		//if	((strncmp(Device,"XC7Z",4)==0) && (RA > 0))
		//	FrameOffset = NULLArea;
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
int getROWByteIndex(int CLB_ROWindex)
{
	if(CLB_ROWindex==0)
		return(0);
	else
		return(CLBBytesIO + (CLB_ROWindex-1)*CLBBytes);
}
