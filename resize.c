/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage // 3+1 arguments needed here done
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy n infile outfile\n");
        return 1;
    }

    // remember filenames  
    int  f=atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
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

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER in_bf;
    fread(&in_bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER in_bi;
    fread(&in_bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (in_bf.bfType != 0x4d42 || in_bf.bfOffBits != 54 || in_bi.biSize != 40 || 
        in_bi.biBitCount != 24 || in_bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

        // size, header, padding calculation

    BITMAPFILEHEADER out_bf;
    BITMAPINFOHEADER out_bi;
    out_bf=in_bf;
    out_bi=in_bi;
    out_bi.biWidth*=f;
    out_bi.biHeight*=f;


     // determine padding for scanlines WHAT ABOUT THIS
    int in_padding = (4 - (in_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding = (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // out_bi.biSizeImage measuring, out_bf.bfSize measuring seems legit
    
    out_bi.biSizeImage= out_bi.biWidth*abs(out_bi.biHeight)*3 + abs(out_bi.biHeight)*out_padding;
    out_bf.bfSize= out_bi.biSizeImage + out_bf.bfOffBits;
    
    
    // write outfile's BITMAPFILEHEADER   // edit needed
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER     // edit neeeded
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

   

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(in_bi.biHeight); i < biHeight; i++)
    {
        // n times each line
        for (int n=0;n<f;n++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < in_bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
    
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    
                    // HERE HAVE TO DO THE RESIZING
                    
                // write RGB triple to outfile f times 
                for (int z =0;z<f;z++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }    
            }

            // skip over padding, if any
            fseek(inptr, in_padding, SEEK_CUR);

            // then add it back to aoutput  // IS THIS OKAY LIKE THAT ?
            for (int k = 0; k < out_padding; k++)
                {
                    fputc(0x00, outptr);
                }
                // shift cursor back with width +paddig steps
            fseek(inptr, -(in_bi.biWidth * 3 + in_padding ), SEEK_CUR);
        }
        //shitft cur forward width + pdding steps
        fseek(inptr,in_bi.biWidth * 3 + in_padding , SEEK_CUR);
        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
