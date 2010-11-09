/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Ali Baharev
*/

#ifdef _WIN32

#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <winioctl.h>

#define BLOCK_SIZE 512

static HANDLE hDevice = 0;
static char ReadBuffer[BLOCK_SIZE] = {0};

int block_size() {
	return BLOCK_SIZE;
}

const char* read_device_block(int i) {

	DWORD  dwBytesRead = 0;
	const char* ret_val = NULL;
	OVERLAPPED ol;

	ol.hEvent = 0;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = BLOCK_SIZE*i;
	ol.OffsetHigh = 0;

/* FIXME Find a better way. Mingw at the moment does not have Pointer. */
#if _MSC_VER >= 1300
	ol.Pointer = 0;
#endif

	if( FALSE == ReadFile(hDevice, ReadBuffer, BLOCK_SIZE, &dwBytesRead, (LPOVERLAPPED) &ol) )
	{
		ret_val = NULL;
	}
	else if (dwBytesRead == BLOCK_SIZE)
	{
		ret_val = ReadBuffer;
	}
	else
	{
		ret_val = NULL;
	}

	return ret_val;
}

double card_size_in_GB(const wchar_t* drive)
{
	DISK_GEOMETRY pdg;
	BOOL bResult;
	DWORD junk;
	double ret_val = 0;

	hDevice = CreateFile(drive,  // drive to open
		GENERIC_READ,                // access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		FILE_FLAG_NO_BUFFERING,                // file attributes
		NULL);            // do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		return 0;
	}

	bResult = DeviceIoControl(hDevice,  // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
		NULL, 0, // no input buffer
		&pdg, sizeof(pdg),     // output buffer
		&junk,                 // # bytes returned
		(LPOVERLAPPED) NULL);  // synchronous I/O

	if (FALSE == bResult) {

		return 0;
	}

	ret_val = ((double) pdg.Cylinders.QuadPart) * (ULONG)pdg.TracksPerCylinder *
		(ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;

	ret_val /= (1024 * 1024 * 1024);

	return ret_val;
}

void close_device() {

	CloseHandle(hDevice);

	hDevice = 0;
}

unsigned long error_code() {

	return GetLastError();
}

#endif
