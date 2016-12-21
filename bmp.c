#include <stdio.h>
#include <stdlib.h>

#pragma pack(push, 1)

typedef struct tagBITMAPFILEHEADER
{
  // Bitmap File Header Structure
  uint16_t bfType;                  //First two bytes should always be 0x42 0x4D
  uint32_t bfSize;                //Size of BMP in bytes
  uint16_t bfReserved1;           //reserved
  uint16_t bfReserved2;           //reserved
  uint32_t bfImageStart;          //the address of the pixel array
} BITMAPFILEHEADER;

#pragma pack(pop)

#pragma pack(push, 1)

typedef struct tagBITMAPINFOHEADER
{
  uint32_t biHeaderSize;
  uint32_t biWidth;
  uint32_t biHeight;
  uint16_t biColorPlanes;
  uint32_t biCompressionMethod;
  uint32_t biSizeImage;
  uint32_t biXPixelsPerMeter;
  uint32_t biYPixelsPerMeter;
  uint32_t biNumColorPalette;
  uint32_t biNumImportantColors;
} BITMAPINFOHEADER;

#pragma pack(pop)

unsigned char *getBitmapPixelArray(char *file, BITMAPINFOHEADER *bitmapInfoHeader)
{
  FILE *fp;
  BITMAPFILEHEADER bitmapFileHeader;
  unsigned char *bitmapImage;
  int imageIdx=0;
  unsigned char tempRGB;

  //open file in read binary
  fp = fopen(file, "rb");
  if(fp == NULL)
  {
    printf("Error\n");
    return -1;
  }

  //fread(void *store, size_t size, size_t count, FILE *stream)
  fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);

  if(bitmapFileHeader.bfType !=0x4D42)
  {
    fclose(fp);
    printf("Error\n");
    return -1;
  }

  //read infoheader to
  fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

  //move filepointer to the start of bitmap data
  fseek(fp, bitmapFileHeader.bfImageStart, SEEK_SET);

  //allocate memory for bitmap data
  bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    if(!bitmapImage)
    {
      fclose(fp);
      printf("Error\n");
      return -2;
    }

    fread(bitmapImage, bitmapInfoHeader->biSizeImage,1, fp);

    for(imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
    {
      tempRGB = bitmapImage[imageIdx];
      bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
      bitmapImage[imageIdx + 2] = tempRGB;
    }

    fclose(fp);
    return bitmapImage;
}

int main(int argc, char *argv[])
{
  BITMAPINFOHEADER bitmapInfoHeader;
  unsigned char *bitmap_data;

  bitmap_data = getBitmapPixelArray(argv[1], &bitmapInfoHeader);

  unsigned char bits[bitmapInfoHeader.biWidth][bitmapInfoHeader.biHeight];

  printf("\t  uint32_t biHeaderSize = %d;\n\
          uint32_t biWidth = %d;\n\
          uint32_t biHeight = %d;\n\
          uint16_t biColorPlanes = %d;\n\
          uint32_t biCompressionMethod = %d;\n\
          uint32_t biSizeImage = %d;\n\
          uint32_t biXPixelsPerMeter = %d;\n\
          uint32_t biYPixelsPerMeter = %d;\n\
          uint32_t biNumColorPalette = %d;\n\
          uint32_t biNumImportantColors = %d;\n",
          bitmapInfoHeader.biHeaderSize,
          bitmapInfoHeader.biWidth,
          bitmapInfoHeader.biHeight,
          bitmapInfoHeader.biColorPlanes,
          bitmapInfoHeader.biCompressionMethod,
          bitmapInfoHeader.biSizeImage,
          bitmapInfoHeader.biXPixelsPerMeter,
          bitmapInfoHeader.biYPixelsPerMeter,
          bitmapInfoHeader.biNumColorPalette,
          bitmapInfoHeader.biNumImportantColors);

          printf("int array[] = {");
  for(int i=0; i < bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * 3; i+=3)
  {
    printf("0x%02x%02x%02x", *(bitmap_data+i), *(bitmap_data+i+1), *(bitmap_data+i+2));
    if(i < bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * 3 - 3)
      printf(",");
    else
      printf("};");


    //printf("%d : %02x %02x %02x\n", i, *(bitmap_data+i), *(bitmap_data+i+1), *(bitmap_data+i+2));
  }
}
