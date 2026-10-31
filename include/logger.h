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

#ifndef __LOGGER_H
#define __LOGGER_H

#include <windows.h>
#include <vector>

class PviObject;

class Logger
{
public:
	static INT_PTR CALLBACK ConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL Toggle();
	static BOOL IsRunning();
	static void DataChanged(PviObject *object);

private:
	Logger() = delete;
	static int CountLoggedObjects();
	static BOOL ReadSettings();
	static BOOL ValidateSettings();
	static BOOL CreateBackupFilename(LPCSTR oldfilename, LPSTR renamedfilename);
	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	static BOOL zip_the_files;
	static BOOL write_by_change;
	static char loggerfile[MAX_PATH];
	static int filesize;
	static int cycletime;
	static BOOL logger_is_running;
	static BOOL start_write_by_change;
	static PviObject *cpu_timemaster;
	static std::vector<PviObject *> logged_object;
	static int no_of_logged_objects;
	static int estimated_row_size;
	static volatile int logger_step;
};

#define UTF8BOM	"\xef\xbb\xbf" 					
#define LOGFILE_HEADER				UTF8BOM "cycle@rtc(yyyy/mm/dd-hh:mm:ss);ticks(ms)"
#define LOGFILE_HEADER_BY_CHANGE	UTF8BOM "event@rtc(yyyy/mm/dd-hh:mm:ss);ticks(ms);name;value"

#endif
