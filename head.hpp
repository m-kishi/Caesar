// ==================================================
// head.hpp
// ヘッダファイル
// 共通定数や関数宣言を定義する
// ==================================================
#include <stdio.h>
#include <stdint.h>

// --------------------------------------------------
// ゲームの状態の定義
// --------------------------------------------------
typedef enum {
  TURN, //手番
  PASS, //パス
  OVER, //終局
} status_t;

// --------------------------------------------------
// プレイヤー(手番)の定義
// 白番を-1としておけば，手番を渡す際は現在の手番に
// -1を掛けるだけでよい
// --------------------------------------------------
typedef enum {
  BLACK =  1,
  WHITE = -1,
} player_t;

// --------------------------------------------------
// ビットボードの定義
// 石の有無を1か0かで保持する
// 盤面は8*8=64なので64ビットの型を使用する
// --------------------------------------------------
typedef uint64_t bitboard_t;

// --------------------------------------------------
// 盤面の定義
// --------------------------------------------------
typedef struct {
  player_t player;        //現在の手番
  status_t status;        //ゲームの状態
  int nblack;             //黒石の数
  int nwhite;             //白石の数
  bitboard_t black;       //黒石のビットボード
  bitboard_t white;       //白石のビットボード
  bitboard_t legal_moves; //現在の局面の合法手
} board_t;

// --------------------------------------------------
// 関数のプロトタイプ宣言(proc.cpp)
// --------------------------------------------------
// 盤面の初期化処理
void initialize(board_t*);
// 石の数を数える
int count_of_discs(bitboard_t);
// 盤面の更新処理
void next_turn(board_t*, bitboard_t);
// 局面の状態をチェックする
void check_board_status(board_t*);
// 現在手番のプレイヤのビットボードを取得する
bitboard_t get_player_bb(board_t*);
// 現在手番でないプレイヤのビットボードを取得する
bitboard_t get_opponent_bb(board_t*);
// 合法手の一覧を生成する
bitboard_t get_legal_moves(board_t*);
// 反転する石の場所を取得する
bitboard_t get_flip_pattern(board_t*, bitboard_t);

// --------------------------------------------------
// 関数のプロトタイプ宣言(disp.cpp)
// --------------------------------------------------
// 盤面の表示処理
void display(board_t*);
// 合法手のビットボードを座標に変換して表示する
void display_legal_moves(bitboard_t);
// AIの着手箇所を座標に変換して表示する
void display_csar_move(bitboard_t);
// 盤面の座標からビットボードへ変換する
bitboard_t cr_to_bb(int, int);
// プレイヤの入力を取得する
bitboard_t get_player_move(board_t*);
