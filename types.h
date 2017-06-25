/*----------------------------------------------------------------
|   Author: Christian Callau Romero
|   e-mail: id.32@outlook.com
|   Date: July 2017                 Version: 2.0
|-----------------------------------------------------------------|
|	Project name: BattleShip
|	Name: types.h
|   File description:   Types and constants.
| ----------------------------------------------------------------*/

#ifndef BATTLESHIP_TIPUS_H
#define BATTLESHIP_TIPUS_H

/* Standard libraries */
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Project constants */
#define DIM_MAX         10
#define NAME_MAX        10
#define SCORE_MAX       25

#define GAME_FILE      "game.bin"
#define SCORE_FILE     "scores.txt"

/* Enums */
typedef enum {
  HORIZONTAL = 0,
  VERTICAL = 1
} Orientation_e;

typedef enum {
  CELL_UNKNOWN = '?',
  CELL_WATER = '.',
  CELL_WATER_HIT = '-',
  CELL_SHIP = '@',
  CELL_SHIP_HIT = 'X'
} Cell_e;

typedef enum {
  ERROR = -1,
  REPEATED = 0,
  WATER = 1,
  HIT = 2,
  SUNK = 3
} Shoot_e;

/* Types */
typedef char Name_t[NAME_MAX];

typedef struct {
  Name_t name;
  int points;
} Score_t;

typedef struct {
  Score_t score[SCORE_MAX];
  int num;
} Scores_t;

typedef struct {
  char col;
  int row;
} Coord_t;

typedef struct {
  Cell_e grid[DIM_MAX][DIM_MAX];
  int dim;
} Table_t;

typedef struct {
  Name_t name;
  Coord_t coord;
  Table_t ships;
  Table_t shots;
  int sunk_ships;
  int shot_count;
  int result_sum;
  int result;
} Player_t;

typedef struct {
  Player_t player1;
  Player_t player2;
  int mode;
} Game_t;

#endif //BATTLESHIP_TIPUS_H