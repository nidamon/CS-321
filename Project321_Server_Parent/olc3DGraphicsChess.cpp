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

	_decaltexWH = new olc::Decal(new olc::Sprite("../Objects/whmarble.jpg"));
	_decaltexGR = new olc::Decal(new olc::Sprite("../Objects/grmarble.jpg"));
	_decaltexBR = new olc::Decal(new olc::Sprite("../Objects/brmarble.jpg"));
	_decaltexBoard = new olc::Decal(new olc::Sprite("../Objects/Chess Board.png"));
	_decaltexOverlay = new olc::Decal(new olc::Sprite("../Objects/Overlay.png"));
	_decaltexOverlayAttack = new olc::Decal(new olc::Sprite("../Objects/OverlayAttack.png"));
	_decaltexWinScreen = new olc::Decal(new olc::Sprite("../Objects/Win Screens.png"));
	_decaltexPawnChangeScreen = new olc::Decal(new olc::Sprite("../Objects/PawnChange.png"));

	std::cout << "Object loaded" << std::endl;

	// Projection Matrix
	matProj = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);

	return true;
}

bool olc3DGraphics::OnUserUpdate(float fElapsedTime)
{
	// Handle Input
	vec3D vForward = Vector_Mul(vLookDir, fCameraSpeed * fElapsedTime);

	if (IsFocused())
	{
		if (GetKey(olc::X).bPressed) // Unmap the game
		{
			_chess->_turn = -1;
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
	if (_myTurn == _chess->_turn && _selectedPiece.piecePresent)
		trangleSubLoaderOverlay(_chess->_overlay, _availableTiles, trangles);

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
	bool clickedOnTheBoard = false;
	if (IsFocused())
	{
		if (!_chess->_pawnPromotion.isPromoting)
			if (_chess->_turn == _myTurn)
				if (!_chess->_pieceInMotion)
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
			_decaltex = nullptr; // Reset the decal

			// White and Green marble for the player peices
			if (t.color.r == 255 && t.color.g == 0 && t.color.b == 0) // Color was red
				_decaltex = _decaltexWH;
			if (t.color.r == 0 && t.color.g == 255 && t.color.b == 255) // Color was light blue
				_decaltex = _decaltexGR;

			// Set decal to Overlay.png empty tile or occupied tile
			if (t.color.r == 255 && t.color.g == 255 && t.color.b == 25) // Color was yellow
				_decaltex = _decaltexOverlay;
			if (t.color.r == 255 && t.color.g == 255 && t.color.b == 20) // Color was light blue
				_decaltex = _decaltexOverlayAttack;

			// Make chessboard brown marble
			if (t.color.r == 106 && t.color.g == 181 && t.color.b == 0) // Color was a green
			{
				//std::cout << float(t.color.a) << std::endl;
				if (t.color.a == 208) // If top of chess board
				{
					bool selectedtri = false;
					if (_pieceSelector)
						if (isInsideTriangle({ (int)t.p[0].x, (int)t.p[0].y }, { (int)t.p[1].x, (int)t.p[1].y }, { (int)t.p[2].x, (int)t.p[2].y }, { _mouseSelectX, _mouseSelectY }))
						{
							selectedtri = true;
						}

					// Special edit of TexturedTriangle input for the top of the chess board
					TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u * 256.0f / 48.0f, t.t[0].v * 256.0f / 48.0f, t.t[0].w,
						t.p[1].x, t.p[1].y, t.t[1].u * 256.0f / 48.0f, t.t[1].v * 256.0f / 48.0f, t.t[1].w,
						t.p[2].x, t.p[2].y, t.t[2].u * 256.0f / 48.0f, t.t[2].v * 256.0f / 48.0f, t.t[2].w,
						_decaltexBoard, t.color, selectedtri);

					if (selectedtri)
					{
						std::cout << "Tile Selected: " << _chess->_tilesAndTime.newTile << std::endl;
						clickedOnTheBoard = true;
						selectedtri = false;
					}
					continue;
				}
				else
					_decaltex = _decaltexBR;
			}

			TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
				t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
				t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
				_decaltex, t.color);
		}
	}

	if (_pieceSelector && _selectedPiece.piecePresent) // If we've picked a tile and we have a pointer to the piece we want to move
	{
		int tile = _chess->_tilesAndTime.newTile;
		if (_availableTiles.end() != std::find_if(_availableTiles.begin(), _availableTiles.end(), [tile](const tileAvailability& t) { return (t._tile == tile); }))
		{
			if (_selectedPiece.pieceTypeNTeam == 1 || _selectedPiece.pieceTypeNTeam == 7)
				if (tile % 8 == 0 || tile % 8 == 7)
					_chess->_pawnPromotion.isPromoting = true;

			if (!_chess->_pawnPromotion.isPromoting)
			{
				_chess->_turn = 0;
				// Move the piece 
				_chess->_pieceInMotion = true;

				_availableTiles.clear(); // Clear the availble tiles
				std::cout << "tiles cleared" << std::endl;
			}
		}
		else // Pick a new piece
		{
			_selectedPiece.piecePresent = false;
		}
		_pieceSelector = false;
	}

	if (_chess->_pieceInMotion && _selectedPiece.piecePresent)
	{
		std::cout << "Picked destination" << std::endl;
		_selectedPiece.piecePresent = false;
	}

	if (_pieceSelector && !_selectedPiece.piecePresent && clickedOnTheBoard)
	{
		_selectedPiece = checkTileForPiece((*_chess), _chess->_tilesAndTime.newTile);
		
		if (_selectedPiece.piecePresent)
			_availableTiles = moveChecker(_myTurn - 1, _selectedPiece.pieceTypeNTeam, _chess->_tilesAndTime.newTile, (*_chess));

		_chess->_tilesAndTime.oldTile = _chess->_tilesAndTime.newTile;
		_chess->_move._pieceTypeNTeam = _selectedPiece.pieceTypeNTeam;
		_pieceSelector = false;
	}

	// For pawn promoting to another piece
	if (_chess->_pawnPromotion.isPromoting)
	{
		if (_chess->_turn == _myTurn)
		{
			DrawPartialDecal({ 64.0f, 96.0f }, _decaltexPawnChangeScreen, { 0.0f, 0.0f }, { 128.0f, 97.0f }, { 3.0f, 3.0f });
			if (IsFocused())
			{
				if (_pawnChangeScreenDelay < 0.7f)
					_pawnChangeScreenDelay += fElapsedTime;
				else
					if (GetMouse(0).bPressed)
					{
						_pawnChangeScreenDelay = 0.0f;

						_mouseSelectX = GetMouseX(); // Initialize mouseX
						_mouseSelectY = GetMouseY(); // Initialize mouseY
						std::cout << "(" << _mouseSelectX << ", " << _mouseSelectY << ")" << std::endl;
						if (_mouseSelectX > 154 && _mouseSelectX < 358)
						{
							int arr[4] = { 4, 3, 2, 6 }; // Need an array of piece type to make things easier here
							for (int i = 0; i < 4; i++)
								if (_mouseSelectY > 141 + i * 51 && _mouseSelectY < 189 + i * 51)
								{
									_chess->_pawnPromotion.type = arr[i] + 6 * (_myTurn - 1);
									std::cout << "Client read pawnChange selection: " << _chess->_pawnPromotion.type << std::endl;

									_chess->_pawnPromotion.hasBeenPromoted = true;
									_chess->_turn = 0;
									_chess->_pieceInMotion = true;

									_availableTiles.clear(); // Clear the availble tiles
									std::cout << "tiles cleared" << std::endl;
								}
						}
					};
			}
		}
	}

	// The game is over
	if (_chess->_winnerWinnerChickenDinner != 0)
	{
		DrawPartialDecal({ 64.0f, 96.0f }, _decaltexWinScreen, { 128.0f * float(_chess->_winnerWinnerChickenDinner - 1), 0.0f }, { 128.0f, 97.0f }, { 3.0f, 3.0f });
		_chess->_turn = -1;
	}


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
								_chess->_tilesAndTime.newTile = (int(tex_v / tex_w) / 32 + (int(tex_u / tex_w) / 32) * 8);
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
								_chess->_tilesAndTime.newTile = (int(tex_v / tex_w) / 32 + (int(tex_u / tex_w) / 32) * 8);
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
		for (int i = 0; i < 16; i++) // Loop over both teams pieces
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
void objectMov(pieceOrBoard& p, const float arr[3])
{
	for (int i = 0; i < p._triCount; i++)
	{
		p._tris[i] += arr;
	}
}

