#include "lvgl.h"
#include "cstdio"
#include "cstdlib"

#define UP 2
#define Down 3
#define Right 4
#define Left 5
#define Max_cercle 50

int tail_length = 3;

int grid = 40;
int vit = 5;

int memoire_tete = 0;

  int x_offset = 0;
  int y_offset = 0;

  int x_offset_2 = 0;
  int y_offset_2 = 0;

  int dir = 0;
  int dir_2 = 0;
  
  int x_max=240;
  int x_min=-240;
  int y_max=120;
  int y_min=-120;

  int memoire_size = 400;
  int memoire_x[400] = {0};
  int memoire_y[400] = {0};

  int target_x; int target_y;

void  target_pos();
void reset_game();
int random_pos (int min_val,int max_val,int grid_size){
  int val;
  min_val = min_val/grid_size;
  max_val = max_val/grid_size;
  val = min_val +(rand()%(max_val-min_val+1));
  return val*grid_size;
}

static void event_handler(lv_event_t *e)
{
  // lv_event_code_t code = lv_event_get_code(e);

  // if(code == LV_EVENT_CLICKED) {
  //     LV_LOG_USER("Clicked");
  // }
  // else if(code == LV_EVENT_VALUE_CHANGED) {
  //     LV_LOG_USER("Toggled");
  // }
}
static lv_obj_t *cercle;
static lv_obj_t *tail_cercle[Max_cercle];
static lv_obj_t *target;

