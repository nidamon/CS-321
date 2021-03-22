/*olc3DGraphicsChess.cpp
Nathan Damon
2/14/2021
Followed tutorial by javidx9 - aka One Lone Coder
This is the cpp file for the olc3DGraphics class
*/

#include "olc3DGraphicsChess.h"

olc3DGraphics::olc3DGraphics()
{
	std::cout << "Did not construct properly!" << std::endl;
	sAppName = "3DGraphics";
}

olc3DGraphics::olc3DGraphics(GameBoard& chess, int myTurn)
{
	_myTurn = myTurn;
	_chess = &chess;
	chess._triCount = 12;
	std::cout << "Gameboard size: " << sizeof(chess) << std::endl;
	sAppName = "3DGraphics";
}

// Called once at the start, so create things here
bool olc3DGraphics::OnUserCreate()
{
	pDepthBuffer = new float[ScreenWidth() * ScreenHeight()];

	// Called in Server/Parent

	if (_myTurn == 1)
	{
		meshCube.LoadFromChessBoardObjectFile((*_chess), "../Objects/Chess Board and 6 type pieces for 2 teams ReducedVTX.txt", true, 48, 48);
		vertexCorrection((*_chess));
		subColorChange((*_chess), { 106, 181, 0, 0 });

		// Player one's pieces
		for (int i = 0; i < 16; i++)
		{
			// Player one's pieces
			vertexCorrection((*_chess)._pOne._pieces[i]);
			// Player two's pieces
			vertexCorrection((*_chess)._pTwo._pieces[i]);
		}
		// Player one's color
		colorChange((*_chess)._pOne, { 255, 0, 0, 0 });

		// Player two's color
		colorChange((*_chess)._pTwo, { 0, 255, 255, 0 });
	}

	decaltexWH = new olc::Decal(new olc::Sprite("../Objects/whmarble.jpg"));
	decaltexGR = new olc::Decal(new olc::Sprite("../Objects/grmarble.jpg"));
	//decaltexPM = new olc::Decal(new olc::Sprite("./Objects/purmesh.jpg"));
	decaltexBR = new olc::Decal(new olc::Sprite("../Objects/brmarble.jpg"));
	decaltexBoard = new olc::Decal(new olc::Sprite("../Objects/Chess Board.png"));

	std::cout << "Object loaded" << std::endl;

	// Projection Matrix
	matProj = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);

	return true;
}

