#include "header_file.h"
#include<stdlib.h>
#define SWAP(num) \
{ \
	unsigned char* str = (unsigned char*) &(num); \
	for(int i=0;i<2;i++) \
	{ \
		unsigned char temp = str[i]; \
		str[i] = str[3-i]; \
		str[3-i] = temp; \
	} \
}

char Title[50]={0},Artist[50]={0},Album[50]={0},Composer[50]={0},Content[50]={0},Year[4]={0},arr[100],str[5]={0},ch;
int size=0,i,sample_size=0,Version; //These are buffers (arrays of chars) used to hold the metadata strings read from the MP3 file.
int read_tags(FILE* fptr)
{
	fread(arr,3,1,fptr);                                  //Read and Validate ID3 Tag:
	arr[3] = '\0';
	if(strcmp(arr,"ID3") != 0 )
	{   printf("\033[1;31m");
		printf("Given file name is not a MP3 file\n");
		printf("\033[0m");
	}
	fread(arr,2,1,fptr); //Read Version Information  , Reads the next 2 bytes which hold version info.
	arr[2] = '\0';
	if( arr[0] == 3 && arr[1] == 00)
	{
		Version = 2; // tell compiler crt version to perform function
	}
	else
	{
		printf("\033[1;31m");
		printf("Unsupported ID3 version\n");
		printf("\033[0m");
        exit(1);
	}

	fseek(fptr,10,SEEK_SET); //Moves the file pointer to byte 10, skipping the ID3 header


	for( i=1;i<=6;i++)      //Loop to Read 6 Tag Frames
	{
		fread(str,4,1,fptr);  //Reads 4 bytes into str which is the frame identifier
		size = 0;
		fread(&size,4,1,fptr); //Reads 4 bytes for the size of the frame data.
		SWAP(size);  //Calls SWAP(size) to convert the size from big endian to little endian.

		fseek(fptr,3,SEEK_CUR);           //Skips 3 bytes which are flags

		if( strncmp(str,"TIT2",4) == 0 )
		{
			fread(Title,size-1,1,fptr);  //read only size-1 bytes of actual string content — excluding the terminating byte.
			Title[size-1] = '\0';
		}
		else if( strncmp(str,"TYER",4) == 0 ) //this 4  ensures accurate and safe comparison without assuming null-termination
		{
			fread(Year,size-1,1,fptr);
			Year[size-1] = '\0';
		}
		else if( strncmp(str,"TALB",4) == 0 )
		{
			fread(Album,size-1,1,fptr);
			Album[size-1] = '\0';
		}
		else if( strncmp(str,"TPE1",4) == 0 )
		{
			fread(Artist,size-1,1,fptr);
			Artist[size-1] = '\0';
		}
		else if( strncmp(str,"TCON",4) == 0 )
		{
			fread(Content,size-1,1,fptr);
			Content[size-1] = '\0';
		}
		else if( strncmp(str,"COMM",4) == 0 )
		{
			fread(Composer,size-1,1,fptr);
			Composer[size-1] = '\0';
		}
	}
	printf("........................................\n");
	printf("MP3 Tag Reader and Editor for ID3 v%d\n",Version);
	printf("........................................\n");
	printf("Title\t :\t %s\nArtist\t :\t %s\nAlbum\t :\t %s\nYear\t :\t %s\nGenre\t :\t %s\nComposer :\t %s\n",Title,Artist,Album,Year,Content,Composer);
	return 0;
}

int copy_remain(FILE* src,FILE* des, char* usr_str,char* usr_tag)
{
// src: File pointer opened for writing
// des: File pointer opened for reading (original MP3 file).
// usr_str: New string value to write for the specified tag.
// usr_tag: The 4-character ID3 tag frame identifier to be updated (e.g., "TIT2" for Title).
	fread(arr,10,1,des); //Copy ID3 Header (First 10 bytes)
	fwrite(arr,10,1,src);
	for(i=1;i<=6;i++) //loop run upto read of 6 tag
	{
		fread(str,4,1,des); //Reads the 4-byte frame identifier
		fwrite(str,4,1,src);

		//Check if current frame is the one to update

		if( strncmp(str,usr_tag,4) == 0 ) //str contains the 4-byte ID3 frame identifier read from the source file.
		{                                 //usr_tag is the tag user wants to update.
           size = strlen(usr_str);          //usr_str is the new string for the tag.
			int temp_size = size;
			size++;  //add one extra byte 
			
			SWAP(size); //conversion big endian to littel endian
			fwrite(&size,4,1,src); //Writing new frame size to destination file

			fread(&sample_size,4,1,des); //Read original frame size from source file to move offset forward

			fread(arr,3,1,des);
			fwrite(arr,3,1,src);//Read and write 3 bytes of frame flags

			fwrite(usr_str,temp_size,1,src);   // Write new tag content string to destination file

			SWAP(sample_size);


			fseek(des,sample_size-1,SEEK_CUR); //jump to next frame

		}
		else
		{

			fread(&sample_size,4,1,des); // Read and Write Frame Size
			fwrite(&sample_size,4,1,src);

			SWAP(sample_size);

			fread(arr,3,1,des); //Read and Write Frame Flags (3 bytes)
			fwrite(arr,3,1,src);


			fread(arr,sample_size-1,1,des);
			arr[sample_size-1] = '\0';
			fwrite(arr,sample_size - 1,1,src);

		}

	}
	while ( fread(&ch,1,1,des) > 0 )
	{
		fwrite(&ch,1,1,src);
	}

	return 0;


}

int check_id3_version(FILE* fptr)
{
    unsigned char arr[3] = {0};

    fseek(fptr, 0, SEEK_SET);
    fread(arr, 3, 1, fptr); // Read first 3 bytes ("ID3")

    if (strncmp((char*)arr, "ID3", 3) != 0)
    {
        printf("\033[1;31m");
        printf("Error: File does not contain a valid ID3 tag header.\n");
        printf("\033[0m");
        return 0; // Not valid
    }

    unsigned char ver[2] = {0};
    fread(ver, 2, 1, fptr); // Read version bytes

    if (ver[0] == 3 && ver[1] == 0) // ID3 v2.3.0
    {
        return 1;
    }
    else
    {
        printf("\033[1;31m");
        printf("Unsupported ID3 version for editing. Only ID3v2.3.0 is supported.\n");
        printf("\033[0m");
        return 0;
    }
}