// Moves the piece to the new tile
bool movePiece(Piece& p, const TileNTime& tilesAndTime, AttemptedMove& attemptedMove)
{
	float tRise = 1.5f;
	float tShift = 1.5f + tRise;
	float tLower = tRise + tShift + 0.1; // Some small amount of additional time to correct with the board

	float zSpeed = 10.0f; // change to height rise to

	// Raise the piece
	if (tilesAndTime.timeSince < tRise)
	{
		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
				p._tris[i].p[k].z += zSpeed * tilesAndTime.elapsedTime;
	}
	// Shift the piece
	else if (tilesAndTime.timeSince < tShift)
	{
		float deltaY = (int)tilesAndTime.oldTile % 8 - (int)tilesAndTime.newTile % 8;
		float deltaX = (int)tilesAndTime.oldTile / 8 - (int)tilesAndTime.newTile / 8;

		float squareDimension = 9.9675f;
		float shiftSpeedx = squareDimension * deltaX / (tShift - tRise);
		float shiftSpeedy = squareDimension * deltaY / (tShift - tRise);

		// Adjust the movement to lock into the right position
		float moveX = -shiftSpeedx * tilesAndTime.elapsedTime;

		// Adjust the movement to lock into the right position
		float moveY = shiftSpeedy * tilesAndTime.elapsedTime;

		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
			{
				p._tris[i].p[k].x += moveX;
				p._tris[i].p[k].y += moveY;
			}
	}
	// Lower the piece
	else if (tilesAndTime.timeSince < tLower)
	{
		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
				p._tris[i].p[k].z -= zSpeed * tilesAndTime.elapsedTime;
	}
	// Stop calling movePiece function
	else if (tilesAndTime.timeSince > tLower)
	{
		// Move piece back to the top of the board if it is below it
		for (int i = 0; i < p._triCount; i++)
			for (int k = 0; k < 3; k++)
				if (p._tris[i].p[k].z < 0.0f)
				{
					float arr[3] = { 0.0f, 0.0f, -(p._tris[i].p[k].z) };
					objectMov(p, arr);
				}

		p._position = tilesAndTime.newTile;
		return false;
	}

	return true;
}

