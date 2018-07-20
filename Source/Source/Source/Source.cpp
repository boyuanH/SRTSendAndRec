/** @file patest_read_record.c
@ingroup test_src
@brief Record input into an array; Save array to a file; Playback recorded
data. Implemented using the blocking API (Pa_ReadStream(), Pa_WriteStream() )
@author Phil Burk  http://www.softsynth.com
@author Ross Bencina rossb@audiomulch.com
*/
/*
* $Id$
*
* This program uses the PortAudio Portable Audio Library.
* For more information see: http://www.portaudio.com
* Copyright (c) 1999-2000 Ross Bencina and Phil Burk
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
* The text above constitutes the entire PortAudio license; however,
* the PortAudio community also makes the following non-binding requests:
*
* Any person wishing to distribute modifications to the Software is
* requested to send the modifications to the original developer so that
* they can be incorporated into the canonical version. It is also
* requested that these non-binding requests be included along with the
* license above.
*/


// 
// #include <stdio.h>
// #include <stdlib.h>
// #include "portaudio.h"
// 
// 
// /* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
// #define SAMPLE_RATE  (44100)
// #define FRAMES_PER_BUFFER (1024)
// #define NUM_SECONDS     (5)
// #define NUM_CHANNELS    (2)
// /* #define DITHER_FLAG     (paDitherOff)  */
// #define DITHER_FLAG     (0) /**/
// 
// /* Select sample format. */
// #if 1
// #define PA_SAMPLE_TYPE  paFloat32
// typedef float SAMPLE;
// #define SAMPLE_SILENCE  (0.0f)
// #define PRINTF_S_FORMAT "%.8f"
// #elif 1
// #define PA_SAMPLE_TYPE  paInt16
// typedef short SAMPLE;
// #define SAMPLE_SILENCE  (0)
// #define PRINTF_S_FORMAT "%d"
// #elif 0
// #define PA_SAMPLE_TYPE  paInt8
// typedef char SAMPLE;
// #define SAMPLE_SILENCE  (0)
// #define PRINTF_S_FORMAT "%d"
// #else
// #define PA_SAMPLE_TYPE  paUInt8
// typedef unsigned char SAMPLE;
// #define SAMPLE_SILENCE  (128)
// #define PRINTF_S_FORMAT "%d"
// #endif
// 
// 
// /*******************************************************************/
// int main(void);
// int main(void)
// {
//     PaStreamParameters inputParameters, outputParameters;
//     PaStream *stream;
//     PaError err;
//     SAMPLE *recordedSamples;
//     int i;
//     int totalFrames;
//     int numSamples;
//     int numBytes;
//     SAMPLE max, average, val;
//     
//     
//     printf("patest_read_record.c\n"); fflush(stdout);
// 
//     totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
//     numSamples = totalFrames * NUM_CHANNELS;
// 
//     numBytes = numSamples * sizeof(SAMPLE);
//     recordedSamples = (SAMPLE *) malloc( numBytes );
//     if( recordedSamples == NULL )
//     {
//         printf("Could not allocate record array.\n");
//         exit(1);
//     }
//     for( i=0; i<numSamples; i++ ) recordedSamples[i] = 0;
// 
//     err = Pa_Initialize();
//     if( err != paNoError ) goto error;
// 
//     inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
//     if (inputParameters.device == paNoDevice) {
//       fprintf(stderr,"Error: No default input device.\n");
//       goto error;
//     }
//     inputParameters.channelCount = NUM_CHANNELS;
//     inputParameters.sampleFormat = PA_SAMPLE_TYPE;
//     inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
//     inputParameters.hostApiSpecificStreamInfo = NULL;
// 
//     /* Record some audio. -------------------------------------------- */
//     err = Pa_OpenStream(
//               &stream,
//               &inputParameters,
//               NULL,                  /* &outputParameters, */
//               SAMPLE_RATE,
//               FRAMES_PER_BUFFER,
//               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
//               NULL, /* no callback, use blocking API */
//               NULL ); /* no callback, so no callback userData */
//     if( err != paNoError ) goto error;
// 
//     err = Pa_StartStream( stream );
//     if( err != paNoError ) goto error;
//     printf("Now recording!!\n"); fflush(stdout);
// 
//     err = Pa_ReadStream( stream, recordedSamples, totalFrames );
//     if( err != paNoError ) goto error;
//     
//     err = Pa_CloseStream( stream );
//     if( err != paNoError ) goto error;
// 
//     /* Measure maximum peak amplitude. */
//     max = 0;
//     average = 0;
//     for( i=0; i<numSamples; i++ )
//     {
//         val = recordedSamples[i];
//         if( val < 0 ) val = -val; /* ABS */
//         if( val > max )
//         {
//             max = val;
//         }
//         average += val;
//     }
// 
//     average = average / numSamples;
// 
//     printf("Sample max amplitude = "PRINTF_S_FORMAT"\n", max );
//     printf("Sample average = "PRINTF_S_FORMAT"\n", average );
// /*  Was as below. Better choose at compile time because this
//     keeps generating compiler-warnings:
//     if( PA_SAMPLE_TYPE == paFloat32 )
//     {
//         printf("sample max amplitude = %f\n", max );
//         printf("sample average = %f\n", average );
//     }
//     else
//     {
//         printf("sample max amplitude = %d\n", max );
//         printf("sample average = %d\n", average );
//     }
// */
//     /* Write recorded data to a file. */
// #if 0
//     {
//         FILE  *fid;
//         fid = fopen("recorded.raw", "wb");
//         if( fid == NULL )
//         {
//             printf("Could not open file.");
//         }
//         else
//         {
//             fwrite( recordedSamples, NUM_CHANNELS * sizeof(SAMPLE), totalFrames, fid );
//             fclose( fid );
//             printf("Wrote data to 'recorded.raw'\n");
//         }
//     }
// #endif
// 
//     /* Playback recorded data.  -------------------------------------------- */
//     
//     outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
//     if (outputParameters.device == paNoDevice) {
//       fprintf(stderr,"Error: No default output device.\n");
//       goto error;
//     }
//     outputParameters.channelCount = NUM_CHANNELS;
//     outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
//     outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
//     outputParameters.hostApiSpecificStreamInfo = NULL;
// 
//     printf("Begin playback.\n"); fflush(stdout);
//     err = Pa_OpenStream(
//               &stream,
//               NULL, /* no input */
//               &outputParameters,
//               SAMPLE_RATE,
//               FRAMES_PER_BUFFER,
//               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
//               NULL, /* no callback, use blocking API */
//               NULL ); /* no callback, so no callback userData */
//     if( err != paNoError ) goto error;
// 
//     if( stream )
//     {
//         err = Pa_StartStream( stream );
//         if( err != paNoError ) goto error;
//         printf("Waiting for playback to finish.\n"); fflush(stdout);
// 
//         err = Pa_WriteStream( stream, recordedSamples, totalFrames );
//         if( err != paNoError ) goto error;
// 
//         err = Pa_CloseStream( stream );
//         if( err != paNoError ) goto error;
//         printf("Done.\n"); fflush(stdout);
//     }
//     free( recordedSamples );
// 
//     Pa_Terminate();
//     return 0;
// 
// error:
//     Pa_Terminate();
//     fprintf( stderr, "An error occured while using the portaudio stream\n" );
//     fprintf( stderr, "Error number: %d\n", err );
//     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
//     return -1;
// }
// 













