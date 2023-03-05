//
// Created by szymon on 01.03.23.
//

#ifndef LINUX_AVI_H
#define LINUX_AVI_H

#include <stdint.h>

typedef uint32_t FOURCC;

typedef struct
{
    uint32_t dwMicroSecPerFrame; // frame display rate (or 0)
    uint32_t dwMaxBytesPerSec; // max. transfer rate
    uint32_t dwPaddingGranularity; // pad to multiples of this
// size;
    uint32_t dwFlags; // the ever-present flags
    uint32_t dwTotalFrames; // # frames in file
    uint32_t dwInitialFrames;
    uint32_t dwStreams;
    uint32_t dwSuggestedBufferSize;
    uint32_t dwWidth;
    uint32_t dwHeight;
    uint32_t dwReserved[4];
} avih;

typedef struct {
    FOURCC fccType;
    FOURCC fccHandler;
    uint32_t dwFlags;
    uint16_t wPriority;
    uint16_t wLanguage;
    uint32_t dwInitialFrames;
    uint32_t dwScale;
    uint32_t dwRate; /* dwRate / dwScale == samples/second */
    uint32_t dwStart;
    uint32_t dwLength; /* In units above... */
    uint32_t dwSuggestedBufferSize;
    uint32_t dwQuality;
    uint32_t dwSampleSize;
} AVIStreamHeader;

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif
//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned int uint;


#define FILE_NAME "./test.avi"

int init_file_avi(FILE* fp);
int get_frame_avi(FILE* fp);

#endif //LINUX_AVI_H
