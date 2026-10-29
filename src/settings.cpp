#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include "main.h"
#include "settings.h"
#include <fstream>
#include <map>
#include <string>

#define INIFILE "brwatch.ini"

static char inifile[MAX_PATH];

char *SettingsGetFileName(void)
{
	return inifile;
}

unsigned SettingsFindCpuSection(const char *descriptor)
{
	for (unsigned cpu = 1; cpu <= 256; ++cpu)
	{
		char section[20];
		char storedDescriptor[256];
		snprintf(section, sizeof(section), "CPU%u", cpu);
		GetPrivateProfileStringA(section, "descriptor", "", storedDescriptor, sizeof(storedDescriptor), inifile);
		if (strcmp(storedDescriptor, descriptor) == 0)
			return cpu;
	}
	return 0;
}

static BOOL SettingsSortCpuSections(void)
{
	WritePrivateProfileStringA(NULL, NULL, NULL, inifile);
	std::ifstream input(inifile, std::ios::binary);
	if (!input)
		return FALSE;
	std::map<unsigned, std::string> cpuSections;
	std::string remaining;
	std::string line;
	std::string *currentCpu = NULL;
	const char *marker = "; enter next CPU here...";
	std::string newline = "\r\n";
	bool foundNewline = false;
	while (std::getline(input, line))
	{
		if (!input.eof())
		{
			if (!foundNewline)
			{
				newline = !line.empty() && line.back() == '\r' ? "\r\n" : "\n";
				foundNewline = true;
			}
			line += '\n';
		}
		size_t start = line.find_first_not_of(" \t\r\n");
		if (start != std::string::npos && line.compare(start, strlen(marker), marker) == 0)
			currentCpu = NULL;
		else if (start != std::string::npos && line[start] == '[')
		{
			currentCpu = NULL;
			size_t end = line.find(']', start + 1);
			if (end != std::string::npos)
			{
				std::string name = line.substr(start + 1, end - start - 1);
				if (name.size() > 3 && _strnicmp(name.c_str(), "CPU", 3) == 0 &&
					name.find_first_not_of("0123456789", 3) == std::string::npos)
				{
					unsigned long cpu = strtoul(name.c_str() + 3, NULL, 10);
					if (cpu >= 1 && cpu <= 256)
						currentCpu = &cpuSections[static_cast<unsigned>(cpu)];
				}
			}
		}
		if (currentCpu != NULL)
			*currentCpu += line;
		else
			remaining += line;
	}
	if (input.bad())
		return FALSE;
	input.close();
	size_t position = remaining.find(marker);
	if (position == std::string::npos)
	{
		if (!remaining.empty() && remaining.back() != '\n')
			remaining += newline;
		remaining += std::string(marker) + newline;
		position = remaining.size();
	}
	else
	{
		position = remaining.find('\n', position);
		if (position == std::string::npos)
		{
			remaining += newline;
			position = remaining.size();
		}
		else
			++position;
	}
	std::string sorted;
	for (const auto &section : cpuSections)
	{
		sorted += section.second;
		if (sorted.back() != '\n')
			sorted += newline;
	}
	remaining.insert(position, sorted);
	std::string directory = inifile;
	directory.erase(directory.find_last_of("\\/") + 1);
	char temporary[MAX_PATH];
	if (!GetTempFileNameA(directory.c_str(), "brw", 0, temporary))
		return FALSE;
	HANDLE file = CreateFileA(temporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	BOOL success = FALSE;
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD written = 0;
		success = WriteFile(file, remaining.data(), static_cast<DWORD>(remaining.size()), &written, NULL) &&
			written == remaining.size();
		if (!CloseHandle(file))
			success = FALSE;
		if (success)
			success = MoveFileExA(temporary, inifile, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
	}
	DeleteFileA(temporary);
	return success;
}

BOOL SettingsStoreCpu(const char *descriptor)
{
	if (SettingsFindCpuSection(descriptor) != 0)
		return SettingsSortCpuSections();
	for (unsigned cpu = 1; cpu <= 256; ++cpu)
	{
		char section[20];
		char storedDescriptor[256];
		snprintf(section, sizeof(section), "CPU%u", cpu);
		GetPrivateProfileStringA(section, "descriptor", "", storedDescriptor, sizeof(storedDescriptor), inifile);
		if (storedDescriptor[0] == 0)
			return WritePrivateProfileStringA(section, "descriptor", descriptor, inifile) && SettingsSortCpuSections();
	}
	return FALSE;
}

BOOL SettingsRemoveCpu(const char *descriptor)
{
	unsigned cpu;
	while ((cpu = SettingsFindCpuSection(descriptor)) != 0)
	{
		char section[20];
		snprintf(section, sizeof(section), "CPU%u", cpu);
		if (!WritePrivateProfileStringA(section, NULL, NULL, inifile))
			return FALSE;
	}
	return TRUE;
}

void SettingsInitialize(void)
{
	FILE *file;
	HRSRC hrsrc;
	HGLOBAL hglobal;
	LPVOID p;
	DWORD length;
	BOOL inifile_exist = FALSE;
	char errorMessage[256];

	strcpy(inifile, GetApplicationPath());
	strcat(inifile, "\\" INIFILE);

	file = fopen(inifile, "r");
	if (file == NULL)
	{
		if ((hrsrc = FindResourceA(NULL, MAKEINTRESOURCEA(IDR_DEFAULT_INI1), "DEFAULT_INI")) != NULL)
		{
			if ((hglobal = LoadResource(NULL, hrsrc)) != NULL)
			{
				length = SizeofResource(NULL, hrsrc);
				if ((p = LockResource(hglobal)) != NULL)
				{
					if ((file = fopen(inifile, "wb+")) != NULL)
					{
						if (fwrite(p, 1, length, file) == length)
						{
							char tempstring[256];
							inifile_exist = TRUE;
							snprintf(tempstring, sizeof(tempstring), "The settings were not found.\n%s\nwas created !", inifile);
							MessageBoxA(MainWindowGetHandle(), tempstring, "Create *.ini file", MB_OK);
						}
						else
						{
							perror(errorMessage);
						}
						fclose(file);
					}
					else
					{
						perror(errorMessage);
					}
				}
			}
		}
	}
	else
	{
		fclose(file);
		inifile_exist = TRUE;
	}

	if (inifile_exist == FALSE)
	{
		char tempstring[256];
		snprintf(tempstring, sizeof(tempstring), "Error creating *.ini file !\n%s", errorMessage);
		MessageBoxA(MainWindowGetHandle(), tempstring, "Error!", MB_OK | MB_ICONERROR);
	}
}

INT_PTR CALLBACK SettingsDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		FILE *file;
		size_t bytes_read;
		char *buf;
		HFONT hfont;

		buf = static_cast<char *>(malloc(32768));
		if (buf == NULL)
		{
			MessageBoxA(MainWindowGetHandle(), "To less memory !", "Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}

		file = fopen(SettingsGetFileName(), "rb");

		if (file != NULL)
		{
			if ((bytes_read = fread(buf, 1, 32768, file)))
			{
				buf[bytes_read] = 0;
				SetDlgItemTextA(hDlg, IDC_EDIT1, buf);
				free(buf);
				fclose(file);
			}
			else if (ferror(file))
			{
				sprintf(buf, "Error reading %s !", SettingsGetFileName());
				MessageBoxA(MainWindowGetHandle(), buf, "Error", MB_OK | MB_ICONERROR);
				free(buf);
				fclose(file);
				return FALSE;
			}
		}
		else
		{
			sprintf(buf, "Could not open %s !", SettingsGetFileName());
			MessageBoxA(MainWindowGetHandle(), buf, "Error", MB_OK | MB_ICONERROR);
			free(buf);
			fclose(file);
			return FALSE;
		}

		hfont = static_cast<HFONT>(GetStockObject(SYSTEM_FIXED_FONT));
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hfont, TRUE);
	}

		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		{
			FILE *file;
			DWORD length;
			char *buf;

			file = fopen(SettingsGetFileName(), "wb+");
			if (file != NULL)
			{
				length = GetWindowTextLengthA(GetDlgItem(hDlg, IDC_EDIT1));
				buf = static_cast<char *>(malloc(length + 1));
				if (buf != NULL)
				{
					GetDlgItemTextA(hDlg, IDC_EDIT1, buf, length + 1);
					buf[length] = 0;
					fwrite(buf, 1, length, file);
					fclose(file);
					free(buf);
					EndDialog(hDlg, TRUE);
					return TRUE;
				}
			}
			MessageBoxA(MainWindowGetHandle(), "Could not save the settings", "Error", MB_OK | MB_ICONERROR);
		}
			// Fall through.

		case IDCANCEL:
			/*
			 * OK or Cancel was clicked, close the dialog.
			 */
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		break;
	}

	return FALSE;
}