#include "portaudio.h"
#include <iostream>
#include <process.h>
#include <list>
#include <memory>
#include <vector>
#include <mutex>
#include "AudioDataBuffer.h"
#include <list>
#include <vector>
#include <fstream>
#include "SRTSource.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (16000)
#define FRAMES_PER_BUFFER (SAMPLE_RATE/1000*200)
#define NUM_SECONDS     (5)
#define NUM_CHANNELS    (1)
/* #define DITHER_FLAG     (paDitherOff)  */
#define DITHER_FLAG     (0) /**/

/* Select sample format. */
#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

int readcount = 0;
AudioDataBuffer dataBuffer;


int TransmitBySRT()
{
	ofstream writedata;
	writedata.open("c://tmp//outsourcedata.raw", ios::out | ios::app | ios::binary);
	string uri = "srt://:9002";
	unique_ptr<SRTSource> srtsrc;
	bool srcConnected = false;
	int pollid = srt_epoll_create();
	while (true)
	{
		if (!srtsrc.get())
		{
			srtsrc = SRTSource::CreateSRTSource(uri);
			if (!srtsrc.get())
			{
				cerr << "Unsupported target type" << endl;
				return 1;
			}
			int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
			switch (srtsrc->uri.type())
			{
			case ParserUri::SRT:
				if (srt_epoll_add_usock(pollid,srtsrc->GetSRTSocket(), &events))
				{
					cerr << "Failed to add SRT destination to poll, "
						<< srtsrc->GetSRTSocket() << endl;
					return 1;
				}
				break;
			default:
				break;
			}
		}

		int srtrfdslen = 1;
		SRTSOCKET srtrfds;
		int sysrfdslen = 1;
		SYSSOCKET sysrfds;
		int epollwait = srt_epoll_wait(pollid, &srtrfds, &srtrfdslen, 0, 0, 100, &sysrfds, &sysrfdslen, 0, 0);
		cout << "Socket srt_epoll_wait:" << epollwait <<" and status is "<<srt_getsockstate(srtsrc->GetSRTSocket())<< endl;
		if (epollwait >= 0)
		{
			bool doabort = false;
			SRTSOCKET s = srtrfds;
			SRT_SOCKSTATUS status = srt_getsockstate(s);
			switch (status)
			{
				case SRTS_LISTENING:
				{
					cout << "Now Listening the socket " << endl;
					bool res = srtsrc->AcceptNewClient();
					if (!res)
					{
						cout << "Failed to accept SRT connection" << endl;
						doabort = true;
						break;
					}
					srt_epoll_remove_usock(pollid, s);
					SRTSOCKET ns = srtsrc->GetSRTSocket();
					int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
					if (srt_epoll_add_usock(pollid, ns, &events))
					{
						cerr << "Failed to add SRT client to poll, "
							<< ns << endl;
						doabort = true;
					}
					else
					{
						cout << "Accepted SRT connection" << endl;
						srcConnected = true;
					}
				}
					break;
				case SRTS_BROKEN:
				case SRTS_NONEXIST:
				case SRTS_CLOSED:
					srcConnected = false;
					srt_epoll_remove_usock(pollid, s);
					srtsrc.release();
					break;
				case SRTS_CONNECTED:
					srcConnected = true;
					break;
				default:
					break;
			}
		}

		if (srcConnected)
		{
			std::shared_ptr<vector<char>> pdata(new vector<char>());
			int readresult = dataBuffer.ReadFromBuffer(*pdata);
			if (readresult == -1)
			{
				continue;
			}
			else
			{
				list<shared_ptr<vector<char>>> tmpdatalist;
				int datalistlength = ((*pdata).size() / 1316)+1;

				for (int i = 0;i<datalistlength;i++)
				{
					int datalength = 1316;					
					if (i == datalistlength-1)
					{
						datalength=(*pdata).size() - 1316 * (datalistlength-1);
					}
					shared_ptr<vector<char>> tmpdata(new vector<char>(datalength));
					std::copy(pdata->begin() + (i * 1316), pdata->begin() + (i * 1316) + datalength, tmpdata->begin());

					tmpdatalist.push_back(tmpdata);
				}

				while (!tmpdatalist.empty())
				{
					std::shared_ptr<vector<char>> tmpdata = tmpdatalist.front();
					cout << "tmpdata size is " << (*tmpdata).size()<<endl;
					writedata.write(tmpdata->data(), sizeof(char)*(*tmpdata).size());
					writedata.flush();
 					if (!srtsrc.get() || !srtsrc->IsOpen())
 					{
 						cout << "Unopened srt src ,so data lost " << (*tmpdata).size() << endl;
 					}
 					else if (!srtsrc->Write(*tmpdata))
 					{
 						cout << "Bad write data " << (*tmpdata).size() << endl;
 					}
					tmpdatalist.pop_front();
				}
			}

			//cout << "write into file " << (*pdata).size() << endl;
			//writedata.write(pdata->data(), sizeof(char)*(*pdata).size());
			//writedata.flush();
		}

	}
}



