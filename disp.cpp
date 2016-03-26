// **************************************************
// disp.cpp
// 画面表示と入力受付の処理を定義する
// **************************************************
#include <string.h>
#include "head.hpp"

// --------------------------------------------------
// 行番号(画面表示用)
// --------------------------------------------------
char* ROW_NUM[8] = {
    (char*)"１", (char*)"２", (char*)"３", (char*)"４",
    (char*)"５", (char*)"６", (char*)"７", (char*)"８",
};

// --------------------------------------------------
// 列番号(入力用)
// --------------------------------------------------
char* COL[8] = {
  (char*)"a", (char*)"b", (char*)"c", (char*)"d",
  (char*)"e", (char*)"f", (char*)"g", (char*)"h",
};

// --------------------------------------------------
// 行番号(入力用)
// --------------------------------------------------
char* ROW[8] = {
  (char*)"1", (char*)"2", (char*)"3", (char*)"4",
  (char*)"5", (char*)"6", (char*)"7", (char*)"8",
};

// ==================================================
// 盤面の表示処理
// ==================================================
void display(board_t *board) {
  // 列番号の表示
  printf("   ａ ｂ ｃ ｄ ｅ ｆ ｇ ｈ \n");
  printf("  +--+--+--+--+--+--+--+--+\n");
  // 左上から調べていく
  bitboard_t pos = 0x8000000000000000;
  for (int i = 0; i < 64; i++) {
    // 一番左端なら行番号を表示
    if (i % 8 == 0) {
      printf("%s|", ROW_NUM[(i%8)+(i/8)]);
    }
    // 石の表示
    if ((board->black & pos) != 0) printf("黒|");
    if ((board->white & pos) != 0) printf("白|");
    if (((board->black | board->white) & pos) == 0) printf("　|");
    // 一番右端なら改行
    if (i % 8 == 7) {
      printf("\n");
      printf("  +--+--+--+--+--+--+--+--+\n");
    }
    // 調べる場所の更新
    pos = pos >> 1;
  }
  // 石の数の表示
  printf("黒石: %d\n", board->nblack);
  printf("白石: %d\n", board->nwhite);
  // 現在の手番
  printf("手番: %s\n", board->player == BLACK ? "黒" : "白");
  // 合法手の表示
  display_legal_moves(board->legal_moves);
}

// ==================================================
// 合法手のビットボードを座標に変換して表示する
// ==================================================
void display_legal_moves(bitboard_t legal_moves) {
  bitboard_t pos = 0x8000000000000000;
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      if ((legal_moves & pos) != 0) {
        printf("%s%s ", COL[c], ROW[r]);
      }
      pos = pos >> 1;
    }
  }
  printf("> ");
}

// ==================================================
// AIの着手箇所を座標に変換して表示する
// ==================================================
void display_csar_move(bitboard_t mv) {
  bitboard_t pos = 0x8000000000000000;
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      if ((mv & pos) != 0) {
        printf("%s%s\n", COL[c], ROW[r]);
        return;
      }
      pos = pos >> 1;
    }
  }
}

// ==================================================
// 盤面の座標からビットボードへ変換する
// ==================================================
bitboard_t cr_to_bb(int col, int row) {
  return 0x8000000000000000 >> (col + row*8);
}

// ==================================================
// プレイヤの入力を取得する
// ==================================================
bitboard_t get_player_move(board_t *board) {
  char buf[4];
  fgets(buf, sizeof(buf), stdin);
  char col = buf[0];
  char row = buf[1];
  // 入力チェック
  if (col < 'a' || 'h' < col) return 0;
  if (row < '1' || '8' < row) return 0;
  // 合法手でないとダメ
  bitboard_t mv = cr_to_bb(col - 'a', row - '1');
  if ((mv & board->legal_moves) == 0) {
    return 0;
  }
  return mv;
}