// Gets the piece specified by pieceTypeNTeam
bool getPieceAndMoveIt(GameBoard& game, const TileNTime& tilesAndTime, AttemptedMove& attemptedMove)
{
	auto playerPieces = [](Player& p, const TileNTime& tilesAndTime, AttemptedMove& attemptedMove) {

		int piece = -1;
		for (int i = 0; i < 16; i++)
		{
			if (p._pieces[i]._position == tilesAndTime.oldTile)
				piece = i;
		}
		if (piece == -1)
			return false;
		else
			return movePiece(p._pieces[piece], tilesAndTime, attemptedMove);
		};
	if (playerPieces(game._pOne, tilesAndTime, attemptedMove))
		return true;
	else if (playerPieces(game._pTwo, tilesAndTime, attemptedMove))
		return true;
	else
		return false;
};

// Sets objects to 0 0 0
template<typename pieceOrBoard>
void vertexCorrection(pieceOrBoard& obj)
{
	float x = obj._tris[0].p[0].x;
	float y = obj._tris[0].p[0].y;
	float z = obj._tris[0].p[0].z;
	float greatestX = -1000.0f; // Nothing should be that far away (at -1000, -1000)
	float greatestY = -1000.0f;
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

			// Get the greatest x and y values
			if (obj._tris[i].p[k].x > greatestX)
				greatestX = obj._tris[i].p[k].x;
			if (obj._tris[i].p[k].y > greatestY)
				greatestY = obj._tris[i].p[k].y;
		}
	}

	float offsetX = 9.9675f / 2.0f - (greatestX - x) / 2.0f;
	float offsetY = 9.9675f / 2.0f - (greatestY - y) / 2.0f;

	// Set the position to 0 0 0 
	float arr[3] = { -x + offsetX + obj._xPos, -y + offsetY + obj._yPos, -z + obj._zPos };
	objectMov(obj, arr);
}

template<typename pieceOrBoard>
void trangleSubLoader(const pieceOrBoard& obj, vector<triangle>& trangles)
{
	for (int i = 0; i < obj._triCount; i++)
		trangles.push_back(obj._tris[i]);
}

