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

    //int tileMovingTo = -1;
    TileData newTileInfo = { false, 0 }; // 0 is invalid type and team
    int turnHold = 2; // Player1 just went first so player 2 goes next - - >  1 = player 1, 2 = player 2

    bool run = true;

    bool gotTileDestination = false;

    mappedViewChess[0]._tilesAndTime.elapsedTime = 0.05f;
    float squareDimension = 9.9675f;
    while (run)
    {
        Sleep(0010);
        if (mappedViewChess[0]._turn == -1)
            run = false;
        if (mappedViewChess[0]._turn == 0)
        {
            mappedViewChess[0]._tilesAndTime.timeSince += 0.05f;
            if (!gotTileDestination)
            {
                newTileInfo = checkTileForPiece(mappedViewChess[0], mappedViewChess[0]._tilesAndTime.newTile);
                //tileMovingTo = mappedViewChess[0]._tilesAndTime.newTile;
                gotTileDestination = true;
            }
            if (!newTileInfo.piecePresent || mappedViewChess[0]._move._pieceTypeNTeam / 7 != newTileInfo.pieceTypeNTeam / 7) // Check for empty tile or different teams
            {
                if (newTileInfo.piecePresent) // Removes the piece at destination
                {
                    auto playerPieces = [](Player& p, const TileNTime& tilesAndTime) {
                        float arr[3] = { 0.0f, 0.0f, -25.0f };
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
                        // Set the old tile in the boardTiles to be empty
                        int oldTile = mappedViewChess[0]._tilesAndTime.oldTile;
                        mappedViewChess[0]._boardTiles[oldTile / 8][oldTile % 8] = { false, 0 };

                        // Set the new tile in the boardTiles to the new value
                        int newTile = mappedViewChess[0]._tilesAndTime.newTile;
                        int pTypeNTeam = mappedViewChess[0]._move._pieceTypeNTeam;
                        mappedViewChess[0]._boardTiles[newTile / 8][newTile % 8] = { true, pTypeNTeam };

                        mappedViewChess[0]._tilesAndTime.timeSince = 0.0f;

                        if (mappedViewChess[0]._pawnPromotion.hasBeenPromoted)
                        {
                            cout << "Server read pawnChange" << endl;
                            pawnChanger(mappedViewChess[0]._pawnPromotion.type, newTile, mappedViewChess[0]);
                            mappedViewChess[0]._pawnPromotion.type = 0;
                            mappedViewChess[0]._pawnPromotion.hasBeenPromoted = false;
                            mappedViewChess[0]._pawnPromotion.isPromoting = false;
                        }

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
                        gotTileDestination = false;
                    }
                }
            }
            else
            {
                mappedViewChess[0]._turn = turnHold;
            }

            if (mappedViewChess[0]._pOne._pieces[11]._position == -1)
            {
                cout << "Server sees victory for Player 2" << endl;
                mappedViewChess[0]._winnerWinnerChickenDinner = 2;
            }
            if (mappedViewChess[0]._pTwo._pieces[11]._position == -1)
            {
                cout << "Server sees victory for Player 1" << endl;
                mappedViewChess[0]._winnerWinnerChickenDinner = 1;
            }
        }
    }

    //STEP 4 : Unmap View Of File
    bResult = UnmapViewOfFile(mappedViewChess);
    if (bResult == FALSE)
    {
        cout << "UnMapViewOfFile Failed & Error NO - " << GetLastError() << endl;
    }
    cout << "UnMapViewOfFile Success" << endl;

    return 0;
}