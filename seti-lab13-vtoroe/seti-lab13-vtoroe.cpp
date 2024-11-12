
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>;
#include <windows.h>

using namespace std;

HANDLE wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE rHnd = GetStdHandle(STD_INPUT_HANDLE);

HANDLE   hNamedPipe;
DWORD   dwBytesRead;     // для количества прочитанных байтов
DWORD   dwBytesWrite;    // для количества записанных байтов

void Error()
{
    cout << "Последняя ошибка: " << GetLastError() << endl;
    cout << "Нажмите любую клавишу для выхода.";
    cin.get();
    return;
}

void Send_Message(char* pchMessage)
{

    int nMessageLength;
    nMessageLength = strlen(pchMessage) + 1;

    if (!WriteFile(
        hNamedPipe,      // дескриптор канала
        pchMessage,      // адрес буфера для вывода данных
        nMessageLength,  // количество записываемых байтов
        &dwBytesWrite,   // количество записанных байтов
        NULL             // передача данных синхронная
    ))
    {
        cout << "Error." << endl;
        CloseHandle(hNamedPipe);
        Error();
    }
}

char ColorADD()
{
    cout << "Возможные цвета: " << endl;
    cout << "0 - чёрный       6  - коричневый          B - светло-голубой" << endl
        << "1 - синий        7  - светло-серый        C - светло-красный" << endl
        << "2 - зелёный      8  - тёмно-серый         D - светло-пурпурный" << endl
        << "3 - голубой      9  - светло-синий        E - жёлтый" << endl
        << "4 - красный      A - светло-зелёный       F - белый" << endl
        << "5 - пурпурный " << endl << endl;

    bool invalid_color = true;
    char color;
    do
    {
        cin >> color;
        switch (color)
        {
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9': case 'A': case 'B':
        case 'C': case 'D': case 'E': case 'F':
            invalid_color = false;
            break;
        default:
            cout << "Incorrect color." << endl;
            cout << "Please enter color one more time." << endl;
        }
    } while (invalid_color);
    return color;
}

int main()
{
    char AvailableColors[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    bool isRunning = true;
    int i = 0;

    DWORD numEvents = 0; // Количество непрочитанных сообщений
    DWORD numEventsRead = 0; // Количество прочитанных сообщений


    setlocale(LC_ALL, "rus");
    /*
    hNamedPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\demo_pipe",       // имя канала
        PIPE_ACCESS_DUPLEX,              // читаем из канала и пишем в канал
        PIPE_TYPE_MESSAGE | PIPE_WAIT,   // синхронная передача сообщений
        10,         // максимальное количество экземпляров канала 
        0,         // размер выходного буфера по умолчанию
        0,         // размер входного буфера по умолчанию
        INFINITE,  // клиент ждет связь бесконечно долго
        NULL       // безопасность по умолчанию
    );
    */
    hNamedPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\demo_pipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        10,
        0,
        0,
        INFINITE,
        NULL
    );

    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        cout << "Chanel creature" << endl;
        Error();
        return 0;
    }

    cout << "Server was created succesfully!" << endl;
    cout << "Waiting for client connection." << endl;

    if (!ConnectNamedPipe(
        hNamedPipe,    // дескриптор канала
        NULL      // связь синхронная
    ))
    {
        cout << "Error while connecting." << endl;
        CloseHandle(hNamedPipe);
        Error();
        return 0;
    }

    char Char_Request[80];

    cout << "Enter output symbols: " << endl;
    cin >> Char_Request;
    Send_Message(Char_Request);

    Send_Message((char*)"Color");
    cout << "Enter color of console: " << endl;
    Char_Request[0] = 0;
    strcpy(Char_Request, "Color ");
    Char_Request[6] = ColorADD();

    cout << "Enter text color: " << endl;
    Char_Request[7] = ColorADD();
    Char_Request[8] = 0;

    Send_Message(Char_Request);
    cout << "Color is being set up. " << endl;

    cout << "Choose an action: " << endl << endl
        << "'Enter'      – Display symbols by coords" << endl
        << "'Space'      – Set new color for console and symbols " << endl
        << "'Backspace'  – Clear display " << endl
        << "'Escape'     – Exit " << endl;

    do
    {
        Char_Request[0] = 0;

        // выделение памяти для хранения данных о событиях
        GetNumberOfConsoleInputEvents(rHnd, &numEvents);
        if (numEvents != 0) {
            INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
            ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);

            for (DWORD i = 0; i < numEventsRead; ++i) {

                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown)
                {
                    switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode)
                    {

                    case VK_ESCAPE:

                        Send_Message((char*)"Exit");
                        isRunning = false;
                        break;

                    case VK_BACK:

                        Send_Message((char*)"cls");
                        cout << "The screen was being cleared. " << endl;
                        break;

                    case VK_SPACE:

                        Send_Message((char*)"Color");
                        //Формируем запрос с рандомными цветами
                        strcpy(Char_Request, "Color ");
                        Char_Request[6] = AvailableColors[rand() % 15];
                        Char_Request[7] = AvailableColors[rand() % 15];
                        Char_Request[8] = 0;

                        Send_Message(Char_Request);
                        cout << "Color is being changed. " << endl;
                        break;

                    case VK_RETURN:
                        Send_Message((char*)"Coord");
                        cout << "Symbol was typed. " << endl;
                        break;
                    }
                }
            }
        }
    } while (isRunning);

    CloseHandle(hNamedPipe);
    system("pause");
    return 0;
}

/*
//13 РАБОТТТТТТТТТТТААААААААААААААА

//Сервер канала

#include <windows.h>
#include <iostream>

using namespace std;

void Error()
{
    cout << "Status code: " << GetLastError() << endl;
    cout << "Please enter something for exit.";
    cin.get();
    return;
}

int main()
{
    HANDLE hNamedPipe;
    bool working = true;
    setlocale(LC_ALL, "rus");

    hNamedPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\demo_pipe",     // имя канала
        PIPE_ACCESS_INBOUND,           // чтение
        PIPE_TYPE_MESSAGE | PIPE_WAIT, // синхронная передача сообщений
        1,         // максимальное количество экземпляров канала
        0,         // размер выходного буфера по умолчанию
        0,         // размер входного буфера по умолчанию
        INFINITE,  // клиент ждет связь бесконечно долго
        NULL       // защита по умолчанию
    );


    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        cout << "Server hasnt been created." << endl;
        Error();
        return 0;
    }

    cout << "Server was created!!" << endl;
    cout << "Waiting to server connection." << endl;

    if (!ConnectNamedPipe(
        hNamedPipe,    // дескриптор канала
        NULL           // связь синхронная
    ))
    {
        cout << "Connection was failed." << endl;
        Error();
        return 0;
    }

    // читаем данные из канала
    do
    {
        int nData;
        DWORD dwBytesRead;
        if (!ReadFile(
            hNamedPipe,    // дескриптор канала
            &nData,        // адрес буфера для ввода данных
            sizeof(nData), // число читаемых байтов
            &dwBytesRead,  // число прочитанных байтов
            NULL           // передача данных синхронная
        ))
        {
            cout << "Reading from file was denied." << endl;
            Error();
            return 0;
        }
        // выводим прочитанные данные на консоль
        cout << "Message '" << nData << "' from server." << endl << endl;
        if (nData == 999) working = false;
    } while (working);

    CloseHandle(hNamedPipe);
    cout << "Server has read data" << endl;
    cout << " Press any key for exit.";
    cin.get();

    return 0;
}


 */