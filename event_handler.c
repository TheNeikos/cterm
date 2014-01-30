#include <math.h>
#include "event_handler.h"

void handle_event(xcb_generic_event_t* event)
{
  for(cterm_event_listener_queue* queue = get_event_queue(event->response_type); queue; queue = queue->next)
  {
    if(queue->event_listener == NULL)
      return;
    if(queue->event_listener->listener(event))
      return; // On non nul we exit
    if(queue->next == NULL)
      return;
  }
}

void add_event_listener(uint8_t type, int (cb)(xcb_generic_event_t*))
{
  cterm_event_listener* new_listener = calloc(1, sizeof(cterm_event_listener));
  new_listener->event_type = type;
  new_listener->listener = cb;

  cterm_event_listener_queue* last_queue = get_event_queue(type);

  if(last_queue->last) //Last is only not set if there are no next ones.
  {
    cterm_event_listener_queue* next_queue = calloc(1, sizeof(cterm_event_listener_queue));
    last_queue->last->next = next_queue;
    last_queue->last = next_queue;
    next_queue->event_listener = new_listener;
  }else //We are the first one!
  {
    last_queue->last = last_queue;
    last_queue->event_listener = new_listener;

  }
}

cterm_event_listener_queue* get_event_queue(uint8_t type){
  if(type < 1) return NULL;
  static cterm_event_listener_queue* queues;
  if(queues == NULL)
  {
    queues = (cterm_event_listener_queue*) calloc(25, sizeof(cterm_event_listener_queue));
  }
  return queues + (int) log2(type); //We are exploiting the fact that the list of events are multiples of 2;
}