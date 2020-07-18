#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "lame/lame.h"

#if defined(__linux__) || defined(__linux)
#include <unistd.h>
#include <pthread.h>

#define boolean _Bool
#define TRUE 1
#define FALSE 0

struct ThreadParameter {
	char* wavFileName;
	boolean isSlotOpen;
};

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winbase.h>
	
#endif

//Linear list of WAV file name
struct MusicFile {
	char* wavFileName;
	struct MusicFile* nextFile;
};

//Root of the linear list
struct MusicFile* MusicFileList = NULL;

//Name of the specified Directory
char* directoryName = NULL;

enum ErrorMessageType {
	WRONG_USAGE, 
	DIRECTORY_UNAVAILABLE, 
	DIRECTORY_NOT_WRITABLE, 
	CANNOT_ALLOCATE_MEMORY, 
	NO_WAV_FILE_FOUND, 
	THREAD_EXCEPTION, 
	FILE_IO_EXCEPTION, 
	NOT_WAV_HEADER, 
	NOT_LINEAR_PCM, 
	LAME_API_ERROR
};

#if defined(__linux__) || defined(__linux)
int findOpenSlot(struct ThreadParameter* handle, int cpuCount, boolean* isAllSlotOpen);
void emptySlot(void* argv);
void* encodingThread(void* argv);

#elif defined(_WIN32) || defined(_WIN64)
int findOpenSlot(HANDLE* handle, int cpuCount, boolean* isAllSlotOpen);
void encodingThread(const char* wavFileName);

#endif

int getCPUCount(void);
void printErrorMessage(enum ErrorMessageType type);
boolean isReadyToProcess(char* dirname);
boolean fetchWavFiles(DIR* directory);
boolean hasWavHeader(char* wavFileName);
void releaseFileList(struct MusicFile* currentFile);
void encode(char* wavFilePath, char* mp3FilePath);


int main(int argc, char *argv[]) {
	
	struct MusicFile** currentMusicFile = &MusicFileList;
	int cpuCount = getCPUCount();
	int openSlot = -1;
	boolean isAllSlotOpen = TRUE;
	int i;

#if defined(__linux__) || defined(__linux)
	
	//Allocate thread resources according to CPU count.
	struct ThreadParameter* handle = malloc(sizeof(*handle) * cpuCount);
	pthread_t* threadID = malloc(sizeof(*threadID) * cpuCount);
	pthread_attr_t detachedAttr;

	//Initialize pthreads with detached attribute.
	if(pthread_attr_init(&detachedAttr) != 0) {
		printErrorMessage(THREAD_EXCEPTION);
		return 1;
	}
	else if(pthread_attr_setdetachstate(&detachedAttr, PTHREAD_CREATE_DETACHED) != 0) {
		printErrorMessage(THREAD_EXCEPTION);
		return 1;
	}
	
	//Flags that indicate # of thread is vacant(open) or occupied.
	for(i = 0; i < cpuCount; i++) {
		handle[i].isSlotOpen = TRUE;
	}
	
#elif defined(_WIN32) || defined(_WIN64)
	
	//Allocate thread resources according to CPU count.
	HANDLE* handle = malloc(sizeof(*handle) * cpuCount);
	DWORD* threadID = malloc(sizeof(*threadID) * cpuCount);

	//Flags that indicate # of thread is vacant(NULL) or occupied.
	for(i = 0; i < cpuCount; i++) {
		handle[i] = NULL;
	}

#endif

	if(argc != 2) {
		printErrorMessage(WRONG_USAGE);
		return 1;
	}
	else if((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--h") == 0) || (strcmp(argv[1], "-help") == 0) || (strcmp(argv[1], "--help") == 0)) {
		printErrorMessage(WRONG_USAGE);
		return 1;
	}
	//Check directory/file condition whether it's possible to proceed.
	else if(isReadyToProcess(argv[1]) == FALSE) {
		return 1;
	}
	else if((handle == NULL) || (threadID == NULL)) {
		printErrorMessage(CANNOT_ALLOCATE_MEMORY);
		return 1;
	}
	
	printf(" Start encoding with %d thread%s.\n\n", cpuCount, cpuCount == 1 ? "" : "s");

	//Continue until all the WAV file elements have been processed.
	while((*currentMusicFile != NULL) || ((*currentMusicFile == NULL) && (isAllSlotOpen == FALSE))) {
		//Find next vacant(available) thread index.
		openSlot = findOpenSlot(handle, cpuCount, &isAllSlotOpen);

#if defined(__linux__) || defined(__linux)
		
		//Start processing next WAV file only if vacant thread index was found and there's still waiting WAV files exist.
		if((openSlot != -1) && (*currentMusicFile != NULL)) {
			
			handle[openSlot].wavFileName = (*currentMusicFile)->wavFileName;
			handle[openSlot].isSlotOpen = FALSE;
			isAllSlotOpen = FALSE;
			
			if(pthread_create(&threadID[openSlot], &detachedAttr, encodingThread, (void*)(&handle[openSlot])) != 0) {
				printErrorMessage(THREAD_EXCEPTION);
				handle[openSlot].isSlotOpen = TRUE;
			}
			
			currentMusicFile = &((*currentMusicFile)->nextFile);
		}

#elif defined(_WIN32) || defined(_WIN64)
		
		//Start processing next WAV file only if vacant thread index was found and there's still waiting WAV files exist.
		if((openSlot != -1) && (*currentMusicFile != NULL)) {
			handle[openSlot] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)encodingThread, (LPVOID)(*currentMusicFile)->wavFileName, 0, &threadID[openSlot]);
			isAllSlotOpen = FALSE;
			currentMusicFile = &((*currentMusicFile)->nextFile);
		}

#endif
	}

	printf(" Completed.\n Encoded MP3 files have been generated under directory \"%s\"\n\n", directoryName);

	//Recursively release the WAV file linear list
	releaseFileList(MusicFileList);
	
	if(directoryName != NULL) {
		free(directoryName);
	}
	if(threadID != NULL) {
		free(threadID);
	}
	if(handle != NULL) {
		free(handle);
	}

	return 0;
}


