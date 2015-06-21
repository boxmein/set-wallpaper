// set-wallpaper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int doTheSetting(LPWSTR filePath);
int setRandomFileIn(LPWSTR directory);

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	doTheSetting(L"C:\\Users\\Johannes\\Desktop\\wallpaper.jpg");
#else

	// do we have a path argument?
	if (argc < 2) {
		std::cerr << "Usage: set-wallpaper <wallpaper-file/folder>\n";
		return 1;
	}

	LPWSTR filename = argv[1];

	return doTheSetting(filename); 
#endif
}

int doTheSetting(LPWSTR filePath) {
	DWORD fileAttrs = GetFileAttributes(filePath);

	if (fileAttrs == INVALID_FILE_ATTRIBUTES) {
		DWORD err = GetLastError();
		
		if (err == 2) {
			std::cerr << "File doesn't exist!";
		}
		else {
			std::cerr << "Error checking file attributes! (" << err << ")\n";
		}
		return err;
	}

	if (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) {
		return setRandomFileIn(filePath);
	}

	else {

		// Expand file path out to full pathname
		wchar_t fullPath[_MAX_PATH];

		_tfullpath(fullPath, filePath, _MAX_PATH);

		if (fullPath == NULL) {
			std::cerr << "Error expanding the pathname of the file!\n";
			return -1;
		}

		BOOL result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, fullPath, SPIF_UPDATEINIFILE);
	
		if (!result) {
			std::cerr  << "Something went wrong setting system parameters. (" << GetLastError() << ")\n";
			std::wcerr << L"File path: " << fullPath << "\n";
			std::cerr  << "File attributes: " << fileAttrs << "\n";
		}
		
		return result ? 0 : 1;
	}
}

int setRandomFileIn(LPWSTR directory) {
	HANDLE currFile = NULL;
	WIN32_FIND_DATA fdFile;
	std::vector<std::wstring> files;

	std::wstring dir(directory);
	dir.append(L"\\*.*");



	if ((currFile = FindFirstFile(dir.c_str(), &fdFile)) == INVALID_HANDLE_VALUE) {
		std::cerr << "Error finding random files: no such directory!\n";
		return 3;
	}

	do {
		if ((wcscmp(fdFile.cFileName, L".") != 0 ||
			wcscmp(fdFile.cFileName, L"..") != 0) &&
			(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			bool is_image = false;
			wchar_t* dot = wcsrchr(fdFile.cFileName, '.');
			is_image = dot && (
				wcscmp(dot, L".jpg") == 0 ||
				wcscmp(dot, L".png") == 0 ||
				wcscmp(dot, L".bmp") == 0);

			if (is_image) {
				std::wstring filename(directory);
				filename.append(L"\\");
				filename.append(fdFile.cFileName);

				files.push_back(filename);
			}
		}
	} while (FindNextFile(currFile, &fdFile));

	FindClose(currFile);

	int index = rand() % files.size();
	const wchar_t* s = files[index].c_str();
	
	int len = wcslen(s) + 1;
	wchar_t* buf = (wchar_t*) malloc(len * sizeof(wchar_t));

	wcscpy_s(buf, (size_t) len, s);

	std::wcout << buf << "\n"; 
	
	return doTheSetting((LPWSTR) buf);
}
