// **************************************************
// eval.cpp
// 評価関数
// **************************************************
#include "head.hpp"
#include "eval.hpp"

// --------------------------------------------------
// 関数のプロトタイプ宣言
// --------------------------------------------------
int diff_discs(board_t*);
// 盤面の重み付け法
int cell_weight(board_t*);
// 盤面の重みを取得
int get_weight(bitboard_t);

// ==================================================
// 評価関数
// ==================================================
int evaluate(board_t *board) {
  return diff_discs(board);
  // return cell_weight(board);
}

// ==================================================
// 評価値 = "自石の数 - 他石の数"
// ==================================================
int diff_discs(board_t *board) {
  int nblack = count_of_discs(board->black);
  int nwhite = count_of_discs(board->white);
  if (board->player == BLACK) {
    return nblack - nwhite;
  } else {
    return nwhite - nblack;
  }
}

// ==================================================
// 盤面の重み付け法
// ==================================================
int cell_weight(board_t *board) {
  // 黒白それぞれの評価値
  int black = 0, white = 0;
  // 左上から調べていく
  bitboard_t mv, pos = 0x8000000000000000;
  for (; pos != 0; pos = pos >> 1) {
    // 黒マスの重みを加算
    mv = (board->black & pos);
    if (mv != 0) {
      black += get_weight(mv);
      continue;
    }
    // 白マスの重みを加算
    mv = (board->white & pos);
    if (mv != 0) {
      white += get_weight(mv);
      continue;
    }
  }
  // 現在の手番から見た評価値を返す
  if (board->player == BLACK) {
    return black - white;
  } else {
    return white - black;
  }
}

// ==================================================
// 盤面の重みを取得
// |  30 | -12 |  0 | -1 | -1 |  0 | -12 |  30 |
// | -12 | -15 | -3 | -3 | -3 | -3 | -15 | -12 |
// |   0 |  -3 |  0 | -1 | -1 |  0 |  -3 |   0 |
// |  -1 |  -3 | -1 | -1 | -1 | -1 |  -3 |  -1 |
// |  -1 |  -3 | -1 | -1 | -1 | -1 |  -3 |  -1 |
// |   0 |  -3 |  0 | -1 | -1 |  0 |  -3 |   0 |
// | -12 | -15 | -3 | -3 | -3 | -3 | -15 | -12 |
// |  30 | -12 |  0 | -1 | -1 |  0 | -12 |  30 |
// ==================================================
int get_weight(bitboard_t mv) {
  switch (mv) {
    case 0x1000000000000000:
    case 0x0800000000000000:
    case 0x0000100000000000:
    case 0x0000080000000000:
    case 0x0000008000000000:
    case 0x0000002000000000:
    case 0x0000001000000000:
    case 0x0000000800000000:
    case 0x0000000400000000:
    case 0x0000000100000000:
    case 0x0000000080000000:
    case 0x0000000020000000:
    case 0x0000000010000000:
    case 0x0000000008000000:
    case 0x0000000004000000:
    case 0x0000000001000000:
    case 0x0000000000100000:
    case 0x0000000000080000:
    case 0x0000000000000010:
    case 0x0000000000000008:
      return -1;
    case 0x0020000000000000:
    case 0x0010000000000000:
    case 0x0008000000000000:
    case 0x0004000000000000:
    case 0x0000400000000000:
    case 0x0000020000000000:
    case 0x0000004000000000:
    case 0x0000000200000000:
    case 0x0000000040000000:
    case 0x0000000002000000:
    case 0x0000000000400000:
    case 0x0000000000020000:
    case 0x0000000000002000:
    case 0x0000000000001000:
    case 0x0000000000000800:
    case 0x0000000000000400:
      return -3;
    case 0x2000000000000000:
    case 0x0400000000000000:
    case 0x0000800000000000:
    case 0x0000200000000000:
    case 0x0000040000000000:
    case 0x0000010000000000:
    case 0x0000000000800000:
    case 0x0000000000200000:
    case 0x0000000000040000:
    case 0x0000000000010000:
    case 0x0000000000000020:
    case 0x0000000000000004:
      return 0;
    case 0x4000000000000000:
    case 0x0200000000000000:
    case 0x0080000000000000:
    case 0x0001000000000000:
    case 0x0000000000008000:
    case 0x0000000000000100:
    case 0x0000000000000040:
    case 0x0000000000000002:
      return -12;
    case 0x0040000000000000:
    case 0x0002000000000000:
    case 0x0000000000004000:
    case 0x0000000000000200:
      return -15;
    default:
      return 30;
  }
}
