#include <math.h>
#include "event_handler.h"

void cterm_handle_event(xcb_generic_event_t* event)
{
  cterm_event_listener_queue* queue = cterm_get_event_queue(event->response_type);
  for(int i = 0; i < queue->count; i++ )
  {
    printf("%p\n", queue->event_listeners[i]);
    if(queue->event_listeners[i]->listener(event))
      return; // On non nul we exit
  }
}

void cterm_add_event_listener(uint8_t type, int (cb)(xcb_generic_event_t*))
{
  cterm_event_listener* new_listener = calloc(1, sizeof(cterm_event_listener));
  cterm_event_listener_queue* queue = cterm_get_event_queue(type);
  new_listener->event_type = type;
  new_listener->listener = cb;
  
  if(queue->max_size < queue->count + 1)
  {
    queue->max_size += 0x1<<3;
    queue->event_listeners = realloc(queue->event_listeners, queue->max_size * sizeof(cterm_event_listener_queue));
  }

  queue->event_listeners[queue->count++] = new_listener;
}

cterm_event_listener_queue* cterm_get_event_queue(uint8_t type){
  if(type < 0) return NULL;
  static cterm_event_listener_queue* queues;
  if(queues == NULL)
  {
    queues = (cterm_event_listener_queue*) calloc(128, sizeof(cterm_event_listener_queue));
  }
  return queues + type; //We are exploiting the fact that the list of events are multiples of 2;
}

void cterm_free_event_handlers()
{
  cterm_event_listener_queue* queue = cterm_get_event_queue(0);
  for(int i = 0; i<128;i++){
    for(int j = 0; j<queue->count;j++)
    {
      free(queue->event_listeners[j]);
    }
    free(queue->event_listeners);
    queue += 1;
  }
  free(cterm_get_event_queue(1)-1);

}