/*----------------------------------------------------------------
|   Author: Christian Callau Romero
|   e-mail: id.32@outlook.com
|   Date: July 2017                 Version: 2.0
|-----------------------------------------------------------------|
|	Project name: BattleShip
|	Name: logic.c
|   File description:   Game logic functions.
| ----------------------------------------------------------------*/

#include "types.h"
#include "functions.h"

bool play_game(Game_t *game, Scores_t *scores) {
  switch (game->mode) {
    case 0:
      return play_game_solo_AI(game, scores);
    case 1:
      return play_game_solo_Player(game, scores);
    case 2:
      return play_game_Player_vs_AI(game, scores);
    default:
      return true;
  }
}

bool play_game_solo_AI(Game_t *game, Scores_t *scores) {
  system("cls");
  print_tables(&game->player1.ships, &game->player1.shots);
  pause();

  while (game->player1.sunk_ships < TOTAL_SHIPS) {
    game->player1.coord = get_coord_from_AI(&game->player1.shots);
    game->player1.result = shoot(&game->player1.ships, &game->player1.coord);
    game->player1.shot_count++;
    game->player1.result_sum += game->player1.result - 1;
    if (game->player1.result == SUNK) game->player1.sunk_ships++;
    update_shots_table(&game->player1.shots, &game->player1.coord, game->player1.result);

    system("cls");
    print_tables(&game->player1.ships, &game->player1.shots);
    printf("  Ships left: %d\n", TOTAL_SHIPS - game->player1.sunk_ships);
    printf("  Last shot: %d-%d\n", game->player1.coord.col, game->player1.coord.row);
    printf("  Result: %s\n", shot_to_string(game->player1.result));
    printf("  Shots: %d\n", game->player1.shot_count);
    pause();
  }

  game->game = false;
  remove(GAME_FILE);

  int score = get_score(&game->player1);

  printf("\n");
  printf("  Game Over!\n");
  printf("  Score: %d\n", score);
  pause();

  return true;
}

bool play_game_solo_Player(Game_t *game, Scores_t *scores) {
  // TODO
  return true;
}

bool play_game_Player_vs_AI(Game_t *game, Scores_t *scores) {
  // TODO
  return true;
}

Shot_e shoot(Table_t *ships_table, const Coord_t *coord) {
  switch (ships_table->grid[coord->row][coord->col]) {
  case WATER_HIT: case SHIP_HIT:
    return REPEATED;
  case WATER:
    ships_table->grid[coord->row][coord->col] = WATER_HIT;
    return MISS;
  case SHIP:
    ships_table->grid[coord->row][coord->col] = SHIP_HIT;
    if (is_ship_sunk(ships_table, *coord)) return SUNK;
    else return HIT;
  default:
    return ERROR;
  }
}

bool is_ship_sunk(const Table_t *ships_table, Coord_t coord) {
  while (coord.row - 1 >= 0 && !is_water(ships_table->grid[coord.row - 1][coord.col])) {
    if (ships_table->grid[coord.row--][coord.col] == SHIP) return false;
  }
  while (coord.col - 1 >= 0 && !is_water(ships_table->grid[coord.row][coord.col - 1])) {
    if (ships_table->grid[coord.row][coord.col--] == SHIP) return false;
  }
  while (coord.row + 1 < ships_table->dim && !is_water(ships_table->grid[coord.row + 1][coord.col])) {
    if (ships_table->grid[coord.row++][coord.col] == SHIP) return false;
  }
  while (coord.row + 1 < ships_table->dim && !is_water(ships_table->grid[coord.row][coord.col + 1])) {
    if (ships_table->grid[coord.row][coord.col++] == SHIP) return false;
  }
  return true;
}

bool is_water(Cell_e cell) {
  return cell == WATER || cell == WATER_HIT;
}

void update_shots_table(Table_t *shots_table, const Coord_t *coord, Shot_e result) {
  switch (result) {
  case MISS:
    shots_table->grid[coord->row][coord->col] = WATER;
    break;
  case HIT:
    shots_table->grid[coord->row][coord->col] = SHIP;
    break;
  case SUNK:
    shots_table->grid[coord->row][coord->col] = SHIP;
    unveil_surroundings(shots_table, coord);
    break;
  default:
    break;
  }
}

