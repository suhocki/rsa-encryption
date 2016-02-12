
#include <windows.h>
#include <string>
#include "resource.h"
#include "rsa.h"

#define _CRT_SECURE_NO_WARNINGS

#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 300
#define BUFF_MAX 256 // The size of the buffers that have to do with opening a file

#define ENCRYPT_BUTTON 0x00CC
#define DECRYPT_BUTTON 0x00DD
#define GENERATE_BUTTON 0x00EE
#define CHOOSE_ENCR_BUTTON 0x00FF
#define CHOOSE_DECR_BUTTON 0x00F1
#define CHOOSE_KEY_BUTTON 0x00F2
#define TEXT_FIELD 0x00F3



LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

void FillOpenParams(OPENFILENAME&, HWND, char*, char*); // This function will fill the 
														// OPENFILENAME structure which we'll
														// need to create the "default open
														// dialog box"

void HandleFiles(WPARAM wParam);
int DisplayResourceNAMessageBox();


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HDC hDCScreen = GetDC(NULL);
	int Horres = GetDeviceCaps(hDCScreen, HORZRES);
	int Vertres = GetDeviceCaps(hDCScreen, VERTRES);

	HWND        hwnd = NULL;
	HMENU		hmenu;
	MSG         msg;
	WNDCLASSEX  wndclass = { 1 };


	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WinProc;
	wndclass.hInstance = hinstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName = "Class";

	hmenu = LoadMenu(hinstance, MAKEINTRESOURCE(IDR_MENU1));

	if (!hmenu)
		return EXIT_FAILURE; // Something bad happened :(


	RegisterClassEx(&wndclass);
											
	hwnd = CreateWindowEx(NULL, // No extra style,
							"Class",
							"RSA Шифрование",
							WS_OVERLAPPEDWINDOW | WS_THICKFRAME, 
							GetDeviceCaps(hDCScreen, HORZRES) / 2 - WINDOW_WIDTH / 2,
							GetDeviceCaps(hDCScreen, VERTRES) / 2 - WINDOW_HEIGHT / 2,
							WINDOW_WIDTH,
							WINDOW_HEIGHT,
							NULL,
							hmenu,
							hinstance,
							NULL);
	SetCursorPos(GetDeviceCaps(hDCScreen, HORZRES) / 2, GetDeviceCaps(hDCScreen, VERTRES) / 2);

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);



	while (1) 
	{
												
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);							
			DispatchMessage(&msg);
		}
	}

	UnregisterClass("Class", hinstance);
	return msg.wParam;
}


LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static string in_encr, in_decr, pb_key, pr_key, key, out;
	static RECT clientRect = { 0 };
	static RECT textRect1;
	static RECT textRect2;
	static RECT textRect3;
	static int xEncrypt, yEncrypt;
	static int xDecrypt, yDecrypt;
	static int xGenerate, yGenerate;
	static HWND EncryptButton = NULL;
	static HWND DecryptButton = NULL;
	static HWND GenerateButton = NULL;
	static HWND ChooseEncrButton = NULL;
	static HWND ChooseDecrButton = NULL;
	static HWND ChooseKeyButton = NULL;
	static HWND HelpWindow = NULL;
	static HWND EncryptTextField = NULL;
	static HWND DecryptTextField = NULL;
	static HWND KeyTextField = NULL;

	static HDC hDCScreen = GetDC(NULL);
	static int Horres = GetDeviceCaps(hDCScreen, HORZRES);
	static int Vertres = GetDeviceCaps(hDCScreen, VERTRES);

	HDC hdc = NULL;
	PAINTSTRUCT paintStruct; // A PAINTSTRUCT structure is something we need to paint (draw) when handling the WM_PAINT message.

	PAINTSTRUCT ps;
	OPENFILENAME open_params = { 0 }; // This structure is used by the either the
									  // GetOpenFileName() function (for opening files)
									  // or the GetSaveFileName() function (for saving files)
									  // We're going to use it for opening a file -- Info about
									  // the file we try to open WILL be stored in this struct

	char filter[BUFF_MAX] = { 0 }; // This will be filled in with a "filter", explained later :)
	char file_name[BUFF_MAX] = { 0 }; // This will be used as the "default filename" for the
									  // dialog box

	wchar_t  str[BUFF_MAX];

	switch (message)
	{

	case WM_CREATE: // This message is sent when the window is created.									
					// We would want to put our initialization code here...
		GetClientRect(hwnd, &clientRect);
		DragAcceptFiles(ChooseKeyButton, TRUE);

		EncryptButton = CreateWindow("Button", "Зашифровать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)ENCRYPT_BUTTON,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		DecryptButton = CreateWindow("Button", "Расшифровать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)DECRYPT_BUTTON,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		GenerateButton = CreateWindow("Button", "Сгенерировать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)GENERATE_BUTTON,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		ChooseEncrButton = CreateWindow("Button", "Выбрать файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)CHOOSE_ENCR_BUTTON,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		ChooseDecrButton = CreateWindow("Button", "Выбрать файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)CHOOSE_DECR_BUTTON,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		ChooseKeyButton = CreateWindowEx(WS_EX_ACCEPTFILES, "Button", "Выбрать файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)CHOOSE_KEY_BUTTON,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		EncryptTextField = CreateWindow("Edit", "", WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)TEXT_FIELD,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		KeyTextField = CreateWindow("Edit", "", WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)TEXT_FIELD,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);

		DecryptTextField = CreateWindow("Edit", "", WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, hwnd, (HMENU)TEXT_FIELD,
			((LPCREATESTRUCT)lparam)->hInstance, NULL);
		break;


	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *minmax = (MINMAXINFO *)lparam;
		minmax->ptMinTrackSize.x = WINDOW_WIDTH;
		minmax->ptMinTrackSize.y = WINDOW_HEIGHT;
		break;
	}

	case WM_PAINT: // This message is sent to the WndProc when the window needs to be repainted.  
				   // Put code here to paint what should be in the window.	

		hdc = BeginPaint(hwnd, &paintStruct);
		GetClientRect(hwnd, &clientRect);

		textRect1.left = clientRect.right * 3 / 16 - 67;
		textRect1.top = clientRect.bottom / 15;
		textRect2.left = clientRect.right * 8 / 16 - 78;
		textRect2.top = clientRect.bottom / 15;
		textRect3.left = clientRect.right * 13 / 16 - 68;
		textRect3.top = clientRect.bottom / 15;

		DrawText(hdc, "Выберите файл для\n     шифрования:", -1, &textRect1, DT_NOCLIP);
		DrawText(hdc, "Выберите файл ключа:", -1, &textRect2, DT_NOCLIP);
		DrawText(hdc, "Выберите файл для\n   расшифрования:", -1, &textRect3, DT_NOCLIP);

		MoveWindow(EncryptButton, clientRect.right * 1 / 16, clientRect.bottom * 9 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(GenerateButton, clientRect.right * 6 / 16, clientRect.bottom * 9 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(DecryptButton, clientRect.right * 11 / 16, clientRect.bottom * 9 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(ChooseEncrButton, clientRect.right * 1 / 16, clientRect.bottom * 3 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(ChooseDecrButton, clientRect.right * 11 / 16, clientRect.bottom * 3 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(ChooseKeyButton, clientRect.right * 6 / 16, clientRect.bottom * 3 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(EncryptTextField, clientRect.right * 1 / 16, clientRect.bottom * 6 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(KeyTextField, clientRect.right * 6 / 16, clientRect.bottom * 6 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		MoveWindow(DecryptTextField, clientRect.right * 11 / 16, clientRect.bottom * 6 / 11, clientRect.right / 4, clientRect.bottom / 7, true);
		break;


	case WM_COMMAND:
		//MessageBox(NULL, (LPCSTR)L"Win Computer", (LPCSTR)L"Ok!", MB_OK);

		
		if (HIWORD(wparam) == BN_CLICKED)
		{
			/* 	But we have 3 buttons!  How do we know which one was clicked?
			The LOWORD(wparam) gets us the "button's ID" -- This was set as the 7th parameter
			in CreateWindow() when were making the buttons.
			So depending on the "button's ID" we know what button has been clicked
			*/
			if (LOWORD(wparam) == ENCRYPT_BUTTON)
			{
				/*
				encryptTxtFile(string in, string out, string pbkey);
				*/
				
				if ((in_encr.size() > 3) && (key.size() > 3))
				{
					out = in_encr;

					out.pop_back();
					out.pop_back();
					out.pop_back();
					out.pop_back();

					out = out + "-crypted.txt";

					rsa::encryptTxtFile(in_encr, out, key);

					string textMsg;
					textMsg = "Зашифрованный файл: \n" + out;
					MessageBox(NULL, textMsg.c_str(), "Уведомление", MB_OK | MB_ICONINFORMATION);

					return 0;
				}
				MessageBox(NULL, "Укажите файл для шифрования и публичный ключ!", "Ошибка!", MB_OK | MB_ICONERROR);
			}
			else if (LOWORD(wparam) == DECRYPT_BUTTON)
			{
				if ((in_decr.size() > 3) && (key.size() > 3))
				{
					string buff;
					out = in_decr;

					out.pop_back();
					out.pop_back();
					out.pop_back();
					out.pop_back();

					int sz = out.size();

					if (sz > 8)
					{
						for (int i = 8; i >= 1; i--)
						{
							buff.push_back(out[sz - i]);
						}

						if (buff == "-crypted")
						{

							for (int i = 8; i >= 1; i--)
							{
								out.pop_back();
							}
						}
					}

					out = out + "-decrypted.txt";
					rsa::decryptTxtFile(in_decr, out, key);

					string textMsg;
					textMsg = "Расшифрованный файл: \n" + out;
					MessageBox(NULL, textMsg.c_str(), "Уведомление", MB_OK | MB_ICONINFORMATION);

					return 0;
				}
				MessageBox(NULL, "Укажите файл для расшифрования и приватный ключ!", "Ошибка!", MB_OK | MB_ICONERROR);
			}
			else if (LOWORD(wparam) == GENERATE_BUTTON)
			{
				char Name[MAX_PATH];

				GetModuleFileNameA(NULL, Name, MAX_PATH);
				pb_key = Name;
				string buff;

				int lastSlash = pb_key.find_last_of('\\');
				for (int i = 0; i <= lastSlash; i++)
				{
					buff.push_back(pb_key[i]);
				}
				pb_key = buff + "pb_key.txt";
				pr_key = buff + "pr_key.txt";
				rsa::saveKeys(pb_key, pr_key);
				string textMsg;
				textMsg = "Публичный ключ находится по пути: \n" + pb_key + "\n\nПриватный ключ находится по пути: \n" + pr_key;
				MessageBox(NULL, textMsg.c_str(), "Уведомление", MB_OK | MB_ICONINFORMATION);

			}
			else if (LOWORD(wparam) == CHOOSE_ENCR_BUTTON)
			{
				strcat(filter, "TXT Files");

				int index = strlen(filter) + 1;

				filter[index++] = '*';
				filter[index++] = '.';
				filter[index++] = 't';
				filter[index++] = 'x';
				filter[index++] = 't';

				FillOpenParams(open_params, hwnd, filter, file_name);
				GetOpenFileName(&open_params);
				in_encr = file_name;
				SendMessage(EncryptTextField, WM_SETTEXT, 0, (LPARAM)in_encr.c_str()); //считали
			}
			else if (LOWORD(wparam) == CHOOSE_KEY_BUTTON)
			{
				strcat(filter, "TXT Files");

				int index = strlen(filter) + 1;

				filter[index++] = '*';
				filter[index++] = '.';
				filter[index++] = 't';
				filter[index++] = 'x';
				filter[index++] = 't';

				FillOpenParams(open_params, hwnd, filter, file_name);
				key = file_name;
				GetOpenFileName(&open_params);
				SendMessage(KeyTextField, WM_SETTEXT, 0, (LPARAM)file_name); //считали
				key = file_name;
			}
			else if (LOWORD(wparam) == CHOOSE_DECR_BUTTON)
			{
				strcat(filter, "TXT Files");

				int index = strlen(filter) + 1;

				filter[index++] = '*';
				filter[index++] = '.';
				filter[index++] = 't';
				filter[index++] = 'x';
				filter[index++] = 't';

				FillOpenParams(open_params, hwnd, filter, file_name);
				GetOpenFileName(&open_params);
				in_decr = file_name;
				SendMessage(DecryptTextField, WM_SETTEXT, 0, (LPARAM)file_name); //считали			}
			}

			// If the user selected "Quit" from the menu
			if (LOWORD(wparam) == IDHELP)
			{
				MessageBox(NULL, (LPCSTR)"С помощью кнопки <Сгенерировать> сгенерируйте пару ключей: публичный и приватный. Далее при помощи публичного ключа можно зашифровать файл, а с помощью приватного - расшифровать.", (LPCSTR)"Помощь", MB_OK | MB_ICONQUESTION);
			}
			//SendMessage(hwnd, WM_CLOSE, 0, 0); // We'll do as they ask and close the program


											   // If the user selected "Open" from the menu
			if (LOWORD(wparam) == VS_VERSION_INFO)
			{
				DisplayResourceNAMessageBox();
			}

			break;

	case WM_DROPFILES:
		SendMessage(ChooseKeyButton, WM_DROPFILES, wparam, 0);
		// Call our function we created to display all the files.
		// We pass the wParam because it's the HDROP handle.
		HandleFiles(wparam);
		break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	default:

		return DefWindowProc(hwnd, message, wparam, lparam);

		}

	}
}
void FillOpenParams(OPENFILENAME &open_params, HWND hwnd, char *filter, char *file_name)
{
	/** Initialize the "open_params" fields we need to for opening a .bmp file **/

	open_params.lStructSize = sizeof(OPENFILENAME); // The size of the struct
	open_params.hwndOwner = hwnd; // The "window owner" of this dialog box
	open_params.lpstrFilter = filter;

	open_params.lpstrFile = file_name; // This is a pointer to a filename used
									   // to initialize the dialog box -- Since our
									   // "file_name" equals NULL, No "default filename"
									   // will be used

	open_params.nMaxFile = BUFF_MAX; // The size of the "file_name" buffer

	open_params.lpstrInitialDir = NULL; // Now this already equals NULL, but I'm setting
										// it again just so I can comment it -- This means
										// (for Windows NT 5.0/Windows 98 and later)
										// that if the "current directory" contains any files of 
										// the specified filter type(s) this directory will be the 
										// initial directory.  Otherwise it will be the "personal
										// files directory" for whoever's running the program 

	open_params.lpstrFileTitle = NULL; // Again it's already NULL but what this is, is the 
									   // title of the dialog box -- Since we're setting it
									   // to NULL it will use the default title ("Open" in 
									   // our case)

									   // As you can imagine, there is a plethora of flags that can be set for the dialog box
									   // I'll break down what these four mean
									   // OFN_FILEMUSTEXIST -- Only allows the typing of existing files in the File Name 
									   //						entry field
									   // OFN_PATHMUSTEXIST -- Allows the user to ONLY type valid paths and filenames
									   // OFN_NOCHANGEDIR -- Changes the current directory back to it's original value
									   //					  if the user changes the directory while searching for files
									   // OFN_HIDEREADONLY -- This hides the "read only" check box (the ability to open files
									   //					   as "read only" is possible)					
	open_params.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR
		| OFN_HIDEREADONLY;

	// Okay we've filled what we wanted to
}


void HandleFiles(WPARAM wParam) 
{
	// DragQueryFile() takes a LPWSTR for the name so we need a TCHAR string
	TCHAR szName[MAX_PATH];

	// Here we cast the wParam as a HDROP handle to pass into the next functions
	HDROP hDrop = (HDROP)wParam;

	// This functions has a couple functionalities.  If you pass in 0xFFFFFFFF in
	// the second parameter then it returns the count of how many filers were drag
	// and dropped.  Otherwise, the function fills in the szName string array with
	// the current file being queried.
	int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

	// Here we go through all the files that were drag and dropped then display them
	for (int i = 0; i < count; i++)
	{
		// Grab the name of the file associated with index "i" in the list of files dropped.
		// Be sure you know that the name is attached to the FULL path of the file.
		DragQueryFile(hDrop, i, szName, MAX_PATH);

		// Bring up a message box that displays the current file being processed
		MessageBox(GetForegroundWindow(), szName, "Current file received", MB_OK);
	}

	// Finally, we destroy the HDROP handle so the extra memory
	// allocated by the application is released.
	DragFinish(hDrop);
}

int DisplayResourceNAMessageBox()
{
	//MessageBox(NULL, (LPCSTR)"Данная программа демонстрирует алгоритм шифрования RSA. \nАвтор - Максим Сухоцкий.", (LPCSTR)"О программе", MB_YESNO | MB_ICONINFORMATION);

	int msgboxID = MessageBox(
		NULL,
		"Данная программа демонстрирует алгоритм шифрования RSA. \nПерейти на сайт автора?",
		"О программе",
		MB_YESNO
		);

	switch (msgboxID)
	{
	case IDYES:
		ShellExecute(NULL, "open", "http://vk.com/llltt",
			0, 0, SW_SHOWNORMAL);
		break;
	}

	return msgboxID;
}