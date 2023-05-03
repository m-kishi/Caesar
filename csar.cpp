// **************************************************
// csar.cpp
// AIプログラム
// 単純に石の数で評価する
// **************************************************
#include <limits.h>
#include <assert.h>
#include "head.hpp"

// --------------------------------------------------
// 探索の深さ
// --------------------------------------------------
#define DEPTH 10

// --------------------------------------------------
// 関数のプロトタイプ宣言
// --------------------------------------------------
// アルファ・ベータ法による探索
int alpha_beta_pruning(board_t*, player_t, int, int, int, bitboard_t[]);
// 評価関数
int evaluate(board_t*);
// 盤面の重み付け方
int cell_weight(bitboard_t);
// 盤面の状態をfromからdestへコピーする
void board_copy(board_t*, board_t*);

// ==================================================
// AIの手を取得する
// ==================================================
bitboard_t get_csar_move(board_t *board) {
  bitboard_t moves[DEPTH] = {0};
  alpha_beta_pruning(board, board->player, DEPTH, INT_MIN, INT_MAX, moves);
  // 念のために合法手かどうかをアサーション
  assert((moves[0] & board->legal_moves) != 0);
  return moves[0];
}

// ==================================================
// アルファ・ベータ法による探索
// ==================================================
int alpha_beta_pruning(board_t *board, player_t player, int depth, int alpha, int beta, bitboard_t moves[]) {

  // 想定の深さまで到達するか終局したら探索終了
  if (depth == 0 || board->status == OVER) {
    // 現在の局面の評価値を返却する
    return evaluate(board);
  }

  // 盤面のバックアップ
  board_t backup;
  board_copy(&backup, board);

  // 自分の手番の場合
  if (board->player == player) {
    // すべての合法手について繰り返し
    bitboard_t pos = 0x8000000000000000;
    bitboard_t legal_moves = board->legal_moves;
    for (; pos != 0; pos = pos >> 1) {
      // 着手箇所
      bitboard_t mv = (legal_moves & pos);
      if (mv == 0) continue;
      // 着手して盤面を進める
      next_turn(board, mv);
      check_board_status(board);
      // 評価値を計算
      int score = alpha_beta_pruning(board, player, depth-1, alpha, beta, moves);
      // 盤面を元に戻す
      board_copy(board, &backup);
      // 一番いい評価値を探す
      if (alpha < score) {
        // 候補手を上書き
        moves[DEPTH - depth] = mv;
        // 評価値を更新
        alpha = score;
        // ベータカット
        if (alpha >= beta) return beta;
      }
    }
    return alpha;

  // 相手の手番の場合
  } else {
    bitboard_t pos = 0x8000000000000000;
    bitboard_t legal_moves = board->legal_moves;
    for (; pos != 0; pos = pos >> 1) {
      bitboard_t mv = (legal_moves & pos);
      if (mv == 0) continue;
      next_turn(board, mv);
      check_board_status(board);
      int score = alpha_beta_pruning(board, player, depth-1, alpha, beta, moves);
      board_copy(board, &backup);
      // 一番悪い評価値を探す
      // (相手にとって良い手=自分にとって悪い手)
      if (score < beta) {
        // 評価値を更新
        beta = score;
        // アルファカット
        if (alpha >= beta) return alpha;
      }
    }
    return beta;
  }
}

// ==================================================
// 評価関数
// 単純に"自石の数-他石の数"を評価値とする
// ==================================================
//int evaluate(board_t *board) {
//  int nblack = count_of_discs(board->black);
//  int nwhite = count_of_discs(board->white);
//  if (board->player == BLACK) {
//    return nblack - nwhite;
//  } else {
//    return nwhite - nblack;
//  }
//}

// ==================================================
// 評価関数
// ==================================================
int evaluate(board_t *board) {
  // 黒白それぞれの評価値
  int black = 0, white = 0;
  // 左上から調べていく
  bitboard_t mv;
  bitboard_t pos = 0x8000000000000000;
  for (; pos != 0; pos = pos >> 1) {
    // 黒マスの重みを加算
    mv = (board->black & pos);
    if (mv != 0) {
      black += cell_weight(mv);
      continue;
    }
    // 白マスの重みを加算
    mv = (board->white & pos);
    if (mv != 0) {
      white += cell_weight(mv);
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
// 盤面の重み付け方
// |  30 | -12 |  0 | -1 | -1 |  0 | -12 |  30 |
// | -12 | -15 | -3 | -3 | -3 | -3 | -15 | -12 |
// |   0 |  -3 |  0 | -1 | -1 |  0 |  -3 |   0 |
// |  -1 |  -3 | -1 | -1 | -1 | -1 |  -3 |  -1 |
// |  -1 |  -3 | -1 | -1 | -1 | -1 |  -3 |  -1 |
// |   0 |  -3 |  0 | -1 | -1 |  0 |  -3 |   0 |
// | -12 | -15 | -3 | -3 | -3 | -3 | -15 | -12 |
// |  30 | -12 |  0 | -1 | -1 |  0 | -12 |  30 |
// ==================================================
int cell_weight(bitboard_t mv) {
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

// ==================================================
// 盤面の状態をfromからdestへコピーする
// ==================================================
void board_copy(board_t *dest, board_t *from) {
  dest->player = from->player;
  dest->status = from->status;
  dest->nblack = from->nblack;
  dest->nwhite = from->nwhite;
  dest->black  = from->black;
  dest->white  = from->white;
  dest->legal_moves = from->legal_moves;
}
