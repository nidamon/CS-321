README

This was my attempt at making a 3D Chess Game with file mapping.

ABOUT:
	This was a project on file mapping. Although limited in the use of mapped files, the entire game
	world (face data, piece postions, which player's turn, etc.) was mapped into one file that was shared
	between the parent process and the two child processes which they could each write to. The game was 
	built off the 3D Graphics Engine made by YouTuber javidx9 aka One Lone Coder in his "Code-It-Yourself! 
	3D Graphics Engine series". Several changes were made to make use of his new PixelGameEngine2.0 and 
	to work in code such as selecting parts of the triangles that are displayed to the screen. The 3D 
	chess game models were made using 3D Builder with the needed data saved as an object file that was 
	then later edited to better handle being read by the program.

REQUIREMENTS TO RUN (maybe):
	The server/parent and the client/child both use Windows related code to get a mapped file between 
	them. Other than that it might run. With visual studio 2019, there should be few to no issues. 
	But I know some people who could not use the PixelGameEngine made by javidx9 without having to 
	install some libraries. Our simple conclusion was that visual studio might have already come with 
	those libraries or otherwise, so be advised.

STATUS:
	The game should be fully playable, however there is no checking the king or checkmating the king. 
	Winning comes from taking the other player's king (moving your piece to the other player's king's 
	position).
	

THE CONTROLS (thus far):
	Left mouse click: to select a tile that a piece is on and left mouse click 
		on another highlighted tile to tell it to go there. (The tile must be clicked on, 
		not the pieces. The pieces themselves cannot be clicked on.)
		This is also used to select the piece type you want a pawn to change to when prompted.
	
	Right mouse click and hold: to drag the view (imagine dragging the world 
		in the direction of the mouse’s movement).

	Scroll wheel (mouse): This increases and decreases the movement speed of the player's camera
	
	W, A, S,  and D: these move forward, left, backwards, and right,  respectively.
	
	Q and E: these move down and up respectively.
