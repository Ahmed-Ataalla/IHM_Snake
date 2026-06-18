#include "lvgl.h"
#include "cstdio"
#include "cstdlib"
#include "../lv_version.h"

#define UP 2
#define Down 3
#define Right 4
#define Left 5
#define Max_cercle 99

int tail_length = 3;
int grid = 40;
int vit = 7;
int memoire_tete = 0;

int x_offset = 0;
int y_offset = 0;
int x_offset_2 = 0;
int y_offset_2 = 0;

int dir = 0;
int dir_2 = 0;

int x_max = 240;
int x_min = -240;
int y_max = 120;
int y_min = -120;

int memoire_size = 400;
int memoire_x[400] = {0};
int memoire_y[400] = {0};

int target_x;
int target_y;
bool target_collect = false;
int step = 0;

int score = 0;

void target_pos();
void reset_game();
void score_update();
int random_pos(int min_val, int max_val, int grid_size);
int recup_memoire_index (int memoire_index);


static lv_obj_t *cercle;
static lv_obj_t *tail_cercle[Max_cercle];
static lv_obj_t *target;
static lv_obj_t *score_label;
static lv_obj_t * menu;
static lv_obj_t * title;

static lv_obj_t * game_over_menu;
static lv_obj_t * game_over_label;
static lv_obj_t * game_over_score_label;
static lv_obj_t * restart_label;

// callback bouton start
static void start_event_handler(lv_event_t *e) {
lv_event_code_t code = lv_event_get_code(e);
if(code == LV_EVENT_CLICKED) {
  if (menu!= NULL) {
    lv_obj_add_flag(menu, LV_OBJ_FLAG_HIDDEN);
    } 
  }
}

// callback bouton restart
static void restart_event_handler(lv_event_t *e) {
lv_event_code_t code = lv_event_get_code(e);
if(code == LV_EVENT_CLICKED) {
  if (game_over_menu!= NULL) {
    lv_obj_add_flag(game_over_menu, LV_OBJ_FLAG_HIDDEN);
    } 
  }
}

