#include<stdio.h>
#include<stdlib.h>
#include<math.h>

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

int width,height;
void RGB_To_YCrCb(unsigned char **Y, unsigned char **Cr, unsigned char **Cb, unsigned char *rgb_data);
void DCT(int x,int y, unsigned char **data, float **DCT_res);
void Quantize(float **data, short **quantize_res, int is_luminance);
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

    RGB_To_YCrCb(Y,Cr,Cb,rgb_data);

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

    for(int i=0;i<height;i+=BLOCK_LEN)
      for(int j=0;j<width;j+=BLOCK_LEN)
      {
        DCT(i,j,Y,DCT_res);
        Quantize(DCT_res, quantize_res, 1);

        for(int i=0;i<height;i++)
            {
                for(int j=0;j<width;j++)
                {
                    printf(" %d",quantize_res[i][j]);
                }
                printf("\n");
            }

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


void RGB_To_YCrCb(unsigned char **Y, unsigned char **Cr, unsigned char **Cb, unsigned char *rgb_data)
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
		return 0;
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

void Quantize(float **data,short **quantize_res, int is_luminance)
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
    return ;
}
