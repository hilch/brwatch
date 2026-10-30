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
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"

static int WritePviPvar(PviObject *object, void *value);

static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PviObject *object;
	int result;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		// disable controls for editing time
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_YEAR), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_DAY), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MONTH), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MINUTE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HOUR), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS), FALSE);
		if (lParam != 0)
		{
			object = (PviObject *)lParam;
			SetDlgItemTextA(hDlg, IDR_STATIC_NAME, object->descriptor);
			SetDlgItemTextA(hDlg, IDR_EDIT_VALUE, "");
			switch (object->ex.pv.type)
			{
			case BR_STRING:
			{
				char tempstring[256];

				sprintf(tempstring, "STRING(%lu) = %lu Bytes", object->ex.pv.length - 1, object->ex.pv.length);
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, tempstring);
				if (object->ex.pv.pvalue != nullptr)
				{
					SetDlgItemTextA(hDlg, IDR_EDIT_VALUE, static_cast<const char *>(object->ex.pv.pvalue));
				}
			}
			break;

			case BR_WSTRING:
			{
				wchar_t tempstring[256];

				_swprintf(tempstring, L"WSTRING(%lu) = %lu Bytes", object->ex.pv.length / 2 - 1, object->ex.pv.length);
				SetDlgItemTextW(hDlg, IDC_STATIC_VARTYPE, tempstring);
				if (object->ex.pv.pvalue != nullptr)
				{
					SetDlgItemTextW(hDlg, IDR_EDIT_VALUE, static_cast<const wchar_t *>(object->ex.pv.pvalue));
				}
			}
			break;

			case BR_BOOL:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "BOOL (0 ... 1)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((unsigned char *)object->ex.pv.pvalue), 0);
				break;

			case BR_USINT:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "USINT (0 ... +255)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((unsigned char *)object->ex.pv.pvalue), 0);
				break;

			case BR_SINT:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "SINT (-128 ... +127)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((signed char *)object->ex.pv.pvalue), 1);
				break;

			case BR_UINT:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "UINT (0 ... +65535)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((unsigned short *)object->ex.pv.pvalue), 0);
				break;

			case BR_INT:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "INT (-32768 ... +32767)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((signed short *)object->ex.pv.pvalue), 1);
				break;

			case BR_UDINT:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "UDINT (0 ... +4294967295)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((unsigned int *)object->ex.pv.pvalue), 0);
				break;

			case BR_DINT:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "DINT (-2147483648 ... +2147483647)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((signed int *)object->ex.pv.pvalue), 1);
				break;

			case BR_REAL:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "REAL (-3.4e38 ... +3.4e38)");
				if (object->ex.pv.pvalue != nullptr)
				{
					char tempstring[256];
					sprintf(tempstring, "%g", *(float *)object->ex.pv.pvalue);
					SetDlgItemTextA(hDlg, IDR_EDIT_VALUE, tempstring);
				}
				break;

			case BR_LREAL:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "LREAL (-1.79769e308 ... +1.79769e308)");
				if (object->ex.pv.pvalue != nullptr)
				{
					char tempstring[256];
					sprintf(tempstring, "%g", *(double *)object->ex.pv.pvalue);
					SetDlgItemTextA(hDlg, IDR_EDIT_VALUE, tempstring);
				}
				break;

			case BR_TIME:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "TIME (-2147483648ms ... +2147483647ms)");
				if (object->ex.pv.pvalue != nullptr)
					SetDlgItemInt(hDlg, IDR_EDIT_VALUE, *((signed int *)object->ex.pv.pvalue), 1);
				break;

			case BR_DATI:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "DATE_AND_TIME (DT#1970-01-01-00:00 ... DT#2106-02-06-06:28:15)");
				if (object->ex.pv.pvalue != nullptr)
				{
					__time64_t t = 0;
					struct tm *ptst;

					memcpy(&t, object->ex.pv.pvalue, sizeof(time_t));
					ptst = _gmtime64(&t);

					if (ptst == nullptr)
					{
						time_t t = time(0);
						ptst = gmtime(&t);
						MessageBoxA(GetParent(hDlg), "Actual time was invalid.\nUsed system time !", "Error", MB_OK | MB_ICONERROR);
					}
					SetDlgItemInt(hDlg, IDC_EDIT_YEAR, ptst->tm_year + 1900, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MONTH, ptst->tm_mon + 1, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_DAY, ptst->tm_mday, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_HOUR, ptst->tm_hour, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MINUTE, ptst->tm_min, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MILLISECONDS, ptst->tm_sec * 1000, FALSE);
				}
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_VALUE), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_YEAR), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_DAY), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MONTH), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MINUTE), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HOUR), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS), TRUE);
				break;

			case BR_DATE:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "DATE (D#1970-01-01 ... D#2106-02-07 )");
				if (object->ex.pv.pvalue != nullptr)
				{
					__time64_t t = 0;
					struct tm *ptst;

					memcpy(&t, object->ex.pv.pvalue, sizeof(time_t));
					ptst = _gmtime64(&t);

					if (ptst == nullptr)
					{
						time_t t = time(0);
						ptst = gmtime(&t);
						MessageBoxA(GetParent(hDlg), "Actual time was invalid.\nUsed system time !", "Error", MB_OK | MB_ICONERROR);
					}
					SetDlgItemInt(hDlg, IDC_EDIT_YEAR, ptst->tm_year + 1900, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MONTH, ptst->tm_mon + 1, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_DAY, ptst->tm_mday, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_HOUR, 0, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MINUTE, 0, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MILLISECONDS, 0, FALSE);
				}
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MONTH), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_YEAR), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_DAY), TRUE);
				break;

			case BR_TOD:
				SetDlgItemTextA(hDlg, IDC_STATIC_VARTYPE, "TIME_OF_DAY (TOD#00:00:00.000 ... TOD#23:59:59.999)");
				if (object->ex.pv.pvalue != nullptr)
				{
					unsigned int intval, hour, minute, milliseconds;
					memcpy(&intval, object->ex.pv.pvalue, 4);

					hour = (int)intval / (3600 * 1000);
					intval -= hour * (3600 * 1000);
					minute = (int)intval / (60 * 1000);
					intval -= minute * (60 * 1000);
					milliseconds = (int)intval;

					SetDlgItemInt(hDlg, IDC_EDIT_YEAR, 1970, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MONTH, 1, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_DAY, 1, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_HOUR, hour, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MINUTE, minute, FALSE);
					SetDlgItemInt(hDlg, IDC_EDIT_MILLISECONDS, milliseconds, FALSE);
				}
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MINUTE), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HOUR), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS), TRUE);
				break;

			default:
				break;
			}
			// get name of task and cpu
			if (object->ex.pv.task != nullptr)
			{
				PviObject *o;
				char tempstring[512];

				o = object->ex.pv.task;
				SetDlgItemTextA(hDlg, IDC_STATIC_TASK, o->descriptor);
				o = object->ex.pv.cpu;

				sprintf(tempstring, "%s %s %s", o->ex.cpu.cputype, o->ex.cpu.arversion, o->descriptor);
				SetDlgItemTextA(hDlg, IDC_STATIC_CPU, tempstring);
			}
			return TRUE;
		}
		return FALSE;

	case WM_COMMAND:

		switch (wParam)
		{
		case IDOK:
		case ID_APPLY:

			result = -1; // assume we have an error
			if (object != nullptr)
			{
				char tempstring[256];

				tempstring[0] = 0;
				GetDlgItemTextA(hDlg, IDR_EDIT_VALUE, tempstring, sizeof(tempstring));

				switch (object->ex.pv.type)
				{
				// String
				case BR_STRING:
					result = WritePviPvar(object, tempstring);
					SendMessageA(GetDlgItem(hDlg, IDR_EDIT_VALUE), EM_SETSEL, 0, -1); // select all
					break;

				case BR_WSTRING:
				{
					wchar_t tempstring[256];
					memset(tempstring, 0, sizeof(tempstring));
					SendDlgItemMessageW(hDlg, IDR_EDIT_VALUE, WM_GETTEXT, sizeof(tempstring) / sizeof(wchar_t), reinterpret_cast<LPARAM>(tempstring));
					result = WritePviPvar(object, tempstring);
					SendMessageW(GetDlgItem(hDlg, IDR_EDIT_VALUE), EM_SETSEL, 0, -1); // select all
				}
				break;

				case BR_BOOL:
				{
					unsigned char byte;

					if ((*tempstring == '1') || !strcmp(tempstring, "true") || !strcmp(tempstring, "True") || !strcmp(tempstring, "TRUE"))
					{
						byte = 1;
						result = WritePviPvar(object, &byte);
					}
					else if ((*tempstring == '0') || !strcmp(tempstring, "false") || !strcmp(tempstring, "False") || !strcmp(tempstring, "FALSE"))
					{
						byte = 0;
						result = WritePviPvar(object, &byte);
					}
				}
					SendMessageA(GetDlgItem(hDlg, IDR_EDIT_VALUE), EM_SETSEL, 0, -1); // select all
					break;

					// Integer- Formate

				case BR_USINT:
				case BR_SINT:
				case BR_UINT:
				case BR_INT:
				case BR_UDINT:
				case BR_DINT:
				case BR_TIME:
				{
					uint32_t integer = 0;
					int32_t signedInteger = 0;
					BOOL unsignedType = object->ex.pv.type == BR_USINT || object->ex.pv.type == BR_UINT || object->ex.pv.type == BR_UDINT;
					BOOL valid;

					valid = FALSE;
					if (sscanf(tempstring, " 16#%8" SCNx32, &integer) == 1)
					{
						valid = TRUE;
					}
					else if (unsignedType)
					{
						valid = sscanf(tempstring, " 10#%10" SCNu32, &integer) == 1 ||
								sscanf(tempstring, " %10" SCNu32, &integer) == 1;
					}
					else
					{
						valid = sscanf(tempstring, " 10#%11" SCNd32, &signedInteger) == 1 ||
								sscanf(tempstring, " %11" SCNd32, &signedInteger) == 1;
						integer = static_cast<uint32_t>(signedInteger);
					}

					// check range of value
					if (unsignedType)
					{
						if (object->ex.pv.length < sizeof(integer) && integer >> (object->ex.pv.length * 8))
						{
							valid = FALSE;
						}
					}
					else if (object->ex.pv.length < sizeof(integer))
					{ // signed integers
						memcpy(&signedInteger, &integer, sizeof(signedInteger));
						int32_t limit = 1 << (object->ex.pv.length * 8 - 1);
						if (signedInteger < -limit || signedInteger > limit - 1)
						{
							valid = FALSE;
						}
					}
					if (valid)
					{
						result = WritePviPvar(object, &integer);
					}
				}
					SendMessageA(GetDlgItem(hDlg, IDR_EDIT_VALUE), EM_SETSEL, 0, -1); // select all
					break;

				// floating point
				case BR_REAL:
				{
					float real;

					if (sscanf(tempstring, "%g", &real) == 1)
					{
						result = WritePviPvar(object, &real);
					}
				}
					SendMessageA(GetDlgItem(hDlg, IDR_EDIT_VALUE), EM_SETSEL, 0, -1); // select all
					break;

				case BR_LREAL:
				{
					double lreal;
					float real;

					if (sscanf(tempstring, "%g", &real) == 1)
					{
						lreal = real;
						result = WritePviPvar(object, &lreal);
					}
				}
					SendMessageA(GetDlgItem(hDlg, IDR_EDIT_VALUE), EM_SETSEL, 0, -1); // select all
					break;

				case BR_DATI:
				{
					struct tm tst;

					memset(&tst, 0, sizeof(tst));

					tst.tm_year = GetDlgItemInt(hDlg, IDC_EDIT_YEAR, nullptr, FALSE) - 1900;
					tst.tm_mon = GetDlgItemInt(hDlg, IDC_EDIT_MONTH, nullptr, FALSE) - 1;
					tst.tm_mday = GetDlgItemInt(hDlg, IDC_EDIT_DAY, nullptr, FALSE);
					tst.tm_hour = GetDlgItemInt(hDlg, IDC_EDIT_HOUR, nullptr, FALSE);
					tst.tm_min = GetDlgItemInt(hDlg, IDC_EDIT_MINUTE, nullptr, FALSE);
					tst.tm_sec = GetDlgItemInt(hDlg, IDC_EDIT_MILLISECONDS, nullptr, FALSE) / 1000;

					if (tst.tm_year > 138 || tst.tm_year < 70)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_YEAR);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong year !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_mon < 0 || tst.tm_mon > 11)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_MONTH);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong month !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_mday < 1 || tst.tm_mday > 31)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_DAY);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong day !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_hour > 23)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_HOUR);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong hour !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_min > 59)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong milliseconds !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_sec > 59)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong milliseconds !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else
					{
						time_t t = mktime(&tst);
						/* apply possible corrections */
						SetDlgItemInt(hDlg, IDC_EDIT_YEAR, tst.tm_year + 1900, FALSE);
						SetDlgItemInt(hDlg, IDC_EDIT_MONTH, tst.tm_mon + 1, FALSE);
						SetDlgItemInt(hDlg, IDC_EDIT_DAY, tst.tm_mday, FALSE);
						if (t == -1)
						{
							SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong date !");
						}
						else
						{
							SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "");
							WritePviPvar(object, &t);
							result = 0;
						}
					}
				}
				break;

				case BR_DATE:
				{
					struct tm tst;

					memset(&tst, 0, sizeof(tst));
					tst.tm_hour = tst.tm_min = tst.tm_sec = 0;
					tst.tm_year = GetDlgItemInt(hDlg, IDC_EDIT_YEAR, nullptr, FALSE) - 1900;
					tst.tm_mon = GetDlgItemInt(hDlg, IDC_EDIT_MONTH, nullptr, FALSE) - 1;
					tst.tm_mday = GetDlgItemInt(hDlg, IDC_EDIT_DAY, nullptr, FALSE);
					if (tst.tm_year > 106 || tst.tm_year < 70)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_YEAR);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong year !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_mon < 0 || tst.tm_mon > 11)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_MONTH);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong month !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else if (tst.tm_mday < 1 || tst.tm_mday > 31)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_DAY);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong day !");
						SetFocus(hwnd);
						SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
					else
					{
						time_t t = mktime(&tst);
						/* apply possible corrections */
						SetDlgItemInt(hDlg, IDC_EDIT_YEAR, tst.tm_year + 1900, FALSE);
						SetDlgItemInt(hDlg, IDC_EDIT_MONTH, tst.tm_mon + 1, FALSE);
						SetDlgItemInt(hDlg, IDC_EDIT_DAY, tst.tm_mday, FALSE);
						if (t == -1)
						{
							SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong date !");
						}
						else
						{
							SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "");
							WritePviPvar(object, &t);
							result = 0;
						}
					}
				}
				break;

				case BR_TOD:
				{
					unsigned int hour, minute, milliseconds;
					hour = GetDlgItemInt(hDlg, IDC_EDIT_HOUR, nullptr, FALSE);
					minute = GetDlgItemInt(hDlg, IDC_EDIT_MINUTE, nullptr, FALSE);
					milliseconds = GetDlgItemInt(hDlg, IDC_EDIT_MILLISECONDS, nullptr, FALSE);
					if (hour > 23)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_HOUR);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong hour !");
						SetFocus(hwnd);
						SendMessageA(hwnd, EM_SETSEL, 0, -1);
					}
					else if (minute > 59)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_MINUTE);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong minute !");
						SetFocus(hwnd);
						SendMessageA(hwnd, EM_SETSEL, 0, -1);
					}
					else if (milliseconds > 59999)
					{
						HWND hwnd = GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "Wrong milliseconds !");
						SetFocus(hwnd);
						SendMessageA(hwnd, EM_SETSEL, 0, -1);
					}
					else
					{
						milliseconds = milliseconds + (60000 * minute) + (3600000 * hour);
						WritePviPvar(object, &milliseconds);
						SetDlgItemTextA(hDlg, IDC_STATUS_MESSAGE, "");
						result = 0;
					}
				}
				break;

				default:
					break;
				}

				if (result == 0)
				{
					if (wParam == IDOK)
						//                  if( IsDlgButtonChecked( hDlg, IDC_CHECK1 ) == BST_CHECKED )
						EndDialog(hDlg, TRUE);
				}
				else
				{
					MessageBeep(MB_ICONEXCLAMATION);
					if (result > 0)
					{ /* PVI- Error ? */
						char tempstring[256];
						sprintf(tempstring, "A PVI-Error %u occured.\nRefer to PVI manual, please", result);
						MessageBoxA(GetParent(hDlg), tempstring, "Error", MB_OK | MB_ICONERROR);
					}
				}
				return TRUE;
			}
			return FALSE;

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

