#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    
    fseek(fptr, 0, SEEK_END);

    uint size = ftell(fptr);

    rewind(fptr);

    return size;
    // Find the size of secret file data
    // return secret file size
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // step1 -> check argv[2] having .bmp or not
         // no -> return e_failure
         // yes -> store argv[2] into encInfo -> src_image_fname = agrv[2]
         
         
    char *dot;


    dot = strrchr(argv[2], '.');

    if(dot == NULL || strcmp(dot, ".bmp") != 0)
    {
        printf("ERROR: Source image must be BMP\n");
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];

    // step2 -> check argv[3] having . or not
        // no -> return e_failure
        // yes -> store argv[3] into encInfo ->secret_fname = argv[3]
    dot = strrchr(argv[3], '.');

    if(dot == NULL)
    {
        printf("ERROR: Secret file extension missing\n");
        return e_failure;
    }

    encInfo->secret_fname = argv[3];

    strcpy(encInfo->extn_secret_file, dot);    

    // step3 -> check argv[4] is NULL or not
        // yes -> store default name encInfo -> stego_image_fname = "stego.bmp"
     if(argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        dot = strrchr(argv[4], '.');

        if(dot == NULL || strcmp(dot, ".bmp") != 0)
        {
            printf("ERROR: Output file must be BMP\n");
            return e_failure;
        }

        encInfo->stego_image_fname = argv[4];
    }

    return e_success;    
        
        // no ->check argv[4] is having .bmp or not
                //no -> return e_failure
                //yes -> store argv[4] into encInfo -> stego_image_fname = argv[4]

    // return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        printf("ERROR: Unable to open %s\n",
                encInfo->src_image_fname);
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        
        printf("ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    
       fprintf(stderr, "ERROR: Unable to open file %s\n",
        encInfo->stego_image_fname);
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
     encInfo->image_capacity =
        get_image_size_for_bmp(
            encInfo->fptr_src_image);

    encInfo->size_secret_file =
        get_file_size(
            encInfo->fptr_secret);

    int required_size =(5 * 8) + 32 + (strlen(encInfo->extn_secret_file) * 8) +  32 + (encInfo->size_secret_file * 8);

    if(encInfo->image_capacity > required_size)
    {
        return e_success;
    }

    return e_failure;
    // step1 -> encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
  
    // step2 -> encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret)

    // step3 -> check encInfo -> image_capacity > 16+32+32+32+(encInfo -> size_secret_file * 8)
                //yes -> return e_success
                //no -> return e_failure
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //step1 -> rewind file pointer
    char header[54];

    rewind(fptr_src_image);

    //step2 -> read 54 bytes from src file
    fread(header, 54, 1, fptr_src_image);

    //step3 -> write 54 bytes into dest file
    fwrite(header, 54, 1, fptr_dest_image);

    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // char buffer[8];
    //step1 -> read 8 bytes from src file
     char buffer[8];

    for(int i = 0; i < strlen(magic_string); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i], buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;

    //step2 -> call encode_byte_to_lsb(magic_string[0], buffer)

    //step3 -> write the buffer into dest file

    //step4 -> repeat this for size of magic_string time

}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // char buffer[32];
    //step1 -> read 32 bytes from src file
     char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(size, buffer);

    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;

    //step2 -> call encode_size_to_lsb(size, buffer)

    //step3 -> write the buffer into dest file
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // char buffer[8];
    //step1 -> read 8 bytes from src file
     char buffer[8];

    for(int i = 0; i < strlen(file_extn); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(file_extn[i], buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;

    //step2 -> call encode_byte_to_lsb(magic_string[0], buffer)

    //step3 -> write the buffer into dest file

    //step4 -> repeat this for size of extn time
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // char buffer[32];
    //step1 -> read 32 bytes from src file
    char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(file_size, buffer);

    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;

    //step2 -> call encode_size_to_lsb(size, buffer)

    //step3 -> write the buffer into dest file   
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // char buffer[8];
    //step1 -> read 8 bytes from src file
    char ch;
    char buffer[8];

    while(fread(&ch, 1, 1, encInfo->fptr_secret) > 0)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

    //step2 -> call encode_byte_to_lsb(magic_string[0], buffer)

    //step3 -> write the buffer into dest file

    //step4 -> repeat this for size of secret_data time


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    //logic to copy reamaining data
        char ch;

    while(fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;


    // return e_success
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // write logic encode one char
        for(int i = 0; i < 8; i++)
    {
        image_buffer[i] &= 0xFE;
        image_buffer[i] |= ((data >> (7 - i)) & 1);
    }

    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // write logic encode size
    for(int i = 0; i < 32; i++)
    {
        imageBuffer[i] &= 0xFE;
        imageBuffer[i] |= ((size >> (31 - i)) & 1);
    }

    return e_success;

}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
    {
        printf("Error: Unable to open files\n");
        return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->src_image_fname);
    printf("INFO: Opened %s\n", encInfo->secret_fname);
    printf("INFO: Opened %s\n", encInfo->stego_image_fname); 
    
    printf("INFO: Checking capacity\n");
    if(check_capacity(encInfo) == e_failure)
    {
        printf("Error: Image does not have enough capacity\n");
        return e_failure;
    }
    printf("Capacity check passed\n");

    if(copy_bmp_header(encInfo->fptr_src_image,
                       encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    printf("INFO: Copying Image Header\n");
    printf("INFO: Done\n");

    if(encode_magic_string("STEGO", encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("INFO: Encoding Magic String Signature\n");
    printf("INFO: Done\n"); 

    if(encode_secret_file_extn_size(
            strlen(encInfo->extn_secret_file),
            encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("INFO: Encoding Secret File Extension\n");
    

    if(encode_secret_file_extn(
            encInfo->extn_secret_file,
            encInfo) == e_failure)
    {
        return e_failure;
    }
   
    printf("INFO: Done\n");

    if(encode_secret_file_size(
            encInfo->size_secret_file,
            encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("INFO: Encoding Secret File Size\n");
    printf("INFO: Done\n");

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("INFO: Encoding Secret File Data\n");
    printf("INFO: Done\n");

    if(copy_remaining_img_data(
            encInfo->fptr_src_image,
            encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Copying Left Over Data\n");
    printf("INFO: Done\n");
    
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}
    // step1 -> check open_files(encInfo) returning e_success or not
        // yes -> print success msg goto next step
        // no -> return e_failure

    // step2 -> check check_capacity(encInfo) is returning success or not
            // yes -> print success msg and goto next step
            // no -> print error msg and return e_failure
    
    // step3 -> call copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)

    // step4 -> call encode_magic_string(MAGIC_STRING, encInfo)

    // step5 -> call encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)

    //step6 -> call encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)

    //step7 -> Call encode_secret_file_size(encInfo -> size_secret_file, encInfo)

    //step8 -> call encode_secret_file_data(encInfo)

    //step9 -> call copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> stego_image_fname);

