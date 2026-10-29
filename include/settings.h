
char *SettingsGetFileName( void );
void SettingsInitialize(void);
unsigned SettingsFindCpuSection(const char *descriptor);
BOOL SettingsStoreCpu(const char *descriptor);
BOOL SettingsRemoveCpu(const char *descriptor);
INT_PTR CALLBACK SettingsDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SettingsDlgProc(HWND, UINT, WPARAM, LPARAM);