void DlgWritePvarShowDialog(PviObject *object)
{
	if (object->type == POBJ_PVAR)
	{
		if (object->ex.pv.type == BR_DATI || object->ex.pv.type == BR_TOD || object->ex.pv.type == BR_DATE)
		{

			DialogBoxParam(g_hInstance, MAKEINTRESOURCE(DLG_WRITE_DATE_AND_TIME), g_hwndMainWindow,
						   DlgProc, (LPARAM)object);
		}
		else if (object->ex.pv.type == BR_STRING)
		{

			DialogBoxParam(g_hInstance, MAKEINTRESOURCE(DLG_WRITE_STRINGS), g_hwndMainWindow,
						   DlgProc, (LPARAM)object);
		}
		else if (object->ex.pv.type == BR_WSTRING)
		{

			DialogBoxParam(g_hInstance, MAKEINTRESOURCE(DLG_WRITE_STRINGS), g_hwndMainWindow,
						   DlgProc, (LPARAM)object);
		}
		else
		{

			DialogBoxParam(g_hInstance, MAKEINTRESOURCE(DLG_WRITE_NUMBERS), g_hwndMainWindow,
						   DlgProc, (LPARAM)object);
		}
	}
}

/* write value to a PVAR */
int WritePviPvar(PviObject *object, void *value)
{
	DWORD length = object->ex.pv.length;
	int result;

	if (object->ex.pv.type == BR_STRING)
	{
		length--;
		if (strlen(static_cast<const char *>(value)) < length)
			length = (DWORD)strlen(static_cast<const char *>(value));
	}

	result = PviWrite(object->linkid, POBJ_ACC_DATA, value, length, 0, 0);
	return (result);
}
