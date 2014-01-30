/* Simple XCB application drawing a box in a window */
/* to compile it use :
 gcc -Wall x.conn -lxcb
*/
#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>

#include "keycodes.h"
#include "event_handler.c"
 
int close_window(xcb_generic_event_t*);
int debug_keypress(xcb_generic_event_t*);
int done = 0;
int main(void)
{

  xcb_connection_t    *conn;
  xcb_screen_t        *screen;
  xcb_window_t         win;
  xcb_gcontext_t       gcontext;
  xcb_generic_event_t *event;
  uint32_t             mask;
  uint32_t             values[2];
  xcb_rectangle_t      r = { 20, 20, 600, 60 };
 
                        /* open connection with the server */
  conn = xcb_connect(NULL,NULL);
  if (xcb_connection_has_error(conn)) {
    printf("Cannot open display\n");
    exit(1);
  }
                        /* get the first screen */
  screen = xcb_setup_roots_iterator( xcb_get_setup(conn) ).data;
 
                       /* create black graphics gcontext */
  gcontext = xcb_generate_id(conn);
  win = screen->root;
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = 0;
  xcb_create_gc(conn, gcontext, win, mask, values);
 
                       /* create window */
  win = xcb_generate_id(conn);
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE 
              | XCB_EVENT_MASK_KEY_PRESS
              | XCB_EVENT_MASK_KEY_RELEASE;
  xcb_create_window(conn, screen->root_depth, win, screen->root,
                    10, 10, 100, 100, 1,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
                    mask, values);
 
                        /* map (show) the window */
  xcb_map_window(conn, win);
 
  xcb_flush(conn);

  add_event_listener(XCB_KEY_PRESS, debug_keypress);
  add_event_listener(XCB_KEY_PRESS, close_window);
 
                        /* event loop */
  while (!done) {
    event = xcb_poll_for_event(conn);
    if(event == NULL) continue;
    handle_event(event);
    free(event);
  }
                        /* close connection to server */
  xcb_disconnect(conn);
 
  return 0;
}


int close_window(xcb_generic_event_t* event)
{
  xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;
  if(bp->detail == KEYCODE_Q)
  {
    done = 1;
    return 1;
  }
  return 0;
}

int debug_keypress(xcb_generic_event_t* event)
{
  xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;
  printf("Keypress: %i\n", bp->detail);
  return 0;
}