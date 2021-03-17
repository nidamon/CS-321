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

// Changes the player's color
//void colorChange(Player& p, const olc::Pixel newColor);

// Changes the piece's color
template<typename obj>
void subColorChange(obj& p, const olc::Pixel newColor);

// Moves the piece a given distance
template<typename pieceOrBoard>
void objectMov(pieceOrBoard& p, const float arr[3]/*deltaX, const float deltaY, const float deltaZ*/);

// Sets objects to 0 0 0
template<typename pieceOrBoard>
void vertexCorrection(pieceOrBoard& obj);

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
                if (!newTileInfo.piecePresent || mappedViewChess[0]._move._pieceTypeNTeam / 17 != newTileInfo.pieceTypeNTeam / 17) // Check for empty tile or different teams
                {
                    if (newTileInfo.piecePresent) // Removes the piece at destination
                    {
                        auto playerPieces = [](Player& p, int pieceType) {
                            float arr[3] = { 0.0f, 0.0f, -10.0f };
                            if (pieceType < 0)
                            {
                                std::cout << "Error with playerPieces lambda in Server Main" << std::endl;
                                return;
                            }
                            if (pieceType < 9)
                            {
                                objectMov(p._pawns[pieceType - 1], arr);
                                p._pawns[pieceType - 1]._position = -1; // Essentially invalid
                            }

                            else
                                switch (pieceType)
                                {
                                case 9:
                                    objectMov(p._rooks[0], arr);
                                    p._rooks[0]._position = -1; // Essentially invalid
                                    break;
                                case 10:
                                    objectMov(p._knights[0], arr);
                                    p._knights[0]._position = -1; // Essentially invalid
                                    break;
                                case 11:
                                    objectMov(p._bishops[0], arr);
                                    p._bishops[0]._position = -1; // Essentially invalid
                                    break;
                                case 12:
                                    objectMov(p._king, arr);
                                    p._king._position = -1; // Essentially invalid
                                    break;
                                case 13:
                                    objectMov(p._queen, arr);
                                    p._queen._position = -1; // Essentially invalid
                                    break;
                                case 14:
                                    objectMov(p._bishops[1], arr);
                                    p._bishops[1]._position = -1; // Essentially invalid
                                    break;
                                case 15:
                                    objectMov(p._knights[1], arr);
                                    p._knights[1]._position = -1; // Essentially invalid
                                    break;
                                case 16:
                                    objectMov(p._rooks[1], arr);
                                    p._rooks[1]._position = -1; // Essentially invalid
                                    break;
                                default:
                                    break;
                                }
                        };
                        // Makes the piece disappear (moves it below the board) as the new piece is about to decend on its former tile
                        if (mappedViewChess[0]._tilesAndTime.timeSince > 3.0f)
                        {
                            if (newTileInfo.pieceTypeNTeam > 0 && newTileInfo.pieceTypeNTeam < 17)
                                playerPieces(mappedViewChess[0]._pOne, newTileInfo.pieceTypeNTeam);
                            else if (newTileInfo.pieceTypeNTeam > 16 && newTileInfo.pieceTypeNTeam < 33)
                                playerPieces(mappedViewChess[0]._pTwo, newTileInfo.pieceTypeNTeam - 16);
                        }
                    }
                    if (mappedViewChess[0]._pieceInMotion)
                    {
                        mappedViewChess[0]._pieceInMotion = pieceInQuestion(mappedViewChess[0], mappedViewChess[0]._move._pieceTypeNTeam, mappedViewChess[0]._tilesAndTime);
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


// Changes the piece's color
template<typename obj>
void subColorChange(obj& p, const olc::Pixel newColor)
{
    for (int i = 0; i < p._triCount; i++)
        p._tris[i].color = newColor;
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

// Sets objects to 0 0 0
template<typename pieceOrBoard>
void vertexCorrection(pieceOrBoard& obj)
{
    float x = obj._tris[0].p[0].x;
    float y = obj._tris[0].p[0].y;
    float z = obj._tris[0].p[0].z;
    for (int i = 0; i < obj._triCount; i++)
    {
        // Find lowest x, y, and z
        for (int k = 0; k < 3; k++)
        {
            if (obj._tris[i].p[k].x < x)
                x = obj._tris[i].p[k].x;
            if (obj._tris[i].p[k].y < y)
                y = obj._tris[i].p[k].y;
            if (obj._tris[i].p[k].z < z)
                z = obj._tris[i].p[k].z;
        }
    }
    // Set the position to 0 0 0 
    float arr[3] = { -x + obj._xPos, -y + obj._yPos, -z + obj._zPos };
    objectMov(obj, arr);
}