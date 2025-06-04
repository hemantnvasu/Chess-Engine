#include <bits/stdc++.h>
using namespace std;

class Board {
public:
	// 8x8 chessboard
	char board[8][8];

	int turn; // 1 for white's turn and -1 for black's turn
	bool gameOver; // Flag to check if the game is over
	string gameResult; // Holds the result of the game (checkmate, stalemate, etc.)
	int moveCount; // Count the number of half-moves since the last capture or pawn move
	unordered_map<string, int> positionHistory; // Store board positions for threefold repetition check

	// To handle castling condition
	bool blackKingHasMoved;
	bool whiteKingHasMoved;
	bool blackQueensideRookHasMoved;
	bool whiteQueensideRookHasMoved;
	bool blackKingsideRookHasMoved;
	bool whiteKingsideRookHasMoved;

	// Constructor to initialize the chessboard
	Board() {
		// Set up the initial board configuration
		char initialBoard[8][8] = {
			{'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},
			{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
			{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}
		};

		// Copy the initial board setup into the actual board array
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				board[i][j] = initialBoard[i][j];
			}
		}

		turn = 1; // White's turn starts first
		gameOver = false;
		gameResult = "ongoing";
		moveCount = 0; // Start counting moves

		// Initialize castling flags
		blackKingHasMoved = false;
		whiteKingHasMoved = false;
		blackQueensideRookHasMoved = false;
		whiteQueensideRookHasMoved = false;
		blackKingsideRookHasMoved = false;
		whiteKingsideRookHasMoved = false;
	}

