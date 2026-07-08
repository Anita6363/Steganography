#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *symbol);

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Usage:\n");
        printf("./steg -e <source.bmp> <secret.txt> [stego.bmp]\n");
        printf("./steg -d <stego.bmp> [output.txt]\n");
        return e_failure;
    }

    OperationType op_type = check_operation_type(argv[1]);

    if(op_type == e_encode)
    {
        printf("Encoding mode selected\n");

        EncodeInfo encInfo;

        if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            printf("Error: Invalid encode arguments\n");
            return e_failure;
        }

        if(do_encoding(&encInfo) == e_success)
        {
           printf("INFO: Encoding Done Successfully\n"); 
        }
        else
        {
            printf("Encoding failed\n");
            return e_failure;
        }
    }
    else if(op_type == e_decode)
    {
        printf("Decoding mode selected\n");

        DecodeInfo decInfo;

        if(read_and_validate_decode_args(argv, &decInfo) == e_failure)
        {
            printf("Error: Invalid decode arguments\n");
            return e_failure;
        }

        if(do_decoding(&decInfo) == e_success)
        {
            printf("Decoding completed successfully\n");
        }
        else
        {
            printf("Decoding failed\n");
            return e_failure;
        }
    }
    else
    {
        printf("Unsupported operation\n");
        return e_failure;
    }

    return e_success;
}

OperationType check_operation_type(char *symbol)
{
    if(strcmp(symbol, "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(symbol, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}