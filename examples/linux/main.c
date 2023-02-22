#include "vid2led.h"
#include "picojpeg.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif
//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned int uint;

//------------------------------------------------------------------------------
static FILE *g_pInFile;
static uint g_nInFileSize;
static uint g_nInFileOfs;

#define FILE_NAME "./test.avi"
//------------------------------------------------------------------------------
unsigned char pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    uint n;
    (void)pCallback_data;

    n = min(g_nInFileSize - g_nInFileOfs, buf_size);
    if (n && (fread(pBuf, 1, n, g_pInFile) != n))
        return PJPG_STREAM_READ_ERROR;
    *pBytes_actually_read = (unsigned char)(n);
    g_nInFileOfs += n;
    return 0;
}

int main() {
//    vid2led_t vid2led;
//
//    vid2led.buffer_length = VID2LEN_STATIC_BUFFER_LENGTH;
//    vid2led.video_format = VID_TYPE_DUMMY;
//    vid2led.video_path = "./video.avi";
//
//    vid2led_init(&vid2led);
//
//    while (!vid2led_service(&vid2led)) {
//        msleep(100);
//    }
//
//    vid2led_deinit(&vid2led);

//    FILE *fp = fopen("JPEG_example_flower.jpg", "rb");
//
//    if (!fp) {
//        printf("Error - cannot open file\n");
//        return -1;
//    }
//
//    print_frame(fp);
//
//    fclose(fp);

    int n = 1;
    const char *pSrc_filename;
    const char *pDst_filename;
    int width = 0, height = 0, comps;
    pjpeg_scan_type_t scan_type;
    const char* p = "?";
    int reduce = 0;

    pjpeg_image_info_t image_info;
    int mcu_x = 0;
    int mcu_y = 0;
    uint row_pitch;
    uint8 *pImage;
    uint8 status;
    uint decoded_width, decoded_height;
    uint row_blocks_per_mcu, col_blocks_per_mcu;

    comps = 0;
    scan_type = PJPG_GRAYSCALE;

    g_pInFile = fopen(FILE_NAME, "rb");
    if (!g_pInFile)
        return 0;

    g_nInFileOfs = 0;

    fseek(g_pInFile, 0x167a, SEEK_SET);
    g_nInFileSize = 0xf5;

    status = pjpeg_decode_init(&image_info, pjpeg_need_bytes_callback, NULL, (unsigned char)reduce);

    if (status)
    {
        printf("pjpeg_decode_init() failed with status %u\n", status);
        if (status == PJPG_UNSUPPORTED_MODE)
        {
            printf("Progressive JPEG files are not supported.\n");
        }

        fclose(g_pInFile);
        return 0;
    }

    scan_type = image_info.m_scanType;

    // In reduce mode output 1 pixel per 8x8 block.
    decoded_width = reduce ? (image_info.m_MCUSPerRow * image_info.m_MCUWidth) / 8 : image_info.m_width;
    decoded_height = reduce ? (image_info.m_MCUSPerCol * image_info.m_MCUHeight) / 8 : image_info.m_height;

    row_pitch = decoded_width * image_info.m_comps;
    pImage = (uint8 *)malloc(row_pitch * decoded_height);
    if (!pImage)
    {
        fclose(g_pInFile);
        return 0;
    }

    row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
    col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

    for ( ; ; )
    {
        int y, x;
        uint8 *pDst_row;

        status = pjpeg_decode_mcu();

        if (status)
        {
            if (status != PJPG_NO_MORE_BLOCKS)
            {
                printf("pjpeg_decode_mcu() failed with status %u\n", status);

                free(pImage);
                fclose(g_pInFile);
                return 0;
            }

            break;
        }

        if (mcu_y >= image_info.m_MCUSPerCol)
        {
            free(pImage);
            fclose(g_pInFile);
            return 0;
        }

        if (reduce)
        {
            // In reduce mode, only the first pixel of each 8x8 block is valid.
            pDst_row = pImage + mcu_y * col_blocks_per_mcu * row_pitch + mcu_x * row_blocks_per_mcu * image_info.m_comps;
            if (image_info.m_scanType == PJPG_GRAYSCALE)
            {
                *pDst_row = image_info.m_pMCUBufR[0];
            }
            else
            {
                uint y, x;
                for (y = 0; y < col_blocks_per_mcu; y++)
                {
                    uint src_ofs = (y * 128U);
                    for (x = 0; x < row_blocks_per_mcu; x++)
                    {
                        pDst_row[0] = image_info.m_pMCUBufR[src_ofs];
                        pDst_row[1] = image_info.m_pMCUBufG[src_ofs];
                        pDst_row[2] = image_info.m_pMCUBufB[src_ofs];
                        pDst_row += 3;
                        src_ofs += 64;
                    }

                    pDst_row += row_pitch - 3 * row_blocks_per_mcu;
                }
            }
        }
        else
        {
            // Copy MCU's pixel blocks into the destination bitmap.
            pDst_row = pImage + (mcu_y * image_info.m_MCUHeight) * row_pitch + (mcu_x * image_info.m_MCUWidth * image_info.m_comps);

            for (y = 0; y < image_info.m_MCUHeight; y += 8)
            {
                const int by_limit = min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

                for (x = 0; x < image_info.m_MCUWidth; x += 8)
                {
                    uint8 *pDst_block = pDst_row + x * image_info.m_comps;

                    // Compute source byte offset of the block in the decoder's MCU buffer.
                    uint src_ofs = (x * 8U) + (y * 16U);
                    const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
                    const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
                    const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

                    const int bx_limit = min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

                    if (image_info.m_scanType == PJPG_GRAYSCALE)
                    {
                        int bx, by;
                        for (by = 0; by < by_limit; by++)
                        {
                            uint8 *pDst = pDst_block;

                            for (bx = 0; bx < bx_limit; bx++)
                                *pDst++ = *pSrcR++;

                            pSrcR += (8 - bx_limit);

                            pDst_block += row_pitch;
                        }
                    }
                    else
                    {
                        int bx, by;
                        for (by = 0; by < by_limit; by++)
                        {
                            uint8 *pDst = pDst_block;

                            for (bx = 0; bx < bx_limit; bx++)
                            {
                                pDst[0] = *pSrcR++;
                                pDst[1] = *pSrcG++;
                                pDst[2] = *pSrcB++;
                                pDst += 3;
                            }

                            pSrcR += (8 - bx_limit);
                            pSrcG += (8 - bx_limit);
                            pSrcB += (8 - bx_limit);

                            pDst_block += row_pitch;
                        }
                    }
                }

                pDst_row += (row_pitch * 8);
            }
        }

        mcu_x++;
        if (mcu_x == image_info.m_MCUSPerRow)
        {
            mcu_x = 0;
            mcu_y++;
        }
    }

    fclose(g_pInFile);

    width = decoded_width;
    height = decoded_height;
    comps = image_info.m_comps;

    printf("width = %d, height = %d, comps = %d\n", width, height, comps);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            printf("(%03d, %03d, %03d) ", *(pImage++), *(pImage++), *(pImage++));
        }
        printf("\n");
    }

    return 0;
}
