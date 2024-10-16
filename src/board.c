#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "board.h"
#include "position.h"

// Get a piece at a position.
// Asserts if position is invalid.
char get_piece(board_t* board, int pos) {
#ifndef NDEBUG
  assert(board->initialized);
#endif
  
  assert(is_valid_pos(pos));
  return board->board_array[pos];
}

// Set a piece at a position.
// Asserts if position is invalid.
void set_piece(board_t* board, int pos, char piece) {
  assert(is_valid_pos(pos));
  board->board_array[pos] = piece;
}

// Load board from FEN string.
bool load_fen(const char* fen, board_t* board) {
  int row = 0;
  int col = 0;

  for (; *fen != '\0'; fen++) {
    switch (*fen) {
      // Skip to next row.
    case '/':
      if (col != 8 || row >= 8)
        return false;

      row++;
      col = 0;
      break;

      // Add spaces.
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':;
      int spaces = *fen - '0';
      if (col + spaces > 8)
        return false;

      while (spaces--)
        set_piece(board, to_position(row, col++), ' ');

      break;

      // Add the corresponding piece.
    case 'p':
    case 'n':
    case 'P':
    case 'N':
      if (row > 7 || col > 7)
        return false;

      set_piece(board, to_position(row, col++), *fen);
      break;
    default:
      return false;
    }
  }
  if (row != 7 || col != 8) {
    return false;
  }

  board->initialized = true;
  return true;
}

void print_board(board_t* board, bool reverse) {
  printf(reverse ? " hgfedcba\n" : " abcdefgh\n");
  for (int row=0; row<8; row++) {
    int board_row = perspective_row(row, reverse);
    printf("%c", '0' + board_row + 1);

    for (int col=0; col<8; col++) {
      int board_col = perspective_col(col, reverse);

      char piece = get_piece(board, to_position(board_row, board_col));

      if (piece == ' ')
        printf("%c", (board_col + board_row) % 2 ? ' ' : '_');
      else
        printf("%c", piece);
    }
    printf("\n");
  }
}

// Change the turn of the board to the other player, and return the new turn.
bool next_turn(board_t *board) { return board->turn = !board->turn; }

void copy_board(board_t* src, board_t* dest) {
#ifndef NDEBUG
  dest->initialized = src->initialized;
#endif
  
  // Copy the turn value.
  dest->turn = src->turn;

  // Copy the board contents.
  for (int row=0; row<8; row++) {
    for (int col=0; col<8; col++) {
      int position = to_position(row, col);
      set_piece(dest, position, get_piece(src, position));
    }
  }
}

// Check if the boards are same.
bool compare(board_t* board1, board_t* board2) {
  // Uninitialized boards cannot be compared.
#ifndef NDEBUG
  assert(board1->initialized);
  assert(board2->initialized);
#endif
  
  if (board1->turn != board2->turn) return false;

  for (int row=0; row<8; row++) {
    for (int col=0; col<8; col++) {
      int position = to_position(row, col);
      
      if (get_piece(board1, position) != get_piece(board2, position)) return false;
    }
  }

  return true;
}
