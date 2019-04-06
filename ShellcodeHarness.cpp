#include <stdio.h>
#include <Windows.h>

int main(int argc, char *argv[])
{
	HANDLE hFile = 0;
	DWORD filesize = 0;
	LPVOID ptr = 0;
	DWORD nRead = 0;
	LPCTSTR filepath = argv[1];
	LARGE_INTEGER size;

	int(*sc)();
	if (argc != 2)
	{
		printf("%s <filename>\r\n", argv[0]);
		return 1;
	}
	hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		printf("\nCould not open %S, error %d.\n", filepath, GetLastError());
		return 1;
	}
	filesize = GetFileSizeEx(hFile, &size);
	if (INVALID_FILE_SIZE == filesize)
	{
		printf("\nCould not get the file size of %S, error %d.\n", filepath, GetLastError());
		CloseHandle(hFile);
		return 1;
	}
	else
	{
		printf("\nFile size is: [%ld] bytes.\n", size.LowPart);
	}
	ptr = VirtualAlloc(NULL, size.LowPart, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (0 == ptr)
	{
		printf("\nMemory allocation failed, error %d.\n", GetLastError());
		CloseHandle(hFile);
		return 1;
	}
	if (ReadFile(hFile, ptr, size.LowPart, &nRead, NULL) == FALSE)
	{
		printf("\nReading file failed, error %d.\n", GetLastError());
		return 1;
	}
	CloseHandle(hFile);
	sc = (int(*)())ptr;
	sc();
	VirtualFree(ptr, NULL, MEM_RELEASE);
	return 0;
}