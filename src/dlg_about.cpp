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

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include "main.h"
#include "resource.h"
#include "pvicom.h"
#include "git_info.h"

INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_INITDIALOG:
		/*
		 * Nothing special to initialize.
		 */

#ifdef __DEBUG__
#warning debug version !!!
#define VERSION "Debug!!!"
#else
#define VERSION ""
#endif

		SetDlgItemTextA(hDlg, IDC_STATIC_VERSION, "BRWATCH (PVI6-64Bit) " BRWATCH_GIT_TAG);
		SetDlgItemTextA(hDlg, IDC_STATIC_GIT_HASH, "Git hash: " BRWATCH_GIT_HASH);
		SetDlgItemTextA(hDlg, IDC_STATIC_BUILD_INFO, "Build: " __DATE__ "  " __TIME__ "  " VERSION);

		// PviGetVersion( versiontext, sizeof(versiontext) );
		SetDlgItemTextA(hDlg, IDC_STATIC_PVIVERSION, g_PVIVersionString);
		return TRUE;

	case WM_NOTIFY:
	{
		const auto *notification = reinterpret_cast<const NMHDR *>(lParam);
		if (notification->idFrom == IDC_LINK_PROJECT &&
			(notification->code == NM_CLICK || notification->code == NM_RETURN))
		{
			const auto *link = reinterpret_cast<const NMLINK *>(lParam);
			INT_PTR result = reinterpret_cast<INT_PTR>(
				ShellExecuteW(hDlg, L"open", link->item.szUrl, nullptr, nullptr, SW_SHOWNORMAL));
			if (result <= 32)
			{
				MessageBoxA(hDlg, "Could not open the project website in your default browser.",
							"Error", MB_OK | MB_ICONERROR);
			}
			return TRUE;
		}
		break;
	}

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
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