void releaseFileList(struct MusicFile* currentFile) {

	if(currentFile == NULL) {
		return;
	}

	//Recursively release the WAV file linear list
	releaseFileList(currentFile->nextFile);
	free(currentFile->wavFileName);
	free(currentFile);
	
	return;
}


#if defined(__linux__) || defined(__linux)
//Find next vacant(available) thread index.
int findOpenSlot(struct ThreadParameter* handle, int cpuCount, boolean* isAllSlotOpen) {

	int openSlot = -1;
	int openCount = 0;
	int i;
	
	for(i = 0; i < cpuCount; i++) {
		if(handle[i].isSlotOpen == TRUE) {
			openSlot = i;
			openCount ++;
			//Intentionally not calling break
		}
	}
	
	*isAllSlotOpen = (openCount == cpuCount);

	//Returns vacant(available) thread index. If it's -1, it means all the slot are occupied with active thread.
	return openSlot;
}

#elif defined(_WIN32) || defined(_WIN64)
//Find next vacant(available) thread index.
int findOpenSlot(HANDLE* handle, int cpuCount, boolean* isAllSlotOpen) {

	int openSlot = -1;
	int openCount = 0;
	int i;

	for(i = 0; i < cpuCount; i++) {
		if(handle[i] == NULL) {
			openSlot = i;
			openCount ++;
			//Intentionally not calling break
		}
		else if(handle[i] != NULL) {
			
			DWORD exitCode;

			//Alive thread was found, then check the thread status.
			GetExitCodeThread(handle[i], &exitCode);

			if(exitCode != STILL_ACTIVE) {
				//This thread has been already finished.
				CloseHandle(handle[i]);
				handle[i] = NULL;
				openSlot = i;
				openCount ++;
				//Intentionally not calling break
			}
		}
	}
	
	*isAllSlotOpen = (openCount == cpuCount);
	
	//Returns vacant(available) thread index. If it's -1, it means all the slot are occupied with active thread.
	return openSlot;
}
#endif


#if defined(__linux__) || defined(__linux)
//Tell main thread that the own thread has been finished by flagging the parameter
void emptySlot(void* argv) {

	struct ThreadParameter* threadParam = (struct ThreadParameter*)argv;
	threadParam->isSlotOpen = TRUE;
}