void testLvgl()
{
  lv_obj_t *screen = lv_screen_active();
  lv_obj_clear_flag(screen,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  cercle = lv_obj_create(screen);
  lv_obj_clear_flag(cercle,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(cercle, 40, 40);
  lv_obj_set_style_radius(cercle, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(cercle, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(cercle, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(cercle, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(cercle, 0, LV_STATE_DEFAULT);
  lv_obj_align(cercle, LV_ALIGN_CENTER, 0, 0);

  for (int i = 0; i < Max_cercle;i++)
  {
    tail_cercle[i] = lv_obj_create(screen);
    lv_obj_clear_flag(tail_cercle[i],LV_OBJ_FLAG_SCROLLABLE);

    int segment_size = 38 - (i/4);

    if (segment_size < 12) segment_size = 12;

    lv_obj_set_size(tail_cercle[i],segment_size,segment_size);
    lv_obj_set_style_radius(tail_cercle[i], LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);

    int pattern =(i/3)%3;
    if (pattern==0){
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_CYAN), LV_STATE_DEFAULT);
    }
    else if (pattern==1){
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    }
    else{
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_INDIGO), LV_STATE_DEFAULT);
    }
    
    lv_obj_set_style_border_width(tail_cercle[i], 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tail_cercle[i], 0, LV_STATE_DEFAULT);

    lv_obj_set_style_bg_opa(tail_cercle[i], LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_align(tail_cercle[i], LV_ALIGN_CENTER, 0, 0);
  }


 target = lv_obj_create(screen);
  lv_obj_clear_flag(target,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(target, 20, 20);
  lv_obj_set_style_radius(target, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(target, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(target, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(target, 0, LV_STATE_DEFAULT);
  lv_obj_align(target, LV_ALIGN_CENTER, 0, 0);

}

#ifdef ARDUINO

#include "lvglDrivers.h"

// à décommenter pour tester la démo
#include "demos/lv_demos.h"

void mySetup()
{
  // à décommenter pour tester la démo
  // lv_demo_widgets();

  // Initialisations générales
  pinMode(UP, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);

  testLvgl();
}

void loop()
{
  // Inactif (pour mise en veille du processeur)
}

void myTask(void *pvParameters)
{
  // Init
  TickType_t xLastWakeTime;
  // Lecture du nombre de ticks quand la tâche débute
  xLastWakeTime = xTaskGetTickCount();

  target_pos();

  lvglLock();
  lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
  for (int i = 0; i < Max_cercle;i++)
  {
    lv_obj_clear_flag(tail_cercle[i],LV_OBJ_FLAG_HIDDEN);
  }
  lvglUnlock();

  for(int i = 0; i < memoire_size; i++){
    memoire_x[i] = x_offset;
    memoire_y[i] = y_offset;
  }

  while (1)
  {
    lv_timer_handler();

    int button_mask = ((!digitalRead(UP))<<0)|
                      ((!digitalRead(Down))<<1)|
                      ((!digitalRead(Right))<<2)|
                      ((!digitalRead(Left))<<3);

    switch (button_mask)
    {
    case 1: if(dir!=2)
    dir_2 =1;
      break;
    case 2: if(dir!=1)
     dir_2 =2;
      break;
    case 4: if(dir!=4)
    dir_2 =3;
      break;
    case 8: if(dir!=3)
    dir_2 =4;
    break;
    default:
      break;
    }

    if(abs(x_offset - x_offset_2) < vit && abs(y_offset - y_offset_2) < vit)
    {
      x_offset = x_offset_2;
      y_offset = y_offset_2;
      dir = dir_2;
    

    if (dir == 1)
    {
      y_offset_2 -= grid;
    }
    if (dir == 2)
    {
      y_offset_2 += grid;
    }
    if (dir == 3)
    {
      x_offset_2 += grid;
    }
    if (dir == 4)
    {
      x_offset_2 -= grid;
    }

    if (x_offset_2>x_max){
      x_offset_2=x_min;
      x_offset=x_min;
    }
    else if (x_offset_2<x_min){
      x_offset_2=x_max-grid;
      x_offset=x_max-grid;
    }
    if (y_offset_2>y_max){
      y_offset_2=y_min;
      y_offset=y_min;
    }
    else if (y_offset_2<y_min){
      y_offset_2=y_max;
      y_offset=y_max;
    }
  }
    if(x_offset<x_offset_2){
    x_offset += vit;}

    if(x_offset>x_offset_2){
    x_offset -= vit;}

    if(y_offset<y_offset_2){
    y_offset += vit;}

    if(y_offset>y_offset_2){
    y_offset -= vit;}


    memoire_tete = (memoire_tete - 1 + memoire_size) % memoire_size;
    memoire_x[memoire_tete] = x_offset;
    memoire_y[memoire_tete] = y_offset;
    
    if (dir != 0){
      int memoire_index = 0;
      for (int i = 0; i < tail_length - 1; i++){
        int spacing = 6 - (i/4);
        if (spacing < 3) spacing = 3;
          memoire_index += spacing;
        if(memoire_index >= memoire_size)
          memoire_index = memoire_size - 1;
        
       int tx =  memoire_x[(memoire_tete + memoire_index) % memoire_size];
       int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];
       if (abs(x_offset - tx) < 20 && abs(y_offset - ty) <20)
       {
        reset_game();
        break;
       }
      }
    }
    
    int diff_x = abs(x_offset - target_x);
    int diff_y = abs(y_offset - target_y);

    if(diff_x < 0) diff_x = -diff_x;
    if(diff_y < 0) diff_y = -diff_y;

    if (diff_x < 20 && diff_y < 20){
        if (tail_length < Max_cercle){
          lvglLock();
          lv_obj_clear_flag(tail_cercle[tail_length],LV_OBJ_FLAG_HIDDEN);
          lv_obj_set_style_bg_opa(tail_cercle[tail_length],LV_OPA_COVER,LV_STATE_DEFAULT);
          lvglUnlock();
          
          tail_length++;}

      target_pos();

      lvglLock();
      lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
      lvglUnlock();
    }

    if (dir != 0)
    {
 
      int active_tail;
      if(tail_length > Max_cercle){
        active_tail = Max_cercle;
      }
      else {
        active_tail = tail_length;
      }

      int memoire_index = 0;
      lvglLock();
      lv_obj_align (cercle,LV_ALIGN_CENTER, x_offset, y_offset);

      for (int i = 0; i < Max_cercle; i++){
        int spacing = 6 - (i/4);
        if (spacing < 3) spacing = 3;
          memoire_index += spacing;
        
        if(memoire_index >= memoire_size){
          memoire_index = memoire_size - 1;
        }

        if (i < active_tail) {

          int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
          int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

          if (x_offset == x_min && tx > 100) continue;
          if (x_offset == (x_max-grid) && tx < -100) continue;
          if (y_offset == y_min && ty > 80) continue;
          if (y_offset == y_max && ty < -80) continue;
          // if(abs(x_offset-tx)>200) continue;
          // if(abs(y_offset-ty)>200) continue;

          // lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_COVER,LV_STATE_DEFAULT);
          
          lv_obj_clear_flag(tail_cercle[i],LV_OBJ_FLAG_HIDDEN);
          lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_COVER,LV_STATE_DEFAULT);
          lv_obj_align (tail_cercle[i],LV_ALIGN_CENTER, tx, ty);
        }
        else {;
          lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_0,LV_STATE_DEFAULT);
          
        }
      }
      lvglUnlock();
    }

    // Endort la tâche pendant le temps restant par rapport au réveil,
    // ici 200ms, donc la tâche s'effectue toutes les 200ms
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50)); // toutes les 200 ms
  }
}



void  target_pos(){
  bool valid_pos = false;

  while(!valid_pos){
    target_x = random_pos(-220,200,grid);
    target_y = random_pos(-100,100,grid);

    valid_pos = true;
    
    if (target_x == x_offset_2 && target_y == y_offset_2){
      valid_pos = false;
      continue;
    }

     int active_tail;
      if(tail_length > Max_cercle){
        active_tail = Max_cercle;
      }
      else {
        active_tail = tail_length;
      }
      
      int memoire_index = 0;
    for (int i = 0; i < active_tail; i++){
        int spacing = 6 - (i/4);
        if (spacing < 3) spacing = 3;
          memoire_index += spacing;
        if(memoire_index >= memoire_size){
          memoire_index = memoire_size - 1;
        }
        int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
        int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

      int diff_x = abs(tx - target_x);
      int diff_y = abs(ty - target_y);

      if(diff_x < 0) diff_x = -diff_x;
      if(diff_y < 0) diff_y = -diff_y;
      
      if (diff_x < 40 && diff_y < 40){
        valid_pos = false;
        break;
      }
    }
  }
}

void reset_game(){
  x_offset = 0;
  y_offset = 0;
  x_offset_2 = 0;
  y_offset_2 = 0;
  dir = 0;
  dir_2 = 0;
  tail_length = 3;

  for(int i = 0; i < memoire_size; i++){
    memoire_x[i] = x_offset;
    memoire_y[i] = y_offset;
  }

  target_pos();

  lvglLock();
  lv_obj_align(cercle, LV_ALIGN_CENTER, x_offset, y_offset);
  
  for (int i = 0; i < Max_cercle;i++)
  {
    lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_0,LV_STATE_DEFAULT);
  }
  lvglUnlock();
}



#else


int main(void)
{

  return 0;
}

#endif


































#include "lvgl.h"
#include "cstdio"
#include "cstdlib"

#define UP 2
#define Down 3
#define Right 4
#define Left 5
#define Max_cercle 50

int tail_length = 3;

int grid = 40;
int vit = 5;

int memoire_tete = 0;

  int x_offset = 0;
  int y_offset = 0;

  int x_offset_2 = 0;
  int y_offset_2 = 0;

  int dir = 0;
  int dir_2 = 0;
  
  int x_max=240;
  int x_min=-240;
  int y_max=120;
  int y_min=-120;

  int memoire_size = 400;
  int memoire_x[400] = {0};
  int memoire_y[400] = {0};

  int target_x; int target_y;

void  target_pos();
int random_pos (int min_val,int max_val,int grid_size){
  int val;
  min_val = min_val/grid_size;
  max_val = max_val/grid_size;
  val = min_val +(rand()%(max_val-min_val+1));
  return val*grid_size;
}

static void event_handler(lv_event_t *e)
{
  // lv_event_code_t code = lv_event_get_code(e);

  // if(code == LV_EVENT_CLICKED) {
  //     LV_LOG_USER("Clicked");
  // }
  // else if(code == LV_EVENT_VALUE_CHANGED) {
  //     LV_LOG_USER("Toggled");
  // }
}
static lv_obj_t *cercle;
static lv_obj_t *tail_cercle[Max_cercle];
static lv_obj_t *target;

void testLvgl()
{
  lv_obj_t *screen = lv_screen_active();
  lv_obj_clear_flag(screen,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
  cercle = lv_obj_create(screen);
  lv_obj_clear_flag(cercle,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(cercle, 40, 40);
  lv_obj_set_style_radius(cercle, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(cercle, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(cercle, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(cercle, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(cercle, 0, LV_STATE_DEFAULT);
  lv_obj_align(cercle, LV_ALIGN_CENTER, 0, 0);

  for (int i = 0; i < Max_cercle;i++)
  {
    tail_cercle[i] = lv_obj_create(screen);
    lv_obj_clear_flag(tail_cercle[i],LV_OBJ_FLAG_SCROLLABLE);

    int segment_size = 38 - (i/4);

    if (segment_size < 12) segment_size = 12;

    lv_obj_set_size(tail_cercle[i],segment_size,segment_size);
    lv_obj_set_style_radius(tail_cercle[i], LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);

    int pattern =(i/3)%3;
    if (pattern==0){
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_CYAN), LV_STATE_DEFAULT);
    }
    else if (pattern==1){
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    }
    else{
      lv_obj_set_style_bg_color(tail_cercle[i], lv_palette_main(LV_PALETTE_INDIGO), LV_STATE_DEFAULT);
    }
    
    lv_obj_set_style_border_width(tail_cercle[i], 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tail_cercle[i], 0, LV_STATE_DEFAULT);

    lv_obj_set_style_bg_opa(tail_cercle[i], LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_align(tail_cercle[i], LV_ALIGN_CENTER, 0, 0);
  }


 target = lv_obj_create(screen);
  lv_obj_clear_flag(target,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(target, 20, 20);
  lv_obj_set_style_radius(target, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(target, LV_OPA_COVER, LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(target, 0, LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(target, 0, LV_STATE_DEFAULT);
  lv_obj_align(target, LV_ALIGN_CENTER, 0, 0);

}

#ifdef ARDUINO

#include "lvglDrivers.h"

// à décommenter pour tester la démo
#include "demos/lv_demos.h"

void mySetup()
{
  // à décommenter pour tester la démo
  // lv_demo_widgets();

  // Initialisations générales
  pinMode(UP, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);

  testLvgl();
}

void loop()
{
  // Inactif (pour mise en veille du processeur)
}

void myTask(void *pvParameters)
{
  // Init
  TickType_t xLastWakeTime;
  // Lecture du nombre de ticks quand la tâche débute
  xLastWakeTime = xTaskGetTickCount();

  target_pos();

  lvglLock();
  lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
  for (int i = 0; i < Max_cercle;i++)
  {
    lv_obj_clear_flag(tail_cercle[i],LV_OBJ_FLAG_HIDDEN);
  }
  lvglUnlock();

  for(int i = 0; i < memoire_size; i++){
    memoire_x[i] = x_offset;
    memoire_y[i] = y_offset;
  }

  while (1)
  {
    lv_timer_handler();

    int button_mask = ((!digitalRead(UP))<<0)|
                      ((!digitalRead(Down))<<1)|
                      ((!digitalRead(Right))<<2)|
                      ((!digitalRead(Left))<<3);

    switch (button_mask)
    {
    case 1: if(dir!=2)
    dir_2 =1;
      break;
    case 2: if(dir!=1)
     dir_2 =2;
      break;
    case 4: if(dir!=4)
    dir_2 =3;
      break;
    case 8: if(dir!=3)
    dir_2 =4;
    break;
    default:
      break;
    }

    if((x_offset == x_offset_2) && (y_offset == y_offset_2)){
      dir = dir_2;
    

    if (dir == 1)
    {
      y_offset_2 -= grid;
    }
    if (dir == 2)
    {
      y_offset_2 += grid;
    }
    if (dir == 3)
    {
      x_offset_2 += grid;
    }
    if (dir == 4)
    {
      x_offset_2 -= grid;
    }

    if (x_offset_2>x_max){
      x_offset_2=x_min;
      x_offset=x_min;
    }
    else if (x_offset_2<x_min){
      x_offset_2=x_max-grid;
      x_offset=x_max-grid;
    }
    if (y_offset_2>y_max){
      y_offset_2=y_min;
      y_offset=y_min;
    }
    else if (y_offset_2<y_min){
      y_offset_2=y_max;
      y_offset=y_max;
    }
  }
    if(x_offset<x_offset_2){
    x_offset += vit;}

    if(x_offset>x_offset_2){
    x_offset -= vit;}

    if(y_offset<y_offset_2){
    y_offset += vit;}

    if(y_offset>y_offset_2){
    y_offset -= vit;}


    // for (int i = memoire_size-1; i > 0; i--)
    // {
    //   memoire_x[i] = memoire_x[i-1];
    //   memoire_y[i] = memoire_y[i-1]; 
    // }
    // memoire_x[0] = x_offset;
    // memoire_y[0] = y_offset;

    memoire_tete = (memoire_tete - 1 + memoire_size) % memoire_size;
    memoire_x[memoire_tete] = x_offset;
    memoire_y[memoire_tete] = y_offset;

    int diff_x = abs(x_offset - target_x);
    int diff_y = abs(y_offset - target_y);

    if(diff_x < 0) diff_x = -diff_x;
    if(diff_y < 0) diff_y = -diff_y;

    if (diff_x < 20 && diff_y < 20){
        if (tail_length < Max_cercle){
          lvglLock();
          lv_obj_clear_flag(tail_cercle[tail_length],LV_OBJ_FLAG_HIDDEN);
          lv_obj_set_style_bg_opa(tail_cercle[tail_length],LV_OPA_COVER,LV_STATE_DEFAULT);
          lvglUnlock();
          
          tail_length++;}

      target_pos();

      lvglLock();
      lv_obj_align(target, LV_ALIGN_CENTER, target_x, target_y);
      lvglUnlock();
    }

    if (dir != 0)
    {
      lvglLock();
      lv_obj_align(cercle, LV_ALIGN_CENTER, x_offset, y_offset);
      lvglUnlock();
 
      int active_tail;
      if(tail_length > Max_cercle){
        active_tail = Max_cercle;
      }
      else {
        active_tail = tail_length;
      }

      int memoire_index = 0;
      lvglLock();
      lv_obj_align (cercle,LV_ALIGN_CENTER, x_offset, y_offset);

      for (int i = 0; i < Max_cercle; i++){
        int spacing = 6 - (i/4);
        if (spacing < 3) spacing = 3;
          memoire_index += spacing;
        
        if(memoire_index >= memoire_size){
          memoire_index = memoire_size - 1;
        }

        if (i < active_tail) {

          int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
          int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

          if (x_offset == x_min && tx > 100) continue;
          if (x_offset == (x_max-grid) && tx < -100) continue;
          if (y_offset == y_min && ty > 80) continue;
          if (y_offset == y_max && ty < -80) continue;
          // if(abs(x_offset-tx)>200) continue;
          // if(abs(y_offset-ty)>200) continue;

          // lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_COVER,LV_STATE_DEFAULT);
          
          lv_obj_clear_flag(tail_cercle[i],LV_OBJ_FLAG_HIDDEN);
          lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_COVER,LV_STATE_DEFAULT);
          lv_obj_align (tail_cercle[i],LV_ALIGN_CENTER, tx, ty);
        }
        else {;
          lv_obj_set_style_bg_opa(tail_cercle[i],LV_OPA_0,LV_STATE_DEFAULT);
          
        }
      }
      lvglUnlock();
    }

    // Endort la tâche pendant le temps restant par rapport au réveil,
    // ici 200ms, donc la tâche s'effectue toutes les 200ms
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50)); // toutes les 200 ms
  }
}
#else

#include "lvgl.h"
#include "app_hal.h"
#include <cstdio>

int main(void)
{
  printf("LVGL Simulator\n");
  fflush(stdout);

  lv_init();
  hal_setup();

  testLvgl();

  hal_loop();
  return 0;
}

#endif
void  target_pos(){
  bool valid_pos = false;

  while(!valid_pos){
    target_x = random_pos(-220,200,grid);
    target_y = random_pos(-100,100,grid);

    valid_pos = true;
    
    if (target_x == x_offset_2 && target_y == y_offset_2){
      valid_pos = false;
      continue;
    }

     int active_tail;
      if(tail_length > Max_cercle){
        active_tail = Max_cercle;
      }
      else {
        active_tail = tail_length;
      }
      
      int memoire_index = 0;
    for (int i = 0; i < active_tail; i++){
        int spacing = 6 - (i/4);
        if (spacing < 3) spacing = 3;
          memoire_index += spacing;
        if(memoire_index >= memoire_size){
          memoire_index = memoire_size - 1;
        }
        int tx = memoire_x[(memoire_tete + memoire_index) % memoire_size];
        int ty = memoire_y[(memoire_tete + memoire_index) % memoire_size];

      int diff_x = abs(tx - target_x);
      int diff_y = abs(ty - target_y);

      if(diff_x < 0) diff_x = -diff_x;
      if(diff_y < 0) diff_y = -diff_y;
      
      if (diff_x < 40 && diff_y < 40){
        valid_pos = false;
        break;
      }
    }
  }
}
