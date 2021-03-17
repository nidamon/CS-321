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
	//int _decals = 0; // Specified as bits

	// Unified postion --> used to change the overall position
	float _xPos = 0.0f;
	float _yPos = 0.0f;
	float _zPos = 0.0f;

	int _triCount = 0;
};

struct Piece : public Object
{
	using Object::Object;
	short _position = 0; // 0 -> 63 for board
	bool _selected = false;
};

struct Pawn : public Piece
{
	using Piece::Piece;
	triangle _tris[12];
};

struct Bishop : public Piece
{
	using Piece::Piece;
	triangle _tris[6];
};

struct Knight : public Piece
{
	using Piece::Piece;
	triangle _tris[24];
};

struct Rook : public Piece
{
	using Piece::Piece;
	triangle _tris[12];
};

struct Queen : public Piece
{
	using Piece::Piece;
	triangle _tris[12];
};

struct King : public Piece
{
	using Piece::Piece;
	triangle _tris[18];
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
			Pawn pawn(0.0f + offsetXY + squareDimension * (float)i, 0.0f + offsetXY + squareDimension + (team * squareDimension * 5.0f), boardTop, 12);
			_pawns[i] = pawn;
			_pawns[i]._position = (6 - 5*team) + 8 * i; // If player one (team 0) set pawns at x = 6 else at 1
		}

		Rook rook1(0.0f + offsetXY, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 12);
		_rooks[0] = rook1;
		_rooks[0]._position = (7 - 7 * team);

		Knight knight1(0.0f + offsetXY - squareDimension * 0.5f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 24);
		_knights[0] = knight1;
		_knights[0]._position = (15 - 7 * team);

		Bishop bishop1(0.0f + offsetXY + squareDimension * 2.0f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 6);
		_bishops[0] = bishop1;
		_bishops[0]._position = (23 - 7 * team);

		King king(0.0f + offsetXY + squareDimension * 3.0f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 18);
		_king = king;
		_king._position = (31 - 7 * team);

		Queen queen(0.0f + offsetXY + squareDimension * 4.0f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 12);
		_queen = queen;
		_queen._position = (39 - 7 * team);

		Bishop bishop2(0.0f + offsetXY + squareDimension * 5.0f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 6);
		_bishops[1] = bishop2;
		_bishops[1]._position = (47 - 7 * team);

		Knight knight2(0.0f + offsetXY + squareDimension * 4.5f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 24);
		_knights[1] = knight2;
		_knights[1]._position = (55 - 7 * team);

		Rook rook2(0.0f + offsetXY + squareDimension * 7.0f, 0.0f + offsetXY + (team * squareDimension * 7.0f), boardTop, 12);
		_rooks[1] = rook2;
		_rooks[1]._position = (63 - 7 * team);
	}

	int _team = 0;

	Pawn _pawns[8];
	Bishop _bishops[2];
	Knight _knights[2];
	Rook _rooks[2];
	Queen _queen;
	King _king;
};

struct AttemptedMove
{
	int _pieceTypeNTeam;
};

struct TileData
{
	bool piecePresent = false;
	int pieceTypeNTeam = 0; // 1-8 for pawn, 9/16 for rook, 10/15 for knight, 11/14 for bishop, 12 for king, 13 for queen and add 16 for player 2 equivalent
	//Piece* peicePtr = nullptr;
};

struct TileNTime
{
	float newTile;
	float oldTile;
	float timeSince;
	float elapsedTime;
};

struct GameBoard : public Object
{
	GameBoard(float offsetXY = 0.98375f, float squareDimension = 9.9675f, float boardTop = 0.0f) :
		//_board(39.872337f, 39.872337f, -3.82f, 12),

		_pOne(offsetXY, squareDimension, boardTop, 0),
		_pTwo(offsetXY, squareDimension, boardTop, 1)
	{
		_xPos = 0.0f;
		_yPos = 0.0f;
		_zPos = -3.82f;
		_triCount = 12;
	};
	int _turn = 0;
	AttemptedMove _move = { 0 }; // EDIT
	TileNTime _tilesAndTime = { 0.0f, 0.0f, 0.0f, 0.0f };
	bool _pieceInMotion = false;
	triangle _tris[12];
	Player _pOne;
	Player _pTwo;
};


// Changes the player's color
void colorChange(Player& p, const olc::Pixel newColor);

// Changes the piece's color
template<typename obj>
void subColorChange(obj& p, const olc::Pixel newColor);

// Checks if the given piece is on the given tile
template<typename piece>
TileData pieceCheck(piece& p, int pieceType, int team, int tile);

// Looks at all the pieces on the gameboard and checks if they are on the given tile
TileData checkTileForPiece(GameBoard& game, int tileToCheck);

// Moves the piece a given distance
template<typename pieceOrBoard>
void objectMov(pieceOrBoard& p, const float arr[3]/*deltaX, const float deltaY, const float deltaZ*/);

// Moves the piece to the new tile
template<typename piece>
bool movePiece(piece& p, const TileNTime& tilesAndTime);

// Gets the piece specified by pieceTypeNTeam
bool pieceInQuestion(GameBoard& game, int pieceTypeNTeam, const TileNTime& tilesAndTime);

// Sets objects to 0 0 0
template<typename pieceOrBoard>
void vertexCorrection(pieceOrBoard& obj);

