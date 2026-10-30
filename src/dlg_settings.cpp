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
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "main.h"

INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM /* lParam */)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		/*
		 * Nothing special to initialize.
		 */
		SendDlgItemMessage(hDlg, IDR_BUTTON_DEVICE1, BM_SETCHECK, BST_CHECKED, 0);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDR_BUTTON_DEVICE1:
			MessageBoxA(hDlg, "SERIAL", "Click", MB_OK);
			return TRUE;

		case IDR_BUTTON_DEVICE2:
			MessageBoxA(hDlg, "SERIAL", "Click", MB_OK);
			return TRUE;

		case IDOK:
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
