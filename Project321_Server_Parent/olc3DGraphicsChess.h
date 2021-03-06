/*olc3DGraphics.h
Nathan Damon
2/14/2021
Followed tutorial by javidx9 - aka One Lone Coder
This is the header file for the olc3DGraphics class
*/

#ifndef OLC3DGRAPHICS_HPP
#define OLC3DGRAPHICS_HPP

#include "olcPixelGameEngine.h"
#include <vector>
using std::vector;
#include <list>
using std::list;
#include <algorithm>
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <strstream>
using std::strstream;
#include <iostream>

using std::swap;



struct vec2D
{
	float u = 0;
	float v = 0;
	float w = 1;
};

struct vec3D
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

struct triangle
{
	vec3D p[3];
	vec2D t[3];
	olc::Pixel color = { 200, 200, 200 };

	// Adds the given float array to the current triangle
	triangle& operator+=(const float rhs[3])
	{
		for (int i = 0; i < 3; i++)
		{
			this->p[i].x += rhs[0];
			this->p[i].y += rhs[1];
			this->p[i].z += rhs[2];
		}
		return *this;
	}

	triangle& operator=(const triangle& rhs)
	{
		for (int i = 0; i < 3; i++)
		{
			this->p[i].x = rhs.p[i].x;
			this->p[i].y = rhs.p[i].y;
			this->p[i].z = rhs.p[i].z;
			this->p[i].w = rhs.p[i].w;

			this->t[i].u = rhs.t[i].u;
			this->t[i].v = rhs.t[i].v;
			this->t[i].w = rhs.t[i].w;
		}
		this->color = rhs.color;
		return *this;
	}
};

struct Point2D
{
	int x;
	int y;
};

// Check if point is within the given triangle
bool isInsideTriangle(Point2D p1, Point2D p2, Point2D p3, Point2D pCheck);

// Find the area of the triangle formed by p1, p2 and p3
float triangleArea(Point2D p1, Point2D p2, Point2D p3);

struct Object
{
public:
	Object() : _xPos(0.0f), _yPos(0.0f), _zPos(0.0f) {};
	Object(float posX, float posY, float posZ, int triangleCount) : _xPos(posX), _yPos(posY), _zPos(posZ), _triCount(triangleCount) {};

	// Unified postion --> used to change the overall position
	float _xPos = 0.0f;
	float _yPos = 0.0f;
	float _zPos = 0.0f;

	int _triCount = 0;
};

struct Piece : public Object
{
	using Object::Object;
	short _position = 0; // 0 -> 63 for board, doubles as a unique ID for the piece
	bool _selected = false;
	triangle _tris[23];
	// 1 for pawn, 2 for rook, 3 for knight, 4 for bishop, 5 for king, 6 for queen and add 6 for player 2 equivalent
	int PieceTypeNTeam = 1; 
};

struct Player
{
	// Should not be constructing a defualt player
	Player() { std::cout << "Error! Player constructed defaulty" << std::endl; };
	Player(float offsetXY, float squareDimension, float boardTop = 0.0f, int team = 0)
	{
		_team = team;

		// Pieces
		for (int i = 0; i < 8; i++)
		{
			Piece pawn(0.0f + squareDimension * (float)i, 0.0f + squareDimension + (team * squareDimension * 5.0f), boardTop, 12);
			_pieces[i] = pawn;
			_pieces[i].PieceTypeNTeam = 1 + 6 * team;
			_pieces[i]._position = (6 - 5 * team) + 8 * i; // If player one (team 0) set pawns at x = 6 else at 1
		}
		float secondRowOffset = 0.0f + (team * squareDimension * 7.0f);
		for (int j = 0; j < 8; j++)
		{
			int i = j + 8;
			float columnOffset = 0.0f + squareDimension * float(j);
			Piece piece(columnOffset, secondRowOffset, boardTop, 0);
			_pieces[i] = piece;
			_pieces[i]._position = (7 + 8 * j - 7 * team); // Set the piece's position

			switch (j)
			{
			case 0: // Rook
				_pieces[i].PieceTypeNTeam = 2 + 6 * team;
				_pieces[i]._triCount = 12;
				break;
			case 1: // Knight
				_pieces[i].PieceTypeNTeam = 3 + 6 * team;
				_pieces[i]._triCount = 23;
				break;
			case 2: // Bishop
				_pieces[i].PieceTypeNTeam = 4 + 6 * team;
				_pieces[i]._triCount = 6;
				break;
			case 3: // King
				_pieces[i].PieceTypeNTeam = 5 + 6 * team;
				_pieces[i]._triCount = 16;
				break;
			case 4: // Queen
				_pieces[i].PieceTypeNTeam = 6 + 6 * team;
				_pieces[i]._triCount = 12;
				break;
			case 5: // Bishop
				_pieces[i].PieceTypeNTeam = 4 + 6 * team;
				_pieces[i]._triCount = 6;
				break;
			case 6: // Knight
				_pieces[i].PieceTypeNTeam = 3 + 6 * team;
				_pieces[i]._triCount = 23;
				break;
			case 7: // Rook
				_pieces[i].PieceTypeNTeam = 2 + 6 * team;
				_pieces[i]._triCount = 12;
				break;
			default:
				break;
			}
		}
	}

