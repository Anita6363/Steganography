#include <stdio.h>
#include <string.h>
#include "decode.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *dot;

    dot = strrchr(argv[2], '.');

    if(dot == NULL || strcmp(dot, ".bmp") != 0)
    {
        printf("ERROR: Stego image must be BMP\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if(argv[3] == NULL)
    {
        decInfo->output_fname = "decoded.txt";
    }
    else
    {
        decInfo->output_fname = argv[3];
    }

    return e_success;
}

Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    *data = 0;

    for(int i = 0; i < 8; i++)
    {
        *data = *data << 1;
        *data |= (image_buffer[i] & 1);
    }

    return e_success;
}

Status decode_size_from_lsb(char *image_buffer, int *size)
{
    *size = 0;

    for(int i = 0; i < 32; i++)
    {
        *size = *size << 1;
        *size |= (image_buffer[i] & 1);
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    char buffer[32];
    char ch;

    int extn_size;
    int secret_size;

    char extn[10];

    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "rb");

    if(decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open stego image\n");
        return e_failure;
    }

    decInfo->fptr_output =
        fopen(decInfo->output_fname, "w");

    if(decInfo->fptr_output == NULL)
    {
        printf("ERROR: Unable to create output file\n");
        return e_failure;
    }
    printf("INFO: Opening required files\n");
printf("INFO: Opened %s\n", decInfo->stego_image_fname);

    printf("INFO: Decoding Magic String Signature\n");
    

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    
    for(int i = 0; i < 5; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(buffer, &ch);
    }
    printf("INFO: Done\n");

    
    printf("INFO: Decoding Output File Extension\n");
    /* Decode Extension Size */
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer, &extn_size);

   printf("INFO: Done\n");

    /* Decode Extension */
    for(int i = 0; i < extn_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(buffer, &extn[i]);
    }

    extn[extn_size] = '\0';

   

   printf("INFO: Decoding Secret File Size\n");

    /* Decode Secret File Size */
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer, &secret_size);

    printf("INFO: Done\n");

   
    printf("INFO: Decoding Secret File Data\n");
    
    for(int i = 0; i < secret_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(buffer, &ch);

        fputc(ch, decInfo->fptr_output);
    }
    printf("INFO: Done\n");

   
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;
}