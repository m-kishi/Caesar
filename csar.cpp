// **************************************************
// csar.cpp
// AIプログラム
// 単純に石の数で評価する
// **************************************************
#include <limits.h>
#include <random>
#include <unordered_map>
#include "head.hpp"
#include "eval.hpp"

// --------------------------------------------------
// 探索の深さ
// --------------------------------------------------
#define DEPTH 10

// --------------------------------------------------
// 置換表(transpose table)
// --------------------------------------------------
std::unordered_map<bitboard_t, int> tt;

// --------------------------------------------------
// ハッシュ値計算に使う乱数
// --------------------------------------------------
bool initialized = false;
uint64_t rand_mask[2][8][255];

// --------------------------------------------------
// 乱数(メルセンヌ・ツイスター64ビット版)
// --------------------------------------------------
std::random_device seed_gen;
std::mt19937_64 engine(seed_gen());

// --------------------------------------------------
// 関数のプロトタイプ宣言
// --------------------------------------------------
// 初期化
bool initialize();
// ネガマックス法による探索
int nega_max_search(board_t*, int, int, int, bool);
// 盤面の状態をfromからdestへコピーする
void board_copy(board_t*, board_t*);
// ハッシュ値生成
uint64_t make_hash(board_t*);

// ==================================================
// 初期化
// ==================================================
bool initialize() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 255; k++) {
        rand_mask[i][j][k] = engine();
      }
    }
  }
  return true;
}

// ==================================================
// AIの手を取得する
// ==================================================
bitboard_t get_csar_move(board_t *board) {
  // 初期化
  if (!initialized) {
    initialized = initialize();
  }
  // 盤面のバックアップ
  board_t backup;
  board_copy(&backup, board);
  // すべての合法手について繰り返し
  bitboard_t mv, move = 0;
  bitboard_t pos = 0x8000000000000000;
  bitboard_t legal_moves = board->legal_moves;
  int score, alpha = INT_MIN, beta = INT_MAX;
  for (; pos != 0; pos = pos >> 1) {
    mv = (legal_moves & pos);
    if (mv == 0) continue;
    // 着手して盤面を進める
    next_turn(board, mv);
    // 指し手の評価値を取得
    score = -nega_max_search(board, DEPTH, alpha, beta, false);
    // 盤面を元に戻す
    board_copy(board, &backup);
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
  // 置換表に登録されているならその評価値を返す
  uint64_t hash = make_hash(board);
  if (tt.find(hash) != tt.end()) {
    return tt[hash];
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
  // 置換表に登録
  tt[hash] = alpha;
  // 評価値を返す
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

// ==================================================
// ハッシュ値生成
// ==================================================
uint64_t make_hash(board_t *board) {
  uint64_t hash = 0;
  bitboard_t own = get_own_bb(board);
  bitboard_t opp = get_opp_bb(board);
  for (int i = 0; i < 8; i++) {
    hash ^= rand_mask[0][i][(uint64_t)((own >> i * 8) & 255)];
    hash ^= rand_mask[1][i][(uint64_t)((opp >> i * 8) & 255)];
  }
  return hash;
}