void trangleSubLoaderOverlay(const BoardOverlay& overlay, const vector<tileAvailability> availables, vector<triangle>& trangles)
{
	for (const auto t : availables)
	{
		trangles.push_back(overlay._tris[2 * t._tile]); // If empty available title
		trangles.push_back(overlay._tris[2 * t._tile + 1]); // If empty available title

		if (t._enemyPiecePresent) // If enemy occupies the available title
		{
			trangles.back().color = { 255, 255, 20 }; // Set for later change to red
			trangles[trangles.size() - 2].color = { 255, 255, 20 };
		}

	};
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

vector<tileAvailability> moveChecker(const int team, const int pieceType, const int currentTileValue, const GameBoard& board)
{
	// Checks if the given x or y are out of bounds
	auto boundsChecker = [](const int x, const int y)
	{
		if (x < 0)
		{
			std::cout << "out of x(0) bounds" << std::endl;
			return true;
		}
		if (x > 7)
		{
			std::cout << "out of x(7) bounds" << std::endl;
			return true;
		}
		if (y < 0)
		{
			std::cout << "out of y(0) bounds" << std::endl;
			return true;
		}
		if (y > 7)
		{
			std::cout << "out of y(7) bounds" << std::endl;
			return true;
		}

		return false;
	};

	auto tileAccumulator = [boundsChecker](const GameBoard& board, const int team, vector<tileAvailability>& availableTiles, const int x, const int y)
	{
		if (boundsChecker(x, y))
			return false;
		if (board._boardTiles[x][y].piecePresent)
		{
			if (board._boardTiles[x][y].pieceTypeNTeam / 7 != team)
				availableTiles.push_back({ true, x * 8 + y });
			return false;
		}
		else
		{
			availableTiles.push_back({ false, x * 8 + y });
		}
		return true;
	};


	vector<tileAvailability> availableTiles;

	int currentTileX = currentTileValue / 8;
	int currentTileY = currentTileValue % 8;

	// Check if we have selected a piece on our team. If not, break
	if (board._boardTiles[currentTileX][currentTileY].pieceTypeNTeam / 7 != team)
		return { { false, -1 } };

	switch ((pieceType - 1) % 6 + 1)
	{
	case 1: // Pawns
		// can only move to 3 tiles or 4 at start
		if (currentTileY == 6 - 5 * team) // Pawns can move two spaces on their first move
		{
			if (!board._boardTiles[currentTileX][currentTileY - 2 + 4 * team].piecePresent)
				availableTiles.push_back({ false, currentTileValue - 2 + 4 * team });
		}
		for (int x = currentTileX - 1; x < currentTileX + 2; x++)
		{
			// Bounds checking
			if (boundsChecker(x, currentTileY - 1 + 2 * team))
				continue;

			if (x != currentTileX && board._boardTiles[x][currentTileY - 1 + 2 * team].piecePresent)
			{
				if (board._boardTiles[x][currentTileY - 1 + 2 * team].pieceTypeNTeam / 7 != team)
					availableTiles.push_back({ true, x * 8 + currentTileY - 1 + 2 * team });
			}
			else if (x == currentTileX && !board._boardTiles[x][currentTileY - 1 + 2 * team].piecePresent)
			{
				availableTiles.push_back({ false, x * 8 + currentTileY - 1 + 2 * team });
			}
		}
		break;
	case 2: // Rooks
	{
		// Direction bools -> false if path blocked
		bool bUp = true;
		bool bDown = true;
		bool bLeft = true;
		bool bRight = true;

		int iUp = 1;
		int iDown = -1;
		int iLeft = -1;
		int iRight = 1;

		while (bUp || bDown || bLeft || bRight)
		{
			if (bUp)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX, currentTileY + iUp))
					iUp++;
				else
					bUp = false;
			}
			if (bDown)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX, currentTileY + iDown))
					iDown--;
				else
					bDown = false;
			}
			if (bLeft)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iLeft, currentTileY))
					iLeft--;
				else
					bLeft = false;
			}
			if (bRight)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iRight, currentTileY))
					iRight++;
				else
					bRight = false;
			}
		}
	}
		break;
	case 3: // Knights
	{
		struct intXY
		{
			int _x = 0;
			int _y = 0;
		};
		vector <intXY> knightTileOffsets = { {-1 , -2}, {1 , -2}, {-1 , 2}, {1 , 2}, {-2 , -1}, {2 , -1}, {-2 , 1}, {2 , 1}, };
		for (int i = 0; i < 8; i++)
		{
			tileAccumulator(board, team, availableTiles, currentTileX + knightTileOffsets[i]._x, currentTileY + knightTileOffsets[i]._y);
		}
	}
		break;
	case 4: // Bishops
	{
		// Direction bools -> false if path blocked
		bool bUpLeft = true;
		bool bUpRight = true;
		bool bDownLeft = true;
		bool bDownRight = true;

		int iUpLeft = 1;
		int iUpRight = 1;
		int iDownLeft = -1;
		int iDownRight = -1;

		while (bUpLeft || bUpRight || bDownLeft || bDownRight)
		{
			if (bUpLeft)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iUpLeft, currentTileY + iUpLeft))
					iUpLeft++;
				else
					bUpLeft = false;
			}
			if (bUpRight)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX - iUpRight, currentTileY + iUpRight))
					iUpRight++;
				else
					bUpRight = false;
			}
			if (bDownLeft)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iDownLeft, currentTileY + iDownLeft))
					iDownLeft--;
				else
					bDownLeft = false;
			}
			if (bDownRight)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX - iDownRight, currentTileY + iDownRight))
					iDownRight--;
				else
					bDownRight = false;
			}
		}
	}
		break;
	case 5: // Kings
		for (int x = currentTileX - 1; x < currentTileX + 2; x++)
			for (int y = currentTileY - 1; y < currentTileY + 2; y++)
			{
				tileAccumulator(board, team, availableTiles, x, y);
			}
		break;
	case 6: // Queens
	{
		// Rook-like movement
		// Direction bools -> false if path blocked
		bool bUp = true;
		bool bDown = true;
		bool bLeft = true;
		bool bRight = true;

		int iUp = 1;
		int iDown = -1;
		int iLeft = -1;
		int iRight = 1;

		while (bUp || bDown || bLeft || bRight)
		{
			if (bUp)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX, currentTileY + iUp))
					iUp++;
				else
					bUp = false;
			}
			if (bDown)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX, currentTileY + iDown))
					iDown--;
				else
					bDown = false;
			}
			if (bLeft)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iLeft, currentTileY))
					iLeft--;
				else
					bLeft = false;
			}
			if (bRight)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iRight, currentTileY))
					iRight++;
				else
					bRight = false;
			}
		}

		// Bishop-like movement
		// Direction bools -> false if path blocked
		bool bUpLeft = true;
		bool bUpRight = true;
		bool bDownLeft = true;
		bool bDownRight = true;

		int iUpLeft = 1;
		int iUpRight = 1;
		int iDownLeft = -1;
		int iDownRight = -1;

		while (bUpLeft || bUpRight || bDownLeft || bDownRight)
		{
			if (bUpLeft)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iUpLeft, currentTileY + iUpLeft))
					iUpLeft++;
				else
					bUpLeft = false;
			}
			if (bUpRight)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX - iUpRight, currentTileY + iUpRight))
					iUpRight++;
				else
					bUpRight = false;
			}
			if (bDownLeft)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX + iDownLeft, currentTileY + iDownLeft))
					iDownLeft--;
				else
					bDownLeft = false;
			}
			if (bDownRight)
			{
				if (tileAccumulator(board, team, availableTiles, currentTileX - iDownRight, currentTileY + iDownRight))
					iDownRight--;
				else
					bDownRight = false;
			}
		}
	}
		break;
	default:
		break;
	}

	availableTiles.push_back({ false, -1 }); // Add an invalid tile for later find_if searching
	for (auto& i : availableTiles)
	{
		std::cout << i._tile << " ";
	}
	std::cout << std::endl;
	return availableTiles;
}

