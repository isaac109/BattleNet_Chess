#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include "Board.h"
#include <list>
#include <string>

namespace GUI
{
	void Initialize(int argc, char* argv[], PlayerType::Player player);
	void InitTextures();
	void PrepareTextures(char* filename, int textureNum);
	std::string Update();
	void Special(int key, int, int);
	void MouseClick(int button, int state, int x, int y);
	void MoveOpponent(std::string move);
	void Render();
	void RenderPieces(int i, int j);
	void RenderPromoteOptions();
	void ChangeViewport(int x, int h);
	struct GUIData
	{
		GUIData()
			: _boardInit(false)
			, _movemsg("")
		{
		};
		void InitBoard(int w, int h);
		void SetBoardSize(int w, int h);
		int GetSize(int w, int h);
		int GetStartWidth(int w, int size);
		int GetStartHeight(int h, int size);
		void HighlightOptions(int w, int h, bool clear);
		void HighlighBishop(int w, int h);
		void HighlighKing(int w, int h);
		void HighlighKnight(int w, int h);
		void HighlighPawn(int w, int h);
		void HighlighQueen(int w, int h);
		void HighlighRook(int w, int h);
		void PromotePawn(PieceType::Piece piece);
		void FindKings();
		void EndGame(PlayerType::Player player);
		void DisplayText();
		Board _board;
		int windowWidth;
		int windowHeight;
		bool _promotePawn;
		bool _boardInit;
		PlayerType::Player _player;
		PlayerType::Player _opponent;
		bool _isTurn;
		Tile* selectedTile;
		std::list<Tile*> highlightTiles;
		std::string _movemsg;

	};
	const GUIData& GetData();
	GUIData& MutableData();
}