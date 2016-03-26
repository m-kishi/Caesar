// **************************************************
// proc.cpp
// ゲーム処理を定義する
// **************************************************
#include "head.hpp"

// ==================================================
// 盤面の初期化処理
// ==================================================
void initialize(board_t *board) {
  board->player = BLACK;
  board->black = 0x0000000810000000;
  board->white = 0x0000001008000000;
  board->nblack = count_of_discs(board->black);
  board->nwhite = count_of_discs(board->white);
  board->legal_moves = get_legal_moves(board);
}

// ==================================================
// 石の数を数える
// 分割統治法による高速ビットカウント
// ==================================================
int count_of_discs(bitboard_t bitboard) {
  bitboard_t bb = bitboard;
  bb = (bb & 0x5555555555555555) + ((bb & 0xaaaaaaaaaaaaaaaa) >>  1);
  bb = (bb & 0x3333333333333333) + ((bb & 0xcccccccccccccccc) >>  2);
  bb = (bb & 0x0f0f0f0f0f0f0f0f) + ((bb & 0xf0f0f0f0f0f0f0f0) >>  4);
  bb = (bb & 0x00ff00ff00ff00ff) + ((bb & 0xff00ff00ff00ff00) >>  8);
  bb = (bb & 0x0000ffff0000ffff) + ((bb & 0xffff0000ffff0000) >> 16);
  bb = (bb & 0x00000000ffffffff) + ((bb & 0xffffffff00000000) >> 32);
  return (int)bb;
}

// ==================================================
// 盤面の更新処理
// 引数のmvは着手箇所(1ビットのみが1で他はすべて0)
// ==================================================
void next_turn(board_t *board, bitboard_t mv) {
  // 反転する石の場所を取得
  bitboard_t flip = get_flip_pattern(board, mv);
  // 石の反転処理
  if (board->player == BLACK) {
    // あるビットと1のXORはそのビットの逆になる
    // 1なら0になり0なら1になる
    // つまり石の反転処理そのものということ
    board->black ^= mv | flip;
    board->white ^= flip;
  } else {
    board->white ^= mv | flip;
    board->black ^= flip;
  }
  // 手番の入れ替え
  board->player = (player_t)(board->player * -1);
  // 石の数を更新
  board->nblack = count_of_discs(board->black);
  board->nwhite = count_of_discs(board->white);
  // 合法手の更新
  board->legal_moves = get_legal_moves(board);
}

// ==================================================
// 局面の状態をチェックする
// ==================================================
void check_board_status(board_t *board) {
  // まだ合法手があるなら続行
  if (board->legal_moves != 0) {
    board->status = TURN;
    return;
  }
  // 自分に合法手がないなら手番を相手に渡して
  board->player = (player_t)(board->player * -1);
  // 相手にも合法手がないなら終局
  board->legal_moves = get_legal_moves(board);
  if (board->legal_moves == 0) {
    board->status = OVER;
    return;
  }
  // 相手に合法手があるなら自分はパス
  board->status = PASS;
  return;
}

// ==================================================
// 現在手番のプレイヤのビットボードを取得する
// ==================================================
bitboard_t get_player_bb(board_t *board) {
  return board->player == BLACK ? board->black : board->white;
}

// ==================================================
// 現在手番でないプレイヤのビットボードを取得する
// ==================================================
bitboard_t get_opponent_bb(board_t *board) {
  return board->player == WHITE ? board->black : board->white;
}