	void displayBoard() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				cout << board[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

	// Function to handling capturing and moving
	void makeMove(int startX, int startY, int endX, int endY) {
		char piece = board[startX][startY];

		if (isLegalMove(startX, startY, endX, endY)) {

			bool isCapture = (board[startX][startY] != ' ');

			board[endX][endY] = board[startX][startY];
			board[startX][startY] = ' ';
			// Pawn Promotion Logic
			if ((board[endX][endY] == 'p' && endY == 0) || (board[endX][endY] == 'P' && endY == 7)) {
				promotePawn(endX, endY, board[endX][endY]);
			}

			// move counter increment to handle 50 moves stalemate condition
			// handle moveCount = 0 in Enpassant separately
			moveCount++;
			if (piece == 'p' || piece == 'p' || isCapture) {
				moveCount = 0;
			}

			// After a valid move, check for game over conditions
			if (checkmate()) {
				gameOver = true;
				gameResult = (turn == 1) ? "White wins by checkmate!" : "Black winds by checkmate!";
				cout << gameResult << endl;
			}
			else if (stalemate()) {
				gameOver = true;
				gameResult = "Stalemate, it's a draw";
				cout << gameResult << endl;
			}
			else if (insufficientMaterial()) {
				gameOver = true;
				gameResult = "Insufficient material, it's a draw";
				cout << gameResult << endl;
			}
			else if (threefoldRepetition()) {
				gameOver = true;
				gameResult = "Threefold repetition, it's a draw";
				cout << gameResult << endl;
			}
			else if (fiftyMoveRule()) {
				gameOver = true;
				gameResult = "Fifty-move rule, it's a draw";
				cout << gameResult << endl;
			}
			switchTurn();
		}
		else {
			cout << "Invalid Move!" << endl;
		}
	}

	void switchTurn() {
		turn = -turn;  // Switch between 1 and -1
	}

	// Function to check if a move is legal for a given piece
	bool isLegalMove(int startX, int startY, int endX, int endY) {

		// Invalid starting or ending position
		if (startX < 0 || startX > 7 || startY < 0 || startY > 7 || endX < 0 || endX > 7 || endY < 0 || endY > 7) {
			return false;
		}

		char piece = board[startX][startY];

		if (piece == ' ') {
			return false;
		}

		switch (piece) {
		// Uppercase : Black, Lowercase: White
		case 'P': // Black Pawn
			return isPawnMove(startX, startY, endX, endY, 'P');
		case 'p': // White Pawn
			return isPawnMove(startX, startY, endX, endY, 'P');
		case 'R': case 'r': // Rook
			return isRookMove(startX, startY, endX, endY);
		case 'N': case 'n': // Knight
			return isKnightMove(startX, startY, endX, endY);
		case 'B': case 'b': // Bishop
			return isBishopMove(startX, startY, endX, endY);
		case 'Q': case 'q': // Queen
			return isQueenMove(startX, startY, endX, endY);
		case 'K': case 'k': // King
			return isKingMove(startX, startY, endX, endY);
		default: // Invalid Piece
			return false;
		}
	}

	// Pawn move logic
	bool isPawnMove(int startX, int startY, int endX, int endY, char pawn) {

		int direction = (pawn == 'P') ? 1 : -1;  // White pawns move up, black pawns move down

		// Normal move (1 square forward)
		if (endY == startY && endX == startX + direction && board[endX][endY] == ' ') {
			return true;
		}

		// Double move (first move only, 2 squares forward)
		if (pawn == 'P' && startX == 1 && endY == startY && endX == startX + 2 && board[endX][endY] == ' ' && board[startX + 1][startY] == ' ') {
			return true;  // White pawn's first double move
		}
		if (pawn == 'p' && startX == 6 && endY == startY && endX == startX - 2 && board[endX][endY] == ' ' && board[startX - 1][startY] == ' ') {
			return true;  // Black pawn's first double move
		}

		// Capturing diagonally
		if (endX == startX + direction && (endY == startY - 1 || endY == startY + 1) &&
		        isOpponentPiece(board[startX][startY], board[endX][endY])) {
			return true;  // Diagonal capture
		}

		// En Passant logic
		if (abs(startX - endX == 1) && abs(startY - endY) == 1 && isEnPassant(startX, startY, endX, endY, pawn)) {
			performEnPassant(startX, startY, endX, endY, pawn);
			moveCount = 0;
		}

		return false;
	}

	// Helper function to choose pawn promotion
	void promotePawn(int x, int y, char pawn) {
		char choice;
		cout << "Pawn promotion! Choose your piece (Q for Queen, R for Rook, B for Bishop, N for Knight): ";
		cin >> choice;

		// Validate the input and ensure it is a valid piece
		while (choice != 'Q' && choice != 'R' && choice != 'B' && choice != 'N') {
			cout << "Invalid choice. Choose your piece (Q for Queen, R for Rook, B for Bishop, N for Knight): ";
			cin >> choice;
		}

		// Assign the promotion
		board[x][y] = (pawn == 'P') ? choice : tolower(choice);
	}

	// Rook move logic (straight lines)
	bool isRookMove(int startX, int startY, int endX, int endY) {
		// Rook moves in a straight line (either horizontally or vertically)

		// Cases where rook moves are invalid:
		if (startX == endX) {  // Horizontal move (same row)
			int minY = min(startY, endY);
			int maxY = max(startY, endY);
			for (int y = minY + 1; y < maxY; y++) {
				if (board[startX][y] != ' ') {
					return false;  // Path is blocked by another piece
				}
			}
		}
		else if (startY == endY) {  // Vertical move (same column)
			int minX = min(startX, endX);
			int maxX = max(startX, endX);
			for (int x = minX + 1; x < maxX; x++) {
				if (board[x][startY] != ' ') {
					return false;  // Path is blocked by another piece
				}
			}
		}
		// both X and Y are different
		else {
			return false;  // Invalid rook move, must move in a straight line
		}

		// The destination square must be empty or contain an opponent's piece
		if (board[endX][endY] == ' ' || isOpponentPiece(board[startX][startY], board[endX][endY])) {
			return true;
		}

		return false;
	}

	// Knight move logic (L-shape)
	bool isKnightMove(int startX, int startY, int endX, int endY) {
		// Knights move in an "L" shape: 2 squares in one direction, 1 square perpendicular
		// Knights can jump over other pieces, so no path obstruction check needed

		int dx = abs(endX - startX);
		int dy = abs(endY - startY);

		if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
			// Valid move: empty or opponent's piece at destination
			if (board[endX][endY] == ' ' || isOpponentPiece(board[startX][startY], board[endX][endY])) {
				return true;
			}
		}
	}

	// Bishop move logic (diagonal)
	bool isBishopMove(int startX, int startY, int endX, int endY) {
		int dx = abs(endX - startX);
		int dy = abs(endY - startY);

		if (dx == dy) {
			int xDir = (endX > startX) ? 1 : -1;
			int yDir = (endY > startY) ? 1 : -1;
			int x = startX + xDir;
			int y = startY + yDir;

			// Check if path is clear
			while (x != endX && y != endY) {
				if (board[x][y] != ' ') {
					return false;  // Path is blocked
				}
				x += xDir;
				y += yDir;
			}

			// The destination square must be empty or contain an opponent's piece
			if (board[endX][endY] == ' ' || isOpponentPiece(board[startX][startY], board[endX][endY])) {
				return true;
			}
		}
		return false; // Invalid Bishop move
	}

