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
unsigned char luminance_quantization_arr[BLOCK_LEN*BLOCK_LEN],ColorDifference_quantization_arr[BLOCK_LEN*BLOCK_LEN];
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


int width,height;
typedef struct REL_data
{
	unsigned char zero_num;
	short color_data;
}REL_coding_data;

typedef struct AC_Code_Info
{
	unsigned short HM_code_len,bits_len;
}AC_code_Len;

void RGB_To_YCrCb(unsigned char **Y, unsigned char **Cr, unsigned char **Cb, unsigned char *rgb_data, int height, int width);
void DCT(int x,int y, unsigned char **data, float **DCT_res);
void Quantize(float **data, short **quantize_res, int is_luminance);
void Build_Huffman_Table_for_All();
void Build_Huffman_Table(unsigned char *code_length, unsigned char *code_value, unsigned short *huffman_table);
void ZigZag_Scan(short **quantize_res, short *color_data_arr);
unsigned short DC_Encode(short diff_DC, int is_luminance);
unsigned short REL_Encode(short *color_data_arr, REL_coding_data *rel_data);
AC_code_Len AC_Encode(unsigned short zero_num, short color_data, unsigned char *HM_codeword, unsigned char *real_data_bitstring, int is_luminance);
void Encode_Jpeg_Header(FILE *fp);

