#include "Bitmap.h"
/* Codigo implementado por Henrique Ferrolho */
/* Apenas utilizei a funcao loadBitmap(const char* filename), alterando ligeiramente a forma como e armazenada a imagem */
/* Todas as outras funcoes foram implementadas por nos (exceto delete_bitmap) */

Bitmap* loadBitmap(const char* filename) {
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd = !1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    short* bitmapImage = (short*) malloc(bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread((char *)bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}

void drawBitmap(Bitmap* bmp, int x, int y,short *buffer){
	int pos,i;
	int width = bmp->bitmapInfoHeader.width;
	int height = bmp->bitmapInfoHeader.height;

	if (bmp == NULL)
		return;

	if (x + width < 0 || x > 1024 || y + height < 0 || y > 768)
		return;

	short* bufferPos; 
	short* bmpPos;

	for (i = 0; i < height; i++)
	{
		pos = y + height - 1;
		bufferPos = (short*) buffer + (pos-i) * HRES + x;
		bmpPos = bmp->bitmapData + i*width; 
		memcpy((char*) bufferPos, (char*) bmpPos, width*2);
	}
}


void drawBitmap_green(Bitmap* bmp, int x, int y,short *buffer){
	int width = bmp->bitmapInfoHeader.width;
	int height = bmp->bitmapInfoHeader.height;

	if (bmp == NULL)
		return;

	short *bmp_map = bmp->bitmapData;
	int i,j,pos;

	for (i=height-1;i>=0;i--){
		pos=y+height-i-1;
		for (j=0;j<width;j++){
			if (bmp_map[j+i*width] != rgb565(0,255,0) )
				buffer[(x+j)+pos*HRES] = bmp_map[j+i*width];		
		}
	}
}

int isball_aboveplayer(short buffer,int shoot){
if(shoot == 2 || (buffer != 17149 && buffer != -3773 && buffer != 0 && buffer != -435))
	return 1;
return 0;
}

int drawBitmap_ball(Bitmap* bmp, int x, int y,short *buffer,int shoot){
	int width = bmp->bitmapInfoHeader.width;
	int height = bmp->bitmapInfoHeader.height;
	int collision=0;

	if (bmp == NULL)
		return;

	short *bmp_map = bmp->bitmapData;
	int i,j,pos;

	for (i=height-1;i>=0;i--){
		pos=y+height-i-1;
		for (j=0;j<width;j++){
			if (bmp_map[j+i*width] != rgb565(0,255,0) ){
				if( isball_aboveplayer(buffer[(x+j)+pos*HRES],shoot) ){
					if((buffer[(x+j)+pos*HRES] == 0) && shoot==2)
						collision = 1;
					buffer[(x+j)+pos*HRES] = bmp_map[j+i*width];					
				}
			}
		}
	}
return collision;
}

void drawBitmap_area(Bitmap* bmp, int x, int y,short *buffer){
	int width = bmp->bitmapInfoHeader.width;
	int height = bmp->bitmapInfoHeader.height;

	if (bmp == NULL)
		return;

	short *bmp_map = bmp->bitmapData;
	int i,j,pos;

	for (i=height-1;i>=0;i--){
		pos=y+height-i-1;
		for (j=0;j<width;j++){
			if (bmp_map[j+i*width] != rgb565(0,255,0) ){
				if( buffer[(x+j)+pos*HRES] != -16905 )
					buffer[(x+j)+pos*HRES] = bmp_map[j+i*width];
			}
		}
	}
}

void drawBitmap_number(Bitmap* bmp,const char *str, int number,short *buffer)
{
	if (str == NULL || number < 0 || bmp == NULL)
		return;

	int positions[10] = { 0, 17, 29, 44, 60, 76, 92, 108, 124, 140};
	int widths[10] = { 16, 12, 14, 14, 14, 14, 14, 14, 16, 14 };
	int height = bmp->bitmapInfoHeader.height;
	int width = bmp->bitmapInfoHeader.width;
	short *bmp_map = bmp->bitmapData;
	int i, j, pos, x, y=23,n,k,kk;

	if (number >= 10)
		k=1;
	else
		k=0;
	kk=k;
	while (k != -1){
		switch(k){
			case 0:
				if(kk)
					x+=14;
				else{
					if (!strcmp(str, "user"))
						x= 490;
					else
						x= 522;
				}					
				n = number%10;
				break;
			case 1:
				if (!strcmp(str, "user"))
					x=476;
				else
					x=522;
				n = number/10;
				break;
		}	
	for (i=height-1;i>=0;i--){
		pos=y+height-i-1;
		for (j=positions[n];j<positions[n] + widths[n];j++)
				buffer[(x+j-positions[n])+pos*HRES] = bmp_map[j+i*width];		
	}
	k--;
	}
}


void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}
