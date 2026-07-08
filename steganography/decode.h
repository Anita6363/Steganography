#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *output_fname;
    FILE *fptr_output;
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *image_buffer, char *data);
Status decode_size_from_lsb(char *image_buffer, int *size);

#endif