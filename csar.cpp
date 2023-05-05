// **************************************************
// csar.cpp
// AIプログラム
// 単純に石の数で評価する
// **************************************************
#include <limits.h>
#include <assert.h>
#include "head.hpp"
#include "eval.hpp"

// --------------------------------------------------
// 探索の深さ
// --------------------------------------------------
#define DEPTH 10

// --------------------------------------------------
// 関数のプロトタイプ宣言
// --------------------------------------------------
// ネガマックス法による探索
int nega_max_search(board_t*, int, int, int, bool);
// 盤面の状態をfromからdestへコピーする
void board_copy(board_t*, board_t*);

// ==================================================
// AIの手を取得する
// ==================================================
bitboard_t get_csar_move(board_t *board) {
  // すべての合法手について繰り返し
  bitboard_t mv, move = 0;
  bitboard_t pos = 0x8000000000000000;
  bitboard_t legal_moves = board->legal_moves;
  int score, alpha = INT_MIN, beta = INT_MAX;
  for (; pos != 0; pos = pos >> 1) {
    mv = (legal_moves & pos);
    if (mv == 0) continue;
    // 指し手の評価値を取得
    score = -nega_max_search(board, DEPTH, alpha, beta, false);
    // 評価値と差し手の更新
    if (alpha < score) {
      alpha = score;
      move = mv;
    }
  }
  return move;
}

// ==================================================
// ネガマックス法による探索
// ==================================================
int nega_max_search(board_t *board, int depth, int alpha, int beta, bool pass) {
  // 想定の深さまで到達したら探索終了
  if (depth == 0) {
    return evaluate(board);
  }
  // パスの処理
  bitboard_t legal_moves = board->legal_moves;
  if (legal_moves == 0) {
    // 前回もパスなら終局
    if (pass) {
      return evaluate(board);
    }
    // 手番を交代して同じ深さで探索
    next_turn(board, 0);
    return -nega_max_search(board, depth, -beta, -alpha, true);
  }
  // 盤面のバックアップ
  board_t backup;
  board_copy(&backup, board);
  // すべての合法手について繰り返し
  int score;
  bitboard_t mv, pos = 0x8000000000000000;
  for (; pos != 0; pos = pos >> 1) {
    // 着手箇所
    mv = (legal_moves & pos);
    if (mv == 0) continue;
    // 着手して盤面を進める
    next_turn(board, mv);
    // 次の深さを探索
    score = -nega_max_search(board, depth-1, -beta, -alpha, false);
    // 盤面を元に戻す
    board_copy(board, &backup);
    // 評価値を更新
    if (alpha < score) {
      alpha = score;
    }
    // 枝刈り
    if (beta <= alpha) {
      return alpha;
    }
  }
  return alpha;
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