// ==================================================
// 合法手の一覧を生成する
// ==================================================
bitboard_t get_legal_moves(board_t *board) {
  bitboard_t t, mask, legal_moves = 0;
  bitboard_t player   = get_player_bb(board);
  bitboard_t opponent = get_opponent_bb(board);

  // 空マスを取得
  // 黒石と白石の場所のNOTで取得
  bitboard_t blank = ~(player | opponent);

  // --------------------------------------------------
  // 右方向の処理
  // --------------------------------------------------
  // 別行の値がシフトしてこないようにマスクする
  // 一番端の他石が反転することはない(関係ない)ので
  // 両端が0のビットボードと他石のANDをとればよい
  mask = opponent & 0x7e7e7e7e7e7e7e7e;
  // 反転可能な石は最大6つ
  t  = mask & (player << 1); //自石の左隣の他石を調べる
  t |= mask & (t << 1);      //さらにその左隣の他石を調べる
  t |= mask & (t << 1);      //さらにその左隣の他石を調べる
  t |= mask & (t << 1);      //さらにその左隣の他石を調べる
  t |= mask & (t << 1);      //さらにその左隣の他石を調べる
  t |= mask & (t << 1);      //さらにその左隣の他石を調べる
  // 上記tの左隣が自石の置ける位置
  // もちろんそこは空マスでなければならない
  legal_moves |= blank & (t << 1);

  // --------------------------------------------------
  // 左方向の処理
  // 考え方は右方向の処理と同じ(他の方向も同様)
  // --------------------------------------------------
  mask = opponent & 0x7e7e7e7e7e7e7e7e;
  t  = mask & (player >> 1);
  t |= mask & (t >> 1);
  t |= mask & (t >> 1);
  t |= mask & (t >> 1);
  t |= mask & (t >> 1);
  t |= mask & (t >> 1);
  legal_moves |= blank & (t >> 1);

  // --------------------------------------------------
  // 上方向の処理
  // --------------------------------------------------
  mask = opponent & 0x00ffffffffffff00;
  t  = mask & (player << 8);
  t |= mask & (t << 8);
  t |= mask & (t << 8);
  t |= mask & (t << 8);
  t |= mask & (t << 8);
  t |= mask & (t << 8);
  legal_moves |= blank & (t << 8);

  // --------------------------------------------------
  // 下方向の処理
  // --------------------------------------------------
  mask = opponent & 0x00ffffffffffff00;
  t  = mask & (player >> 8);
  t |= mask & (t >> 8);
  t |= mask & (t >> 8);
  t |= mask & (t >> 8);
  t |= mask & (t >> 8);
  t |= mask & (t >> 8);
  legal_moves |= blank & (t >> 8);

  // --------------------------------------------------
  // 右上方向の処理
  // --------------------------------------------------
  mask = opponent & 0x007e7e7e7e7e7e00;
  t  = mask & (player << 7);
  t |= mask & (t << 7);
  t |= mask & (t << 7);
  t |= mask & (t << 7);
  t |= mask & (t << 7);
  t |= mask & (t << 7);
  legal_moves |= blank & (t << 7);

  // --------------------------------------------------
  // 左上方向の処理
  // --------------------------------------------------
  mask = opponent & 0x007e7e7e7e7e7e00;
  t  = mask & (player << 9);
  t |= mask & (t << 9);
  t |= mask & (t << 9);
  t |= mask & (t << 9);
  t |= mask & (t << 9);
  t |= mask & (t << 9);
  legal_moves |= blank & (t << 9);

  // --------------------------------------------------
  // 右下方向の処理
  // --------------------------------------------------
  mask = opponent & 0x007e7e7e7e7e7e00;
  t  = mask & (player >> 9);
  t |= mask & (t >> 9);
  t |= mask & (t >> 9);
  t |= mask & (t >> 9);
  t |= mask & (t >> 9);
  t |= mask & (t >> 9);
  legal_moves |= blank & (t >> 9);

  // --------------------------------------------------
  // 左下方向の処理
  // --------------------------------------------------
  mask = opponent & 0x007e7e7e7e7e7e00;
  t  = mask & (player >> 7);
  t |= mask & (t >> 7);
  t |= mask & (t >> 7);
  t |= mask & (t >> 7);
  t |= mask & (t >> 7);
  t |= mask & (t >> 7);
  legal_moves |= blank & (t >> 7);

  return legal_moves;
}

// ==================================================
// 反転する石の場所を取得する
// 引数のmvは着手箇所(1ビットのみが1で他はすべて0)
// ==================================================
bitboard_t get_flip_pattern(board_t *board, bitboard_t mv) {
  bitboard_t temp, mask, flip = 0;
  bitboard_t player   = get_player_bb(board);
  bitboard_t opponent = get_opponent_bb(board);

  // 着手箇所が空マスで無いなら終了(通常はありえない)
  if (((player | opponent) & mv) != 0) {
    return flip;
  }

  // --------------------------------------------------
  // 右方向の処理
  // --------------------------------------------------
  temp = 0;
  // 前の行がシフトしてきたときと
  // 着手箇所が次の行へ進んでしまった時のためにマスクする
  mask = (mv >> 1) & 0x7f7f7f7f7f7f7f7f;
  // 他石が連続している間だけ繰り返し
  while (mask != 0 && (mask & opponent) != 0) {
    // 反転箇所を保存
    temp |= mask;
    // さらに右隣を調べる
    mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
  }
  // 他石の繰り返しが終わった直後に自石がないといけない
  if ((mask & player) != 0) {
    // OKなら反転パターンに追加
    flip |= temp;
  }

  // --------------------------------------------------
  // 左方向の処理
  // 考え方は右方向の処理と同じ(他の方向も同様)
  // --------------------------------------------------
  temp = 0;
  mask = (mv << 1) & 0xfefefefefefefefe;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask << 1) & 0xfefefefefefefefe;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  // --------------------------------------------------
  // 上方向の処理
  // --------------------------------------------------
  temp = 0;
  mask = (mv << 8) & 0xffffffffffffff00;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask << 8) & 0xffffffffffffff00;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  // --------------------------------------------------
  // 下方向の処理
  // --------------------------------------------------
  temp = 0;
  mask = (mv >> 8) & 0x00ffffffffffffff;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask >> 8) & 0x00ffffffffffffff;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  // --------------------------------------------------
  // 右上方向の処理
  // --------------------------------------------------
  temp = 0;
  mask = (mv << 7) & 0x7f7f7f7f7f7f7f00;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  // --------------------------------------------------
  // 左上方向の処理
  // --------------------------------------------------
  temp = 0;
  mask = (mv << 9) & 0xfefefefefefefe00;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask << 9) & 0xfefefefefefefe00;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  // --------------------------------------------------
  // 右下方向の処理
  // --------------------------------------------------
  temp = 0;
  mask = (mv >> 9) & 0x007f7f7f7f7f7f7f;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  // --------------------------------------------------
  // 左下方向の処理
  // --------------------------------------------------
  temp = 0;
  mask = (mv >> 7) & 0x00fefefefefefefe;
  while (mask != 0 && (mask & opponent) != 0) {
    temp |= mask;
    mask = (mask >> 7) & 0x00fefefefefefefe;
  }
  if ((mask & player) != 0) {
    flip |= temp;
  }

  return flip;
}
