#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#define PI 3.1415926
#define BLOCK_LEN 8
unsigned short luminance_quantization_matrix[BLOCK_LEN][BLOCK_LEN] = {
	{16,  11,  10,  16,  24,  40,  51,  61},
	{12,  12,  14,  19,  26,  58,  60,  55},
	{14,  13,  16,  24,  40,  57,  69,  56},
	{14,  17,  22,  29,  51,  87,  80,  62},
	{18,  22,  37,  56,  68, 109, 103,  77},
	{24,  35,  55,  64,  81, 104, 113,  92},
	{49,  64,  78,  87, 103, 121, 120, 101},
	{72,  92,  95,  98, 112, 100, 103,  99}
};

unsigned short ColorDifference_quantization_matrix[BLOCK_LEN][BLOCK_LEN] = {
	{17, 18, 24,  47,  99,  99,  99,  99},
	{18, 21,  26,  66,  99,  99,  99,  99},
	{24,  26,  56,  99,  99,  99,  99,  99},
	{47,  66,  99,  99,  99,  99,  99,  99},
	{99,  99,  99,  99,  99,  99,  99,  99},
	{99,  99,  99,  99,  99,  99,  99,  99},
	{99,  99,  99,  99,  99,  99,  99,  99},
	{99,  99,  99,  99,  99,  99,  99,  99}
};

unsigned char luminance_DC_code_length[] = {0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
unsigned char luminance_DC_value[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

unsigned char chrominance_DC_code_length[] = {0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char chrominance_DC_value[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

unsigned char luminance_AC_code_length[] = {0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D};  
unsigned char luminance_AC_value[] = {
0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
0xF9, 0xFA
};

unsigned char chrominance_AC_code_length[] = {0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77};
unsigned char chrominance_AC_value[] = {
0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
0xF9, 0xFA
};

unsigned short luminance_AC_huffman_table[160], chrominance_AC_huffman_table[160],
luminance_DC_huffman_table[12], chrominance_DC_huffman_table[12];


typedef struct REL_data
{
	unsigned char zero_num;
	short color_data;
}REL_coding_data;

typedef struct AC_Code_Info
{
	unsigned short HM_code_len,bits_len;
}AC_code_Len;

int width,height;

// void RGB_To_YCrCb(unsigned char **Y, unsigned char **Cr, unsigned char **Cb, unsigned char *rgb_data);
// void DCT(int x,int y, unsigned char **data, float **DCT_res);
// void Quantize(float **data, short **quantize_res, int is_luminance);
void Build_Huffman_Table_for_All();
void Build_Huffman_Table(unsigned char *code_length, unsigned char *code_value, unsigned short *huffman_table);
// void ZigZag_Scan(short **quantize_res, short *color_data_arr);
// unsigned short DC_Encode(short diff_DC, int is_luminance);
// unsigned short REL_Encode(short *color_data_arr, REL_coding_data *rel_data);
// AC_code_Len AC_Encode(unsigned short zero_num, short color_data, unsigned char *HM_codeword, unsigned char *real_data_bitstring, int is_luminance);

int main()
{
		FILE *input_fp = fopen("Lenna.bmp","rb");
		if(input_fp == NULL)
		{
			perror("FAIL TO OPEN FILE");
			exit(-1);
		}

		fseek(input_fp,0x0012,SEEK_SET);

		int n;
		n=fread(&width,sizeof(width),1,input_fp);
		printf("width=%d\n", width);

		int temp_h;
		fseek(input_fp,0x0016,SEEK_SET);
		n=fread(&temp_h,sizeof(int),1,input_fp);
		printf("height=%d\n", temp_h);
		if(temp_h<0)
			height=-temp_h;
		else height=temp_h;

		unsigned int header_size;
		fseek(input_fp,0x000E,SEEK_SET);
		n=fread(&header_size,sizeof(header_size),1,input_fp);

		unsigned short bits_per_pixel;
		fseek(input_fp,0x001C,SEEK_SET);
		n=fread(&bits_per_pixel,sizeof(bits_per_pixel),1,input_fp);
//		printf("header_size=%d\n", header_size);

		fseek(input_fp,header_size,SEEK_SET);

    //read rgb data from original bmp file
		unsigned char *rgb_data=(unsigned char*)malloc(width*height*3*sizeof(unsigned char));
		if(temp_h>0)
		{
			for(int i=0;i<height;i++)
			{
				fread(rgb_data+(height-1-i)*width*3,3,width,input_fp);
			}
		}
        	else
        	{
           		fread(rgb_data,3,width*height,input_fp);
        	}

		width=8;
		height = 8;

		if(width % BLOCK_LEN !=0 || height % BLOCK_LEN !=0)
		{
			perror("WIDTH OR HEIGHT IS NOT MULTIPLE OF BLOCK_LEN!");
			exit(-1);
		}


		unsigned char **Y=(unsigned char**)malloc(height*sizeof(unsigned char*));
		for(int i=0;i<height;i++)
			Y[i]=(unsigned char*)malloc(width*sizeof(unsigned char));

		unsigned char **Cr=(unsigned char **)malloc(height*sizeof(unsigned char*));
		for(int i=0;i<height;i++)
			Cr[i]=(unsigned char*)malloc(width*sizeof(unsigned char));

		unsigned char **Cb=(unsigned char **)malloc(height*sizeof(unsigned char*));
		for(int i=0;i<height;i++)
			Cb[i]=(unsigned char*)malloc(width*sizeof(unsigned char));


    Build_Huffman_Table_for_All();
    short last_DC=0;

  	return 0;
}

void Build_Huffman_Table_for_All()
{
	Build_Huffman_Table(luminance_AC_code_length, luminance_AC_value, luminance_AC_huffman_table);
	
	Build_Huffman_Table(chrominance_AC_code_length, chrominance_AC_value, chrominance_AC_huffman_table);
	Build_Huffman_Table(luminance_DC_code_length, luminance_DC_value, luminance_DC_huffman_table);
	Build_Huffman_Table(chrominance_DC_code_length, chrominance_DC_value, chrominance_DC_huffman_table);
}

void Build_Huffman_Table(unsigned char *code_length, unsigned char *code_value, unsigned short *huffman_table)
{
	unsigned char pos=0;
	unsigned short code_word=0;
		for(int i=1; i<=16; i++)
		{
			for(int j=1; j<=code_length[i-1]; j++)
			{
				huffman_table[code_value[pos]] = code_word;
				pos++;
				code_word++;
			}
			code_word <<= 1;
		}
}





