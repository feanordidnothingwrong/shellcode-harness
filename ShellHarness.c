#include <Windows.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#pragma warning(disable:4996)
#define FREE(x) HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))

int main(int argc, char** argv, char** envp)
{
	HANDLE fHANDLE = NULL;
	LPCTSTR path = argv[1];
	DWORD err = 0;
	DWORD numRead = 0;
	LARGE_INTEGER size;
	LPVOID pointer = NULL;
	BOOL isWritten = FALSE;
	BOOL sizeGotten = FALSE;
	BOOL didItClose = FALSE;
	
	int(*sc)();

	if (argc != 2)
	{
		printf("%s <filename>\r\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fHANDLE = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fHANDLE == INVALID_HANDLE_VALUE)
	{
		printf("\nCould not open %S, error %d.\n", path, GetLastError());
		exit(EXIT_FAILURE);
	}

	sizeGotten = GetFileSizeEx(fHANDLE, &size);
	if (sizeGotten != TRUE)
	{
		printf("\nCould not get the size of file %S, error %d.\n", path, GetLastError());
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("File size is: [%ld] bytes.\n", size.LowPart);
	}

	pointer = VirtualAlloc(NULL, size.LowPart, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pointer == NULL)
	{
		printf("\nMemory allocation failed, error %d.\n", GetLastError());
		CloseHandle(fHANDLE);
		exit(EXIT_FAILURE);
	}

	isWritten = ReadFile(fHANDLE, pointer, size.LowPart, &numRead, NULL);
	if (isWritten != TRUE)
	{
		printf("\nReading file failed, error %d.\n", GetLastError());
		exit(EXIT_FAILURE);
	}

	didItClose = CloseHandle(fHANDLE);
	if (didItClose != TRUE)
	{
		printf("\nFailed to close file handle, error %d", GetLastError());
		exit(EXIT_FAILURE);
	}

	sc = (int(*)())pointer;
	sc();

	FREE(pointer); 

	return 0;
}