bool olc3DGraphics::OnUserUpdate(float fElapsedTime)
{
	// Handle Input
	vec3D vForward = Vector_Mul(vLookDir, fCameraSpeed * fElapsedTime);

	//float squareDimension = 9.9675f;

	if (IsFocused())
	{
		if (GetKey(olc::X).bPressed) // Close the game
		{
			(*_chess)._turn = -1;
		}

		if (GetKey(olc::Q).bHeld) // Ascend
		{
			vCamera.z -= fCameraSpeed * fElapsedTime;
			viewChanged = true;
		}
		if (GetKey(olc::E).bHeld) // Descend
		{
			vCamera.z += fCameraSpeed * fElapsedTime;
			viewChanged = true;
		}

		float forward = 0.0f;
		float left = 0.0f;
		if (GetKey(olc::W).bHeld) // Up
		{
			forward += fCameraSpeed * fElapsedTime;
			viewChanged = true;
		}
		if (GetKey(olc::S).bHeld) // Down
		{
			forward -= fCameraSpeed * fElapsedTime;
			viewChanged = true;
		}
		if (GetKey(olc::A).bHeld) // Left
		{
			left -= fCameraSpeed * fElapsedTime;
			viewChanged = true;
		}
		if (GetKey(olc::D).bHeld) // Right
		{
			left += fCameraSpeed * fElapsedTime;
			viewChanged = true;
		}

		float halfPi = -1.5707f;

		// Forward with respect to camera
		vCamera.y += sinf(fCameraSideToSideOffset - halfPi) * forward;
		vCamera.x += cosf(fCameraSideToSideOffset - halfPi) * forward;
		forward = 0.0f;

		// Left with respect to camera
		vCamera.y += sinf(fCameraSideToSideOffset) * left;
		vCamera.x += cosf(fCameraSideToSideOffset) * left;
		left = 0.0f;


		// Camera handling
		if (GetMouse(1).bPressed)
		{
			_pieceSelector = false; // Deselect a selected point
			mouseX = GetMouseX(); // Initialize mouseX
			mouseY = GetMouseY(); // Initialize mouseY
		}
		if (GetMouse(1).bHeld)
		{
			float changeX = (float)(mouseX - GetMouseX()); // Delta X
			mouseX = GetMouseX(); // Reset mouseX
			fCameraSideToSideOffset -= changeX / 300.0f;

			float changeY = (float)(mouseY - GetMouseY()); // Delta Y
			mouseY = GetMouseY(); // Reset mouseY
			fCameraTopDownOffset -= changeY / 300.0f;
			viewChanged = true;
		}

		if (fCameraSideToSideOffset > 6.28319f) // Clamp at 360 and -360 degrees up/down
			fCameraSideToSideOffset -= 6.28319f;
		if (fCameraSideToSideOffset < -6.28319f)
			fCameraSideToSideOffset += 6.28319f;

		if (fCameraTopDownOffset > 1.0f) // Clamp at 90 and -90 degrees up/down
			fCameraTopDownOffset = 1.0f;
		if (fCameraTopDownOffset < -1.0f)
			fCameraTopDownOffset = -1.0f;

		if (int(GetMouseWheel()) != 0) // Allows speed changing via scrolling
		{
			if ((fCameraSpeed / fCamSpeedMultipier + float(int(GetMouseWheel()) / 120)) < 1.0f) // Reduce speed drop
				fCamSpeedMultipier /= 10.0f;
			if ((fCameraSpeed / fCamSpeedMultipier + float(int(GetMouseWheel()) / 120)) > 10.0f) // Increase speed increase
				fCamSpeedMultipier *= 10.0f;

			fCameraSpeed += float(int(GetMouseWheel()) / 120) * fCamSpeedMultipier;
			if (fCameraSpeed < 0.0001f)
				fCameraSpeed = 0.0001f;
			if (fCameraSpeed > 10000.0f)
				fCameraSpeed = 10000.0f;
			std::cout << fCameraSpeed << std::endl;

		}
		//if (GetKey(olc::N).bHeld) // Negate
		//{
		//	if (GetKey(olc::C).bHeld) // worldRotationX
		//		worldRotationX -= 1.0f * fElapsedTime;
		//	if (GetKey(olc::V).bHeld) // worldRotationy
		//		worldRotationY -= 1.0f * fElapsedTime;
		//	if (GetKey(olc::B).bHeld) // worldRotationZ
		//		worldRotationZ -= 1.0f * fElapsedTime;
		//}
		//else
		//{
		//	if (GetKey(olc::C).bHeld) // worldRotationX
		//		worldRotationX += 1.0f * fElapsedTime;
		//	if (GetKey(olc::V).bHeld) // worldRotationy
		//		worldRotationY += 1.0f * fElapsedTime;
		//	if (GetKey(olc::B).bHeld) // worldRotationZ
		//		worldRotationZ += 1.0f * fElapsedTime;
		//}
		//if (GetKey(olc::M).bHeld) // Angle Offsets
		//{
		//	std::cout << "OffsetX: " << worldRotationX << std::endl;
		//	std::cout << "OffsetY: " << worldRotationY << std::endl;
		//	std::cout << "OffsetZ: " << worldRotationZ << std::endl;
		//}

	}

	// Set up rotation matrices
	matrix4x4 matRotX, matRotY, matRotZ;

	matRotX = Matrix_MakeRotationX(worldRotationX);
	matRotY = Matrix_MakeRotationX(worldRotationY);
	matRotZ = Matrix_MakeRotationZ(worldRotationZ);

	matrix4x4 matTrans;
	matTrans = Matrix_MakeTranslation(-40.0f, 2.0f, -50.0f); // Place the camera

	matrix4x4 matWorld;
	matWorld = Matrix_MakeIdentity();
	matWorld = Matrix_MultiplyMatrix(matRotY, matRotX);
	matWorld = Matrix_MultiplyMatrix(matWorld, matRotZ);
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

	if (viewChanged)
	{
		vec3D vUp = { 0, 0, -1 }; //{ 0, 1, 0 };
		vec3D vTarget = { 0, 1, 0 }; //{ 0, 0, 1 };
		// Rotate side to side
		matrix4x4 matCameraRotZ = Matrix_MakeRotationZ(fCameraSideToSideOffset);
		// Rotate up/down
		matrix4x4 matCameraRotX = Matrix_MakeRotationX(fCameraTopDownOffset);
		// Combine rotations
		matrix4x4 matCameraRot = Matrix_MultiplyMatrix(matCameraRotX, matCameraRotZ);
		vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
		vTarget = Vector_Add(vCamera, vLookDir);

		matrix4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// Make view matrix from camera
		matView = Matrix_QuickInverse(matCamera);
		viewChanged = false; // We can skip this section if the camera isn't moving 
	}

	// Store triangles for rastering later
	vector<triangle> vecTrianglesToRaster;

	vector<triangle> trangles = trangleLoader(*_chess);

	// Draw Triangles
	for (auto tri : trangles)
	{
		triangle triProjected, triTransformed, triViewed;

		triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
		triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
		triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);
		triTransformed.t[0] = tri.t[0];
		triTransformed.t[1] = tri.t[1];
		triTransformed.t[2] = tri.t[2];

		// Calculate triangle Normal
		vec3D normal, line1, line2;

		line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
		line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

		// Take cross product of lines to get normal to triangle surface
		normal = Vector_CrossProduct(line1, line2);

		// Normalize the normal
		normal = Vector_Normalize(normal);

		// Get Ray from triangle to camera
		vec3D vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

		// ray is aligned with normal, then triangle is visible
		if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
		{
			// Illumination
			light_direction = Vector_Normalize(light_direction);

			// How "aligned" are light direction and triangle surface normal?
			float dotProduct = (std::max)(0.1f, Vector_DotProduct(light_direction, normal));

			triTransformed.color = olc::Pixel(tri.color.r, tri.color.g, tri.color.b, int(255.0f * dotProduct)); // olc::Pixel(255, 255, 255, int(255.0f * dotProduct));

			// Convert World Space -> View Space
			triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
			triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
			triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
			triViewed.color = triTransformed.color;
			triViewed.t[0] = triTransformed.t[0];
			triViewed.t[1] = triTransformed.t[1];
			triViewed.t[2] = triTransformed.t[2];

			// Clip Viewed Triangle against near plane, this could form two additional triangle.
			int nClippedTriangles = 0;
			triangle clipped[2];
			nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

			for (int n = 0; n < nClippedTriangles; n++)
			{
				// Project triangles from 3D -> 2D
				triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
				triProjected.color = clipped[n].color;
				triProjected.t[0] = clipped[n].t[0];
				triProjected.t[1] = clipped[n].t[1];
				triProjected.t[2] = clipped[n].t[2];

				triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
				triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
				triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

				triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
				triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
				triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

				triProjected.t[0].w = 1.0f / triProjected.p[0].w;
				triProjected.t[1].w = 1.0f / triProjected.p[1].w;
				triProjected.t[2].w = 1.0f / triProjected.p[2].w;



				// Scale into view
				triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

				// Offset verts into visible normalised space
				vec3D vOffsetView = { 1, 1, 0 };
				triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
				triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
				triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
				triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

				// Store triangle for sorting
				vecTrianglesToRaster.push_back(triProjected);
			}
		}
	}

	// Clear Screen
	Clear(olc::BLACK);

	// Clear Depth Buffer
	for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
		pDepthBuffer[i] = 0.0f;

	//vector<triangle> topBoardTrangles;

	if (IsFocused())
	{
		if ((*_chess)._turn == _myTurn)
			if (!(*_chess)._pieceInMotion)
				if (GetMouse(0).bPressed)
				{
					_pieceSelector = true;
					_mouseSelectX = GetMouseX(); // Initialize mouseX
					_mouseSelectY = GetMouseY(); // Initialize mouseY
					std::cout << "(" << _mouseSelectX << ", " << _mouseSelectY << ")" << std::endl;
				}
	}

	for (auto& triToRaster : vecTrianglesToRaster)
	{
		// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to 
			//  ensure we only test new triangles generated against planes
		triangle clipped[2];
		list<triangle> listTriangles;

		// Add initial triangle
		listTriangles.push_back(triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				// Take triangle from front of queue
				triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
				case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < nTrisToAdd; w++)
					listTriangles.push_back(clipped[w]);
			}
			nNewTriangles = listTriangles.size();
		}

		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto& t : listTriangles)
		{
			// White and Green marble for the player peices
			if (t.color.r == 255 && t.color.g == 0 && t.color.b == 0) // Color was red
				decaltex = decaltexWH;
			if (t.color.r == 0 && t.color.g == 255 && t.color.b == 255) // Color was light blue
				decaltex = decaltexGR;

			// Make chessboard brown marble
			if (t.color.r == 106 && t.color.g == 181 && t.color.b == 0) // Color was a green
			{
				//std::cout << float(t.color.a) << std::endl;
				if (t.color.a == 208) // If top of chess board
				{
					bool selectedtri = false;
					//topBoardTrangles.push_back(t);
					if (_pieceSelector)
						if (isInsideTriangle({ (int)t.p[0].x, (int)t.p[0].y }, { (int)t.p[1].x, (int)t.p[1].y }, { (int)t.p[2].x, (int)t.p[2].y }, { _mouseSelectX, _mouseSelectY }))
						{
							selectedtri = true;
						}

					// Special edit of TexturedTriangle input for the top of the chess board
					TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u * 256.0f / 48.0f, t.t[0].v * 256.0f / 48.0f, t.t[0].w,
						t.p[1].x, t.p[1].y, t.t[1].u * 256.0f / 48.0f, t.t[1].v * 256.0f / 48.0f, t.t[1].w,
						t.p[2].x, t.p[2].y, t.t[2].u * 256.0f / 48.0f, t.t[2].v * 256.0f / 48.0f, t.t[2].w,
						decaltexBoard, t.color, selectedtri);

					if (selectedtri)
					{
						std::cout << "Tile Selected: " << (*_chess)._tilesAndTime.newTile /*_tileSelected*/ << std::endl;
						selectedtri = false;
					}
					continue;
				}
				else
					decaltex = decaltexBR;
			}

			TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
				t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
				t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
				decaltex, t.color);
		}
	}

	if (_pieceSelector && _selectedPiece.piecePresent) // If we've picked a tile and we have a pointer to the piece we want to move
	{
		(*_chess)._turn = 0;
		// Move the piece 
		(*_chess)._pieceInMotion = true;
		_pieceSelector = false;
	}
	if ((*_chess)._pieceInMotion && _myTurn == 1) // player 1 runs the piece moving time
	{
		(*_chess)._tilesAndTime.timeSince += fElapsedTime; // Increment the time since
		(*_chess)._tilesAndTime.elapsedTime = fElapsedTime; // Server uses these increments
	}
	if ((*_chess)._pieceInMotion && _selectedPiece.piecePresent) // player 1 runs the piece moving time
	{
		std::cout << "Picked destination" << std::endl;
		_selectedPiece.piecePresent = false;
		/*auto pieceInQuestion = [](GameBoard& game, int pieceTypeNTeam, const TileNTime& tilesAndTime) {
			auto playerPieces = [](Player& p, int pieceType, const TileNTime& tilesAndTime) {
				if (pieceType < 0)
				{
					std::cout << "Error with pieceInQuestion lambda" << std::endl;
					return false;
				}
				if (pieceType < 9)
					return movePiece(p._pawns[pieceType-1], tilesAndTime);
				else
					switch (pieceType)
					{
					case 9:
						return movePiece(p._rooks[0], tilesAndTime);
						break;
					case 10:
						return movePiece(p._knights[0], tilesAndTime);
						break;
					case 11:
						return movePiece(p._bishops[0], tilesAndTime);
						break;
					case 12:
						return movePiece(p._king, tilesAndTime);
						break;
					case 13:
						return movePiece(p._queen, tilesAndTime);
						break;
					case 14:
						return movePiece(p._bishops[1], tilesAndTime);
						break;
					case 15:
						return movePiece(p._knights[1], tilesAndTime);
						break;
					case 16:
						return movePiece(p._rooks[1], tilesAndTime);
						break;
					default:
						break;
					}
			};
			if (pieceTypeNTeam > 0 && pieceTypeNTeam < 17)
				return playerPieces(game._pOne, pieceTypeNTeam, tilesAndTime);
			else if (pieceTypeNTeam > 16 && pieceTypeNTeam < 33)
				return playerPieces(game._pTwo, pieceTypeNTeam - 16, tilesAndTime);
			else
				return false;
		};*/
		//(*_chess)._pieceInMotion = pieceInQuestion((*_chess), _selectedPiece.pieceTypeNTeam, (*_chess)._tilesAndTime);
		//if (!(*_chess)._pieceInMotion)
		//	(*_chess)._tilesAndTime.timeSince = 0.0f;
	}

	if (_pieceSelector && !_selectedPiece.piecePresent)
	{
		_selectedPiece = checkTileForPiece((*_chess), (*_chess)._tilesAndTime.newTile);
		(*_chess)._tilesAndTime.oldTile = (*_chess)._tilesAndTime.newTile;
		(*_chess)._move._pieceTypeNTeam = _selectedPiece.pieceTypeNTeam;
		_pieceSelector = false;
	}

	//if (IsFocused())
	//{
	//	if (GetMouse(0).bPressed)
	//	{
	//		pieceSelector = true;
	//		mouseSelectX = GetMouseX(); // Initialize mouseX
	//		mouseSelectY = GetMouseY(); // Initialize mouseY
	//		std::cout << "(" << mouseSelectX << ", " << mouseSelectY << ")" << std::endl;
	//	}
	//}
	//olc::Pixel triCol = olc::RED;
	//for (auto& t : topBoardTrangles) 
	//{
	//	if (pieceSelector)
	//	{
	//		if (isInsideTriangle({ (int)t.p[0].x, (int)t.p[0].y }, { (int)t.p[1].x, (int)t.p[1].y }, { (int)t.p[2].x, (int)t.p[2].y }, { mouseSelectX, mouseSelectY }))
	//		{
	//			//std::cout << ".";
	//			triCol = olc::YELLOW;
	//		}
	//		else
	//			triCol = olc::RED;
	//	}
	//	//FillTriangle(boardHalf1.p[0].x, boardHalf1.p[0].y, boardHalf1.p[1].x, boardHalf1.p[1].y, boardHalf1.p[2].x, boardHalf1.p[2].y, boardHalf1.color);
	//	DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, triCol);
	//	//if(mouseSelectX)
	//}	


	return true;
}

