#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc,char *argv[])
{
    if(argc!=4)
    {
        fprintf(stderr,"Usage: ./resize n infile outfile\n");
        return 1;
    }
    float scale=atof(argv[1]);
    char *infile=argv[2];
    char *outfile=argv[3];
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
     BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    int originalWidth=bi.biWidth;
    int originalHeight=bi.biHeight;
    bi.biWidth*=scale;
    bi.biHeight*=scale;
    
    int originalpadding=(4-(originalWidth*3)%4)%4;
    int padding=(4-(bi.biWidth*3)%4)%4;
    
    bi.biSizeImage=(bi.biWidth*3+padding)*abs(bi.biHeight);
    bf.bfSize=bf.bfOffBits+bi.biSizeImage;
    
    fwrite(&bf,sizeof(BITMAPFILEHEADER),1,outptr);
    
    fwrite(&bi,sizeof(BITMAPINFOHEADER),1,outptr);
    
    if(scale<1)
    {
        scale=1/scale;
         for (int i = 0, biHeight = abs(originalHeight); i < biHeight; i=i+scale)
    {
        // iterate over pixels in scanline
        //for(int mn = 0;mn <scale ;mn++)
        //{
         fseek(inptr,bf.bfOffBits+i*(originalWidth*3+originalpadding),SEEK_SET);   
        for (int j = 0; j <originalWidth; j=j+scale)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            fseek(inptr,(scale-1)*3,SEEK_CUR);
            //for(int m=0;m<scale;m++)
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, originalpadding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }
    
        
    }
    else{
    for (int i = 0, biHeight = abs(originalHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for(int mn = 0;mn <scale ;mn++)
        {
         fseek(inptr,bf.bfOffBits+i*(originalWidth*3+originalpadding),SEEK_SET);   
        for (int j = 0; j <originalWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            for(int m=0;m<scale;m++)
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, originalpadding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }}}
    fclose(inptr);
    fclose(outptr);
    return 0;
}