void pawnChanger(const int pieceType, const int currentTileValue, GameBoard& board)
{
	auto playerPieces = [](const Player& p, const int currentTileValue) {
		for (int i = 0; i < 16; i++)
		{
			if (p._pieces[i]._position == currentTileValue)
				return i;
		}
	};

	auto getPiecetTemplate = [](const Player& p, const int type) {
		for (int i = 0; i < 16; i++)
		{
			if (p._pieces[i].PieceTypeNTeam == type)
				return i;
		}
	};

	auto pieceChange = [](Piece& c /*Copy*/, const Piece& o /*Origin*/) {
		c.PieceTypeNTeam = o.PieceTypeNTeam;
		c._triCount = o._triCount;
		for (int i = 0; i < o._triCount; i++)
		{
			c._tris[i] = o._tris[i];
		}
	};

	float squareDimension = 9.9675f;

	int playerPiece = 16;
	if (pieceType / 7 == 0)
	{
		playerPiece = playerPieces(board._pOne, currentTileValue);
		if (playerPiece < 16)
		{
			pieceChange(board._pOne._pieces[playerPiece], board._pOne._pieces[getPiecetTemplate(board._pOne, pieceType)]);

			board._pOne._pieces[playerPiece]._xPos = (currentTileValue / 8) * squareDimension;
			board._pOne._pieces[playerPiece]._yPos = 7.0f * squareDimension; // Only one possible side
			vertexCorrection(board._pOne._pieces[playerPiece]);
		}
	}
	else
	{
		playerPiece = playerPieces(board._pTwo, currentTileValue);
		if (playerPiece < 16)
		{
			pieceChange(board._pTwo._pieces[playerPiece], board._pTwo._pieces[getPiecetTemplate(board._pTwo, pieceType)]);

			board._pTwo._pieces[playerPiece]._xPos = (currentTileValue / 8) * squareDimension;
			board._pTwo._pieces[playerPiece]._yPos = 0.0f; // Only one possible side
			vertexCorrection(board._pTwo._pieces[playerPiece]);
		}
	}
}