int main()
{
		FILE *input_fp = fopen("Lenna.bmp","rb");
//		int width,height;
		if(input_fp == NULL)
		{
			perror("FAIL TO OPEN FILE");
			exit(-1);
		}

		fseek(input_fp,0x0012,SEEK_SET);

		int n;
		/*
		unsigned char bm[2];
		unsigned int size;
		n=fread(bm,2*sizeof(unsigned char),1,input_fp);

		if(n!=1)
			perror("ERROR IN fread\n");

	//	fseek(input_fp,0x0002,SEEK_SET);
		fread(&size,sizeof(unsigned int),1,input_fp);
		printf("bm=%c%c**size=%u\n",bm[0],bm[1],size);
		*/

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

    RGB_To_YCrCb(Y,Cr,Cb,rgb_data, height, width);

    //allocate and initialize the block
    float **DCT_res;
    DCT_res=(float**)malloc(BLOCK_LEN*sizeof(float*));
    for(int i=0;i<BLOCK_LEN;i++)
        DCT_res[i]=(float*)malloc(BLOCK_LEN*sizeof(float));
    //allocate and initialize the matrix for quantization value
    short **quantize_res;
        quantize_res=(short**)malloc(BLOCK_LEN*sizeof(short*));
    for(int i=0;i<BLOCK_LEN;i++)
        quantize_res[i]=(short*)malloc(BLOCK_LEN*sizeof(short));

    short *color_data_arr;
    	color_data_arr = (short*)malloc(BLOCK_LEN*BLOCK_LEN*sizeof(short));

    REL_coding_data *rel_data;
    	rel_data = (REL_coding_data*)malloc(BLOCK_LEN*BLOCK_LEN*sizeof(REL_coding_data));


    Build_Huffman_Table_for_All();
    short Y_last_DC = 0, Cr_last_DC = 0, Cb_last_DC = 0;
    FILE *output_fp = fopen("out.jpg","wb");
    Encode_Jpeg_Header(output_fp);

    for(int i=0;i<height;i+=BLOCK_LEN)
      for(int j=0;j<width;j+=BLOCK_LEN)
      {
      	unsigned short DC_code;
      	unsigned short pair_num;
      	unsigned char *HM_codeword, *real_data_bitstring;
        short diff_DC;
	int is_luminance = 1;
	HM_codeword = (unsigned char*)malloc(16*sizeof(unsigned char));
        real_data_bitstring = (unsigned char*) malloc(16*sizeof(unsigned char));
/*
// Y data transform
        DCT(i,j,Y,DCT_res);
        Quantize(DCT_res, quantize_res, is_luminance );
        ZigZag_Scan(quantize_res, color_data_arr);
        
//	for(int ii=0; ii<64; ii++)
//        	printf("i=%d %d\n", ii, color_data_arr[ii]);
        	
	diff_DC = color_data_arr[0]-y_last_DC;
        Y_last_DC = color_data_arr[0];
        DC_code = DC_Encode(diff_DC,is_luminance );  
        pair_num = REL_Encode(color_data_arr, rel_data);
  
        printf("DC_code=%u pair_num=%u\n", DC_code, pair_num);
  	//	pair_num = 1;
        for(int v=0; v<pair_num; v++)
        {
//        	printf("zero_num=%u data=%d\n", rel_data[v].zero_num,rel_data[v].color_data);
        	//initialize HM_codeword, real_data_bitstring
        	for(int k=0; k<16; k++)
        	{
        		HM_codeword[k] = NULL;
        		real_data_bitstring[k] = NULL;
        	}
        	AC_code_Len AC_code_len = AC_Encode(rel_data[v].zero_num, rel_data[v].color_data, HM_codeword, real_data_bitstring, is_luminance );
        
        
        	int jj = 0;
        	printf("FM_code=");  	
        //	while(HM_codeword[jj]!='\0')
        	while(jj < AC_code_len.HM_code_len)
           	{
        		//if(HM_codeword[jj]!=NULL)
        			printf("%x", HM_codeword[jj]);		
        			jj++;
        	}
        	printf(" bits=");
        	jj = 0;
        	while(jj < AC_code_len.bits_len)
        	{
        			printf("%x", real_data_bitstring[jj]);        		
        			jj++;
            	}
          	printf("\n");
         
        }

*/
	is_luminance = 0;


// Cr data trnasform
	DCT(i,j,Cr,DCT_res);
        Quantize(DCT_res, quantize_res, is_luminance );
        ZigZag_Scan(quantize_res, color_data_arr);

//	for(int ii=0; ii<64; ii++)
//        	printf("i=%d %d\n", ii, color_data_arr[ii]);
     
        diff_DC = color_data_arr[0] - Cr_last_DC;
        Cr_last_DC = color_data_arr[0];
        DC_code = DC_Encode(diff_DC, is_luminance);  
        pair_num = REL_Encode(color_data_arr, rel_data);
        
	printf("DC_code=%u pair_num=%u\n", DC_code, pair_num);
	for(int v=0; v<pair_num; v++)
        {
//        	printf("zero_num=%u data=%d\n", rel_data[v].zero_num,rel_data[v].color_data);
        	//initialize HM_codeword, real_data_bitstring
        	for(int k=0; k<16; k++)
        	{
        		HM_codeword[k] = NULL;
        		real_data_bitstring[k] = NULL;
        	}
        	AC_code_Len AC_code_len = AC_Encode(rel_data[v].zero_num, rel_data[v].color_data, HM_codeword, real_data_bitstring,is_luminance );
	 	int jj = 0;
        	printf("FM_code=");  	
        //	while(HM_codeword[jj]!='\0')
        	while(jj < AC_code_len.HM_code_len)
           	{
        		//if(HM_codeword[jj]!=NULL)
        			printf("%x", HM_codeword[jj]);		
        			jj++;
        	}
        	printf(" bits=");
        	jj = 0;
        	while(jj < AC_code_len.bits_len)
        	{
        			printf("%x", real_data_bitstring[jj]);        		
        			jj++;
            	}
          	printf("\n");
        
        }


/*
// Cb data transform
	DCT(i,j,Cb,DCT_res);
        Quantize(DCT_res, quantize_res, is_luminance );
        ZigZag_Scan(quantize_res, color_data_arr);

        diff_DC = color_data_arr[0] - Cb_last_DC;
        Cb_last_DC = color_data_arr[0];
        DC_code = DC_Encode(diff_DC, is_luminance );  
        pair_num = REL_Encode(color_data_arr, rel_data);
	
	printf("DC_code=%u pair_num=%u\n", DC_code, pair_num);
	for(int v=0; v<pair_num; v++)
        {
//        	printf("zero_num=%u data=%d\n", rel_data[v].zero_num,rel_data[v].color_data);
        	//initialize HM_codeword, real_data_bitstring
        	for(int k=0; k<16; k++)
        	{
        		HM_codeword[k] = NULL;
        		real_data_bitstring[k] = NULL;
        	}
        	AC_code_Len AC_code_len = AC_Encode(rel_data[v].zero_num, rel_data[v].color_data, HM_codeword, real_data_bitstring, is_luminance );

	 	int jj = 0;
        	printf("FM_code=");  	
        //	while(HM_codeword[jj]!='\0')
        	while(jj < AC_code_len.HM_code_len)
           	{
        		//if(HM_codeword[jj]!=NULL)
        			printf("%x", HM_codeword[jj]);		
        			jj++;
        	}
        	printf(" bits=");
        	jj = 0;
        	while(jj < AC_code_len.bits_len)
        	{
        			printf("%x", real_data_bitstring[jj]);        		
        			jj++;
            	}
          	printf("\n");
 
        }
*/
	free(HM_codeword);
        free(real_data_bitstring);


      }

		/*
		fseek(input_fp,0x0026,SEEK_SET);
		n=fread(&height,sizeof(height),1,input_fp);
		if(n!=1)
			perror("FREAD ERROR");
		printf("resolution=%d\n", height);
	*/
		return 0;
}


