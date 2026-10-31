/*
 * brwatch - PVI diagnosis tool for B&R PLCs
 *
 * Repository: https://github.com/hilch/brwatch
 *
 * SPDX-License-Identifier: GPL-3.0-only
 * Licensed under the GNU General Public License version 3.
 * See LICENSE for details.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * Application-wide INI settings are encapsulated by ProgramSettings, declared
 * in settings.h. Its static members Initialize() and GetFileName() manage the
 * settings file; FindCpuSection(), StoreCpu() and RemoveCpu() manage saved CPUs.
 * DlgProc() is the Win32 settings editor callback.
 */

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include "main.h"
#include "settings.h"
#include <fstream>
#include <iterator>
#include <map>
#include <string>

#define INIFILE "brwatch.ini"

// Shared settings path; zero-initialized until Initialize() selects the file.
char ProgramSettings::inifile[MAX_PATH];

// Returns the shared filename buffer without transferring ownership.
char *ProgramSettings::GetFileName()
{
	return inifile;
}

// Returns the first matching CPU section number, or 0 when the descriptor is not stored.
unsigned ProgramSettings::FindCpuSection(const char *descriptor)
{
	for (unsigned cpu = 1; cpu <= MAX_CPU_SECTIONS; ++cpu)
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

// Writes a temporary file beside the settings file, then replaces the original only after a successful write and close.
BOOL ProgramSettings::WriteText(const std::string &text)
{
	std::string directory = inifile;
	directory.erase(directory.find_last_of("\\/") + 1);
	char temporary[MAX_PATH];
	if (!GetTempFileNameA(directory.c_str(), "brw", 0, temporary))
		return FALSE;
	HANDLE file = CreateFileA(temporary, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	BOOL success = FALSE;
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD written = 0;
		success = WriteFile(file, text.data(), static_cast<DWORD>(text.size()), &written, nullptr) &&
			written == text.size();
		if (!CloseHandle(file))
			success = FALSE;
		if (success)
			success = MoveFileExA(temporary, inifile, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
	}
	DeleteFileA(temporary);
	return success;
}

// Collects CPU1..CPU256 sections and reinserts them in numeric order after the marker, preserving non-CPU text.
BOOL ProgramSettings::SortCpuSections()
{
	WritePrivateProfileStringA(nullptr, nullptr, nullptr, inifile);
	std::ifstream input(inifile, std::ios::binary);
	if (!input)
		return FALSE;
	std::map<unsigned, std::string> cpuSections;
	std::string remaining;
	std::string line;
	std::string *currentCpu = nullptr;
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
			currentCpu = nullptr;
		else if (start != std::string::npos && line[start] == '[')
		{
			currentCpu = nullptr;
			size_t end = line.find(']', start + 1);
			if (end != std::string::npos)
			{
				std::string name = line.substr(start + 1, end - start - 1);
				if (name.size() > 3 && _strnicmp(name.c_str(), "CPU", 3) == 0 &&
					name.find_first_not_of("0123456789", 3) == std::string::npos)
				{
					unsigned long cpu = strtoul(name.c_str() + 3, nullptr, 10);
					if (cpu >= 1 && cpu <= MAX_CPU_SECTIONS)
						currentCpu = &cpuSections[static_cast<unsigned>(cpu)];
				}
			}
		}
		if (currentCpu != nullptr)
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
	return WriteText(remaining);
}

// Reuses an existing descriptor or fills the first free CPU slot, then sorts the sections.
BOOL ProgramSettings::StoreCpu(const char *descriptor)
{
	if (FindCpuSection(descriptor) != 0)
		return SortCpuSections();
	for (unsigned cpu = 1; cpu <= MAX_CPU_SECTIONS; ++cpu)
	{
		char section[20];
		char storedDescriptor[256];
		snprintf(section, sizeof(section), "CPU%u", cpu);
		GetPrivateProfileStringA(section, "descriptor", "", storedDescriptor, sizeof(storedDescriptor), inifile);
		if (storedDescriptor[0] == 0)
			return WritePrivateProfileStringA(section, "descriptor", descriptor, inifile) && SortCpuSections();
	}
	return FALSE;
}

// Removes all matching CPU sections, including duplicate descriptors.
BOOL ProgramSettings::RemoveCpu(const char *descriptor)
{
	unsigned cpu;
	while ((cpu = FindCpuSection(descriptor)) != 0)
	{
		char section[20];
		snprintf(section, sizeof(section), "CPU%u", cpu);
		if (!WritePrivateProfileStringA(section, nullptr, nullptr, inifile))
			return FALSE;
	}
	return TRUE;
}

// Uses APPDATA\brwatch when available, otherwise the application folder; creates a missing INI from the embedded defaults.
void ProgramSettings::Initialize()
{
	FILE *file;
	HRSRC hrsrc;
	HGLOBAL hglobal;
	LPVOID p;
	DWORD length;
	BOOL inifile_exist = FALSE;
	char errorMessage[256];

	const char *appData = getenv("APPDATA");
	BOOL useAppData = appData != nullptr && appData[0] != 0;
	std::string directory = useAppData ? appData : GetApplicationPath();
	if (useAppData)
		directory += "\\brwatch";
	std::string filename = directory + "\\" INIFILE;
	if (filename.size() >= sizeof(inifile))
	{
		MessageBoxA(MainWindowGetHandle(), "The settings file path is too long.", "Settings", MB_OK | MB_ICONERROR);
		return;
	}
	strcpy(inifile, filename.c_str());
	if (useAppData && !CreateDirectoryA(directory.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		MessageBoxA(MainWindowGetHandle(), "Could not create the settings directory in APPDATA.", "Settings", MB_OK | MB_ICONERROR);
		return;
	}

	file = fopen(inifile, "r");
	if (file == nullptr)
	{
		if ((hrsrc = FindResourceA(nullptr, MAKEINTRESOURCEA(IDR_DEFAULT_INI1), "DEFAULT_INI")) != nullptr)
		{
			if ((hglobal = LoadResource(nullptr, hrsrc)) != nullptr)
			{
				length = SizeofResource(nullptr, hrsrc);
				if ((p = LockResource(hglobal)) != nullptr)
				{
					if ((file = fopen(inifile, "wb+")) != nullptr)
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

// Converts lone LF line endings to CRLF for the Win32 edit control without duplicating existing CR characters.
void ProgramSettings::SetEditorText(HWND hDlg, const std::string &text)
{
	std::string editorText;
	for (size_t index = 0; index < text.size(); ++index)
	{
		if (text[index] == '\n' && (index == 0 || text[index - 1] != '\r'))
			editorText += '\r';
		editorText += text[index];
	}
	SetDlgItemTextA(hDlg, IDC_EDIT1, editorText.c_str());
}

// Handles settings editor loading and resizing; defaults remain a preview until OK saves the text.
INT_PTR CALLBACK ProgramSettings::DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		std::string title = std::string("Settings - ") + GetFileName();
		SetWindowTextA(hDlg, title.c_str());
		SendDlgItemMessageA(hDlg, IDC_EDIT1, EM_SETLIMITTEXT, 0, 0);
		HFONT hfont = static_cast<HFONT>(GetStockObject(SYSTEM_FIXED_FONT));
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hfont, TRUE);
		WritePrivateProfileStringA(nullptr, nullptr, nullptr, inifile);
		std::ifstream input(inifile, std::ios::binary);
		if (!input)
		{
			MessageBoxA(hDlg, "Could not open brwatch.ini.", "Settings", MB_OK | MB_ICONERROR);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		if (input.bad())
		{
			MessageBoxA(hDlg, "Could not read brwatch.ini.", "Settings", MB_OK | MB_ICONERROR);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		SetEditorText(hDlg, text);
		SendMessage(hDlg, WM_SIZE, SIZE_RESTORED, 0);
	}

		return TRUE;

	case WM_GETMINMAXINFO:
	{
		RECT minimum = {0, 0, 260, 120};
		MapDialogRect(hDlg, &minimum);
		AdjustWindowRectEx(&minimum, static_cast<DWORD>(GetWindowLongPtr(hDlg, GWL_STYLE)), FALSE,
			static_cast<DWORD>(GetWindowLongPtr(hDlg, GWL_EXSTYLE)));
		MINMAXINFO *limits = reinterpret_cast<MINMAXINFO *>(lParam);
		limits->ptMinTrackSize.x = minimum.right - minimum.left;
		limits->ptMinTrackSize.y = minimum.bottom - minimum.top;
		return TRUE;
	}

	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
			return TRUE;
		RECT client;
		GetClientRect(hDlg, &client);
		RECT margin = {0, 0, 7, 7};
		RECT button = {0, 0, 50, 14};
		RECT spacing = {0, 0, 9, 6};
		MapDialogRect(hDlg, &margin);
		MapDialogRect(hDlg, &button);
		MapDialogRect(hDlg, &spacing);
		int buttonTop = client.bottom - margin.bottom - button.bottom;
		MoveWindow(GetDlgItem(hDlg, IDC_EDIT1), margin.right, margin.bottom,
			client.right - 2 * margin.right, buttonTop - spacing.bottom - margin.bottom, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDOK), margin.right, buttonTop, button.right, button.bottom, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDCANCEL), margin.right + button.right + spacing.right,
			buttonTop, button.right, button.bottom, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_BUTTON_SETTINGS_DEFAULTS), client.right - margin.right - button.right,
			buttonTop, button.right, button.bottom, TRUE);
		return TRUE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_SETTINGS_DEFAULTS:
		{
			HRSRC resource = FindResourceA(nullptr, MAKEINTRESOURCEA(IDR_DEFAULT_INI1), "DEFAULT_INI");
			HGLOBAL loaded = resource != nullptr ? LoadResource(nullptr, resource) : nullptr;
			const char *data = loaded != nullptr ? static_cast<const char *>(LockResource(loaded)) : nullptr;
			if (data == nullptr)
			{
				MessageBoxA(hDlg, "Could not load the default settings.", "Settings", MB_OK | MB_ICONERROR);
				return TRUE;
			}
			SetEditorText(hDlg, std::string(data, SizeofResource(nullptr, resource)));
			return TRUE;
		}
		case IDOK:
		{
			int length = GetWindowTextLengthA(GetDlgItem(hDlg, IDC_EDIT1));
			std::string text(static_cast<size_t>(length) + 1, '\0');
			int copied = GetDlgItemTextA(hDlg, IDC_EDIT1, &text[0], length + 1);
			text.resize(static_cast<size_t>(copied));
			WritePrivateProfileStringA(nullptr, nullptr, nullptr, inifile);
			if (copied == length && WriteText(text))
			{
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
			MessageBoxA(hDlg, "Could not save brwatch.ini.", "Settings", MB_OK | MB_ICONERROR);
			return TRUE;
		}

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}

	return FALSE;
}