	int _team = 0;
	Piece _pieces[16];
};

struct AttemptedMove
{
	int _pieceTypeNTeam;
	float _xDist;
	float _yDist;
};

struct TileData
{
	bool piecePresent = false;
	// 1 for pawn, 2 for rook, 3 for knight, 4 for bishop, 5 for king, 6 for queen and add 6 for player 2 equivalent
	int pieceTypeNTeam = 0;
};

struct TileNTime
{
	float newTile;
	float oldTile;
	float timeSince;
	float elapsedTime;
};

struct tileAvailability
{
	bool _enemyPiecePresent;
	int _tile;
};

struct BoardOverlay : public Object
{
	using Object::Object;
	BoardOverlay(float x, float y, float z) : Object(x, y, z, 128){}
	triangle _tris[128];
};

struct PawnPromotion
{
	bool isPromoting = false;
	bool hasBeenPromoted = false;
	int type = 1;
};

struct GameBoard : public Object
{
	GameBoard(float offsetXY = 0.98375f, float squareDimension = 9.9675f, float boardTop = 0.0f) :
		_pOne(offsetXY, squareDimension, boardTop, 0),
		_pTwo(offsetXY, squareDimension, boardTop, 1),
		_overlay(3.5f * squareDimension, 3.5f * squareDimension, 0.2f, 128)
	{
		_xPos = 3.5f * squareDimension;
		_yPos = 3.5f * squareDimension;
		_zPos = -3.82f;
		_triCount = 12;

		for (int x = 0; x < 8; x++)
			for (int y = 2; y < 6; y++)
			{
				_boardTiles[x][y] = { false, 0 };
			}

		for (int i = 0; i < 16; i++)
		{
			_boardTiles[_pOne._pieces[i]._position / 8][_pOne._pieces[i]._position % 8] = { true, _pOne._pieces[i].PieceTypeNTeam };
			_boardTiles[_pTwo._pieces[i]._position / 8][_pTwo._pieces[i]._position % 8] = { true, _pTwo._pieces[i].PieceTypeNTeam };
		}

		LoadGame(*this);
	};
	int _turn = 0;
	AttemptedMove _move = { 0 }; // EDIT
	TileNTime _tilesAndTime = { 0.0f, 0.0f, 0.0f, 0.0f };
	bool _pieceInMotion = false;
	triangle _tris[12];
	Player _pOne;
	Player _pTwo;
	TileData _boardTiles[8][8];
	BoardOverlay _overlay;
	PawnPromotion _pawnPromotion;
	int _winnerWinnerChickenDinner = 0;

	void LoadGame(GameBoard& theGame);
};



// Changes the player's color
void colorChange(Player& p, const olc::Pixel newColor);

// Changes the piece's color
template<typename obj>
void subColorChange(obj& p, const olc::Pixel newColor);

// Checks if the given piece is on the given tile
TileData pieceTileCheck(Piece& p, int tile);

// Looks at all the pieces on the gameboard and checks if they are on the given tile
TileData checkTileForPiece(GameBoard& game, int tileToCheck);

// Moves the piece a given distance
template<typename pieceOrBoard>
void objectMov(pieceOrBoard& p, const float arr[3]);

// Moves the piece to the new tile
bool movePiece(Piece& p, const TileNTime& tilesAndTime, AttemptedMove& attemptedMove);

// Gets the piece specified by pieceTypeNTeam
bool getPieceAndMoveIt(GameBoard& game, const TileNTime& tilesAndTime, AttemptedMove& attemptedMove);

// Sets objects to 0 0 0
template<typename pieceOrBoard>
void vertexCorrection(pieceOrBoard& obj);

