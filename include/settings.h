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

#ifndef PROGRAM_SETTINGS_H
#define PROGRAM_SETTINGS_H

#include <windows.h>
#include <string>

#define MAX_CPU_SECTIONS 256

class ProgramSettings
{
public:
	// Returns the shared, mutable settings filename buffer; callers must not free it.
	static char *GetFileName();

	// Selects APPDATA\brwatch\brwatch.ini (or the application folder) and creates missing settings from defaults.
	static void Initialize();

	// Finds the first matching CPU1..CPU256 descriptor; returns its section number, or 0 if absent.
	static unsigned FindCpuSection(const char *descriptor);

	// Stores a descriptor in the first free CPU slot and sorts CPU sections; returns FALSE on failure or no free slot.
	static BOOL StoreCpu(const char *descriptor);

	// Deletes every CPU section matching the descriptor; returns FALSE on a write failure.
	static BOOL RemoveCpu(const char *descriptor);

	// Handles the resizable INI editor, default preview, save and cancel; returns TRUE for handled messages.
	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	// Prevents instances because all settings state is shared application-wide.
	ProgramSettings() = delete;

	// Replaces the settings file via a temporary file in the same directory; returns FALSE on failure.
	static BOOL WriteText(const std::string &text);

	// Orders CPU sections numerically after the insertion marker, preserving other text; returns FALSE on failure.
	static BOOL SortCpuSections();

	// Loads the editor text, converting lone LF line endings to Win32 CRLF.
	static void SetEditorText(HWND hDlg, const std::string &text);

	// Shared, null-terminated settings file path, populated by Initialize().
	static char inifile[MAX_PATH];
};

INT_PTR CALLBACK SettingsDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif
