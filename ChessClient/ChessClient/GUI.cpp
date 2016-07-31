#include "stdafx.h"
#include "GUI.h"
#include <cstring>
#include <iostream>
#include <string>
#include <stdio.h>
#include <algorithm>


GLuint texture[12];

struct Image {

    unsigned long sizeX;
	unsigned long sizeY;
	char *data ;

};

typedef struct Image Image;
#define checkImageWidth 64
#define checkImageHeight 64


GLubyte checkImage[checkImageWidth][checkImageHeight][3];

void MakeCheckImage(void){

    int i, j, c;

    for (i = 0; i < checkImageWidth; i++) {

        for (j = 0; j < checkImageHeight; j++) {

            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;

            checkImage[i][j][0] = (GLubyte) c;

            checkImage[i][j][1] = (GLubyte) c;

            checkImage[i][j][2] = (GLubyte) c;

        }

    }

}

int ImageLoad(char *filename, Image *image) {

    FILE *file;
    unsigned long size; // size of the image in bytes.
    unsigned long i; // standard counter.
	unsigned short int planes; // number of planes in image (must be 1)
    unsigned short int bpp; // number of bits per pixel (must be 24)
    char temp; // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
	errno_t err = fopen_s(&file,filename,"rb");
    if (file==NULL){
        printf("File Not Found : %s\n",filename);
        return 0;
    }

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);
    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }

    //printf("Width of %s: %lu\n", filename, image->sizeX);
    // read the height

    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }

    //printf("Height of %s: %lu\n", filename, image->sizeY);
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;
    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }

    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }

    // read the bitsperpixel

    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }

    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }

    // seek past the rest of the bitmap header.

    fseek(file, 24, SEEK_CUR);

    // read the data.

    image->data = (char *) malloc(size);

    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }

    // we're done.

    return 1;

}

Image * LoadTexture(char *filename){
    Image *image1;
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
		//exit(0);
    }

	if (!ImageLoad(filename, image1)) {
        //exit(1);
    }

    return image1;

}
// -----------------------------------------------------------------------------------------------

namespace GUI
{
	GUIData data;
}

//Inizialises the chess board
void GUI::GUIData::InitBoard(int w, int h)
{
	int size = GetSize(w,h);
	w = GetStartWidth(w,size);
	h = GetStartHeight(h,size);
	_board = Board(w, h, size);
	_board.InitBoard(_player, _opponent);
	_boardInit = true;
	if(_player == PlayerType::P_ONE)
	{
		_isTurn = true;
	}
	else
	{
		_isTurn = false;
	}
}

//Determines the size of the pieces and tiles based on the size of the window
void GUI::GUIData::SetBoardSize(int w, int h)
{
	int size = GetSize(w,h);
	w = GetStartWidth(w,size);
	h = GetStartHeight(h,size);

	for(int i = 0; i < 8; ++i)
	{
		for (int j= 0; j < 8; j++)
		{
			_board.board[i][j]._height = size;
			_board.board[i][j]._width = size;
			_board.board[i][j]._posH = i * size + h;
			_board.board[i][j]._posW = j * size + w;
		}
	}
}

//Gets the appropriate size for the tiles and pieces
//This is based on whether the window is wider than it is tall or vise versa
int GUI::GUIData::GetSize(int w, int h)
{
	int size;
	if(w>h)
		size = h;
	else
		size = w;
	size /= 10;
	return size;
}

//Calculates size based on width
int GUI::GUIData::GetStartWidth(int w, int size)
{
	int width = windowWidth/2;
	width-=size*4;	
	return width;
}

//Calculates size based on height
int GUI::GUIData::GetStartHeight(int h, int size)
{
	int height = windowHeight/2;
	height-=size*4;
	return height;
}

//Displays text to the screen
void GUI::GUIData::DisplayText()
{
	//In case text needs to be displayed
}

//Updates the game every frame
std::string GUI::Update()
{
	glutMainLoopEvent();
	std::string msg = data._movemsg;
	if(msg.find("|P") != std::string::npos && msg.find("|P->") == std::string::npos)
	{
		msg = "";
	}
	else
	{
		data._movemsg = "";
	}
	return msg;
}

//Detects keypress for opengl
void GUI::Special(int key, int i, int j)
{
	//Incase need to detect keypress
}

//Determines available moves for a bishop at the given location
void GUI::GUIData::HighlighBishop(int w, int h)
{
	int y = h + 1;
	int x = w + 1;
	while(y < 8 && x < 8)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		y++;
		x++;
	}
	y = h + 1;
	x = w - 1;
	while(y < 8 && x > -1)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		y++;
		x--;
	}
	y = h - 1;
	x = w + 1;
	while(y > -1 && x < 8)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		y--;
		x++;
	}
	y = h - 1;
	x = w - 1;
	while(y > -1 && x > -1)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		y--;
		x--;
	}
}

//Determines available moves for a King at the given location
void GUI::GUIData::HighlighKing(int w, int h)
{
	for(int i = h - 1; i < h + 2; i++)
	{
		if(i < 0)
		{
			i = 0;
		}
		if(i > 7)
		{
			break;
		}
		for(int j = w - 1; j < w + 2; j++)
		{
			if(j < 0)
			{
				j = 0;
			}
			if(j > 7)
			{
				break;
			}
			if(i == h && j == w)
			{
				continue;
			}
			if(_board.board[i][j]._player == _player)
			{
				continue;
			}
			else
			{
				highlightTiles.push_back(&_board.board[i][j]);
			}
		}
	}
}

//Determines available moves for a Knight at the given location
void GUI::GUIData::HighlighKnight(int w, int h)
{
	if(h + 2 <= 7)
	{
		if(w + 1 <= 7 && _board.board[h+2][w+1]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h+2][w+1]);
		}
		if(w - 1 >= 0 && _board.board[h+2][w-1]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h+2][w-1]);
		}
	}
	if(h - 2 >= 0)
	{
		if(w + 1 <= 7 && _board.board[h-2][w+1]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h-2][w+1]);
		}
		if(w - 1 >= 0 && _board.board[h-2][w-1]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h-2][w-1]);
		}
	}

	if(w + 2 <= 7)
	{
		if(h + 1 <= 7 && _board.board[h+1][w+2]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h+1][w+2]);
		}
		if(h - 1 >= 0 && _board.board[h-1][w+2]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h-1][w+2]);
		}
	}
	if(w - 2 >= 0)
	{
		if(h + 1 <= 7 && _board.board[h+1][w-2]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h+1][w-2]);
		}
		if(h - 1 >= 0 && _board.board[h-1][w-2]._player != _player)
		{
			highlightTiles.push_back(&_board.board[h-1][w-2]);
		}
	}
}

//Determines available moves for a pawn at the given location
void GUI::GUIData::HighlighPawn(int w, int h)
{
	//moving forward
	if(h == 6 && _board.board[h-2][w]._player == PlayerType::NONE)
	{
		highlightTiles.push_back(&_board.board[h-2][w]);
	}
	if(_board.board[h-1][w]._player == PlayerType::NONE)
	{
		highlightTiles.push_back(&_board.board[h-1][w]);
	}
	//attacking
	if(w<=6 && _board.board[h-1][w+1]._player == _opponent)
	{
		highlightTiles.push_back(&_board.board[h-1][w+1]);
	}
	if(w>=1 && _board.board[h-1][w-1]._player == _opponent)
	{
		highlightTiles.push_back(&_board.board[h-1][w-1]);
	}
}

//Determines available moves for a Queen at the given location
void GUI::GUIData::HighlighQueen(int w, int h)
{
	HighlighBishop(w,h);
	HighlighRook(w,h);
}

//Determines available moves for a Rook at the given location
void GUI::GUIData::HighlighRook(int w, int h)
{
	int y = h + 1;
	int x = w;
	while(y < 8)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		y++;
	}
	y = h - 1;
	while(y > -1)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		y--;
	}
	y = h;
	x = w + 1;
	while(x < 8)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		x++;
	}
	x = w - 1;
	while(x > -1)
	{
		if(_board.board[y][x]._player == _player)
		{
			break;
		}
		else if(_board.board[y][x]._player == _opponent)
		{
			highlightTiles.push_back(&_board.board[y][x]);
			break;
		}
		else
		{
			highlightTiles.push_back(&_board.board[y][x]);
		}
		x--;
	}
}

//Determines the moves for a given piece
void GUI::GUIData::HighlightOptions(int w, int h, bool clear)
{
	if(clear)
	{
		for_each(highlightTiles.begin(), highlightTiles.end(), [](Tile* tile) {tile->_isHighlighted = false;});
		highlightTiles.clear();
	}
	else
	{
		switch(_board.board[h][w]._piece)
		{
			case PieceType::BISHOP:
				HighlighBishop(w, h);
				break;
			case PieceType::KING:
				HighlighKing(w,h);
				break;
			case PieceType::KNIGHT:
				HighlighKnight(w,h);
				break;
			case PieceType::PAWN:
				HighlighPawn(w,h);
				break;
			case PieceType::QUEEN:
				HighlighQueen(w,h);
				break;
			case PieceType::ROOK:
				HighlighRook(w,h);
				break;
			default:
				break;
		}
		for_each(highlightTiles.begin(), highlightTiles.end(), [](Tile* tile) {tile->_isHighlighted = true;});
	}
}

//Searches if both kings exsist on the board
void GUI::GUIData::FindKings()
{
	bool whiteKing = false;
	bool blackKing = false;
	for each (Tile tile in _board.board)
	{
		if(tile._piece == PieceType::KING)
		{
			if(tile._player == PlayerType::P_ONE)
			{
				whiteKing = true;
			}
			else
			{
				blackKing = true;
			}
		}
	}
	if(!blackKing && whiteKing)
	{
		EndGame(PlayerType::P_ONE);
	}
	if(blackKing && !whiteKing)
	{
		EndGame(PlayerType::P_TWO);
	}
}

//If a king is missing, declare a winner
void GUI::GUIData::EndGame(PlayerType::Player player)
{
	_isTurn = false;
	if(player == _player)
	{
		std::cout<<"CONGRADULATIONS! YOU WIN!"<<std::endl;
	}
	else
	{
		std::cout<<"Sorry, you lost"<<std::endl;
	}
	std::cout<< "The game is over and the winner has been declared. \nYou may return to the lobby by entering (Return to lobby)."<<std::endl;
}

//If a pawn reaches the other end of the board, promote it
void GUI::GUIData::PromotePawn(PieceType::Piece piece)
{
	
	for (int i = 0; i < 8; i++)
	{
		if(_board.board[0][i]._player == _player && _board.board[0][i]._piece == PieceType::PAWN)
		{
			_board.board[0][i]._piece = piece;
			break;
		}
	}
	_isTurn = false;
	_promotePawn = false;
	_movemsg+="->";
	switch (piece)
	{
	case PieceType::QUEEN:
		_movemsg+="Q";
		break;
	case PieceType::BISHOP:
		_movemsg+="B";
		break;
	case PieceType::KNIGHT:
		_movemsg+="K";
		break;
	case PieceType::ROOK:
		_movemsg+="R";
		break;
	default:
		break;
	}
	
}

//Detect a mouse click for opengl
void GUI::MouseClick(int button, int state, int x, int y)
{
	if(data._isTurn && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if(data._promotePawn)
		{
			int size = data.GetSize(data.windowWidth,data.windowHeight);
			int w = data.GetStartWidth(data.windowWidth,size);
			int h = data.GetStartHeight(data.windowHeight,size);

			size *= 2;

			if(y >= h && y <= h+size)
			{
				if(x >= w && x <= w+size)
				{
					data.PromotePawn(PieceType::BISHOP);
				}
				if(x >= w+size && x <= w+size*2)
				{
					data.PromotePawn(PieceType::KNIGHT);
				}
				if(x >= w+size*2 && x <= w+size*3)
				{
					data.PromotePawn(PieceType::QUEEN);
				}
				if(x >= w+size*3 && x <= w+size*4)
				{
					data.PromotePawn(PieceType::ROOK);
				}
			}
		}
		else
		{
			Tile *tile = data._board.FindTile(x,y);
			if(tile->_isHighlighted)
			{
				std::string move = "MOVE:";
				move += std::to_string(data.selectedTile->_x) + ",";
				move += std::to_string(data.selectedTile->_y) + ",";
				move += std::to_string(tile->_x) + ",";
				move += std::to_string(tile->_y) + "|";
				data._movemsg = move;
				data._isTurn = false;
				data.selectedTile->_isSelected = false;
				data.HighlightOptions(data.selectedTile->_x, data.selectedTile->_y, true);
				tile->_player = data.selectedTile->_player;
				tile->_piece = data.selectedTile->_piece;
				data.selectedTile->_player = PlayerType::NONE;
				data.selectedTile->_piece = PieceType::NONE;
				data.selectedTile = NULL;
				if(tile->_piece == PieceType::PAWN && tile->_y == 0)
				{
					data._isTurn = true;
					data._promotePawn = true;
					data._movemsg+="|P";
				}
				data.FindKings();
			}
			else if(tile->_player == data._player)
			{
				if(data.selectedTile != NULL && data.selectedTile->_posH == tile->_posH && data.selectedTile->_posW == tile->_posW)
				{
					data.selectedTile->_isSelected = false;
					data.HighlightOptions(data.selectedTile->_x, data.selectedTile->_y, true);
					data.selectedTile = NULL;
				}
				else if(data.selectedTile != NULL)
				{
					data.selectedTile->_isSelected = false;
					data.HighlightOptions(data.selectedTile->_x, data.selectedTile->_y, true);
					data.selectedTile = tile;
					data.selectedTile->_isSelected = true;
					data.HighlightOptions(data.selectedTile->_x, data.selectedTile->_y, false);
				}
				else
				{
					data.selectedTile = tile;
					data.selectedTile->_isSelected = true;
					data.HighlightOptions(data.selectedTile->_x, data.selectedTile->_y, false);
				}
			}
		}
	}
}

//Moves the opponent piece when the instruction to do so has been recieved
void GUI::MoveOpponent(std::string move)
{
	int startx;
	int starty;
	int endx;
	int endy;
	startx = (int)move.at(5) - '0';
	starty = (int)move.at(7) - '0';
	endx = (int)move.at(9) - '0';
	endy = (int)move.at(11) - '0';
	starty = 7- starty;
	endy =7 - endy;
	data._isTurn = true;
	data._board.board[endy][endx]._player = data._board.board[starty][startx]._player;
	data._board.board[endy][endx]._piece = data._board.board[starty][startx]._piece;
	data._board.board[starty][startx]._player = PlayerType::NONE;
	data._board.board[starty][startx]._player = PlayerType::NONE;
	if(move.find("|P->") != std::string::npos)
	{
		if(move.find("Q") != std::string::npos)
		{
			data._board.board[endy][endx]._piece = PieceType::QUEEN;
		}
		else if(move.find("B") != std::string::npos)
		{
			data._board.board[endy][endx]._piece = PieceType::BISHOP;
		}
		else if(move.find("K") != std::string::npos)
		{
			data._board.board[endy][endx]._piece = PieceType::KNIGHT;
		}
		else
		{
			data._board.board[endy][endx]._piece = PieceType::ROOK;
		}
	}
	data.FindKings();
	glutPostRedisplay();
}

//Renders the images into their locations if a piece exsits
void GUI::RenderPieces(int i, int j)
{
	int size = data._board.board[i][j]._width / 10;
	int x = data._board.board[i][j]._posW + size;
	int y = data._board.board[i][j]._posH + size;

	size *= 8;

	glEnable(GL_TEXTURE_2D);
	switch (data._board.board[i][j]._player)
	{
	case PlayerType::P_TWO:
		switch (data._board.board[i][j]._piece)
		{
		case PieceType::BISHOP:
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			break;
		case PieceType::KING:
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			break;
		case PieceType::KNIGHT:
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			break;
		case PieceType::PAWN:
			glBindTexture(GL_TEXTURE_2D, texture[3]);
			break;
		case PieceType::QUEEN:
			glBindTexture(GL_TEXTURE_2D, texture[4]);
			break;
		case PieceType::ROOK:
			glBindTexture(GL_TEXTURE_2D, texture[5]);
			break;
		default:
			break;
		}
		break;
	case PlayerType::P_ONE:
		switch (data._board.board[i][j]._piece)
		{
		case PieceType::BISHOP:
			glBindTexture(GL_TEXTURE_2D, texture[6]);
			break;
		case PieceType::KING:
			glBindTexture(GL_TEXTURE_2D, texture[7]);
			break;
		case PieceType::KNIGHT:
			glBindTexture(GL_TEXTURE_2D, texture[8]);
			break;
		case PieceType::PAWN:
			glBindTexture(GL_TEXTURE_2D, texture[9]);
			break;
		case PieceType::QUEEN:
			glBindTexture(GL_TEXTURE_2D, texture[10]);
			break;
		case PieceType::ROOK:
			glBindTexture(GL_TEXTURE_2D, texture[11]);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	


      glBegin( GL_QUADS );
      glTexCoord2d(0,1); glVertex2d(x,y);
      glTexCoord2d(0, 0); glVertex2d(x,y+size);
      glTexCoord2d(1,0); glVertex2d(x+size,y+size);
      glTexCoord2d(1,1); glVertex2d(x+size,y);
      glEnd();

	glDisable(GL_TEXTURE_2D);

}

//Renders the promote options if a pawn can be promoted
void GUI::RenderPromoteOptions()
{
	int size = data.GetSize(data.windowWidth,data.windowHeight);
	int w = data.GetStartWidth(data.windowWidth,size);
	int h = data.GetStartHeight(data.windowHeight,size);

	size *= 2;
	switch (data._player)
	{
	case PlayerType::P_TWO:
		for(int i = 0; i < 4; i++)
		{
			glEnable(GL_TEXTURE_2D);
			if(i==0)
				glBindTexture(GL_TEXTURE_2D, texture[0]);
			if(i==1)
				glBindTexture(GL_TEXTURE_2D, texture[2]);
			if(i==2)
				glBindTexture(GL_TEXTURE_2D, texture[4]);
			if(i==3)
				glBindTexture(GL_TEXTURE_2D, texture[5]);
			
			glBegin( GL_QUADS );
			glTexCoord2d(0,1); glVertex2d(w,h);
			glTexCoord2d(0, 0); glVertex2d(w,h+size);
			glTexCoord2d(1,0); glVertex2d(w+size,h+size);
			glTexCoord2d(1,1); glVertex2d(w+size,h);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			w+=size;
		}
		break;
	case PlayerType::P_ONE:
		for(int i = 0; i < 4; i++)
		{
			glEnable(GL_TEXTURE_2D);
			if(i == 0)
				glBindTexture(GL_TEXTURE_2D, texture[6]);
			if(i==1)
				glBindTexture(GL_TEXTURE_2D, texture[8]);
			if(i == 2)
				glBindTexture(GL_TEXTURE_2D, texture[10]);
			if(i==3)
				glBindTexture(GL_TEXTURE_2D, texture[11]);

			glBegin( GL_QUADS );
			glTexCoord2d(0,1); glVertex2d(w,h);
			glTexCoord2d(0, 0); glVertex2d(w,h+size);
			glTexCoord2d(1,0); glVertex2d(w+size,h+size);
			glTexCoord2d(1,1); glVertex2d(w+size,h);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			w+=size;
		}
		break;
	default:
		break;
	}

}

//The general render function
void GUI::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if(data._board.board[i][j]._isSelected)
			{
				glColor3f(0.0f,1.0f,0.0f);
			}
			else if(data._board.board[i][j]._isHighlighted && data._board.board[i][j]._player == data._opponent)
			{
				glColor3f(1.0f,0.0f,0.0f);
			}
			else if(data._board.board[i][j]._isHighlighted)
			{
				glColor3f(0.0f,0.0f,1.0f);
			}
			else if(data._board.board[i][j]._isBlack)
			{
				glColor3f(0.0f,0.0f,0.0f);
			}
			else
			{
				glColor3f(1.0f,1.0f,1.0f);
			}
			glBegin(GL_QUADS);
			glVertex2d(data._board.board[i][j]._posW,data._board.board[i][j]._posH);
			glVertex2d(data._board.board[i][j]._posW + data._board.board[i][j]._width,data._board.board[i][j]._posH);
			glVertex2d(data._board.board[i][j]._posW + data._board.board[i][j]._width,data._board.board[i][j]._posH + data._board.board[i][j]._height);
			glVertex2d(data._board.board[i][j]._posW,data._board.board[i][j]._posH + data._board.board[i][j]._height);
			glEnd();
			if(data._board.board[i][j]._player != PlayerType::NONE)
			{
				RenderPieces(i, j);
			}
			
		}
	}
	if(data._promotePawn)
	{
		RenderPromoteOptions();
	}
	glutSwapBuffers();
}