void olc3DGraphics::TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
	int x2, int y2, float u2, float v2, float w2,
	int x3, int y3, float u3, float v3, float w3,
	olc::Decal* tex, olc::Pixel& tCol, bool selectedTri)
{
	if (y2 < y1)
	{
		swap(y1, y2);
		swap(x1, x2);
		swap(u1, u2);
		swap(v1, v2);
		swap(w1, w2);
	}

	if (y3 < y1)
	{
		swap(y1, y3);
		swap(x1, x3);
		swap(u1, u3);
		swap(v1, v3);
		swap(w1, w3);
	}

	if (y3 < y2)
	{
		swap(y2, y3);
		swap(x2, x3);
		swap(u2, u3);
		swap(v2, v3);
		swap(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				swap(ax, bx);
				swap(tex_su, tex_eu);
				swap(tex_sv, tex_ev);
				swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i * ScreenWidth() + j])
				{
					if (tex != nullptr)
					{
						Draw(j, i, tex->sprite->GetPixel(tex_u / tex_w, tex_v / tex_w));
						if (selectedTri)
						{
							if (j == _mouseSelectX && i == _mouseSelectY)
								(*_chess)._tilesAndTime.newTile = (int(tex_v / tex_w) / 32 + (int(tex_u / tex_w) / 32) * 8);
						}
					}
					else
						Draw(j, i, tCol); // Use triangle color if no texture available
					pDepthBuffer[i * ScreenWidth() + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				swap(ax, bx);
				swap(tex_su, tex_eu);
				swap(tex_sv, tex_ev);
				swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * ScreenWidth() + j])
				{
					if (tex != nullptr)
					{
						Draw(j, i, tex->sprite->GetPixel(tex_u / tex_w, tex_v / tex_w));
						if (selectedTri)
						{
							if (j == _mouseSelectX && i == _mouseSelectY)
								(*_chess)._tilesAndTime.newTile = (int(tex_v / tex_w) / 32 + (int(tex_u / tex_w) / 32) * 8);
						}
					}
					else
						Draw(j, i, tCol); // Use triangle color if no texture available
					pDepthBuffer[i * ScreenWidth() + j] = tex_w;
				}
				t += tstep;
			}
		}
	}
}