template<typename pieceOrBoard>
void trangleSubLoader(const pieceOrBoard& obj, vector<triangle>& trangles);

void tranglePlayerSubLoader(const Player& p, vector<triangle>& trangles);

vector<triangle> trangleLoader(const GameBoard& gameBoard);


struct mesh
{
	int trisSize = 174;
	vector<triangle> tris;

	bool LoadFromChessBoardObjectFile(GameBoard& board, string sFilename, bool bHasTexture = false, int imageWidth = 128, int imageHeight = 128)
	{
		ifstream file(sFilename);
		if (!file.is_open())
			return false;

		// Local cache of verts
		vector<vec3D> verts;
		vector<vec2D> texs;

		vector<int> dataLineStarting;

		char junk;
		while (!file.eof())
		{
			char line[128];
			file.getline(line, 128);

			strstream s;
			s << line;

			int objectcount = 0;
			if (line[0] == 'o')
			{
				dataLineStarting.push_back(tris.size());
				objectcount++;
				//std::cout << tris.size() << " ";
			}

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					if (objectcount == 12)
					{
						imageHeight = 256;
						imageHeight = 256;
					}
					vec2D v;
					s >> junk >> junk >> v.u >> v.v;
					// A little hack for the spyro texture
					//v.u = 1.0f - v.u;
					//v.v = 1.0f - v.v;
					v.u = (v.u) * imageWidth;
					v.v = (1.0f - v.v) * imageHeight;
					texs.push_back(v);
				}
				else
				{
					vec3D v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			else
			{
				if (line[0] == 'f')
				{
					s >> junk;

					string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();


					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
									 texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
				}
			}
		}

		dataLineStarting.push_back(174);
		int triIndex = 0;
		int modelIndex = 0;
		//std::cout << "tris size: " << tris.size() << std::endl;
		for (int i = 0; i < tris.size(); i++)
		{
			if (i == dataLineStarting[modelIndex])
			{
				modelIndex++;
				triIndex = 0;
				//std::cout << "            modelIndex: " << modelIndex << std::endl;
			}

			//std::cout << "tri num: " << i << std::endl;
			switch (modelIndex - 1)
			{
				// P1
			case 0: // WhBishops
				if (board._pOne._bishops[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pOne._bishops[0]._tris[triIndex] = tris[i];
				board._pOne._bishops[1]._tris[triIndex] = tris[i];
				break;

			case 1: // WhPawns
				if (board._pOne._pawns[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				for (int k = 0; k < 8; k++)
					board._pOne._pawns[k]._tris[triIndex] = tris[i];
				break;

			case 2: // WhQueen
				if (board._pOne._queen._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pOne._queen._tris[triIndex] = tris[i];
				break;

			case 3: // WhKnight
				if (board._pOne._knights[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pOne._knights[0]._tris[triIndex] = tris[i];
				board._pOne._knights[1]._tris[triIndex] = tris[i];
				break;

			case 4: // WhKing
				if (board._pOne._king._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pOne._king._tris[triIndex] = tris[i];
				break;

			case 5: // WhRook
				if (board._pOne._rooks[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pOne._rooks[0]._tris[triIndex] = tris[i];
				board._pOne._rooks[1]._tris[triIndex] = tris[i];
				break;

				// P2
			case 6: // GrBishops
				if (board._pTwo._bishops[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pTwo._bishops[0]._tris[triIndex] = tris[i];
				board._pTwo._bishops[1]._tris[triIndex] = tris[i];
				break;

			case 7: // GrPawns
				if (board._pTwo._pawns[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				for (int k = 0; k < 8; k++)
					board._pTwo._pawns[k]._tris[triIndex] = tris[i];
				break;

			case 8: // GrQueen
				if (board._pTwo._queen._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pTwo._queen._tris[triIndex] = tris[i];
				break;

			case 9: // GrKnight
				if (board._pTwo._knights[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pTwo._knights[0]._tris[triIndex] = tris[i];
				board._pTwo._knights[1]._tris[triIndex] = tris[i];
				break;

			case 10: // GrKing
				if (board._pTwo._king._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pTwo._king._tris[triIndex] = tris[i];
				break;

			case 11: // GrRook
				if (board._pTwo._rooks[0]._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._pTwo._rooks[0]._tris[triIndex] = tris[i];
				board._pTwo._rooks[1]._tris[triIndex] = tris[i];
				break;

			case 12: // Board
				if (board._triCount < triIndex)
				{
					std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
					break;
				}
				board._tris[triIndex] = tris[i];
				break;

			default:
				break;
			}

			triIndex++;
		}

		return true;
	}

};

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

	mesh meshCube;
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

	//float fYaw;
	//float fTheta = 0.0f;

	olc::Decal* decaltex;
	olc::Decal* decaltexWH;
	olc::Decal* decaltexGR;
	//olc::Decal* decaltexPM;
	olc::Decal* decaltexBR;
	olc::Decal* decaltexBoard;

	float* pDepthBuffer = nullptr;

	// Game handling
	bool _pieceSelector = false;
	int _mouseSelectX = 0;
	int _mouseSelectY = 0;
	int _myTurn;
	TileData _selectedPiece = { false, 0 };

	/*float colorTimer = 0.0f;
	float colorChangeMultiplier = 100.0f;
	float colorOfTime1 = 0.0f;
	float colorOfTime2 = 0.0f;
	float colorOfTime3 = 0.0f;*/

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