void RGB_To_YCrCb(unsigned char **Y, unsigned char **Cr, unsigned char **Cb, unsigned char *rgb_data, int height, int width)
{
		for(int i=0;i<height;i++)
				{
				for(int j=0;j<width;j++)
				{
					unsigned char b=*rgb_data++;
					unsigned char g=*rgb_data++;
					unsigned char r=*rgb_data++;
//					printf("   r=%d g=%d b=%d",r,g,b);
					Y[i][j] = 0.299*r + 0.587*g + 0.114*b ;
					Cb[i][j] = - 0.1687*r - 0.3313*g + 0.5*b + 128;
					Cr[i][j] = 0.5*r - 0.4187*g - 0.0813*b + 128;
				}
//				printf("\n");
				}
/*
	    for(int i=0;i<height;i++)
				{
					for(int j=0;j<width;j++)
					{
						printf(" %d",Y[i][j]);
					}
					printf("\n");
				}
*/
		return ;
}

void DCT(int x,int y, unsigned char **data, float **DCT_res)
{
 //   int xbound=x+BLOCK_LEN,ybound=y+BLOCK_LEN;
    float alpha_i,alpha_j;
    for(int i=0;i<BLOCK_LEN;i++)
      for(int j=0;j<BLOCK_LEN;j++)
      {
        if(i==0)  alpha_i=1/sqrt(BLOCK_LEN);
          else alpha_i=0.5;
        if(j==0)  alpha_j=1/sqrt(BLOCK_LEN);
          else alpha_j=0.5;
        float temp_res=0.0;
        for(int u=0;u<BLOCK_LEN;u++)
          for(int v=0;v<BLOCK_LEN;v++)
            {
              float temp1=(float)(2*u+1)/16.0;
              float temp2=(float)(2*v+1)/16.0;
              temp_res+=(float)data[u+x][v+y]*cos(temp1*PI*i)*cos(temp2*PI*j);
            }
        DCT_res[i][j]=alpha_i*alpha_j*temp_res;
      }
   return;
}

void Quantize(float **data, short **quantize_res, int is_luminance)
{
    unsigned short quantization_matrix[BLOCK_LEN][BLOCK_LEN];
    if(is_luminance)
        {
            for(int i=0;i<BLOCK_LEN;i++)
                for(int j=0;j<BLOCK_LEN;j++)
                    quantization_matrix[i][j]=luminance_quantization_matrix[i][j];
        }
    else
        {
            for(int i=0;i<BLOCK_LEN;i++)
                for(int j=0;j<BLOCK_LEN;j++)
                    quantization_matrix[i][j]=ColorDifference_quantization_matrix[i][j];
        }

    for(int i=0;i<BLOCK_LEN;i++)
        for(int j=0;j<BLOCK_LEN;j++)
        {
            quantize_res[i][j]=round(data[i][j]/quantization_matrix[i][j]);
        }

    printf("After Quantize:\n");
    for(int i=0;i<BLOCK_LEN;i++)
        {
        for(int j=0;j<BLOCK_LEN;j++)
            printf(" %d", quantize_res[i][j]);
         printf("\n");
        }

    return ;
}

