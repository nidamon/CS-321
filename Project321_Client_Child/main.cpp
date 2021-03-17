/*main.cpp
Nathan Damon
2/13/2021
This is the main.cpp for the client/child process
This video was a great help - > https://www.youtube.com/watch?v=Dk6v8wIpJWk
*/

#define OLC_PGE_APPLICATION

#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <Windows.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <sstream>
using std::stringstream;
//#include "olc3DGraphicsChess.h"
#include "../Project321_Server_Parent/olc3DGraphicsChess.h"
#include "../Project321_Server_Parent/olc3DGraphicsChess.cpp"

int main()
{
    cout << "\t\t....FILEMAPPING CLIENT or CHILD...." << endl;
    cout << endl;

    // Local Variable Definitions
    HANDLE  hFileMap;
    BOOL    bResult;
    //PCHAR   lpBuffer = NULL;
    GameBoard* mappedViewChess = NULL;
    size_t  szGame = sizeof(GameBoard);


    // STEP 1 : OpenFileMapping
    hFileMap = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        L"Local\\MyFileMap");

    if (hFileMap == NULL)
    {
        cout << "OpenFileMap Failed & Error NO - " << GetLastError() << endl;
    }
    cout << "OpenFileMap Success" << endl;


    //STEP 2 : MapViewOfFile
    mappedViewChess = (GameBoard*)MapViewOfFile(
        hFileMap,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        szGame);

    if (mappedViewChess == NULL)
    {
        cout << "MapViewOfFile Failed & Error NO - " << GetLastError() << endl;
    }
    cout << "MapViewOfFile Success " << endl;


    //STEP 3 : Reading the data from File Map Object
    cout << "DATA READING FROM SERVER or PARENT PROCESS -> " << mappedViewChess << endl;

    olc3DGraphics demo(mappedViewChess[0], 1);
    if (demo.Construct(512, 480, 1, 1))
        demo.Start();
    return 0;


    //vector<int>* p* = 0x011A0000;
    //auto dataHold = *mappedViewAddress;
    //string response;
    //int category = 1;
    //int item = 1;
    //float newValue = 1;
    //while (true)
    //{
    //    //cout << "DATA READING FROM SERVER or PARENT PROCESS -> " << (*mappedViewAddress).access << endl;
    //    for (int i = 0; i < 3; i++)
    //    {
    //        cout << "[" << i << "] = ";
    //        cout << "Access: " << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).access << " Array = { ";
    //        for (int k = 0; k < 5; k++)
    //            cout << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).arr[k] << " ";
    //        cout << "}" << endl;
    //    }
    //    cout << "Respond with (category, item, value) (0 0 0 to quit): ";
    //    std::getline(cin, response);
    //    stringstream itemChange(response);
    //    itemChange >> category >> item >> newValue;
    //    // Bounds checking
    //    if (category > 2)
    //        category = 2;
    //    if (category < 0)
    //        category = 0;
    //    if (item > 5)
    //        item = 5;
    //    if(item < 0)
    //        item = 0;
    //    if(item == 5)
    //        cout << category << " " << item << " " << newValue << endl;
    //    else
    //        cout << category << " " << item << " " << (int)newValue << endl;
    //    //char Buffer[256] = "No response";
    //    if (item != 5)
    //        (*(mappedViewAddress + category)).arr[item] = (int)newValue;
    //    else
    //        (*(mappedViewAddress + category)).access = newValue;
    //    // Let the server know the data has changed
    //    (*(mappedViewAddress + 2)).access = 1; 
    //    if (category == 0 && item == 5 && newValue == 0)
    //    {
    //        cout << "Exiting with 0 5 0" << endl;
    //        break;
    //        //char BufferUP[256] = "up";
    //        //CopyMemory(lpBuffer, BufferUP, sizeof(BufferUP));
    //    }
    //}

    //STEP 4 : UnMapViewOfFile
    bResult = UnmapViewOfFile(mappedViewChess);
    if (bResult == FALSE)
    {
        cout << "UnMapViewOfFile Failed & Error No - " << GetLastError() << endl;
    }
    cout << "UnMapViewOfFile Success" << endl;


    //STEP 5 : Close Handle
    CloseHandle(hFileMap);

    int stop = 0;
    std::cin >> stop;
    return 0;
}