//Thread entry for WAV to MP3 encoding
void* encodingThread(void* argv) {

	char* wavFilePath = NULL;
	char* mp3FilePath = NULL;
	
	struct ThreadParameter* threadParam = (struct ThreadParameter*)argv;

	pthread_cleanup_push(emptySlot, (void*)threadParam);

	wavFilePath = malloc(strlen(directoryName) + strlen(threadParam->wavFileName) + 1);
	mp3FilePath = malloc(strlen(directoryName) + strlen(threadParam->wavFileName) + 1);

	if((wavFilePath == NULL) || (mp3FilePath == NULL)) {
		printErrorMessage(CANNOT_ALLOCATE_MEMORY);
		return NULL;
	}
	
	strcpy(wavFilePath, directoryName);
	strcat(wavFilePath, threadParam->wavFileName);

	//put mp3 extension as the output file name
	strcpy(mp3FilePath, wavFilePath);
	mp3FilePath[strlen(mp3FilePath) - 4] = '\0';
	strcat(mp3FilePath, ".mp3");
	
	printf(" Encoding \"%s\" ...\n", wavFilePath);
	encode(wavFilePath, mp3FilePath);
	printf(" Done encoding \"%s\".\n New MP3 file generated \"%s\".\n\n", threadParam->wavFileName, mp3FilePath);
	
	free(wavFilePath);
	free(mp3FilePath);

	pthread_cleanup_pop(1);

	return NULL;
}

#elif defined(_WIN32) || defined(_WIN64)
//Thread entry for WAV to MP3 encoding
void encodingThread(const char* wavFileName) {

	char* wavFilePath = malloc(strlen(directoryName) + strlen(wavFileName) + 1);
	char* mp3FilePath = malloc(strlen(directoryName) + strlen(wavFileName) + 1);

	if((wavFilePath == NULL) || (mp3FilePath == NULL)) {
		printErrorMessage(CANNOT_ALLOCATE_MEMORY);
		return;
	}
	
	strcpy(wavFilePath, directoryName);
	strcat(wavFilePath, wavFileName);
	
	//put mp3 extension as the output file name
	strcpy(mp3FilePath, wavFilePath);
	mp3FilePath[strlen(mp3FilePath) - 4] = '\0';
	strcat(mp3FilePath, ".mp3");
	
	printf(" Encoding \"%s\" ...\n", wavFilePath);
	encode(wavFilePath, mp3FilePath);
	printf(" Done encoding \"%s\".\n New MP3 file generated \"%s\".\n\n", wavFileName, mp3FilePath);
	
	free(wavFilePath);
	free(mp3FilePath);

	return;
}
#endif


#define PCM_BUF_SIZE 8192
#define MP3_BUF_SIZE 8192