	// Queen move logic (combines rook and bishop moves)
	bool isQueenMove(int startX, int startY, int endX, int endY) {
		// A queen moves like both a rook and a bishop

		// Rook-like move: same row or column
		if (startX == endX || startY == endY) {
			return isRookMove(startX, startY, endX, endY);  // Rook move logic
		}

		// Bishop-like move: diagonal move
		else if (abs(endX - startX) == abs(endY - startY)) {
			return isBishopMove(startX, startY, endX, endY);  // Bishop move logic
		}

		return false;  // Invalid queen move if neither rook-like nor bishop-like
	}

	// King move logic (one square in any direction)
	bool isKingMove(int startX, int startY, int endX, int endY) {
		int dx = abs(endX - startX);
		int dy = abs(endY - startY);

		// King moves one square in any direction (horizontally, vertically, or diagonally)
		if (dx <= 1 && dy <= 1) {
			// The destination square must be empty or contain an opponent's piece
			if (board[endX][endY] == ' ' || isOpponentPiece(board[startX][startY], board[endX][endY])) {
				return true;
			}
		}

		// castling logic can be handled here or separately
	}

	// Helper function to check if the target square has an opponent's piece
	bool isOpponentPiece(char playerPiece, char opponentPiece) {
		if ((playerPiece >= 'a' && playerPiece <= 'z' && opponentPiece >= 'A' && opponentPiece <= 'Z') ||
		        (playerPiece >= 'A' && playerPiece <= 'Z' && opponentPiece >= 'a' && opponentPiece <= 'z')) {
			return true;
		}
		return false;
	}
	bool isOpponentPiece(int player, char opponentPiece) {
		if ((player == 1 && opponentPiece >= 'A' && opponentPiece <= 'Z') ||
		        (player == -1 && opponentPiece >= 'a' && opponentPiece <= 'z')) {
			return true;
		}
		return false;
	}

