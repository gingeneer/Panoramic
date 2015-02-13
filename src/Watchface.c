#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;

static GFont s_time_font;
static GFont s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void bluetooth_connection_callback(bool connected) {
  if(!connected)
    vibes_double_pulse();
  else
    vibes_long_pulse();
}

static void update_time() {
  //get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //create a long-lived buffer
  static char time_text[] = "00:00";
  
  //write the current houres and minutes unto buffer
  if(clock_is_24h_style() == true) {
    //use 24h time
    strftime(time_text, sizeof("00:00"), "%H:%M", tick_time);
  }
  else {
    //12h time
    strftime(time_text, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  //display time on textlayer
  text_layer_set_text(s_time_layer, time_text);
  
  //update date
   static char date_text[] = "XXX 00 XXX";
  
  strftime(date_text, sizeof(date_text), "%a %e %b", tick_time);
  text_layer_set_text(s_date_layer, date_text);
}

static void tick_handler(struct tm *ticktime, TimeUnits units_changed){
  update_time();
}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  s_time_layer = text_layer_create(GRect(0, 80, 144, 58));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_47));
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  s_date_layer = text_layer_create(GRect(0, 140, 144, 28));
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_20));
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
}

static void main_window_unload(Window *window) {
  // Destroy textlayers
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  //display time at start
  update_time();
  
  //register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);
}

static void deinit() {
  bluetooth_connection_service_unsubscribe();
  // Destroy Window
  window_destroy(s_main_window);

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}