// Multiplies the given matrix vector
vec3D Matrix_MultiplyVector(matrix4x4& mat, vec3D& in)
{
	vec3D v;
	v.x = in.x * mat.m[0][0] + in.y * mat.m[1][0] + in.z * mat.m[2][0] + in.w * mat.m[3][0];
	v.y = in.x * mat.m[0][1] + in.y * mat.m[1][1] + in.z * mat.m[2][1] + in.w * mat.m[3][1];
	v.z = in.x * mat.m[0][2] + in.y * mat.m[1][2] + in.z * mat.m[2][2] + in.w * mat.m[3][2];
	v.w = in.x * mat.m[0][3] + in.y * mat.m[1][3] + in.z * mat.m[2][3] + in.w * mat.m[3][3];
	return v;
}

matrix4x4 Matrix_MakeIdentity()
{
	matrix4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

matrix4x4 Matrix_MakeRotationX(float fAngleRad)
{
	matrix4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

matrix4x4 Matrix_MakeRotationY(float fAngleRad)
{
	matrix4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

matrix4x4 Matrix_MakeRotationZ(float fAngleRad)
{
	matrix4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

matrix4x4 Matrix_MakeTranslation(float x, float y, float z)
{
	matrix4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

matrix4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	matrix4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

matrix4x4 Matrix_MultiplyMatrix(matrix4x4& m1, matrix4x4& m2)
{
	matrix4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

matrix4x4 Matrix_PointAt(vec3D& pos, vec3D& target, vec3D& up)
{
	// Calculate new forward direction
	vec3D newForward = Vector_Sub(target, pos);
	newForward = Vector_Normalize(newForward);

	// Calculate new up direction
	vec3D a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
	vec3D newUp = Vector_Sub(up, a);
	newUp = Vector_Normalize(newUp);

	// Calculate new right direction
	vec3D newRight = Vector_CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix
	matrix4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;
	return matrix;
}

matrix4x4 Matrix_QuickInverse(matrix4x4& m) // Only for Rotation/Translation Matrices
{
	matrix4x4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

vec3D Vector_Add(vec3D& v1, vec3D& v2)
{
	return { v1.x + v2.x, v1.y + v2.y , v1.z + v2.z };
}

vec3D Vector_Sub(vec3D& v1, vec3D& v2)
{
	return { v1.x - v2.x, v1.y - v2.y , v1.z - v2.z };
}

vec3D Vector_Mul(vec3D& v, float k)
{
	return { v.x * k, v.y * k , v.z * k };
}

vec3D Vector_Div(vec3D& v, float k)
{
	return { v.x / k, v.y / k , v.z / k };
}

float Vector_DotProduct(vec3D& v1, vec3D& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector_Length(vec3D& v)
{
	return sqrtf(Vector_DotProduct(v, v));
}

vec3D Vector_Normalize(vec3D& v)
{
	float pythagResult = Vector_Length(v);
	return { v.x / pythagResult, v.y / pythagResult, v.z / pythagResult };
}

vec3D Vector_CrossProduct(vec3D& v1, vec3D& v2)
{
	vec3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

vec3D Vector_IntersectPlane(vec3D& plane_p, vec3D& plane_n, vec3D& lineStart, vec3D& lineEnd, float& t)
{
	plane_n = Vector_Normalize(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	t = (-plane_d - ad) / (bd - ad);
	vec3D lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	vec3D lineToIntersect = Vector_Mul(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}

int Triangle_ClipAgainstPlane(vec3D plane_p, vec3D plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n = Vector_Normalize(plane_n);

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](vec3D& p)
	{
		vec3D n = Vector_Normalize(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3D* inside_points[3];  int nInsidePointCount = 0;
	vec3D* outside_points[3]; int nOutsidePointCount = 0;
	vec2D* inside_tex[3]; int nInsideTexCount = 0;
	vec2D* outside_tex[3]; int nOutsideTexCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0]; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1]; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2]; }

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		out_tri1.color = in_tri.color;

		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];
		out_tri1.t[0] = *inside_tex[0]; // Left this out when I followed javidx9's tutorial and it made me sad

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t;
		out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
		out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
		out_tri1.color = in_tri.color;
		out_tri2.color = in_tri.color;

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.t[0] = *inside_tex[0];
		out_tri1.t[1] = *inside_tex[1];

		float t;
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.t[0] = *inside_tex[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.t[1] = out_tri1.t[2];

		out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
		out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
		out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
		out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;

		return 2; // Return two newly formed triangles which form a quad
	}

	return 0;
}

// Check if point is within the given triangle
bool isInsideTriangle(Point2D p1, Point2D p2, Point2D p3, Point2D pCheck)
{
	float area = triangleArea(p1, p2, p3);        
	float area1 = triangleArea(pCheck, p2, p3);
	float area2 = triangleArea(p1, pCheck, p3);
	float area3 = triangleArea(p1, p2, pCheck);

	return (area == area1 + area2 + area3);
}

// Find the area of the triangle formed by p1, p2 and p3
float triangleArea(Point2D p1, Point2D p2, Point2D p3)
{
	return abs(((float)(p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y))) / 2.0f);
}

// Changes the player's pieces' color
void colorChange(Player& p, const olc::Pixel newColor)
{
	for (int i = 0; i < 16; i++)
		subColorChange(p._pieces[i], newColor);
}

// Changes the piece's color
template<typename obj>
void subColorChange(obj& p, const olc::Pixel newColor)
{
	for (int i = 0; i < p._triCount; i++)
		p._tris[i].color = newColor;
}

// Looks at all the pieces on the gameboard and checks if they are on the given tile
TileData checkTileForPiece(GameBoard& game, int tileToCheck)
{
	// Checks if a player's piece is on the given tile
	auto playerCheck = [](Player& p, int tile) {
		TileData t;
		for (int i = 0; i < 16; i++) // Pawns
		{
			t = pieceTileCheck(p._pieces[i], tile);
			if (t.piecePresent)
			{
				t.pieceTypeNTeam = p._pieces[i].PieceTypeNTeam;
				return t;
			}
		}
		t = { false, 0 };
		return t;
	};

	TileData t;
	t = playerCheck(game._pOne, tileToCheck);
	if (t.piecePresent)
		return t;
	t = playerCheck(game._pTwo, tileToCheck);
	if (t.piecePresent)
		return t;
	
	std::cout << "(checkTileForPiece()) -> No piece on that tile." << std::endl;
	return { false, 0 };
}

// Checks if the given piece is on the given tile
TileData pieceTileCheck(Piece& p, int tile) 
{
	//std::cout << "(pieceCheck()) -> Piece type: " << pieceType << " Supposed position: " << p._position << std::endl;
	if (p._position == tile)
	{
		p._selected = true;
		return { true, 0 };
	}
	else
		return { false, 0};
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

// Moves the piece to the new tile
bool movePiece(Piece& p, const TileNTime& tilesAndTime)
{
	float tRise = 1.5f;
	float tShift = 1.5f + tRise;
	float tLower = tRise + tShift;

	float zSpeed = 0.18f; // change to height rise to

	// Raise the piece
	if (tilesAndTime.timeSince < tRise)
	{
		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
				p._tris[i].p[k].z += zSpeed;
	}
	// Shift the piece
	else if (tilesAndTime.timeSince < tShift)
	{
		float deltaY = (int)tilesAndTime.oldTile % 8 - (int)tilesAndTime.newTile % 8;
		float deltaX = (int)tilesAndTime.oldTile / 8 - (int)tilesAndTime.newTile / 8;

		float squareDimension = 9.9675f;
		float shiftSpeedx = squareDimension * deltaX / (tShift - tRise);
		float shiftSpeedy = squareDimension * deltaY / (tShift - tRise);
		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
			{
				p._tris[i].p[k].x += -shiftSpeedx * tilesAndTime.elapsedTime;
				p._tris[i].p[k].y += shiftSpeedy * tilesAndTime.elapsedTime;
			}
	}
	// Lower the piece
	else if (tilesAndTime.timeSince < tLower)
	{
		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
				p._tris[i].p[k].z -= zSpeed;
	}
	// Stop calling movePiece function
	else if (tilesAndTime.timeSince > tLower)
	{
		p._position = tilesAndTime.newTile;
		return false;
	}
	return true;
}

// Gets the piece specified by pieceTypeNTeam
bool getPieceAndMoveIt(GameBoard& game/*, int pieceTypeNTeam*/, const TileNTime& tilesAndTime)
{
	auto playerPieces = [](Player& p,/* int pieceType,*/ const TileNTime& tilesAndTime) {

		int piece = -1;
		for (int i = 0; i < 16; i++)
		{
			if (p._pieces[i]._position == tilesAndTime.oldTile)
				piece = i;
		}
		if (piece == -1)
			return false;
		else
			return movePiece(p._pieces[piece], tilesAndTime);
		/* piece moving rules here

		int pieceType = p._pieces[piece].PieceTypeNTeam;
		if(pieceType > 6)
			pieceType - 6;
		if (pieceType < 0)
		{
			std::cout << "Error with pieceInQuestion function" << std::endl;
			return false;
		}
		else
			switch (pieceType)
			{
			case 1:
				return movePiece(p._pieces[piece], tilesAndTime);
				break;
			case 2:
				return movePiece(p._pieces[piece], tilesAndTime);
				break;
			case 3:
				return movePiece(p._pieces[piece], tilesAndTime);
				break;
			case 4:
				return movePiece(p._king, tilesAndTime);
				break;
			case 5:
				return movePiece(p._queen, tilesAndTime);
				break;
			case 6:
				return movePiece(p._bishops[1], tilesAndTime);
				break;
			default:
				break;
			}*/
		};
	if (playerPieces(game._pOne, tilesAndTime))
		return true;
	else if (playerPieces(game._pTwo, tilesAndTime))
		return true;
	else
		return false;
	//auto playerPieces = [](Player& p, int pieceType, const TileNTime& tilesAndTime) {
	//	if (pieceType < 0)
	//	{
	//		std::cout << "Error with pieceInQuestion function" << std::endl;
	//		return false;
	//	}
	//	if (pieceType < 9)
	//		return movePiece(p._pawns[pieceType - 1], tilesAndTime);
	//	else
	//		switch (pieceType)
	//		{
	//		case 9:
	//			return movePiece(p._rooks[0], tilesAndTime);
	//			break;
	//		case 10:
	//			return movePiece(p._knights[0], tilesAndTime);
	//			break;
	//		case 11:
	//			return movePiece(p._bishops[0], tilesAndTime);
	//			break;
	//		case 12:
	//			return movePiece(p._king, tilesAndTime);
	//			break;
	//		case 13:
	//			return movePiece(p._queen, tilesAndTime);
	//			break;
	//		case 14:
	//			return movePiece(p._bishops[1], tilesAndTime);
	//			break;
	//		case 15:
	//			return movePiece(p._knights[1], tilesAndTime);
	//			break;
	//		case 16:
	//			return movePiece(p._rooks[1], tilesAndTime);
	//			break;
	//		default:
	//			break;
	//		}
	//};
	//if (pieceTypeNTeam > 0 && pieceTypeNTeam < 17)
	//	return playerPieces(game._pOne, pieceTypeNTeam, tilesAndTime);
	//else if (pieceTypeNTeam > 16 && pieceTypeNTeam < 33)
	//	return playerPieces(game._pTwo, pieceTypeNTeam - 16, tilesAndTime);
	//else
	//	return false;
};

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

template<typename pieceOrBoard>
void trangleSubLoader(const pieceOrBoard& obj, vector<triangle>& trangles)
{
	for (int i = 0; i < obj._triCount; i++)
		trangles.push_back(obj._tris[i]);
}

void tranglePlayerSubLoader(const Player& p, vector<triangle>& trangles)
{
	for (int i = 0; i < 16; i++)
		trangleSubLoader(p._pieces[i], trangles);
}

vector<triangle> trangleLoader(const GameBoard& gameBoard)
{
	vector<triangle> trangles;
	trangleSubLoader(gameBoard, trangles);
	tranglePlayerSubLoader(gameBoard._pOne, trangles);
	tranglePlayerSubLoader(gameBoard._pTwo, trangles);
	return trangles;
}