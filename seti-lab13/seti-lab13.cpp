#include <windows.h>
#include <iostream>

using namespace std;

void Error()
{
	cout << "Error: " << GetLastError() << endl;
	cout << "Please enter somethong for exit.";
	cin.get();
	return;
}

int main()
{
	setlocale(LC_ALL, "rus");

	bool isRunning = true;
	string   machineName;
	LPCWSTR   pipeName;
	HANDLE   hNamedPipe;
	DWORD   dwBytesWritten;    // для количества записанных байтов
	DWORD   dwBytesRead;       // для количества прочитанных байтов
	char     pchMessage[80];   // для сообщения
	int    nMessageLength;     // длина сообщения

	cout << "Please enter name of server machine: ";
	cin >> machineName;
	//machineName = "\\\\" + machineName + "\\pipe\\demo_pipe";
	machineName = "\\\\.\\pipe\\demo_pipe";
	//pipeName = machineName.c_str();

	wstring full_name = wstring(machineName.begin(), machineName.end());
	pipeName = full_name.c_str();

	hNamedPipe = CreateFile(
		pipeName,                           // имя канала
		GENERIC_READ | GENERIC_WRITE,       // читаем и записываем в канал
		FILE_SHARE_READ | FILE_SHARE_WRITE, // разрешаем чтение и запись 
		NULL,                  // безопасность по умолчанию
		OPEN_EXISTING,         // открываем существующий канал
		FILE_ATTRIBUTE_NORMAL, // атрибуты по умолчанию
		NULL);                 // дополнительных атрибутов нет

	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		cout << "Connection was denied." << endl;
		Error();
		return 0;
	}

	cout << "Connection was succesfull!" << endl;

	if (!ReadFile(
		hNamedPipe,      // дескриптор канала
		pchMessage,      // данные
		sizeof(pchMessage),  // размер данных
		&dwBytesRead,    // количество записанных байтов
		NULL))           // синхронное чтение
	{
		cout << "Message wasn't delivered. " << endl;
	}
	//Получение выводимых символов
	string Output = pchMessage;
	//Получение цвета консоли
	while (isRunning) {


		if (!ReadFile(
			hNamedPipe,      // дескриптор канала
			pchMessage,      // данные
			sizeof(pchMessage),  // размер данных
			&dwBytesRead,    // количество записанных байтов
			NULL))           // синхронное чтение
		{
			cout << "essage wasn't delivered. " << endl;
		}

		if (strcmp(pchMessage, "Color") == 0)
		{
			if (!ReadFile(
				hNamedPipe,      // дескриптор канала
				pchMessage,      // данные
				sizeof(pchMessage),  // размер данных
				&dwBytesRead,    // количество записанных байтов
				NULL))           // синхронное чтение
			{
				cout << "Message wasn't delivered. " << endl;
			}
			system(pchMessage);
		}
		else if (strcmp(pchMessage, "Coord") == 0)
		{
			for (int i = 0; i < rand() % 5; i++)
			{
				cout << endl;
			}

			for (int i = 0; i < rand() % 10; i++)
			{
				cout << "\t";
			}

			cout << Output;
		}
		else if (strcmp(pchMessage, "cls") == 0)
		{
			system("cls");
		}
		else if (strcmp(pchMessage, "Exit") == 0)
		{
			cout << "Finish." << endl;
			CloseHandle(hNamedPipe);
			bool isRunning = false;
			system("pause");
			return 0;
		}
	}
}

/*
13 РАБОООООООООООООООООООООООООТАААААААААААА
//Клиент канала

#include <windows.h>
#include <iostream>

using namespace std;

void Error()
{
    cout << "Status code: " << GetLastError() << endl;
    cout << "Please press any key for exit.";
    cin.get();
    return;
}

int main()
{
    HANDLE hNamedPipe;
    int data;
    setlocale(LC_ALL, "rus");

    // связываемся с именованным каналом
    hNamedPipe = CreateFile(
        L"\\\\.\\pipe\\demo_pipe",        // имя канала
        GENERIC_WRITE,      // записываем в канал
        FILE_SHARE_READ,    // разрешаем одновременное чтение из канала
        NULL,               // защита по умолчанию
        OPEN_EXISTING,      // открываем существующий канал
        0,                  // атрибуты по умолчанию
        NULL                // дополнительных атрибутов нет
    );

    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        cout << "error wgile connecting to server." << endl;
        Error();
        return 0;
    }

    // пишем в именованный канал
    cout << "Connection was sucesfull!" << endl;
    cout << "Please enter for exit '999'" << endl;
    cout << "Enter message for server: " << endl << endl;
    do
    {
        cin >> data;

        DWORD dwBytesWritten;
        if (!WriteFile(
            hNamedPipe,        // дескриптор канала
            &data,                // данные
            sizeof(data),         // размер данных
            &dwBytesWritten,   // количество записанных байтов
            NULL               // синхронная запись
        ))
        {
            // ошибка записи
            cout << "Error while delivering data to server." << endl;
            Error();
            return 0;
        }
        cout << "Message '" << data << "' was delivered." << endl;
        Sleep(1000);

    } while (data != 999);

    CloseHandle(hNamedPipe);
    cout << "Client get data!" << endl;
    cout << "Please press any key for exit.";
    cin.get();

    return 0;
}
 */