template<typename pieceOrBoard>
void trangleSubLoader(const pieceOrBoard& obj, vector<triangle>& trangles);

void trangleSubLoaderOverlay(const BoardOverlay& overlay, const vector<tileAvailability> availables, vector<triangle>& trangles);

void tranglePlayerSubLoader(const Player& p, vector<triangle>& trangles);

vector<triangle> trangleLoader(const GameBoard& gameBoard);

vector<tileAvailability> moveChecker(const int turn, const int pieceType, const int currentTileValue, const GameBoard& board);

void pawnChanger(const int pieceType, const int currentTileValue, GameBoard& board);


bool LoadFromChessBoardObjectFile(GameBoard& board, string sFilename, bool bHasTexture = false, int imageWidth = 128, int imageHeight = 128);

struct matrix4x4
{
	float m[4][4] = { 0 };
};



class olc3DGraphics : public olc::PixelGameEngine
{
public:
	olc3DGraphics();
	olc3DGraphics(GameBoard& chess, int myTurn);
	virtual bool OnUserCreate();// Called once at the start, so create things here
	virtual bool OnUserUpdate(float fElapsedTime);

private:
	GameBoard* _chess;

	matrix4x4 matProj;

	// CAMERA
	vec3D vCamera;
	vec3D vLookDir;
	float fCamSpeedMultipier = 1.0f;
	float fCameraSpeed = 5.0f;
	// Offset rotation
	int mouseX = 0;
	int mouseY = 0;
	float fCameraTopDownOffset = 0.0f;
	float fCameraSideToSideOffset = 0.0f;
	// Final camera matrix
	matrix4x4 matView;
	bool viewChanged = true;


	// WORLD
	// Illumination
	vec3D light_direction = { 1.0f, -1.0f, 2.0f };
	// World orientation
	float worldRotationX = 0.0f;
	float worldRotationY = 0.0f;
	float worldRotationZ = 0.0f;

	olc::Decal* _decaltex;
	olc::Decal* _decaltexWH;
	olc::Decal* _decaltexGR;
	olc::Decal* _decaltexBR;
	olc::Decal* _decaltexBoard;
	olc::Decal* _decaltexOverlay;
	olc::Decal* _decaltexOverlayAttack;
	olc::Decal* _decaltexWinScreen;
	olc::Decal* _decaltexPawnChangeScreen;

	float* pDepthBuffer = nullptr;

	// Game handling
	bool _pieceSelector = false;
	int _mouseSelectX = 0;
	int _mouseSelectY = 0;
	int _myTurn;
	TileData _selectedPiece = { false, 0 };
	vector<tileAvailability> _availableTiles;

	float _pawnChangeScreenDelay = 0.0f;

	void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		olc::Decal* tex, olc::Pixel& tCol, bool selectedTri = false);
};

vec3D Matrix_MultiplyVector(matrix4x4& mat, vec3D& in);
matrix4x4 Matrix_MakeIdentity();
matrix4x4 Matrix_MakeRotationX(float fAngleRad);
matrix4x4 Matrix_MakeRotationY(float fAngleRad);
matrix4x4 Matrix_MakeRotationZ(float fAngleRad);
matrix4x4 Matrix_MakeTranslation(float x, float y, float z);
matrix4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
matrix4x4 Matrix_MultiplyMatrix(matrix4x4& m1, matrix4x4& m2);

matrix4x4 Matrix_PointAt(vec3D& pos, vec3D& target, vec3D& up);
matrix4x4 Matrix_QuickInverse(matrix4x4& m); // Only for Rotation/Translation Matrices

vec3D Vector_Add(vec3D& v1, vec3D& v2);
vec3D Vector_Sub(vec3D& v1, vec3D& v2);
vec3D Vector_Mul(vec3D& v, float k);
vec3D Vector_Div(vec3D& v, float k);
float Vector_DotProduct(vec3D& v1, vec3D& v2);
float Vector_Length(vec3D& v);
vec3D Vector_Normalize(vec3D& v);
vec3D Vector_CrossProduct(vec3D& v1, vec3D& v2);

vec3D Vector_IntersectPlane(vec3D& plane_p, vec3D& plane_n, vec3D& lineStart, vec3D& lineEnd, float& t);
int Triangle_ClipAgainstPlane(vec3D plane_p, vec3D plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2);

#endif // !OLC3DGRAPHICSCHESS_HPP
