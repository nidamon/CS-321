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
#include <chrono>
using std::chrono::steady_clock;
#include "olc3DGraphicsChess.h"

GameBoard game[1]; // Using an array got me the const void* that I needed to move the memory

int main()
{
    cout << "\t\t.....FILEMAPPING SERVER or PARENT....." << endl;
    cout << endl;

    //Local Variable Definitions

    cout << sizeof(GameBoard) << endl;

    HANDLE  hFileMap;
    BOOL    bResult;
    GameBoard*   mappedViewChess = NULL;
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
    MoveMemory(mappedViewChess, game, szGame);

    mappedViewChess[0]._turn = 1; // Player one goes first
    mappedViewChess[0]._pieceInMotion = false;
    mappedViewChess[0]._move._pieceTypeNTeam = 0;

    int currentTurn = -1; // -1 is server's turn
    int tileMovingTo = -1;
    TileData newTileInfo = { false, 0 }; // 0 is invalid type and team
    int turnHold = 2; // Player1 just went first so player 2 goes next - - >  1 = player 1, 2 = player 2

    bool run = true;

    bool gotTileDestination = false;
    // Use chrono to time movements
    //bool chronoStart = true;
    //steady_clock::time_point timeStart;
    //std::chrono::duration<float> timeSinceStart;
    //std::chrono::duration<float> timeElapsed;

    mappedViewChess[0]._tilesAndTime.elapsedTime = 0.05f;
    float squareDimension = 9.9675f;
    while (run)
    {
        Sleep(0010);
        if (mappedViewChess[0]._turn == -1)
            run = false;
        if (mappedViewChess[0]._turn == 0)
        {
            //if (chronoStart)
            //{
            //    // Get exact movement distances
            //    mappedViewChess[0]._move._xDist = squareDimension * float((int)mappedViewChess[0]._tilesAndTime.oldTile % 8 - (int)mappedViewChess[0]._tilesAndTime.newTile % 8);
            //    mappedViewChess[0]._move._yDist = squareDimension * float((int)mappedViewChess[0]._tilesAndTime.oldTile / 8 - (int)mappedViewChess[0]._tilesAndTime.newTile / 8);
            //    cout << mappedViewChess[0]._move._xDist << ", " << mappedViewChess[0]._move._yDist << endl;
            //    if (mappedViewChess[0]._move._yDist < 0.0f)
            //        mappedViewChess[0]._move._yDist = -mappedViewChess[0]._move._yDist;
            //    if (mappedViewChess[0]._move._xDist < 0.0f)
            //        mappedViewChess[0]._move._xDist = -mappedViewChess[0]._move._xDist;
            //    cout << mappedViewChess[0]._move._xDist << ", " << mappedViewChess[0]._move._yDist << endl;


            //    timeStart = steady_clock::now();
            //    timeSinceStart.zero(); // Reset clock
            //    //cout << timeSinceStart.count() << endl; // check the clock reset REMOVE THIS
            //    chronoStart = false;
            //}
            //if (!chronoStart)
            //{
            //    timeElapsed.zero();
            //    steady_clock::time_point checkTime = steady_clock::now();
            //    timeElapsed = std::chrono::duration_cast<std::chrono::duration<float>>(checkTime - timeStart);
            //    timeStart = steady_clock::now();
            //    //timeSinceStart += timeElapsed; // Increment the time since beginning the action

            //    // Change the time varibles in tilesAndTime
            //    mappedViewChess[0]._tilesAndTime.elapsedTime = 0.05f;//timeElapsed.count();
            //    mappedViewChess[0]._tilesAndTime.timeSince += 0.05f;//timeElapsed.count();//timeSinceStart.count()*1000;
            //    //cout << timeElapsed.count() << endl;
            //}
            //if (timeElapsed.count() != 0)
            //{
                mappedViewChess[0]._tilesAndTime.timeSince += 0.05f;
                if (!gotTileDestination/*mappedViewChess[0]._tilesAndTime.newTile != tileMovingTo*/)
                {
                    newTileInfo = checkTileForPiece(mappedViewChess[0], mappedViewChess[0]._tilesAndTime.newTile);
                    tileMovingTo = mappedViewChess[0]._tilesAndTime.newTile;
                    gotTileDestination = true;
                }
                if (!newTileInfo.piecePresent || mappedViewChess[0]._move._pieceTypeNTeam / 7 != newTileInfo.pieceTypeNTeam / 7) // Check for empty tile or different teams
                {
                    if (newTileInfo.piecePresent) // Removes the piece at destination
                    {
                        auto playerPieces = [](Player& p, const TileNTime& tilesAndTime) {
                            float arr[3] = { 0.0f, 0.0f, -25.0f};
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
                                        cout << "Piece has fallen." << endl;
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
                            if (newTileInfo.pieceTypeNTeam > 6 && newTileInfo.pieceTypeNTeam < 13)
                                playerPieces(mappedViewChess[0]._pTwo, mappedViewChess[0]._tilesAndTime);
                        }
                    }
                    if (mappedViewChess[0]._pieceInMotion)
                    {
                        mappedViewChess[0]._pieceInMotion = getPieceAndMoveIt(mappedViewChess[0], mappedViewChess[0]._tilesAndTime, mappedViewChess[0]._move);
                        if (!mappedViewChess[0]._pieceInMotion)
                        {
                            mappedViewChess[0]._tilesAndTime.timeSince = 0.0f;
                            if (turnHold == 1)
                            {
                                turnHold = 1; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                mappedViewChess[0]._turn = 1; // Give turn to player 1
                            }
                            else //(turnHold == 2)
                            {
                                turnHold = 1;
                                mappedViewChess[0]._turn = 1; // Give turn to player 2
                            }
                            /*cout << "time = " << timeSinceStart.count() << endl;
                            chronoStart = true;*/
                            gotTileDestination = false;
                        }
                    }
                }
                else
                {
                    mappedViewChess[0]._turn = turnHold;
                }
                //mappedViewChess[0]._tilesAndTime.elapsedTime = 0.0f; // Set to zero to prevent crazy speed objects
        }
    }

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