/*main.cpp
Nathan Damon
2/13/2021
This is the main.cpp for the server/parent
This video was a great help - > https://www.youtube.com/watch?v=Dk6v8wIpJWk
*/

#define OLC_PGE_APPLICATION

#include <iostream>
using std::cout;
using std::endl;
#include <Windows.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include "olc3DGraphicsChess.h"

// Moves the piece a given distance
template<typename pieceOrBoard>
void objectMov(pieceOrBoard& p, const float arr[3]/*deltaX, const float deltaY, const float deltaZ*/);


GameBoard game[1]; // Using an array got me the const void* that I needed to move the memory

int main()
{
    /*olc3DGraphics demo(game[0], 1);
    if (demo.Construct(512, 480, 1, 1))
        demo.Start();
    return 0;*/


    cout << "\t\t.....FILEMAPPING SERVER or PARENT....." << endl;
    cout << endl;

    //Local Variable Definitions

    cout << sizeof(GameBoard) << endl;

    HANDLE  hFileMap;
    BOOL    bResult;
    GameBoard*   mappedViewChess = NULL;
    //char    Buffer[256] = "Hello From File Map Server";
    //10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
    //
    //Tester a = { 10, 9, 8, 7, 6, 1 };
   // cout << a.access << " " << a.arr[0] << " " << a.arr[4] << endl;
   // cout << sizeof(a.arr) << endl;
   // cout << sizeof(a.access) << endl;
   // cout << sizeof(a) << endl;
    //Tester buffer[3] = { {10, 9, 8, 7, 6, 0.5f} , {20, 19, 18, 17, 16, 0.75f} , {30, 29, 28, 27, 26, 0.825f} };
    size_t  szGame = sizeof(game);



    // STEP 1 : Create File Map
    hFileMap = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        szGame,
        L"Local\\MyFileMap");


    if (hFileMap == FALSE)
    {
        cout << "CreateFileMapping Failed & Error NO - " << GetLastError() << endl;
    }
    cout << "CreateFileMapping Success" << endl;


    // STEP 2 : Map View of File
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



    //STEP 3 : Copy Memory Function
    MoveMemory(mappedViewChess, game, szGame); // < --
    //CopyMemory(mappedViewAddress, buffer, szBuffer);

    mappedViewChess[0]._turn = 1; // Player one goes first
    mappedViewChess[0]._pieceInMotion = false;
    mappedViewChess[0]._move._pieceTypeNTeam = 0;

    int currentTurn = -1; // -1 is server's turn
    int tileMovingTo = -1;
    TileData newTileInfo = { false, 0 }; // 0 is invalid type and team
    int turnHold = 2; // Player1 just went first so player 2 goes next - - >  1 = player 1, 2 = player 2
    //AttemptedMove actionInput;

    bool run = true;
    // Use chrono to time movements
    float totalTime = 0.0f;
    float timeHold = 0.0f;
    while (run)
    {
        if (mappedViewChess[0]._turn == -1)
            run = false;
        if (mappedViewChess[0]._turn == 0)
        {
            totalTime += mappedViewChess[0]._tilesAndTime.elapsedTime; // Add the time delta
            if (totalTime != timeHold)
            {
                timeHold = totalTime;
                if (mappedViewChess[0]._tilesAndTime.newTile != tileMovingTo)
                {
                    newTileInfo = checkTileForPiece(mappedViewChess[0], mappedViewChess[0]._tilesAndTime.newTile);
                    tileMovingTo = mappedViewChess[0]._tilesAndTime.newTile;
                }
                if (!newTileInfo.piecePresent || mappedViewChess[0]._move._pieceTypeNTeam / 7 != newTileInfo.pieceTypeNTeam / 7) // Check for empty tile or different teams
                {
                    if (newTileInfo.piecePresent) // Removes the piece at destination
                    {
                        auto playerPieces = [](Player& p, const TileNTime& tilesAndTime) {
                            float arr[3] = { 0.0f, 0.0f, -1.0f };
                            if (tilesAndTime.newTile < 0 || tilesAndTime.newTile > 63)
                            {
                                std::cout << "Error with playerPieces lambda in Server Main" << std::endl;
                                return;
                            }
                            else
                            {
                                for (int i = 0; i < 16; i++)
                                {
                                    if (p._pieces[i]._position == tilesAndTime.newTile)
                                    {
                                        objectMov(p._pieces[i], arr);
                                        p._pieces[i]._position = -1;
                                        return;
                                    }
                                }
                            }
                        };
                        // Makes the piece disappear (moves it below the board) as the new piece is about to decend on its former tile
                        if (mappedViewChess[0]._tilesAndTime.timeSince > 3.0f)
                        {
                            if (newTileInfo.pieceTypeNTeam > 0 && newTileInfo.pieceTypeNTeam < 7)
                                playerPieces(mappedViewChess[0]._pOne, mappedViewChess[0]._tilesAndTime);
                            else if (newTileInfo.pieceTypeNTeam > 6 && newTileInfo.pieceTypeNTeam < 13)
                                playerPieces(mappedViewChess[0]._pTwo, mappedViewChess[0]._tilesAndTime);
                        }
                    }
                    if (mappedViewChess[0]._pieceInMotion)
                    {
                        mappedViewChess[0]._pieceInMotion = getPieceAndMoveIt(mappedViewChess[0], mappedViewChess[0]._tilesAndTime);
                        if (!mappedViewChess[0]._pieceInMotion)
                        {
                            mappedViewChess[0]._tilesAndTime.timeSince = 0.0f;
                            if (turnHold == 1)
                            {
                                turnHold = 2;
                                mappedViewChess[0]._turn = 1; // Give turn to player 1
                            }
                            else //(turnHold == 2)
                            {
                                turnHold = 1;
                                mappedViewChess[0]._turn = 2; // Give turn to player 2
                            }
                            cout << "time = " << totalTime << endl;
                        }
                    }
                }
                mappedViewChess[0]._tilesAndTime.elapsedTime = 0.0f; // Set to zero to prevent crazy speed objects
            }
        }
    }



    //auto dataHold = (*mappedViewAddress).access = 1.0f;
    //int data = 5;
    //cout << "Buffer -> " << mappedViewAddress << endl;
    //(*(mappedViewAddress + 2)).access = 0; // Set access to 0
    //for (int i = 0; i < 3; i++)
    //{
    //    cout << "[" << i << "] = ";
    //    cout << "Access: " << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).access << " Array = { ";
    //    for (int k = 0; k < 5; k++)
    //        cout << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).arr[k] << " ";
    //    cout << "}" << endl;
    //}
    //while (run)
    //{        
    //    Sleep(200);
    //    if ((*(mappedViewAddress + 2)).access != 0)
    //    {
    //        (*(mappedViewAddress + 2)).access = 0;
    //        system("cls");
    //        cout << "\t\t.....FILEMAPPING SERVER or PARENT....." << endl;
    //        cout << endl;
    //        cout << "Buffer -> " << mappedViewAddress << endl;
    //        dataHold = (*mappedViewAddress);
    //        cout << "Data is now: " << endl;
    //        for (int i = 0; i < 3; i++)
    //        {
    //            cout << "[" << i << "] = ";
    //            cout << "Access: " << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).access << " Array = { ";
    //            for (int k = 0; k < 5; k++)
    //                cout << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).arr[k] << " ";
    //            cout << "}" << endl;
    //        }
    //        /*for (int i = 0; i < 11; i++)
    //            cout << "[" << i << "] = " << (*(mappedViewAddress + sizeof(int) * i)) << endl;*/

    //(*mappedViewAddress).push_back(9);
    //auto dataHold = (*mappedViewAddress).access = 1.0f;
    //int data = 5;
    //vector<char> nums = {
    //    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    //};
    //cout << "Buffer -> " << mappedViewAddress << endl;
    //(*(mappedViewAddress + 2)).access = 0; // Set access to 0
    //for (int i = 0; i < 3; i++)
    //{
    //    cout << "[" << i << "] = ";
    //    cout << "Access: " << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).access << " Array = { ";
    //    for (int k = 0; k < 5; k++)
    //        cout << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).arr[k] << " ";
    //    cout << "}" << endl;
    //}
    //bool run = true;
    //while (run)
    //{        
    //    //Sleep(200);
    //    if ((*(mappedViewAddress + 2)).access != 0)
    //    {
    //        (*(mappedViewAddress + 2)).access = 0;
    //        system("cls");
    //        cout << "\t\t.....FILEMAPPING SERVER or PARENT....." << endl;
    //        cout << endl;
    //        cout << "Buffer -> " << mappedViewAddress << endl;
    //        //dataHold = (*mappedViewAddress);
    //        cout << "Data is now: " << endl;
    //        for (int i = 0; i < 3; i++)
    //        {
    //            cout << "[" << i << "] = ";
    //            cout << "Access: " << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).access << " Array = { ";
    //            for (int k = 0; k < 5; k++)
    //                cout << (*(mappedViewAddress + i/* i here is incrementing the pointer mappedViewAddress by its size*/)).arr[k] << " ";
    //            cout << "}" << endl;
    //        }
    //        /*for (int i = 0; i < 11; i++)
    //            cout << "[" << i << "] = " << (*(mappedViewAddress + sizeof(int) * i)) << endl;*/

    //    }
    //    if ((*mappedViewAddress).access == 0)
    //        break;

    //    ////cout << "Buffer -> " << lpBuffer << endl;
    //    //
    //    ////Sleep(3000);
    //    //if (mappedViewAddress[0] != dataHold)
    //    //{
    //    //    dataHold = mappedViewAddress[0];
    //    //    cout << "There is change" << endl;
    //    //    if (dataHold == 'u')
    //    //    {
    //    //        cout << "Received an \"up\"" << endl;
    //    //        data++;
    //    //        if (data > 9)
    //    //            data = 9;
    //    //        char BufferUP = nums[data];
    //    //        CopyMemory(mappedViewAddress, &BufferUP, szBuffer);
    //    //    }
    //    //    else if (dataHold == 'd')
    //    //    {
    //    //        cout << "Received a \"down\"" << endl;
    //    //        data--;
    //    //        if (data < 0)
    //    //            data = 0;
    //    //        char BufferDown = nums[data];
    //    //        CopyMemory(mappedViewAddress, &BufferDown, szBuffer);
    //    //    }
    //    //    else if (dataHold < '0' || dataHold > '9')
    //    //    {
    //    //        cout << "User has passed invalid response" << endl;
    //    //        run = false;
    //    //    }
    //    //    cout << "Data -> " << data << endl;
    //    //}

    //    //if (Buffer[3] != dataHold)
    //    //{
    //    //    dataHold = Buffer[3];
    //    //    if (strcmp(Buffer, "up") == 0)
    //    //    {
    //    //        cout << "Received an \"up\"" << endl;
    //    //        data++;
    //    //        if (data > 9)
    //    //            data = 9;
    //    //        char BufferUP[256] = "Data has increased to ";
    //    //        const char constData = nums[data];
    //    //        strcat_s(BufferUP, &constData);
    //    //        CopyMemory(lpBuffer, BufferUP, sizeof(BufferUP));
    //    //    }
    //    //    else if (strcmp(Buffer, "down") == 0)
    //    //    {
    //    //        cout << "Received a \"down\"" << endl;
    //    //        data--;
    //    //        if (data < 0)
    //    //            data = 0;
    //    //        char BufferDown[256] = "Data has decreased to ";
    //    //        const char constData = nums[data];
    //    //        strcat_s(BufferDown, &constData);
    //    //        CopyMemory(lpBuffer, BufferDown, sizeof(BufferDown));
    //    //    }
    //    //    else if (strcmp(Buffer, "No response") == 0)
    //    //    {
    //    //        cout << "User has passed invalid response" << endl;
    //    //        run = false;
    //    //    }
    //    //}
    //}


    //STEP 4 : Unmap View Of File
    bResult = UnmapViewOfFile(mappedViewChess);
    if (bResult == FALSE)
    {
        cout << "UnMapViewOfFile Failed & Error NO - " << GetLastError() << endl;
    }
    cout << "UnMapViewOfFile Success" << endl;

    int stop = 0;
    std::cin >> stop;
    return 0;
}


// Moves the piece a given distance
template<typename pieceOrBoard>
void objectMov(pieceOrBoard& p, const float arr[3]/*deltaX, const float deltaY, const float deltaZ*/)
{
    //float arr[3] = { deltaX, deltaY, deltaZ };
    for (int i = 0; i < p._triCount; i++)
    {
        p._tris[i] += arr;
    }
}