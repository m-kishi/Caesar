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
// 評価値を計算する
int evaluation_value(board_t*);
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
    return evaluation_value(board);
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
// 評価値を計算する
// 単純に"自石の数-他石の数"を評価値とする
// ==================================================
int evaluation_value(board_t *board) {
  int nblack = count_of_discs(board->black);
  int nwhite = count_of_discs(board->white);
  if (board->player == BLACK) {
    return nblack - nwhite;
  } else {
    return nwhite - nblack;
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