bool LoadFromChessBoardObjectFile(GameBoard& board, string sFilename, bool bHasTexture, int imageWidth, int imageHeight)
{
	vector<triangle> _tris;
	ifstream file(sFilename);
	if (!file.is_open())
		return false;

	// Local cache of verts
	vector<vec3D> verts;
	vector<vec2D> texs;

	vector<int> dataLineStarting;

	int objectcount = 0;
	char junk;
	while (!file.eof())
	{
		char line[128];
		file.getline(line, 128);

		strstream s;
		s << line;

		if (line[0] == 'o')
		{
			dataLineStarting.push_back(_tris.size());
			objectcount++;
			std::cout << _tris.size() << " ";
		}

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				if (objectcount == 14)
				{
					imageWidth = 256;
					imageHeight = 256;
				}
				vec2D v;
				s >> junk >> junk >> v.u >> v.v;
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
				_tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
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


				_tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
								 texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
			}
		}
	}

	std::cout << std::endl;

	struct doubleTri
	{
		triangle _one;
		triangle _two;
	};

	vector<doubleTri> trisToSort;
	for (int i = _tris.size() - board._overlay._triCount; i < _tris.size(); i += 2)
		trisToSort.push_back({ _tris[i], _tris[i + 1] }); // Keep pairs of triangles as they already form their own squares

	std::sort(trisToSort.begin(), trisToSort.end(),
		[](const doubleTri& a, const doubleTri& b) {

			float xa = a._one.p[0].x;
			float ya = a._one.p[0].y;
			for (int k = 0; k < 3; k++) // Find lowest x and y
			{
				if (a._one.p[k].x < xa)
					xa = a._one.p[k].x;
				if (a._one.p[k].y < ya)
					ya = a._one.p[k].y;
			}

			float xb = b._one.p[0].x;
			float yb = b._one.p[0].y;
			for (int k = 0; k < 3; k++) // Find lowest x and y
			{
				if (b._one.p[k].x < xb)
					xb = b._one.p[k].x;
				if (b._one.p[k].y < yb)
					yb = b._one.p[k].y;
			}

			return (xa * 400.0f - ya < xb * 400.0f - yb);
		});

	int idx = 0;
	for (int i = 0; i < trisToSort.size(); i++)
	{
		board._overlay._tris[idx] = trisToSort[i]._one;
		board._overlay._tris[idx + 1] = trisToSort[i]._two;
		idx += 2;
	}

	//dataLineStarting.push_back(174);
	int triIndex = 0;
	int modelIndex = 0;
	//std::cout << "tris size: " << tris.size() << std::endl;
	for (int i = 0; i < 174/*_tris.size()*/; i++)
	{
		if (i == dataLineStarting[modelIndex])
		{
			modelIndex++;
			triIndex = 0;
			//std::cout << "            modelIndex: " << modelIndex << std::endl;
		}

		//std::cout << "tri num: " << i << "   triIndex: " << triIndex << std::endl;
		switch (modelIndex - 1)
		{
			// P1
		case 0: // WhBishops
			if (board._pOne._pieces[10]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << " triCount: " << board._pOne._pieces[11]._triCount << std::endl;
				break;
			}
			board._pOne._pieces[10]._tris[triIndex] = _tris[i];
			board._pOne._pieces[13]._tris[triIndex] = _tris[i];
			break;

		case 1: // WhPawns
			if (board._pOne._pieces[0]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << " triCount: " << board._pOne._pieces[0]._triCount << std::endl;
				break;
			}
			for (int k = 0; k < 8; k++)
				board._pOne._pieces[k]._tris[triIndex] = _tris[i];
			break;

		case 2: // WhQueen
			if (board._pOne._pieces[12]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << " triCount: " << board._pOne._pieces[13]._triCount << std::endl;
				break;
			}
			board._pOne._pieces[12]._tris[triIndex] = _tris[i];
			break;

		case 3: // WhKnight
			if (board._pOne._pieces[9]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << " triCount: " << board._pOne._pieces[10]._triCount << std::endl;
				break;
			}
			board._pOne._pieces[9]._tris[triIndex] = _tris[i];
			board._pOne._pieces[14]._tris[triIndex] = _tris[i];
			break;

		case 4: // WhKing
			if (board._pOne._pieces[11]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << " triCount: " << board._pOne._pieces[12]._triCount << std::endl;
				break;
			}
			board._pOne._pieces[11]._tris[triIndex] = _tris[i];
			break;

		case 5: // WhRook
			if (board._pOne._pieces[8]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << " triCount: " << board._pOne._pieces[9]._triCount << std::endl;
				break;
			}
			board._pOne._pieces[8]._tris[triIndex] = _tris[i];
			board._pOne._pieces[15]._tris[triIndex] = _tris[i];
			break;

			// P2
		case 6: // GrBishops
			if (board._pTwo._pieces[10]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			board._pTwo._pieces[10]._tris[triIndex] = _tris[i];
			board._pTwo._pieces[13]._tris[triIndex] = _tris[i];
			break;

		case 7: // GrPawns
			if (board._pTwo._pieces[0]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			for (int k = 0; k < 8; k++)
				board._pTwo._pieces[k]._tris[triIndex] = _tris[i];
			break;

		case 8: // GrQueen
			if (board._pTwo._pieces[12]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			board._pTwo._pieces[12]._tris[triIndex] = _tris[i];
			break;

		case 9: // GrKnight
			if (board._pTwo._pieces[9]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			board._pTwo._pieces[9]._tris[triIndex] = _tris[i];
			board._pTwo._pieces[14]._tris[triIndex] = _tris[i];
			break;

		case 10: // GrKing
			if (board._pTwo._pieces[11]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			board._pTwo._pieces[11]._tris[triIndex] = _tris[i];
			break;

		case 11: // GrRook
			if (board._pTwo._pieces[8]._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			board._pTwo._pieces[8]._tris[triIndex] = _tris[i];
			board._pTwo._pieces[15]._tris[triIndex] = _tris[i];
			break;

		case 12: // Board
			if (board._triCount < triIndex)
			{
				std::cout << "TriIndex error! BREAKING " << modelIndex << std::endl;
				break;
			}
			board._tris[triIndex] = _tris[i];
			break;

		default:
			break;
		}

		triIndex++;
	}

	return true;
}

void GameBoard::LoadGame(GameBoard& theGame)
{
	LoadFromChessBoardObjectFile(theGame, "../Objects/Chess Board and 6 type pieces for 2 teams ReducedVTX.txt", true, 48, 48);
	vertexCorrection(theGame);
	vertexCorrection(theGame._overlay);

	subColorChange(theGame, { 106, 181, 0, 0 });
	subColorChange(theGame._overlay, { 255, 255, 25, 0 });

	// All pieces
	for (int i = 0; i < 16; i++)
	{
		// Player one's pieces
		vertexCorrection(theGame._pOne._pieces[i]);
		// Player two's pieces
		vertexCorrection(theGame._pTwo._pieces[i]);
	}

	// Player one's color
	colorChange(theGame._pOne, { 255, 0, 0, 0 });

	// Player two's color
	colorChange(theGame._pTwo, { 0, 255, 255, 0 });
}