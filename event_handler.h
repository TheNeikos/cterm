#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H


typedef struct cterm_event_listener {
  uint8_t event_type;
  int (*listener)(xcb_generic_event_t*);
} cterm_event_listener;

typedef struct cterm_event_listener_queue {
  cterm_event_listener* event_listener;
  struct cterm_event_listener_queue* next;
  struct cterm_event_listener_queue* last;
} cterm_event_listener_queue;

void handle_event(xcb_generic_event_t*);

// This adds to a growing list of event listeners that get invoked if the
// specified event is fired. Any non nul return stops propagation.
void add_event_listener(uint8_t, int (xcb_generic_event_t*));

//Returns the correct queue for a given id
cterm_event_listener_queue* get_event_queue(uint8_t);

#endif