	// Function to check whether the king is in check or not
	bool isKingInCheck() {
		// if turn is 1 - white's chance(check if white's king is in check)
		// if turn is -1 - black's chance(check if black's king is in check)

		// Find the player's king position
		int kingX = -1, kingY = -1;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((turn == 1 && board[i][j] == 'k') || (turn == -1 && board[i][j] == 'K')) {
					kingX = i;
					kingY = j;
					break;
				}
			}
			if (kingX != -1) break;  // Exit early if the king is found
		}

		// Check if the king is under attack (by any opposing piece)
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				// Check for opponent's pieces
				if (isOpponentPiece(-1, board[i][j]) || isOpponentPiece(1, board[i][j])) {
					// If the opposing piece can legally move to the king's position, it's a check
					if (isLegalMove(i, j, kingX, kingY)) {
						return true;  // The king is in check
					}
				}
			}
		}
		return false;  // No opposing piece can attack the king
	}

	// Function to check whether the king can castle or not
	bool canCastle(int startX, int startY, int endX, int endY, char piece) {
		// Castling is only possible for the King (both white and black)
		if (piece != 'K' && piece != 'k') {
			return false;
		}

		// checking whether the kings want to castle king side or queen side
		bool kingside = (endY == startY + 2);  // Kingside castling (King moves 2 squares to the right)
		bool queenside = (endY == startY - 2); // Queenside castling (King moves 2 squares to the left)

		// Check if the King has already moved
		bool kingHasMoved = (piece == 'K' && blackKingHasMoved) || (piece == 'k' && whiteKingHasMoved);

		// Check if the Rook has already moved
		bool rookHasMoved = (piece == 'K' && ((kingside && blackKingsideRookHasMoved) || (queenside && blackQueensideRookHasMoved))) ||
		                    (piece == 'k' && ((kingside && whiteKingsideRookHasMoved) || (queenside && whiteQueensideRookHasMoved)));

		// If the King or the Rook has moved, castling is not allowed
		if (kingHasMoved || rookHasMoved) {
			return false;  // King or Rook has moved, no castling allowed
		}

		// Check if there are any pieces between the King and the Rook
		if (kingside) {
			if (board[startX][startY + 1] != ' ' || board[startX][startY + 2] != ' ') {
				return false;  // Path is blocked
			}
		}
		else if (queenside) {
			if (board[startX][startY - 1] != ' ' || board[startX][startY - 2] != ' ' || board[startX][startY - 3] != ' ') {
				return false;  // Path is blocked
			}
		}
		else {
			return false;  // Invalid castling direction
		}

		// Ensure the King is not in check
		if (isKingInCheck()) {
			return false;  // The king is in check, so castling is not allowed
		}

		// Ensure the squares the King will move through are not under attack
		if (kingside) {
			if (isSquareUnderAttack(startX, startY + 1, piece) || isSquareUnderAttack(startX, startY + 2, piece)) {
				return false;  // Path is under attack
			}
		}
		else if (queenside) {
			if (isSquareUnderAttack(startX, startY - 1, piece) || isSquareUnderAttack(startX, startY - 2, piece) || isSquareUnderAttack(startX, startY - 3, piece)) {
				return false;  // Path is under attack
			}
		}

		return true;  // Castling is possible
	}

	// En Passant Logic

	// Global variable to track the last move details (specifically the last pawn move)
	int lastMoveStartX = -1, lastMoveStartY = -1, lastMoveEndX = -1, lastMoveEndY = -1;
	char lastMovePiece = ' ';

	// Function to set the last move (after a move is made)
	void setLastMove(int startX, int startY, int endX, int endY, char piece) {
		lastMoveStartX = startX;
		lastMoveStartY = startY;
		lastMoveEndX = endX;
		lastMoveEndY = endY;
		lastMovePiece = piece;
	}

	// Function to check if the move is En Passant
	bool isEnPassant(int startX, int startY, int endX, int endY, char pawn) {
		// En Passant only works if the opponent's pawn just moved two squares forward

		// For white pawns(current player), check if the last move was black's two-square advance(opponent)
		if (pawn == 'p' && startX == 4 && abs(endY - startY) == 1 && abs(endX - startX) == 1) {
			// Check if the opponent's pawn just moved 2 squares forward (Black's pawn)
			if (lastMovePiece == 'P' && lastMoveStartX == 6 && abs(lastMoveEndY - startY) == 1 && endX == startX - 1) {
				return true; // Valid en passant move
			}
		}
		// For black pawns(current player), check if the last move was white's two-square advance(opponent)
		if (pawn == 'P' && startX == 3 && abs(endY - startY) == 1 && abs(endX - startX) == 1) {
			// Check if the opponent's pawn just moved 2 squares forward (White's pawn)
			if (lastMovePiece == 'p' && lastMoveStartX == 1 && abs(lastMoveEndY - startY) == 1 && endX == startX + 1) {
				return true; // Valid en passant move
			}
		}
		return false;
	}

	void performEnPassant(int startX, int startY, int endX, int endY, char pawn) {
		if (isEnPassant(startX, startY, endX, endY, pawn)) {
			// Remove the opponent's pawn
			board[lastMoveEndX][lastMoveEndY] = ' ';

			// Move the player's pawn to the en passant square
			board[endX][endY] = pawn;

			cout << "En Passant performed!" << endl;
		}
		else {
			cout << "Invalid En Passant move!" << endl;
		}
	}


	// Helper function to check whether the opponent is attacking the current square to prevent castling
	bool isSquareUnderAttack(int x, int y, int player) {

		int oppo = -turn; // opponent

		// Check for all opponents pieces(by iterating over the board)
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (isOpponentPiece(1, board[i][j]) || isOpponentPiece(-1, board[i][j])) {
					// Checking if the opponent piece can target the square(x, y)
					if (isLegalMove(i, j, x, y)) {
						return true; // The square is under attack by this opponent piece
					}
				}
			}
		}
		return false;  // The square is not under attack
	}

	// function to check if it's checkmate
	bool checkmate() {
		// Step 1: Check if the king is in check
		if (!isKingInCheck()) {
			return false; // Not checkmate if the king in not in check
		}

		// Step 2: Check if the player has any legal moves to escape the check / king can escape check or not

		// Find the player's king position
		int kingX = -1, kingY = -1;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((turn == 1 && board[i][j] == 'k') || (turn == -1 && board[i][j] == 'K')) {
					kingX = i;
					kingY = j;
					break;
				}
			}
			if (kingX != -1) break;  // Exit early if the king is found
		}

		// trying to check all the neighbouring squares where the king can reach
		int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1};
		int dy[8] = {1, 1, 1, 0, -1, -1, -1, 0};
		bool canEscape = false;
		for (int i = 0; i < 8; i++) {
			int nx = kingX + dx[i];
			int ny = kingY + dy[i];
			// check if the new square is valid and empty
			if (nx <= 0 && nx < 8 && ny >= 0 && ny < 8 && board[nx][ny] == ' ') {
				board[nx][ny] = (turn == 1) ? 'k' : 'K';
				board[kingX][kingY] = ' ';
				if (!isKingInCheck()) canEscape = true;
				board[nx][ny] = ' ';
				board[kingX][kingY] = (turn == 1) ? 'k' : 'K';
			}
			if (canEscape) break;
		}
		if (canEscape) return false; // return false if the king can escape

		// Step 3: Check if the player can block the check of capture the attacker
		// (if blocking or capturing the attacker is possible, it's not checkmate)
		int oppo = (turn == 1) ? -1 : 1;  // Opponent's pieces
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((oppo == 1 && board[i][j] >= 'a' && board[i][j] <= 'z') ||
				        (oppo == -1 && board[i][j] >= 'A' && board[i][j] <= 'Z')) {
					// If the opponent piece can legally move to the king's position
					if (isLegalMove(i, j, kingX, kingY)) {
						// If an opponent's piece is attacking the king, check if it can be captured
						// Capture the attacker logic (if piece is capturable)
						if (isOpponentPiece(board[i][j], board[kingX][kingY])) {
							return false; // The attacking piece can be captured, not checkmate
						}

						// Try to block the attack by placing a piece between the king and attacker
						if (canBlockAttack(i, j, kingX, kingY)) {
							return false; // The check can be blocked
						}
					}
				}
			}
		}

		// If no moves to escape check and can't block or capture, it's checkmate
		return true;
	}

	// helper function to check if attack on king can be blocked by placing a piece
	bool canBlockAttack(int i, int j, int kingX, int kingY) {

	}

	// Function to check if it's stalemate
	// After every move condition for stalemate will be checked before the player takes turn
	bool stalemate() {
		// If the king is in check it's not stalemate, it's checkmate
		if (isKingInCheck()) return false;

		// Try to find any legal move for the current player
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {

				// Check if the current piece belongs to the player whose turn it is
				if ((turn == 1 && board[i][j] >= 'a' && board[i][j] <= 'z') || (turn == -1 && board[i][j] >= 'A' && board[i][j] <= 'Z')) {
					// Check all possible destination squares for this piece
					for (int x = 0; x < 8; x++) {
						for (int y = 0; y < 8; y++) {
							if (isLegalMove(i, j, x, y)) {
								return false;  // A legal move exists, it's not stalemate
							}
						}
					}
				}
			}
		}
		// If no legal moves were found, it's stalemate
		return true;
	}

	bool insufficientMaterial() {
		// King vs King
		// King + Bishop vs King
		// King + knight vs king
		// King + Bishop vs King + Bishop (Both Bishop of same color)

		bool kingWhite = 0;
		bool kingBlack = 0;

		int numKnightsWhite = 0;
		int numKnightsBlack = 0;

		int whiteBishopDark = 0;
		int whiteBishopLight = 0;

		int blackBishopDark = 0;
		int blackBishopLight = 0;

		int total = 0;

		// Iterate over the board and count material for the current player
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				char piece = board[i][j];
				if (piece == ' ') continue;
				else if (piece >= 'A' && piece <= 'Z') {
					if (piece == 'K') kingBlack = true;
					else if (piece == 'N') numKnightsBlack++;
					else if (piece == 'B') {
						if ((i + j) % 2 == 0 ) blackBishopLight++;
						else blackBishopDark++;
					}
					total++;
				}
				else {
					if (piece == 'k') kingWhite = true;
					else if (piece == 'n') numKnightsWhite++;
					else if (piece == 'b') {
						if ((i + j) % 2 == 0 ) whiteBishopLight++;
						else whiteBishopDark++;
					}
					total++;
				}
			}
		}

		if (kingBlack && kingWhite) {
			// king vs king
			if (total - 2) {
				return true;
			}
			// king + bishop vs king
			else if ((whiteBishopLight + whiteBishopDark == 1 && total - 3 == 0) || (blackBishopDark + blackBishopLight == 1 && total - 3 == 0)) {
				return true;
			}
			// king + knight vs king
			else if ((numKnightsWhite == 1 && total - 3 == 0) || (numKnightsBlack == 1 && total - 3 == 0)) {
				return true;
			}
			// King + Bishop vs King + Bishop (Both Bishop of same color)
			else if (whiteBishopDark == 1 && blackBishopDark == 1 && whiteBishopLight == 0 && blackBishopLight == 0 && total - 4 == 0) {
				return true;
			}
			else if (whiteBishopDark == 0 && blackBishopDark == 0 && whiteBishopLight == 1 && blackBishopLight == 1 && total - 4 == 0) {
				return true;
			}
		}
	}

	bool threefoldRepetition() {

	}

	bool fiftyMoveRule() {
		if (moveCount >= 50) {
			return true;  // No capture or pawn move for 50 moves
		}
		return false;
	}
};

int main() {
	Board b;
	b.displayBoard();
	// b.makeMove(6, 4, 4, 4); // white pawn
	// b.displayBoard();
	// b.makeMove(1, 3, 3, 3); // black pawn
	// b.displayBoard();
	// b.makeMove(4, 4, 3, 3); // white pawn captures black pawn
	// b.displayBoard();
}