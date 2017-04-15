/* Copyright (c) 2017, libopenglrecorder contributors
 *
 * Use of this source code is governed by a BSD-style license
 * that can be found in the LICENSE file in the root of the source
 * tree.
 */

#ifndef HEADER_OPENGLRECORDER_H
#define HEADER_OPENGLRECORDER_H

#include <stddef.h>

/**
 * \mainpage libopenglrecorder
 *
 * libopenglrecorder is a library allowing optional async readback opengl
 * frame buffer with optional audio recording. It will do video and audio
 * encoding together. The user of this library has to setup opengl context
 * himself and load suitable callbacks. All functions exposed by
 * libopenglrecorder should be called by the same thread which created the
 * opengl context.
 *
 * See \ref openglrecorder.h for usage.
 */

/**
 * List of audio encoder supported by libopenglrecorder, if you want to record
 * without sound, just set m_record_audio in \ref RecorderConfig to 0 and use
 * any encoder below.
 */
typedef enum
{
    /**
     * Vorbis encoder by libvorbisenc.
     */
    OGR_AF_VORBIS = 0,
    /**
    * Total numbers of audio encoder.
    */
    OGR_AF_COUNT
} AudioFormat;

/**
 * List of video encoder supported by libopenglrecorder
 */
typedef enum
{
    /**
     * VP8 encoder by libvpx.
     */
    OGR_VF_VP8 = 0,
    /**
     * VP9 encoder by libvpx. Notice: this is very slow.
     */
    OGR_VF_VP9,
    /**
     * MJPEG encoder, it's provided by turbojpeg and will be always present.
     */
    OGR_VF_MJPEG,
    /**
     * H264 encoder by openh264.
     */
    OGR_VF_H264,
    /**
     * Total numbers of video encoder.
     */
    OGR_VF_COUNT
} VideoFormat;

/**
 * Callback which takes a string pointer to work with.
 */
typedef void(*StringCallback)(const char* s, void* user_data);
/**
 * Callback which takes a int to work with.
 */
typedef void(*IntCallback)(const int i, void* user_data);
/**
 * Callback which takes nothing (void) to work with.
 */
typedef void(*GeneralCallback)(void* user_data);

/**
 * List of callbacks currently using.
 */
typedef enum
{
    /**
     * A \ref GeneralCallback which notify the starting of recording.
     */
    OGR_CBT_START_RECORDING = 0,
    /**
     * A \ref StringCallback which notify the saved filename of recorded file.
     * This will not be shown if \ref ogrDestroy is called anywhere, which
     * avoid calling the user_data potentially deleted by user of this library.
     */
    OGR_CBT_SAVED_RECORDING,
    /**
     * A \ref StringCallback which display error generated by
     * libopenglrecorder, useful for logger.
     */
    OGR_CBT_ERROR_RECORDING,
    /**
     * A \ref IntCallback which the tells the progress in percentage about
     * video encoding after the issue of \ref ogrStopCapture. This will not be
     * shown if \ref ogrDestroy is called anywhere, which avoid calling the
     * user_data potentially deleted by user of this library.
     */
    OGR_CBT_PROGRESS_RECORDING,
    /**
     * Total callback numbers.
     */
    OGR_CBT_COUNT
} CallBackType;

/**
 * Settings for libopenglrecorder
 */
typedef struct
{
    /**
     * 1 if triple buffering is used when capture the opengl frame buffer.
     * It will create 3 pixel buffer objects for async reading, recommend on.
     * 0 otherwise.
     */
    unsigned int m_triple_buffering;
    /**
     * 1 if audio is recorded together, it will use wasapi in windows,
     * pulseaudio in linux. 0 means no audio will be recorded.
     */
    unsigned int m_record_audio;
    /**
     * Width of the capture, it will be floored down to the closest integer divisble
     * by 8 if needed.
     */
    unsigned int m_width;
    /**
     * Height of the capture, it will be floored down to the closest integer divisble
     * by 2 if needed.
     */
    unsigned int m_height;
    /**
     * Encoder for video, see \ref VideoFormat.
     */
    VideoFormat m_video_format;
    /**
     * Encoder for video, see \ref AudioFormat.
     */
    AudioFormat m_audio_format;
    /**
     * Bitrate for audio encoding.
     */
    unsigned int m_video_bitrate;
    /**
     * Bitrate for video encoding.
     */
    unsigned int m_audio_bitrate;
    /**
     * Framerate for the video, 30 is recommended.
     */
    unsigned int m_record_fps;
    /**
     * Jpeg quality for the captured image, from 0 to 100.
     */
    unsigned int m_record_jpg_quality;
} RecorderConfig;

/* List of opengl function used by libopenglrecorder: */
typedef void(*ogrFucReadPixels)(int, int, int, int, unsigned int, unsigned int,
    void*);
typedef void(*ogrFucGenBuffers)(int, unsigned int*);
typedef void(*ogrFucBindBuffer)(unsigned int, unsigned int);
typedef void(*ogrFucBufferData)(unsigned int, ptrdiff_t, const void*,
    unsigned int);
typedef void(*ogrFucDeleteBuffers)(int, const unsigned int*);
typedef void*(*ogrFucMapBuffer)(unsigned int, unsigned int);
typedef unsigned char(*ogrFucUnmapBuffer)(unsigned int);

#ifdef  __cplusplus
extern "C"
{
#endif
/**
 * Initialize the configuration, call this first before using the library.
 *  \return 1 if succesfully configured, 0 otherwise and a default
 * configuration will be used.
 */
int ogrInitConfig(RecorderConfig*);
/**
 * Set the full path with filename for saving the recorded video, excluding
 * extension, libopenglrecorder will automatically add .webm or .mkv as needed.
 */
void ogrSetSavedName(const char*);
/**
 * Reset libopenglrecorder, call this before first \ref ogrCapture.
 */
void ogrPrepareCapture(void);
/**
 * Capture the current frame buffer image as frame, make sure you have called
 * \ref ogrPrepareCapture first.
 */
void ogrCapture(void);
/**
 * Stop the recorder of libopenglrecorder.
 */
void ogrStopCapture(void);
/**
 * Destroy the recorder of libopenglrecorder.
 */
void ogrDestroy(void);
/**
 * (Optional) Register the callback(s) for \ref GeneralCallback, you have to
 *  make sure the enum CallBackType matches the callback type, see
 * \ref CallBackType.
 */
void ogrRegGeneralCallback(CallBackType, GeneralCallback, void*);
/**
 * (Optional) Register the callback(s) for \ref StringCallback, you have to
 *  make sure the enum CallBackType matches the callback type, see
 * \ref CallBackType.
 */
void ogrRegStringCallback(CallBackType, StringCallback, void*);
/**
 * (Optional) Register the callback(s) for \ref IntCallback, you have to
 *  make sure the enum CallBackType matches the callback type, see
 * \ref CallBackType.
 */
void ogrRegIntCallback(CallBackType, IntCallback, void*);
/**
 * Return 1 if recording is happening in libopenglrecorder, 0 otherwise.
 */
int ogrCapturing(void);
/**
 * Set opengl function for read pixels (always required).
 */
void ogrRegReadPixelsFunction(ogrFucReadPixels);
/**
 * Set opengl functions for using PBOs (required if triple buffering is used).
 */
void ogrRegPBOFunctions(ogrFucGenBuffers, ogrFucBindBuffer, ogrFucBufferData,
                        ogrFucDeleteBuffers, ogrFucMapBuffer,
                        ogrFucUnmapBuffer);
#ifdef  __cplusplus
}
#endif

#endif