void unveil_surroundings(Table_t *shots_table, const Coord_t *coord) {
  Coord_t upper_left;
  Coord_t lower_right;

  upper_left.row = lower_right.row = coord->row;
  upper_left.col = lower_right.col = coord->col;

  while (upper_left.row - 1 >= 0 && shots_table->grid[upper_left.row - 1][upper_left.col] == SHIP) {
    upper_left.row--;
  }
  while (upper_left.col - 1 >= 0 && shots_table->grid[upper_left.row][upper_left.col - 1] == SHIP) {
    upper_left.col--;
  }
  if (upper_left.row - 1 >= 0) {
    upper_left.row--;
  }
  if (upper_left.col - 1 >= 0) {
    upper_left.col--;
  }

  while (lower_right.row + 1 < shots_table->dim && shots_table->grid[lower_right.row + 1][lower_right.col] == SHIP) {
    lower_right.row++;
  }
  while (lower_right.col + 1 < shots_table->dim && shots_table->grid[lower_right.row][lower_right.col + 1] == SHIP) {
    lower_right.col++;
  }
  if (lower_right.row + 1 < shots_table->dim) {
    lower_right.row++;
  }
  if (lower_right.col + 1 < shots_table->dim) {
    lower_right.col++;
  }

  for (int i = upper_left.row; i <= lower_right.row; i++) {
    for (int j = upper_left.col; j <= lower_right.col; j++) {
      if (shots_table->grid[i][j] == UNKNOWN) {
        shots_table->grid[i][j] = WATER;
      }
    }
  }
}

Coord_t get_coord_from_AI(const Table_t *table) {
  Coord_t coord;

  for (coord.row = 0; coord.row < table->dim; coord.row++) {
    for (coord.col = 0; coord.col < table->dim; coord.col++) {
      if (is_ship_not_sunk(table, &coord)) {
        if (is_ship_horizont(table, &coord)) {
          if (coord.row - 1 >= 0 && table->grid[coord.row - 1][coord.col] == UNKNOWN) {
            coord.row--;
          } else {
            while (table->grid[coord.row][coord.col] == SHIP) coord.row++;
          }
        } else if (is_ship_vertical(table, &coord)) {
          if (coord.col - 1 >= 0 && table->grid[coord.row][coord.col - 1] == UNKNOWN) {
            coord.col--;
          } else {
            while (table->grid[coord.row][coord.col] == SHIP) coord.col++;
          }
        } else {
          int row, col;
          do {
            if (rand() % 2) {
              row = rand() % 2 ? -1 : 1;
              col = 0;
            } else {
              row = 0;
              col = rand() % 2 ? -1 : 1;
            }
          } while (table->grid[coord.row + row][coord.col + col] != UNKNOWN);
          coord.row += row;
          coord.col += col;
        }
        return coord;
      }
    }
  }

  do {
    coord.row = rand() % table->dim;
    coord.col = rand() % table->dim;
  } while (table->grid[coord.row][coord.col] != UNKNOWN);

  return coord;
}

bool is_ship_not_sunk(const Table_t *table, const Coord_t *coord) {
  return table->grid[coord->row][coord->col] == SHIP && (
         coord->row - 1 >= 0 && table->grid[coord->row - 1][coord->col] == UNKNOWN ||
         coord->col - 1 >= 0 && table->grid[coord->row][coord->col - 1] == UNKNOWN ||
         coord->row + 1 < table->dim && table->grid[coord->row + 1][coord->col] == UNKNOWN ||
         coord->col + 1 < table->dim && table->grid[coord->row][coord->col + 1] == UNKNOWN);
}

bool is_ship_horizont(const Table_t *table, const Coord_t *coord) {
  return coord->row - 1 >= 0 && table->grid[coord->row - 1][coord->col] == SHIP ||
         coord->row + 1 < table->dim && table->grid[coord->row + 1][coord->col] == SHIP;
}

bool is_ship_vertical(const Table_t *table, const Coord_t *coord) {
  return coord->col - 1 >= 0 && table->grid[coord->row][coord->col - 1] == SHIP ||
         coord->col + 1 < table->dim && table->grid[coord->row][coord->col + 1] == SHIP;
}

int get_score(const Player_t *player) {
  return (int) ((float) player->ships.dim / player->shot_count * player->result_sum * 100);
}