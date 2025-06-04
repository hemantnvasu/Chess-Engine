#include <bits/stdc++.h>
using namespace std;

class Board {
public:
    // 8x8 chessboard
    char board[8][8];
    int turn;  // 1 for white's turn, -1 for black's turn
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

        turn = 1;  // White's turn starts first
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

    // Function to display the board
    void displayBoard() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Function to Handle piece capturing and Castling
    void makeMove(int startX, int startY, int endX, int endY) {
        cout <<"make move start/end: "<< startX << ' ' << startY << ' ' << endX << ' ' << endY << endl;
        char piece = board[startX][startY];

        // Handling Castling Moves First
        if (piece == 'K' || piece == 'k') {
            if (canCastle(startX, startY, endX, endY, piece)) {
                // Handle kingside castling
                if (endY == startY + 2) {  // Kingside castling
                    // Move the king
                    board[endX][endY] = board[startX][startY];
                    board[startX][startY] = ' ';
                    // Move the rook
                    board[endX][endY - 1] = board[startX][startY + 3];
                    board[startX][startY + 3] = ' ';
                    // Update castling variables
                    if (piece == 'K') {  // White king
                        whiteKingHasMoved = true;
                        whiteKingsideRookHasMoved = true;
                    }
                    else {  // Black king
                        blackKingHasMoved = true;
                        blackKingsideRookHasMoved = true;
                    }
                }
                // Handle queenside castling
                else if (endY == startY - 2) {  // Queenside castling
                    // Move the king
                    board[endX][endY] = board[startX][startY];
                    board[startX][startY] = ' ';
                    // Move the rook
                    board[endX][endY + 1] = board[startX][startY - 4];
                    board[startX][startY - 4] = ' ';
                    // Update castling variables
                    if (piece == 'K') {  // White king
                        whiteKingHasMoved = true;
                        whiteQueensideRookHasMoved = true;
                    }
                    else {  // Black king
                        blackKingHasMoved = true;
                        blackQueensideRookHasMoved = true;
                    }
                }
                switchTurn();
                return;
            }
        }


        if (isLegalMove(startX, startY, endX, endY)) {
            // If the destination is occupied by an opponent's piece, capture it
            bool isCapture = board[endX][endY] != ' ';

            // Move the piece to the new square
            board[endX][endY] = board[startX][startY];
            board[startX][startY] = ' ';

            // Update move counter: reset after a pawn move or capture
            if (piece == 'P' || piece == 'p' || isCapture) {
                moveCount = 0;
            }
            else {
                moveCount++;
            }

            // Add the current board position to history (for threefold repetition)
            string boardState = getBoardState();
            positionHistory[boardState]++;


            // After a valid move, check for game over conditions
            if (checkmate()) {
                gameOver = true;
                gameResult = (turn == 1) ? "Black wins by checkmate" : "White wins by checkmate";
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
            cout << "Invalid move. Try again!" << endl;
        }

        // Update castling conditions if a king or rook has moved
        if (piece == 'K') {
            whiteKingHasMoved = true;
        }
        else if (piece == 'k') {
            blackKingHasMoved = true;
        }
        else if (piece == 'R') {
            if (startY == 0) {  // White queenside rook
                whiteQueensideRookHasMoved = true;
            }
            else if (startY == 7) {  // White kingside rook
                whiteKingsideRookHasMoved = true;
            }
        }
        else if (piece == 'r') {
            if (startY == 0) {  // Black queenside rook
                blackQueensideRookHasMoved = true;
            }
            else if (startY == 7) {  // Black kingside rook
                blackKingsideRookHasMoved = true;
            }
        }
    }

    // Switch turns
    void switchTurn() {
        turn = -turn;  // Switch between 1 and -1
    }

    // Function to check if a move is legal for a given piece
    bool isLegalMove(int startX, int startY, int endX, int endY) {
        char piece = board[startX][startY];

        // Invalid starting or ending position
        if (piece == ' ' || startX < 0 || startX >= 8 || startY < 0 || startY >= 8 || endX < 0 || endX >= 8 || endY < 0 || endY >= 8) {
            return false;
        }

        // Check if the piece belongs to the current player
        if ((turn == -1 && piece >= 'a' && piece <= 'z') || (turn == 1 && piece >= 'A' && piece <= 'Z')) {
            return false;  // It is the wrong player's turn
        }

        // Move logic for each piece
        switch (piece) {
            case 'P':  // Black Pawn
                return isPawnMove(startX, startY, endX, endY, 'P');
            case 'p':  // White Pawn
                return isPawnMove(startX, startY, endX, endY, 'p');
            case 'R':  // Black rook
            case 'r':  // White rook
                return isRookMove(startX, startY, endX, endY);
            case 'N':  // Black knight
            case 'n':  // White knight
                return isKnightMove(startX, startY, endX, endY);
            case 'B':  // Black bishop
            case 'b':  // White bishop
                return isBishopMove(startX, startY, endX, endY);
            case 'Q':  // Black queen
            case 'q':  // White queen
                return isQueenMove(startX, startY, endX, endY);
            case 'K':  // Black king
            case 'k':  // White king
                // Handle castling here
                if (canCastle(startX, startY, endX, endY, piece)) {
                    return true;  // Castling is valid
                }
                return isKingMove(startX, startY, endX, endY);
            default:
                return false;  // Invalid piece
        }
    }

    // Generate a string representation of the board to track its state
    string getBoardState() {
        string state = "";
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                state += board[i][j];
            }
        }
        return state;
    }

    // Check for insufficient material
    bool insufficientMaterial() {
        int whitePieces = 0, blackPieces = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j] >= 'A' && board[i][j] <= 'Z') {
                    whitePieces++;
                } else if (board[i][j] >= 'a' && board[i][j] <= 'z') {
                    blackPieces++;
                }
            }
        }
        // Check if either side has insufficient material
        // Common cases: king vs king, king + bishop vs king, king + knight vs king
        if ((whitePieces == 1 && blackPieces <= 1) || (whitePieces <= 1 && blackPieces == 1)) {
            return true;
        }
        if ((whitePieces == 2 && blackPieces == 1) || (whitePieces == 1 && blackPieces == 2)) {
            return true; // Example: White has only a king and a bishop, Black has only a king
        }
        if ((whitePieces == 1 && blackPieces == 2) || (whitePieces == 2 && blackPieces == 1)) {
            return true; // Example: White has a king and a knight, Black has a king
        }

        return false;
    }

    // Check for threefold repetition
    bool threefoldRepetition() {
        // A position must occur three times with the same player to move
        string boardState = getBoardState();
        if (positionHistory[boardState] >= 3) {
            return true;
        }
        return false;
    }

    // Check for the fifty-move rule
    bool fiftyMoveRule() {
        if (moveCount >= 50) {
            return true;  // No capture or pawn move for 50 moves
        }
        return false;
    }

    bool isKingInCheck(int player) {
        // Find the player's king position
        int kingX = -1, kingY = -1;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if ((player == 1 && board[i][j] == 'K') || (player == -1 && board[i][j] == 'k')) {
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
                if ((player == 1 && board[i][j] >= 'a' && board[i][j] <= 'z') || (player == -1 && board[i][j] >= 'A' && board[i][j] <= 'Z')) {
                    // If the opposing piece can legally move to the king's position, it's a check
                    if (isLegalMove(i, j, kingX, kingY)) {
                        return true;  // The king is in check
                    }
                }
            }
        }
        return false;  // No opposing piece can attack the king
    }

    bool canCastle(int startX, int startY, int endX, int endY, char piece) {
        // Castling is only possible for the King (both white and black)
        if (piece != 'K' && piece != 'k') {
            return false;
        }

        bool kingside = (endY == startY + 2);  // Kingside castling
        bool queenside = (endY == startY - 2); // Queenside castling

        // Check if the King has moved
        if ((piece == 'K' && whiteKingHasMoved) || (piece == 'k' && blackKingHasMoved)) {
            return false;  // King has moved, no castling allowed
        }

        // Check if the Rook has moved for castling
        if ((piece == 'K' && ((kingside && whiteKingsideRookHasMoved) || (queenside && whiteQueensideRookHasMoved))) ||
                (piece == 'k' && ((kingside && blackKingsideRookHasMoved) || (queenside && blackQueensideRookHasMoved)))) {
            return false;  // Rook has moved, no castling allowed
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
        if (isKingInCheck(piece == 'K' ? 1 : -1)) {
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

    bool isSquareUnderAttack(int x, int y, char piece) {
        // Determine the color of the current player
        bool isWhitePlayer = (piece == 'K');
        
        // Check for all opponent pieces (by iterating over the board)
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                char opponentPiece = board[i][j];
                
                // Check if the opponent's piece is of the opposite color
                bool isOpponent = (isWhitePlayer && opponentPiece >= 'a' && opponentPiece <= 'z') ||
                                (!isWhitePlayer && opponentPiece >= 'A' && opponentPiece <= 'Z');
                
                if (isOpponent) {
                    // Check if the opponent's piece can attack the target square
                    if (isLegalMove(i, j, x, y)) {
                        return true;  // The square is under attack by this opponent piece
                    }
                }
            }
        }
        return false;  // The square is not under attack
    }

    // Checking for checkmate
    bool checkmate() {
        // If the current player's king is in check and the player cannot move out of check, it's checkmate
        if (isKingInCheck(turn)) {
            // First, check if the king can move to a safe square
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if ((turn == 1 && board[i][j] >= 'A' && board[i][j] <= 'Z') || 
                        (turn == -1 && board[i][j] >= 'a' && board[i][j] <= 'z')) {
                        for (int x = 0; x < 8; x++) {
                            for (int y = 0; y < 8; y++) {
                                // If this move is legal, simulate it and check if it removes the check
                                if (isLegalMove(i, j, x, y)) {
                                    char originalPiece = board[x][y];
                                    board[x][y] = board[i][j];
                                    board[i][j] = ' ';
                                    
                                    if (!isKingInCheck(turn)) {  // If the king is no longer in check
                                        board[i][j] = board[x][y];  // Restore the board state
                                        board[x][y] = originalPiece;
                                        return false;  // A legal move exists, it's not checkmate
                                    }

                                    // Restore the board state if the move did not get the king out of check
                                    board[i][j] = board[x][y];
                                    board[x][y] = originalPiece;
                                }
                            }
                        }
                    }
                }
            }

            // Now check if any piece can block or capture the attacking piece
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if ((turn == 1 && board[i][j] >= 'A' && board[i][j] <= 'Z') || 
                        (turn == -1 && board[i][j] >= 'a' && board[i][j] <= 'z')) {
                        for (int x = 0; x < 8; x++) {
                            for (int y = 0; y < 8; y++) {
                                if (isLegalMove(i, j, x, y)) {
                                    // Try blocking the check (assuming isLegalMove takes care of this)
                                    char originalPiece = board[x][y];
                                    board[x][y] = board[i][j];
                                    board[i][j] = ' ';
                                    
                                    if (!isKingInCheck(turn)) {
                                        board[i][j] = board[x][y];  // Restore the board state
                                        board[x][y] = originalPiece;
                                        return false;  // A legal move exists to block the check
                                    }

                                    // Restore the board state if the move did not block the check
                                    board[i][j] = board[x][y];
                                    board[x][y] = originalPiece;
                                }
                            }
                        }
                    }
                }
            }
            // If no legal moves were found, it's checkmate
            return true;
        }
        return false;  // If the king is not in check, it's not checkmate
    }

    // Check for stalemate condition
    bool stalemate() {
        if (isKingInCheck(turn)) {
            return false;  // If the king is in check, it's not stalemate
        }

        // Try to find any legal move for the current player
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // Check if the current piece belongs to the player whose turn it is
                if ((turn == 1 && board[i][j] >= 'A' && board[i][j] <= 'Z') || 
                    (turn == -1 && board[i][j] >= 'a' && board[i][j] <= 'z')) {
                    
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
                ((pawn == 'P' && board[endX][endY] >= 'a' && board[endX][endY] <= 'z') ||
                (pawn == 'p' && board[endX][endY] >= 'A' && board[endX][endY] <= 'Z'))) {
            return true;  // Diagonal capture
        }

        // En Passant (check if the last move was a double pawn move)
        // if ((pawn == 'P' && endX == startX - 1 && (endY == startY - 1 || endY == startY + 1) && board[endX][endY] == ' ' && board[startX][endY] == 'p') ||
        //     (pawn == 'p' && endX == startX + 1 && (endY == startY - 1 || endY == startY + 1) && board[endX][endY] == ' ' && board[startX][endY] == 'P')) {
        //     if (isLastMoveDoublePawnAdvance()) {
        //         return true;  // En Passant
        //     }
        // }

        // Pawn promotion
        if ((pawn == 'P' && endX == 7) || (pawn == 'p' && endX == 0)) {
            promotePawn(endX, endY, pawn);
            return true;
        }

        return false;
    }

    // Helper function to promote pawn
    void promotePawn(int x, int y, char pawn) {
        char promotionPiece;

        // Ask the player for the promotion piece
        cout << "Pawn promotion! Choose your piece (Q for Queen, R for Rook, B for Bishop, N for Knight): ";
        cin >> promotionPiece;

        // Validate the input and ensure it is a valid piece
        while (promotionPiece != 'Q' && promotionPiece != 'R' && promotionPiece != 'B' && promotionPiece != 'N') {
            cout << "Invalid choice. Choose your piece (Q for Queen, R for Rook, B for Bishop, N for Knight): ";
            cin >> promotionPiece;
        }

        // Assign the chosen piece
        board[x][y] = (pawn == 'P') ? promotionPiece : tolower(promotionPiece);  // Handle uppercase for white, lowercase for black
    }

    // Rook move logic (straight lines)
    bool isRookMove(int startX, int startY, int endX, int endY) {
        // Rook moves in a straight line (either horizontally or vertically)
        
        if (startX == endX) {  // Horizontal move (same row)
            int minY = std::min(startY, endY);
            int maxY = std::max(startY, endY);
            for (int y = minY + 1; y < maxY; y++) {
                if (board[startX][y] != ' ') {
                    return false;  // Path is blocked by another piece
                }
            }
        }
        else if (startY == endY) {  // Vertical move (same column)
            int minX = std::min(startX, endX);
            int maxX = std::max(startX, endX);
            for (int x = minX + 1; x < maxX; x++) {
                if (board[x][startY] != ' ') {
                    return false;  // Path is blocked by another piece
                }
            }
        }
        else {
            return false;  // Invalid rook move, must move in a straight line
        }

        // The destination square must be empty or contain an opponent's piece
        char destinationPiece = board[endX][endY];

        // Check if the destination is empty or contains an opponent's piece
        if (destinationPiece == ' ' || (isOpponentPiece(destinationPiece))) {
            return true;  // Valid move
        }

        return false;
    }

    // Helper function to check if the piece is an opponent's piece
    bool isOpponentPiece(char piece) {
        if ((turn == 1 && piece >= 'a' && piece <= 'z') ||  // White's opponent (black pieces)
            (turn == -1 && piece >= 'A' && piece <= 'Z')) {  // Black's opponent (white pieces)
            return true;
        }
        return false;
    }


    // Knight move logic (L-shape)
    bool isKnightMove(int startX, int startY, int endX, int endY) {
        int dx = abs(endX - startX);
        int dy = abs(endY - startY);

        
        if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
            // Knights can jump over other pieces, so no path obstruction check needed
            if (board[endX][endY] == ' ' || isOpponentPiece(board[endX][endY])) {
                return true;  // Valid move: empty or opponent's piece at destination
            }
        }
        
        return false;  // Invalid knight move
    }

    // Bishop move logic (diagonal)
    bool isBishopMove(int startX, int startY, int endX, int endY) {
        int dx = abs(endX - startX);
        int dy = abs(endY - startY);

        if (dx == dy) {  // Bishops move diagonally
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
            if (board[endX][endY] == ' ' || isOpponentPiece(board[endX][endY])) {
                return true;
            }
        }

        return false;  // Invalid bishop move
    }

    // Queen move logic (combines rook and bishop moves)
    bool isQueenMove(int startX, int startY, int endX, int endY) {
        // A queen moves like both a rook and a bishop:
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

        // Check if the destination is within the bounds of the board
        if (endX < 0 || endX >= 8 || endY < 0 || endY >= 8) {
            return false;  // Invalid move: out of bounds
        }

        // King moves one square in any direction (horizontally, vertically, or diagonally)
        if (dx <= 1 && dy <= 1) {
            // The destination square must be empty or contain an opponent's piece
            if (board[endX][endY] == ' ' || 
                (board[endX][endY] >= 'a' && board[endX][endY] <= 'z') || 
                (board[endX][endY] >= 'A' && board[endX][endY] <= 'Z')) {
                return true;
            }
        }
        // Castling logic is handled separately
        return false;
    }
};

int main() {
    // Create a chessboard object and display it
    Board b;
    b.displayBoard();
    b.makeMove(6, 3, 4, 3); // white pawn move
    b.displayBoard();

    // b.makeMove(1, 4, 3, 4); // black pawn move
    // cout << endl;
    // b.displayBoard();

    // You can add more moves to test the conditions.

    return 0;
}