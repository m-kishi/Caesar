// **************************************************
// main.cpp
// メイン処理を定義する
// **************************************************
#include <unistd.h>
#include "head.hpp"
#include "csar.hpp"

// ==================================================
// プログラムメイン
// ==================================================
int main() {
  board_t board;
  initialize(&board);

  while (board.status != OVER) {
    // 現在の局面を表示
    display(&board);
    // 着手を取得
    bitboard_t mv = 0;
    if (board.player == BLACK) {
      mv = get_player_move(&board);
    } else {
      printf("AI 考え中...");
      fflush(stdout);
      mv = get_csar_move(&board);
      sleep(1); //1秒は待つ
      printf(" > ");
      display_csar_move(mv);
    }
    // 合法手ならば
    if (mv != 0) {
      // 局面を更新
      next_turn(&board, mv);
      // 局面の状態をチェック
      check_board_status(&board);
    }
  }
  display(&board);

  // 勝敗の表示
  if (board.nblack >  board.nwhite) printf("黒の勝ち！\n");
  if (board.nblack <  board.nwhite) printf("白の勝ち！\n");
  if (board.nblack == board.nwhite) printf("引き分け．\n");

  return 0;
}