//The general resize function
void GUI::ChangeViewport(int w, int h)
{	
	data.windowWidth = w;
	data.windowHeight = h;
	if(data._boardInit)
		data.SetBoardSize(data.windowWidth, data.windowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0,0,data.windowWidth,data.windowHeight);
	gluOrtho2D(0,data.windowWidth,data.windowHeight,0);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();

}

//Loades in the textures of the pieces
void GUI::PrepareTextures(char* filename, int textureNum)
{
	Image *image1 = LoadTexture(filename);
	if(image1 == NULL){

        printf("Image was not returned from loadTexture\n");

        //exit(0);

    }
	MakeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create Texture
    glBindTexture(GL_TEXTURE_2D, texture[textureNum]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

//Initalizes the textures to be used
void GUI::InitTextures()
{
	glGenTextures(12, texture);
	PrepareTextures("textures/BBishop.bmp", 0);
	PrepareTextures("textures/BKing.bmp", 1);
	PrepareTextures("textures/BKnight.bmp", 2);
	PrepareTextures("textures/BPawn.bmp", 3);
	PrepareTextures("textures/BQueen.bmp", 4);
	PrepareTextures("textures/BRook.bmp", 5);
	PrepareTextures("textures/WBishop.bmp", 6);
	PrepareTextures("textures/WKing.bmp", 7);
	PrepareTextures("textures/WKnight.bmp", 8);
	PrepareTextures("textures/WPawn.bmp", 9);
	PrepareTextures("textures/WQueen.bmp", 10);
	PrepareTextures("textures/WRook.bmp", 11);
}

//Initializes OpenGl
void GUI::Initialize(int argc, char* argv[], PlayerType::Player player)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("BattleNet!");
	glClearColor(0.5f,0.5f,0.5f,1.0f);
	InitTextures();
	glutReshapeFunc(&GUI::ChangeViewport);
	glutDisplayFunc(&GUI::Render);
	glutSpecialFunc(&GUI::Special);
	glutMouseFunc(&GUI::MouseClick);
	

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error");
	}
	data._player = player;
	if(player == PlayerType::P_ONE)
	{
		data._opponent = PlayerType::P_TWO;
	}
	else
	{
		data._opponent = PlayerType::P_ONE;
	}
	data.InitBoard(data.windowWidth, data.windowHeight);
}

const GUI::GUIData& GUI::GetData()
{
	 return data;
}

GUI::GUIData& GUI::MutableData()
{
	 return data;
}