// Init de l'affichage, des styles et des objets graphiques
void testLvgl()
{
  lv_obj_t *screen = lv_screen_active();
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  int32_t width = lv_display_get_horizontal_resolution(NULL);
  int32_t height = lv_display_get_vertical_resolution(NULL);

  // style de fond dégradé
  static lv_style_t bg_style;
  lv_style_init(&bg_style);
  lv_style_set_radius(&bg_style, 0);
  lv_style_set_border_width(&bg_style, 0);
  lv_style_set_pad_all(&bg_style, 0);

  lv_style_set_bg_color(&bg_style, lv_color_hex(0x9b1842));
  lv_style_set_bg_grad_color(&bg_style, lv_color_hex(0x000000));
  lv_style_set_bg_grad_dir(&bg_style, LV_GRAD_DIR_VER);
  
  lv_obj_t * bg_panel = lv_obj_create(screen);
  lv_obj_clear_flag(bg_panel, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_style(bg_panel, &bg_style, 0);
  lv_obj_set_size(bg_panel, width, height);
  lv_obj_center(bg_panel);

  lv_obj_move_background(bg_panel);

  // création de la tête du serpent
  cercle = lv_obj_create(screen);
  lv_obj_clear_flag(cercle, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(cercle, 40, 40);
  lv_obj_set_style_radius(cercle, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(cercle, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(cercle, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(cercle, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(cercle, 0, LV_STATE_DEFAULT);
  lv_obj_align(cercle, LV_ALIGN_CENTER, 0, 0);

  // génération des segments de la queue
  for (int i = 0; i < Max_cercle; i++)
  {
    tail_cercle[i] = lv_obj_create(screen);
    lv_obj_clear_flag(tail_cercle[i], LV_OBJ_FLAG_SCROLLABLE);

    int segment_size = 38 - (i / 4);
    if (segment_size < 12)
      segment_size = 12;

    lv_obj_set_size(tail_cercle[i], segment_size, segment_size);
    lv_obj_set_style_radius(tail_cercle[i], LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);

    int pattern = (i / 3) % 3;
    if (pattern == 0)
    {
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_CYAN), LV_STATE_DEFAULT);
    }
    else if (pattern == 1)
    {
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    }
    else
    {
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_INDIGO), LV_STATE_DEFAULT);
    }

    lv_obj_set_style_border_width(tail_cercle[i], 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tail_cercle[i], 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(tail_cercle[i], LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_align(tail_cercle[i], LV_ALIGN_CENTER, 0, 0);
  }

  // création de la pomme 
  target = lv_obj_create(screen);
  lv_obj_clear_flag(target, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(target, 20, 20);
  lv_obj_set_style_radius(target, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(target, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(target, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(target, 0, LV_STATE_DEFAULT);
  lv_obj_align(target, LV_ALIGN_CENTER, 0, 0);

  // texte du score
  score_label = lv_label_create(screen);
  lv_obj_set_style_text_color(score_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  lv_obj_align(score_label, LV_ALIGN_TOP_RIGHT, -10, 10);
  score_update();

  // popup du menu principal
  menu = lv_obj_create(screen);
  lv_obj_set_size(menu, width, height);
  lv_obj_clear_flag(target, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_style(menu, &bg_style, 0);
  lv_obj_center(menu);

  title = lv_label_create(menu);
  lv_label_set_text(title, "SNAKE GAME");
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -40);

  // bouton start
  lv_obj_t * start_btn = lv_btn_create(menu);
  lv_obj_align(start_btn, LV_ALIGN_CENTER, 0, 30);
  lv_obj_set_size(start_btn, 120, 50);  
  lv_obj_add_event_cb(start_btn, start_event_handler, LV_EVENT_ALL, NULL);

  lv_obj_t * start_label = lv_label_create(start_btn);
  lv_label_set_text(start_label, "START");
  lv_obj_set_style_text_color(start_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);  
  lv_obj_center(start_label);

  // popup menu game over
  game_over_menu = lv_obj_create(screen);
  lv_obj_set_size(game_over_menu, width, height);
  lv_obj_clear_flag(target, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_style(game_over_menu, &bg_style, 0);
  lv_obj_center(game_over_menu);
  lv_obj_add_flag(game_over_menu, LV_OBJ_FLAG_HIDDEN);

  game_over_label = lv_label_create(game_over_menu);
  lv_label_set_text(game_over_label, "GAME OVER");
  lv_obj_set_style_text_color(game_over_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  lv_obj_align(game_over_label, LV_ALIGN_CENTER, 0, -50);

  game_over_score_label = lv_label_create(game_over_menu);
  lv_label_set_text_fmt(game_over_score_label, "SCORE: %d", 0);
  lv_obj_set_style_text_color(game_over_score_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  lv_obj_align(game_over_score_label, LV_ALIGN_CENTER, 0, -15);

  // bouton game over
  lv_obj_t * restart_btn = lv_btn_create(game_over_menu);
  lv_obj_align(restart_btn, LV_ALIGN_CENTER, 0, 40);
  lv_obj_set_size(restart_btn, 140, 50);
  lv_obj_add_event_cb(restart_btn, restart_event_handler, LV_EVENT_ALL, NULL);

  restart_label = lv_label_create(restart_btn);
  lv_label_set_text(restart_label, "RESTART");
  lv_obj_set_style_text_color(restart_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  lv_obj_center(restart_label);
}

#ifdef ARDUINO

#include "lvglDrivers.h"
#include "demos/lv_demos.h"

void mySetup()
{
  pinMode(UP, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);

  testLvgl();
}

void loop() {}

void myTask(void *pvParameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  target_pos();

  // cacher la position des segments de la queue
  lvglLock();
  lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
  for (int i = 0; i < Max_cercle; i++)
  {
    lv_obj_clear_flag(tail_cercle[i], LV_OBJ_FLAG_HIDDEN);
  }
  lvglUnlock();

  // remplissage des variables de mémoire avec des valeurs hors-champ
  for (int i = 0; i < memoire_size; i++)
  {
    memoire_x[i] = 9999;
    memoire_y[i] = 9999;
  }

  while (1)
  {
    lv_timer_handler();

    // lecture des boutons de direction
    int button_mask = ((!digitalRead(UP)) << 0) |
                      ((!digitalRead(Down)) << 1) |
                      ((!digitalRead(Right)) << 2) |
                      ((!digitalRead(Left)) << 3);

    // filtrage de direction
    switch (button_mask)
    {
    case 1:
      if (dir != 2)
        dir_2 = 1;
      break;
    case 2:
      if (dir != 1)
        dir_2 = 2;
      break;
    case 4:
      if (dir != 4)
        dir_2 = 3;
      break;
    case 8:
      if (dir != 3)
        dir_2 = 4;
      break;
    default:
      break;
    }

    // déplacement sur la grille et téléportation aux bordures de l'écran
    if (abs(x_offset - x_offset_2) < vit && abs(y_offset - y_offset_2) < vit)
    {
      x_offset = x_offset_2;
      y_offset = y_offset_2;
      dir = dir_2;
      step++;

      if (dir == 1)
        y_offset_2 -= grid;
      if (dir == 2)
        y_offset_2 += grid;
      if (dir == 3)
        x_offset_2 += grid;
      if (dir == 4)
        x_offset_2 -= grid;

      if (x_offset_2 > x_max)
      {
        x_offset_2 = x_min;
        x_offset = x_min;
      }
      else if (x_offset_2 < x_min)
      {
        x_offset_2 = x_max - grid;
        x_offset = x_max - grid;
      }
      if (y_offset_2 > y_max)
      {
        y_offset_2 = y_min;
        y_offset = y_min;
      }
      else if (y_offset_2 < y_min)
      {
        y_offset_2 = y_max;
        y_offset = y_max;
      }
    }

    // transition fluide de mouvement s'il n'y a pas de menu
    if (lv_obj_has_flag(menu, LV_OBJ_FLAG_HIDDEN) && lv_obj_has_flag(game_over_menu, LV_OBJ_FLAG_HIDDEN))
    {
    if (x_offset < x_offset_2)
      x_offset += vit;
    if (x_offset > x_offset_2)
      x_offset -= vit;
    if (y_offset < y_offset_2)
      y_offset += vit;
    if (y_offset > y_offset_2)
      y_offset -= vit;
    }

    // enregistrement de la position de la tête
    memoire_tete = (memoire_tete - 1 + memoire_size) % memoire_size;
    memoire_x[memoire_tete] = x_offset;
    memoire_y[memoire_tete] = y_offset;

    // analyse des collisions de la tête contre les segments de la queue
    if (dir != 0 && step > 2)
    {
      int active_tail = (tail_length > Max_cercle) ? Max_cercle : tail_length;
      int memoire_index = 0;

      for (int i = 0; i < active_tail; i++)
      {
        int memoire_index = recup_memoire_index(i);

        if (i >= 4)
        {
          int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
          int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

          if (tx != 9999 && ty != 9999)
          {
            if (abs(x_offset - tx) <= 18 && abs(y_offset - ty) <= 18)
            {
              reset_game();
              break;
            }
          }
        }
      }
    }

    // analyse de la récupération de la pomme, incrémentation du score et ajout d'un segment
    target_collect = false;

    int diff_x = abs(x_offset - target_x);
    int diff_y = abs(y_offset - target_y);

    if (diff_x <= 20 && diff_y <= 20 && dir != 0)
    {
      target_collect = true;
      
      score += 10;
      
      if (tail_length < Max_cercle)
      {
        lvglLock();
        lv_obj_clear_flag(tail_cercle[tail_length], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_bg_opa(tail_cercle[tail_length], LV_OPA_COVER, LV_STATE_DEFAULT);
        lvglUnlock();
        tail_length++;
      }
      target_pos();
      lvglLock();
      lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
      lvglUnlock();
      score_update();
    }

    // actualisation de l'alignement de tous les segments 
    if (dir != 0)
    {
      int active_tail = (tail_length > Max_cercle) ? Max_cercle : tail_length;
      int memoire_index = 0;

      lvglLock();
      lv_obj_align(cercle, LV_ALIGN_CENTER, x_offset, y_offset);

      for (int i = 0; i < Max_cercle; i++)
      {

        int memoire_index = recup_memoire_index(i);

        if (i < active_tail)
        {
          int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
          int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

          // Masque temporairement les segments de la queue pendant qu'ils traversent le mur pour éviter un bug visuel étiré.
          if (x_offset == x_min && tx > 100)
            continue;
          if (x_offset == (x_max - grid) && tx < -100)
            continue;
          if (y_offset == y_min && ty > 80)
            continue;
          if (y_offset == y_max && ty < -80)
            continue;

          lv_obj_clear_flag(tail_cercle[i], LV_OBJ_FLAG_HIDDEN);
          lv_obj_set_style_bg_opa(tail_cercle[i], LV_OPA_COVER, LV_STATE_DEFAULT);
          lv_obj_align(tail_cercle[i], LV_ALIGN_CENTER, tx, ty);
        }
        else
        {
          lv_obj_set_style_bg_opa(tail_cercle[i], LV_OPA_0, LV_STATE_DEFAULT);
        }
      }
      lvglUnlock();
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
  }
}

// calcul de l'apparition des pommes d'une manière aléatoire sur la grille
int random_pos(int min_val, int max_val, int grid_size)
{
  int val;
  min_val = min_val / grid_size;
  max_val = max_val / grid_size;
  val = min_val + (rand() % (max_val - min_val + 1));
  return val * grid_size;
}

// détermination de l'emplacement libre de la pomme pour ne pas apparaître sur la tête ni la queue
void target_pos()
{
  bool valid_pos = false;

  while (!valid_pos)
  {
    target_x = random_pos(-220, 200, grid);
    target_y = random_pos(-100, 100, grid);

    valid_pos = true;

    if (target_x == x_offset && target_y == y_offset)
    {
      valid_pos = false;
      continue;
    }

    int active_tail = (tail_length > Max_cercle) ? Max_cercle : tail_length;
    int memoire_index = 0;

    for (int i = 0; i < active_tail; i++)
    {

      int memoire_index = recup_memoire_index(i);

      int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
      int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

      int diff_x = abs(tx - target_x);
      int diff_y = abs(ty - target_y);

      if (diff_x < 40 && diff_y < 40)
      {
        valid_pos = false;
        break;
      }
    }
  }
}

// réinitialisation du jeu en basculant vers le menu game over
void reset_game()
{

  if (game_over_score_label != NULL && game_over_menu != NULL) {
    lvglLock();
    lv_label_set_text_fmt(game_over_score_label,"SCORE: %d", score);
    lv_obj_clear_flag(game_over_menu, LV_OBJ_FLAG_HIDDEN);
    lvglUnlock();
  }
  x_offset = 0;
  y_offset = 0;
  x_offset_2 = 0;
  y_offset_2 = 0;
  dir = 0;
  dir_2 = 0;
  tail_length = 3;
  memoire_tete = 0;
  target_collect = false;
  step = 0;
  score = 0;

  for (int i = 0; i < memoire_size; i++)
  {
    memoire_x[i] = 9999;
    memoire_y[i] = 9999;
  }

  target_pos();

  lvglLock();
  lv_obj_align(cercle, LV_ALIGN_CENTER, 0, 0);
  lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
  for (int i = 0; i < Max_cercle; i++)
  {
    lv_obj_set_style_bg_opa(tail_cercle[i], LV_OPA_0, LV_STATE_DEFAULT);
  }
  lvglUnlock();
  score_update();

  if(menu != NULL) {
    lvglLock();
    lv_obj_clear_flag(menu, LV_OBJ_FLAG_HIDDEN);
    lvglUnlock();
  }
}

// mettre à jour la valeur du score
void score_update()
{
  if (score_label != NULL)
  {
    lvglLock();
    lv_label_set_text_fmt(score_label, "%d",score);
    lvglUnlock();
  }
}

// calcul de l'espacement entre chaque segment
int recup_memoire_index (int memoire_index){

  int index = 0;
  for (int i = 0; i <= memoire_index; i++)
  {
    int spacing = ((grid / 10) - (i / 4));
    if (spacing < 3)
      spacing = 3;
    index += spacing;
    if (index >= memoire_size)
      index = memoire_size - 1;
  }
  return index;
}

#else

int main(void)
{
  return 0;
}

#endif