void Build_Huffman_Table_for_All()
{
	Build_Huffman_Table(luminance_AC_code_length, luminance_AC_value, luminance_AC_huffman_table);
	
//	for(int i=0; i<10; i++)
//		printf("HM_i=%d %u\n", i, luminance_AC_huffman_table[i]);

	Build_Huffman_Table(chrominance_AC_code_length, chrominance_AC_value, chrominance_AC_huffman_table);
//	for(int i=0;i<10;i++)
//		printf("i=%d Chrominance_AC=%u\n",i,chrominance_AC_huffman_table[i]);
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

void ZigZag_Scan(short **quantize_res, short *color_data_arr)
{

	unsigned short scan_x[] = {0,0,1,2,1,0,0,1,2,3,4,3,2,1,0,0,1,2,3,4,5,6,5,4,3,2,1,0,0,1,2,3,4,
	5,6,7,7,6,5,4,3,2,1,2,3,4,5,6,7,7,6,5,4,3,4,5,6,7,7,6,5,6,7,7 };

	unsigned short scan_y[] = {0,1,0,0,1,2,3,2,1,0,0,1,2,3,4,5,4,3,2,1,0,0,1,2,3,4,5,6,7,6,5,4,3,
	2,1,0,1,2,3,4,5,6,7,7,6,5,4,3,2,3,4,5,6,7,7,6,5,4,5,6,7,7,6,7 };

	unsigned short arr_len = BLOCK_LEN*BLOCK_LEN;
	for(int i=0;i<arr_len;i++)
	{
		int x=scan_x[i];
		int y=scan_y[i];
		color_data_arr[i] = quantize_res[x][y];
	}
	return; 
}

unsigned short DC_Encode(short diff_DC, int is_luminance)
{
	unsigned short category=0;
	if(diff_DC<0) 
		diff_DC = -diff_DC;
	while(diff_DC)
	{
		diff_DC >>= 1;
		category++;
	}
	unsigned short code_word;
	if(is_luminance)
		code_word = luminance_DC_huffman_table[category];
	else
		code_word = chrominance_DC_huffman_table[category];
	return code_word;

}

unsigned short REL_Encode(short *color_data_arr, REL_coding_data *rel_data)
{
		int j=0;
		unsigned short arr_len = BLOCK_LEN*BLOCK_LEN;
		int cnt=0;
		for(int i = 1;i < arr_len; i++)
		{
			if(color_data_arr[i] != 0)
			{
				rel_data[j].zero_num = cnt;
				rel_data[j].color_data = color_data_arr[i];
				j++;
				cnt = 0;
			}
			else
				cnt++;
		}
		
		if(color_data_arr[arr_len-1] == 0)
		{
			rel_data[j].zero_num = 0;
			rel_data[j].color_data = 0;
			j++;
		}		

		return j;
}

AC_code_Len AC_Encode(unsigned short zero_num, short color_data, unsigned char *HM_codeword, unsigned char *real_data_bitstring, int is_luminance)
{
		unsigned short is_negative=0;
		if(color_data < 0)
		{
			is_negative = 1;
			color_data = -color_data;
		}
		unsigned short j=0;
		while(color_data)
		{
			unsigned char bit_value = color_data & 1;
			real_data_bitstring[j] = bit_value;
			j++;
			color_data >>= 1;
		}

		if(is_negative)
		{
			for(int i=0; i<j; i++)
			{
				real_data_bitstring[i] = 1 - real_data_bitstring[i];
			}
		}
		for(int i=0;i<j/2;i++)
			{
				unsigned char temp=real_data_bitstring[i];
				real_data_bitstring[i] = real_data_bitstring[j-i-1];
				real_data_bitstring[j-i-1] = temp;
			}

//		printf("j=%u\n", j);

		unsigned short index_in_table = zero_num*16+j;
		unsigned short HM_value;
		if(is_luminance)
			HM_value =  luminance_AC_huffman_table[index_in_table];	
		else
			HM_value = chrominance_AC_huffman_table[index_in_table];
//		printf("index_in_table=%u HM_value=%u\n", index_in_table, HM_value);
		//unsigned char temp_bit[17];
		unsigned short bit_len=0;
		if(HM_value == 0)
		{
			HM_codeword[0] = 0;
			HM_codeword[1] = 0;
			bit_len = 2;
		}
		else if(HM_value == 1)
		{
			HM_codeword[0] = 1;
			HM_codeword[1] = 0;
			bit_len = 2;			
		}
		else
		while(HM_value)
		{
			HM_codeword[bit_len] = HM_value & 1;
			bit_len++;
			HM_value >>= 1;
		}
		for(int i=0; i<bit_len/2; i++)
		{
			unsigned char temp_bit = HM_codeword[i];
			HM_codeword[i] = HM_codeword[bit_len-i-1];
			HM_codeword[bit_len-i-1] = temp_bit;
		}

		AC_code_Len code_len;
		code_len.HM_code_len = bit_len;
		code_len.bits_len = j;
/*		
		printf("bit_len=%u HM_codeword=",bit_len);
		for(int i=0; i<bit_len; i++)
		{
			printf("%x", HM_codeword[i]);
		}
		printf(" bits=");
		for(int i=0; i<j; i++)
		{
			printf("%x", real_data_bitstring[i]);
		}	
		printf("\n");
*/
		return code_len;
}

void Encode_Jpeg_Header(FILE *fp)
{
	//SOI
	write_func(0xFFD8,fp,2);

	//APPO
	write_func(0xFFE0,fp,2);
	write_func(16,fp,2);
	write_func("JFIF",fp,5);

	write_func(0x1,fp,1);
	write_func(0x1,fp,1);
	write_func(0x0,fp,1);
	write_func(0x1,fp,2);
	write_func(0x1,fp,2);
	write_func(0x0,fp,1);
	write_func(0x0,fp,1);


    //DQT
    for(int i=0;i<BLOCK_LEN;i++)
        for(int j=0;j<BLOCK_LEN;j++)
        {
            luminance_quantization_arr[i*BLOCK_LEN+j] = luminance_quantization_matrix[i][j];
            ColorDifference_quantization_arr[i*BLOCK_LEN+j] = ColorDifference_quantization_matrix[i][j];
        }
	write_func(0xFFDB, fp);		//marker = 0xFFDB
	write_func(132, fp);			//size=132
	write_func(0x0, fp);			//QTYinfo== 0:  bit 0..3: number of QT = 0 (table for Y)
									//				bit 4..7: precision of QT
									//				bit 8	: 0
	write_func(luminance_quantization_arr, fp, 64);		//YTable
	write_func(1, fp);			//QTCbinfo = 1 (quantization table for Cb,Cr)
	write_func(ColorDifference_quantization_arr, fp, 64);	//CbCrTable

	
	//SOFO
	write_func(0xFFC0, fp, 2);			//marker = 0xFFC0
	write_func(17, fp, 2);				//length = 17 for a truecolor YCbCr JPG
	write_func(0x8, fp);				//precision = 8: 8 bits/sample
	write_func(height&0xFFFF, fp, 2);	//height
	write_func(width&0xFFFF, fp, 2);	//width
	write_func(0x3, fp, 1);				//nrofcomponents = 3: We encode a truecolor JPG

	write_func(0x1, fp, 1);				//IdY = 1
	write_func(0x11, fp);				//HVY sampling factors for Y (bit 0-3 vert., 4-7 hor.)(SubSamp 1x1)
	write_func(0x0, fp, 1);				//QTY  Quantization Table number for Y = 0

	write_func(0x2, fp, 1);				//IdCb = 2
	write_func(0x11, fp, 1);				//HVCb = 0x11(SubSamp 1x1)
	write_func(0x1, fp, 1);				//QTCb = 1

	write_func(0x3, fp, 1);				//IdCr = 3
	write_func(0x11, fp, 1);				//HVCr = 0x11 (SubSamp 1x1)
	write_func(0x1, fp, 1);				//QTCr Normally equal to QTCb = 1

	//DHT
	write_func(0xFFC4, fp, 2);		// marker = 0xFFC4
	write_func(0x01A2, fp, 2);		// length = 0x01A2
	write_func(0x0, fp, 1);			// HTYDCinfo bit 0..3	: number of HT (0..3), for Y =0
									// bit 4 : type of HT, 0 = DC table,1 = AC table
									// bit 5..7	: not used, must be 0
	write_func(luminance_DC_code_length, fp, sizeof(luminance_DC_code_length));	// DC_L_NRC
	write_func(luminance_DC_value, fp, sizeof(luminance_DC_value));	// DC_L_VALUE
	write_func(0x10, fp);			// HTYACinfo
	write_func(luminance_AC_code_length, fp, sizeof(luminance_AC_code_length));
	write_func(luminance_AC_value, fp, sizeof(luminance_AC_value)); // we'll use the standard Huffman tables
	write_func(0x01, fp);			// HTCbDCinfo
	write_func(chrominance_DC_code_length, fp, sizeof(chrominance_DC_code_length));
	write_func(chrominance_DC_value, fp, sizeof(chrominance_DC_value));
	write_func(0x11, fp);			// HTCbACinfo
	write_func(chrominance_AC_code_length, fp, sizeof(chrominance_AC_code_length));
	write_func(chrominance_AC_value, fp, sizeof(chrominance_AC_value));

	//SOS
	write_func(0xFFDA, fp, 2);		//marker = 0xFFC4
	write_func(12, fp, 2);			//length = 12
	write_func(0x3, fp, 1);			//nrofcomponents, Should be 3: truecolor JPG

	write_func(1, fp, 1);			//Idy=1
	write_func(0, fp, 1);			//HTY	bits 0..3: AC table (0..3)
									//		bits 4..7: DC table (0..3)
	write_func(2, fp, 1);			//IdCb
	write_func(0x11, fp, 1);			//HTCb

	write_func(3, fp, 1);			//IdCr
	write_func(0x11, fp, 1);			//HTCr

	write_func(0, fp, 1);			//Ss not interesting, they should be 0,63,0
	write_func(0x3F, fp, 1);			//Se
	write_func(0, fp, 1);			//Bf
	
}

void write_func(FILE *fp, unsigned short k, int byte_size)
{
    //    unsigned char temp_k;
    //    if(byte_size == 1)

		fwrite(k,1,byte_size,f);	
}