//core function to encide WAV to MP3, relying on LAME library
void encode(char* wavFilePath, char* mp3FilePath) {

	FILE* wavFile = fopen(wavFilePath, "rb");
	FILE* mp3File = fopen(mp3FilePath, "wb+");
	int readPCMSampleCount;
	
	short int pcmBuffer[PCM_BUF_SIZE];
	unsigned char mp3Buffer[MP3_BUF_SIZE];

	unsigned short channel;
	unsigned long sampleRate;
	unsigned short blockAlign;

	lame_t lame = lame_init();

	if((wavFile == NULL) || (mp3File == NULL)) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	
	if(lame == NULL) {
		printErrorMessage(LAME_API_ERROR);
		return;
	}

	//Set number of channels
	else if(fseek(wavFile, 22, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	else if(fread(&channel, 2, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	else {
		lame_set_num_channels(lame, channel);
		lame_set_mode(lame, (channel == 1) ? MONO : JOINT_STEREO);
	}

	//Set number of sample rate
	if(fseek(wavFile, 24, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	else if(fread(&sampleRate, 4, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	else {
		lame_set_in_samplerate(lame, sampleRate);
		lame_set_out_samplerate(lame, sampleRate);
	}
	
	//Set block align (byte/sample)
	if(fseek(wavFile, 32, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	else if(fread(&blockAlign, 4, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}

	//Look for data chank in the WAV file
	if(fseek(wavFile, 36, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
		return;
	}
	else {
		char data[5];
		data[4] = '\0';
		
		while(fread(data, 4, 1, wavFile) == 1) {
			
			if(strcmp(data, "data") != 0) {
				//Search data chank by 1 byte
				fseek(wavFile, -3, SEEK_CUR);
				continue;
			}
			else if(fseek(wavFile, 4, SEEK_CUR)) {
				//data chank was found, then skip next 4 byte(indicating data size) to proceed
				printErrorMessage(FILE_IO_EXCEPTION);
				return;
			}
			else {
				//Now we are pointing the head of the PCM data, let's proceed
				break;
			}
		}
		
		//If data chank cannot be detected
		if(feof(wavFile)) {
			printErrorMessage(NOT_WAV_HEADER);
			return;
		}
	}
	
	
	//Encoding conficuration with "good" level quality
	lame_set_quality(lame, 5);
	lame_set_VBR(lame, vbr_default);
	lame_set_VBR_q(lame, 5);

	if(lame_init_params(lame) == -1) {
		printErrorMessage(LAME_API_ERROR);
		return;
	}
	
	//Perfoem encoding
	do {
		int encodedSampleCount;

		readPCMSampleCount = fread(pcmBuffer, blockAlign, PCM_BUF_SIZE / blockAlign, wavFile);
		
		if(readPCMSampleCount != 0) {

			if(channel == 1) { //For monoral case
				
				short int pcmBufferRight[PCM_BUF_SIZE];
				short int pcmBufferLeft[PCM_BUF_SIZE];
				int i;
				
				for(i = 0; i < PCM_BUF_SIZE; i++) {
					pcmBufferRight[i] = pcmBuffer[i];
					pcmBufferLeft[i] = pcmBuffer[i];
				}
				encodedSampleCount = lame_encode_buffer(lame, pcmBufferLeft, pcmBufferRight, readPCMSampleCount, mp3Buffer, MP3_BUF_SIZE);
			}
			
			else { //For stereo case
				encodedSampleCount = lame_encode_buffer_interleaved(lame, pcmBuffer, readPCMSampleCount, mp3Buffer, MP3_BUF_SIZE);
			}
		}
		else {
			encodedSampleCount = lame_encode_flush(lame, mp3Buffer, MP3_BUF_SIZE);
		}
		
		if(encodedSampleCount < 0) {
			printErrorMessage(LAME_API_ERROR);
		}
		else {
			fwrite(mp3Buffer, encodedSampleCount, 1, mp3File);
		}
		
	} while(readPCMSampleCount != 0);
	
	lame_close(lame);

	fclose(wavFile);
	fclose(mp3File);
}

//Obtain available CPU number depending on the system.
int getCPUCount(void) {

#if defined(__linux__) || defined(__linux)
	return sysconf(_SC_NPROCESSORS_ONLN);

#elif defined(_WIN32) || defined(_WIN64)
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
	
#endif
}

//Scan the specified directory and remember the existing WAV files with the linear list.
boolean fetchWavFiles(DIR* directory) {
	
	struct dirent *entry = NULL;
	struct MusicFile** endOfMusicFileList = &MusicFileList;
	int wavFileCount = 0;

	boolean ret = FALSE;

	while((entry = readdir(directory)) != NULL) {
		
		size_t nameLength = strlen(entry->d_name);
		if(nameLength > 4) {
			if(strcmp(&(entry->d_name[nameLength - 4]), ".wav") == 0 || strcmp(&(entry->d_name[nameLength - 4]), ".WAV") == 0) {
				
				if(hasWavHeader(entry->d_name) == FALSE) {
					continue;
				}
				else if((*endOfMusicFileList = malloc(sizeof(*endOfMusicFileList))) == NULL) {
					printErrorMessage(CANNOT_ALLOCATE_MEMORY);
					return FALSE;
				}
				else if(((*endOfMusicFileList)->wavFileName = malloc(nameLength + 1)) == NULL) {
					printErrorMessage(CANNOT_ALLOCATE_MEMORY);
					return FALSE;
				}
				else {
					strcpy((*endOfMusicFileList)->wavFileName, entry->d_name);
					(*endOfMusicFileList)->wavFileName[nameLength] = '\0';
					(*endOfMusicFileList)->nextFile = NULL;
					endOfMusicFileList = &((*endOfMusicFileList)->nextFile);
					wavFileCount ++;
					ret = TRUE;
				}
			}
		}
	}
	
	if(ret == FALSE) {
		printErrorMessage(NO_WAV_FILE_FOUND);
	}
	else if(wavFileCount == 1) {
		printf(" 1 WAV file was found.\n");
	}
	else {
		printf(" %d WAV files were found.\n", wavFileCount);
	}

	return ret;
}


//Check whether the given file has the WAV header
boolean hasWavHeader(char* wavFileName) {

	FILE* wavFile = NULL;
	char data[5];
	char* wavFilePath = malloc(strlen(directoryName) + strlen(wavFileName) + 1);
	boolean ret = FALSE;
	
	if(wavFilePath == NULL) {
		printErrorMessage(CANNOT_ALLOCATE_MEMORY);
		return FALSE;
	}
	
	strcpy(wavFilePath, directoryName);
	strcat(wavFilePath, wavFileName);

	data[4] = '\0';

	if((wavFile = fopen(wavFilePath, "rb")) == NULL) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	
	else if(fseek(wavFile, 0, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if(fread(data, 4, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if(strcmp(data, "RIFF") != 0) {
		printErrorMessage(NOT_WAV_HEADER);
	}
	
	else if(fseek(wavFile, 8, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if(fread(data, 4, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if(strcmp(data, "WAVE") != 0) {
		printErrorMessage(NOT_WAV_HEADER);
	}

	else if(fread(data, 4, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if(strcmp(data, "fmt ") != 0) {
		printErrorMessage(NOT_WAV_HEADER);
	}

	//Check if the Format code is Linear PCM
	else if(fseek(wavFile, 20, SEEK_SET) != 0) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if(fread(data, 2, 1, wavFile) < 1) {
		printErrorMessage(FILE_IO_EXCEPTION);
	}
	else if((data[0] != 1) || (data[1] != 0)) {
		printErrorMessage(NOT_LINEAR_PCM);
	}

	//WAV header check was passed
	else {
		ret = TRUE;
	}

	free(wavFilePath);
	fclose(wavFile);
	
	return ret;
}


//Check directory/file condition whether it's possible to proceed.
boolean isReadyToProcess(char* dirName) {

	DIR* directory = NULL;
	FILE* file = NULL;
	boolean ret = TRUE;
	
	char* writeTestPath = malloc(strlen(dirName) + 16); //For write access check
	directoryName = malloc(strlen(dirName) + 2); //Remember the directory name in the global variable
	if((writeTestPath == NULL) || (directoryName == NULL)) {
		printErrorMessage(CANNOT_ALLOCATE_MEMORY);
		return FALSE;
	}

	strcpy(writeTestPath, dirName);
	strcpy(directoryName, dirName);

#if defined(__linux__) || defined(__linux)
	strcat(writeTestPath, "/writeTest");
	strcat(directoryName, "/");
#elif defined(_WIN32) || defined(_WIN64)
	strcat(writeTestPath, "\\writeTest");
	strcat(directoryName, "\\");
#endif

	printf("\n Checking directory status ...\n (%s)\n", directoryName);

	//Check directory existence and read permission.
	if((directory = opendir(dirName)) == NULL) {
		printErrorMessage(DIRECTORY_UNAVAILABLE);
		ret = FALSE;
	}
	//Check write permission into the speficied directory.
	else if((file = fopen(writeTestPath, "w")) == NULL) {
		printErrorMessage(DIRECTORY_NOT_WRITABLE);
		ret = FALSE;
	}
	else {
		fclose(file);
		remove(writeTestPath);
	}

	if(ret == TRUE) {
		//Scan the specified directory and remember the existing WAV files with the linear list.
		ret = fetchWavFiles(directory);
	}
	
	if(directory != NULL) {
		closedir(directory);
	}
	
	if(writeTestPath != NULL) {
		free(writeTestPath);
	}
	
	return ret;
}


void printErrorMessage(enum ErrorMessageType type) {

	switch(type) {	
		case WRONG_USAGE:
			printf("\n This program encodes a set of WAV files into MP3 format.\n\n");
			printf(" usage: Encode_WAV_To_MP3 <directory path>\n");
			printf(" Give a directory path that contains *.wav files as the first argument.\n");
			printf(" Only one argument(path) is acceptable.\n");
			break;
		case DIRECTORY_UNAVAILABLE:
			printf(" Could not open the specified directory.\n");
			printf(" Check the path name, character code, and its access right.\n");
			break;
		case DIRECTORY_NOT_WRITABLE:
			printf(" The specified directory doesn't have write permission.\n");
			printf(" Cannot create new mp3 files.\n");
			break;
		case CANNOT_ALLOCATE_MEMORY:
			printf(" Failed to allocate memory.\n");
			printf(" Check if the other apps are consuming too much heap memory.\n");
			break;
		case NO_WAV_FILE_FOUND:
			printf(" No WAV file exists on the speficied directory.\n");
			break;
		case THREAD_EXCEPTION:
			printf(" Thread operation returned with error.\n");
			break;
		case FILE_IO_EXCEPTION:
			printf(" File operation returned with error.\n");
			break;
		case NOT_WAV_HEADER:
			printf(" Wrong WAV header was detected, ignoring the file.\n");
			break;
		case NOT_LINEAR_PCM:
			printf(" Compression format is not Linear(uncompressed) PCM, ignoring the file.\n");
			break;
		case LAME_API_ERROR:
			printf(" LAME API returned with error.\n");
			break;
	}

	return;
}
