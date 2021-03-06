#include <cs50.h>
#include <stdlib.h>
#include <stdio.h>

#include "bmp.h"
int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        fprintf(stderr,"Usage: ./whodunit infile outfile\n");
        return 1;
    }
    char *infile=argv[1];
    char *outfile=argv[2];
    FILE *inptr=fopen(infile,"r");
    if(inptr==NULL)
    {
        fprintf(stderr,"Input file %s not available for reading\n",infile);
        return 2;
    }
    FILE *outptr=fopen(outfile,"w");
    if(outptr==NULL)
    {
        fprintf(stderr,"Output file %s can not be opened for writing",outfile);
        fclose(inptr);
        return 3;
    }
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    RGBTRIPLE white;
    white.rgbtBlue=0xff;white.rgbtGreen=0xff;white.rgbtRed=0xff;
    RGBTRIPLE red;
    red.rgbtBlue=0x00;red.rgbtGreen=0x00;red.rgbtRed=0xff;
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            if(triple.rgbtBlue==0x00 && triple.rgbtGreen==0x00 && triple.rgbtRed==0xff)
            {
                fwrite(&white,sizeof(RGBTRIPLE), 1, outptr);
            }
            else if(triple.rgbtBlue==0xff && triple.rgbtGreen==0xff && triple.rgbtRed==0xff)
            {
                
              fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
                
            }
            else
            {
                fwrite(&red ,sizeof(RGBTRIPLE),1,outptr);
            }
           /* if(!(triple.rgbtBlue==0xff && triple.rgbtGreen==0xff && triple.rgbtRed==0xff))
            {
                fwrite(&red,sizeof(RGBTRIPLE),1,outptr);
            }
            else
            {
                fwrite(&triple,sizeof(RGBTRIPLE),1,outptr);
            }*/
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}