int ReadMicData(
	const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	//printf("recv %lu frames\n", frameCount);
	//cout << "recv " << frameCount << " frames and current is " << readcount << endl;
	shared_ptr<vector<char>> pdata(new vector<char>(NUM_CHANNELS * sizeof(SAMPLE)* frameCount));
	memcpy(pdata->data(), input, NUM_CHANNELS * sizeof(SAMPLE)* frameCount);
	int writeresult = dataBuffer.WriteIntoBuffer(*pdata);
	//cout << "Succces writeintobuffer " << writeresult << endl;
	readcount++;

// 	if (readcount > 30)
// 	{
// 		printf("exiting loop\n");
// 		return paComplete;
// 	}
// 	else
		return paContinue;
}


int main(int argc, char** argv)
{
	PaStreamParameters inputParameters, outputParameters;
	PaStream *stream;
	PaError err;
	int i;

	char *savePath = "c://tmp//outsourcedata.raw";
	int removeresult = remove(savePath);
	printf("remove outsourcedata.raw %d\n", removeresult); fflush(stdout);
	printf("patest_read_record.c\n"); fflush(stdout);

	err = Pa_Initialize();
	if (err != paNoError)
	{
		cout << "Mic Init Error for " << Pa_GetErrorText(err) << endl;
		Pa_Terminate();
		return -1;
	}

	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	if (inputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default input device.\n");
		cout << "No default input device. " << endl;
		return -1;
	}
	inputParameters.channelCount = NUM_CHANNELS;
	inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	/* Record some audio. -------------------------------------------- */
	err = Pa_OpenStream(
		&stream,
		&inputParameters,
		NULL,                  /* &outputParameters, */
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		ReadMicData, /* no callback, use blocking API */
		NULL); /* no callback, so no callback userData */
	if (err != paNoError)
	{
		cout << "Error to Openstream. " << Pa_GetErrorText(err) << endl;
		Pa_Terminate();
		return -1;
	}

	err = Pa_StartStream(stream);
	if (err != paNoError)
	{
		cout << "Error to Startstream. " << Pa_GetErrorText(err) << endl;
		Pa_Terminate();
		return -1;
	}

	printf("Now recording!!\n"); fflush(stdout);
	
	thread task01(TransmitBySRT);
	task01.detach();
	printf("Now Writing!!\n"); fflush(stdout);

	while (Pa_IsStreamActive(stream))
	{
		//cout << "Main Thread Sleeping" << endl;
		Sleep(100*1000 );
	}



	err = Pa_CloseStream(stream);

	if (err != paNoError)
	{
		cout << "Error to Closestream. " << Pa_GetErrorText(err) << endl;
		Pa_Terminate();
		return -1;
	}

	Pa_Terminate();

	return 0;

}