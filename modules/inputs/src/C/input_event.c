/**
 * @file input_event.c
 * @brief Implementation of input event system
 */

#include "inputs/input_event.h"

#include <string.h>

/* ============================================================================
 * Event Queue Implementation
 * ============================================================================ */

void atom_event_queue_init(atom_event_queue_t *queue)
{
    if (!queue)
        return;
    memset(queue, 0, sizeof(atom_event_queue_t));
}

void atom_event_queue_clear(atom_event_queue_t *queue)
{
    if (!queue)
        return;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
}

bool atom_event_queue_push(atom_event_queue_t *queue, const atom_input_event_t *event)
{
    if (!queue || !event)
        return false;

    if (atom_event_queue_is_full(queue))
    {
        return false; /* Queue is full, drop event */
    }

    queue->events[queue->tail] = *event;
    queue->tail = (queue->tail + 1) % ATOM_EVENT_QUEUE_SIZE;
    queue->count++;

    return true;
}

bool atom_event_queue_pop(atom_event_queue_t *queue, atom_input_event_t *event)
{
    if (!queue)
        return false;

    if (atom_event_queue_is_empty(queue))
    {
        return false;
    }

    if (event)
    {
        *event = queue->events[queue->head];
    }
    queue->head = (queue->head + 1) % ATOM_EVENT_QUEUE_SIZE;
    queue->count--;

    return true;
}

bool atom_event_queue_peek(const atom_event_queue_t *queue, atom_input_event_t *event)
{
    if (!queue || !event)
        return false;

    if (atom_event_queue_is_empty(queue))
    {
        return false;
    }

    *event = queue->events[queue->head];
    return true;
}

bool atom_event_queue_is_empty(const atom_event_queue_t *queue)
{
    return !queue || queue->count == 0;
}

bool atom_event_queue_is_full(const atom_event_queue_t *queue)
{
    return queue && queue->count >= ATOM_EVENT_QUEUE_SIZE;
}

uint32_t atom_event_queue_count(const atom_event_queue_t *queue)
{
    return queue ? queue->count : 0;
}

/* ============================================================================
 * Listener Manager Implementation
 * ============================================================================ */

void atom_listener_manager_init(atom_listener_manager_t *manager)
{
    if (!manager)
        return;
    memset(manager, 0, sizeof(atom_listener_manager_t));
}

void atom_listener_manager_clear(atom_listener_manager_t *manager)
{
    if (!manager)
        return;
    memset(manager->listeners, 0, sizeof(manager->listeners));
    manager->count = 0;
}

bool atom_listener_add(atom_listener_manager_t *manager, atom_input_event_type_t type,
                       atom_event_callback_t callback, void *user_data)
{
    if (!manager || !callback)
        return false;

    /* Find empty slot */
    for (uint32_t i = 0; i < ATOM_MAX_LISTENERS; i++)
    {
        if (!manager->listeners[i].active)
        {
            manager->listeners[i].type = type;
            manager->listeners[i].callback = callback;
            manager->listeners[i].user_data = user_data;
            manager->listeners[i].active = true;
            manager->count++;
            return true;
        }
    }

    return false; /* No free slots */
}

bool atom_listener_remove(atom_listener_manager_t *manager, atom_event_callback_t callback)
{
    if (!manager || !callback)
        return false;

    bool removed = false;
    for (uint32_t i = 0; i < ATOM_MAX_LISTENERS; i++)
    {
        if (manager->listeners[i].active && manager->listeners[i].callback == callback)
        {
            manager->listeners[i].active = false;
            manager->listeners[i].callback = NULL;
            manager->listeners[i].user_data = NULL;
            manager->count--;
            removed = true;
        }
    }

    return removed;
}

bool atom_listener_remove_all_for_type(atom_listener_manager_t *manager,
                                       atom_input_event_type_t type)
{
    if (!manager)
        return false;

    bool removed = false;
    for (uint32_t i = 0; i < ATOM_MAX_LISTENERS; i++)
    {
        if (manager->listeners[i].active && manager->listeners[i].type == type)
        {
            manager->listeners[i].active = false;
            manager->listeners[i].callback = NULL;
            manager->listeners[i].user_data = NULL;
            manager->count--;
            removed = true;
        }
    }

    return removed;
}

void atom_listener_dispatch(atom_listener_manager_t *manager, const atom_input_event_t *event)
{
    if (!manager || !event)
        return;

    for (uint32_t i = 0; i < ATOM_MAX_LISTENERS; i++)
    {
        atom_event_listener_t *listener = &manager->listeners[i];

        if (listener->active && listener->type == event->type && listener->callback)
        {
            listener->callback(event, listener->user_data);
        }
    }
}

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

const char *atom_keycode_name(atom_keycode_t key)
{
    static const char *names[] = {
        [ATOM_KEY_UNKNOWN] = "Unknown",
        [ATOM_KEY_A] = "A",
        [ATOM_KEY_B] = "B",
        [ATOM_KEY_C] = "C",
        [ATOM_KEY_D] = "D",
        [ATOM_KEY_E] = "E",
        [ATOM_KEY_F] = "F",
        [ATOM_KEY_G] = "G",
        [ATOM_KEY_H] = "H",
        [ATOM_KEY_I] = "I",
        [ATOM_KEY_J] = "J",
        [ATOM_KEY_K] = "K",
        [ATOM_KEY_L] = "L",
        [ATOM_KEY_M] = "M",
        [ATOM_KEY_N] = "N",
        [ATOM_KEY_O] = "O",
        [ATOM_KEY_P] = "P",
        [ATOM_KEY_Q] = "Q",
        [ATOM_KEY_R] = "R",
        [ATOM_KEY_S] = "S",
        [ATOM_KEY_T] = "T",
        [ATOM_KEY_U] = "U",
        [ATOM_KEY_V] = "V",
        [ATOM_KEY_W] = "W",
        [ATOM_KEY_X] = "X",
        [ATOM_KEY_Y] = "Y",
        [ATOM_KEY_Z] = "Z",
        [ATOM_KEY_0] = "0",
        [ATOM_KEY_1] = "1",
        [ATOM_KEY_2] = "2",
        [ATOM_KEY_3] = "3",
        [ATOM_KEY_4] = "4",
        [ATOM_KEY_5] = "5",
        [ATOM_KEY_6] = "6",
        [ATOM_KEY_7] = "7",
        [ATOM_KEY_8] = "8",
        [ATOM_KEY_9] = "9",
        [ATOM_KEY_F1] = "F1",
        [ATOM_KEY_F2] = "F2",
        [ATOM_KEY_F3] = "F3",
        [ATOM_KEY_F4] = "F4",
        [ATOM_KEY_F5] = "F5",
        [ATOM_KEY_F6] = "F6",
        [ATOM_KEY_F7] = "F7",
        [ATOM_KEY_F8] = "F8",
        [ATOM_KEY_F9] = "F9",
        [ATOM_KEY_F10] = "F10",
        [ATOM_KEY_F11] = "F11",
        [ATOM_KEY_F12] = "F12",
        [ATOM_KEY_LSHIFT] = "Left Shift",
        [ATOM_KEY_RSHIFT] = "Right Shift",
        [ATOM_KEY_LCTRL] = "Left Ctrl",
        [ATOM_KEY_RCTRL] = "Right Ctrl",
        [ATOM_KEY_LALT] = "Left Alt",
        [ATOM_KEY_RALT] = "Right Alt",
        [ATOM_KEY_SPACE] = "Space",
        [ATOM_KEY_ENTER] = "Enter",
        [ATOM_KEY_ESCAPE] = "Escape",
        [ATOM_KEY_TAB] = "Tab",
        [ATOM_KEY_BACKSPACE] = "Backspace",
        [ATOM_KEY_DELETE] = "Delete",
        [ATOM_KEY_INSERT] = "Insert",
        [ATOM_KEY_UP] = "Up",
        [ATOM_KEY_DOWN] = "Down",
        [ATOM_KEY_LEFT] = "Left",
        [ATOM_KEY_RIGHT] = "Right",
        [ATOM_KEY_HOME] = "Home",
        [ATOM_KEY_END] = "End",
        [ATOM_KEY_PAGEUP] = "Page Up",
        [ATOM_KEY_PAGEDOWN] = "Page Down",
        [ATOM_KEY_CAPSLOCK] = "Caps Lock",
        [ATOM_KEY_NUMLOCK] = "Num Lock",
        [ATOM_KEY_SCROLLLOCK] = "Scroll Lock",
    };

    if (key >= 0 && key < ATOM_KEY_COUNT && names[key])
    {
        return names[key];
    }
    return "Unknown";
}

const char *atom_mouse_button_name(atom_mouse_button_t button)
{
    switch (button)
    {
    case ATOM_MOUSE_LEFT:
        return "Left";
    case ATOM_MOUSE_MIDDLE:
        return "Middle";
    case ATOM_MOUSE_RIGHT:
        return "Right";
    case ATOM_MOUSE_X1:
        return "X1";
    case ATOM_MOUSE_X2:
        return "X2";
    default:
        return "Unknown";
    }
}

const char *atom_event_type_name(atom_input_event_type_t type)
{
    switch (type)
    {
    case ATOM_EVENT_NONE:
        return "None";
    case ATOM_EVENT_KEY_DOWN:
        return "Key Down";
    case ATOM_EVENT_KEY_UP:
        return "Key Up";
    case ATOM_EVENT_KEY_REPEAT:
        return "Key Repeat";
    case ATOM_EVENT_MOUSE_BUTTON_DOWN:
        return "Mouse Button Down";
    case ATOM_EVENT_MOUSE_BUTTON_UP:
        return "Mouse Button Up";
    case ATOM_EVENT_MOUSE_MOVE:
        return "Mouse Move";
    case ATOM_EVENT_MOUSE_SCROLL:
        return "Mouse Scroll";
    case ATOM_EVENT_WINDOW_FOCUS_GAINED:
        return "Window Focus Gained";
    case ATOM_EVENT_WINDOW_FOCUS_LOST:
        return "Window Focus Lost";
    case ATOM_EVENT_WINDOW_RESIZE:
        return "Window Resize";
    case ATOM_EVENT_WINDOW_CLOSE:
        return "Window Close";
    default:
        return "Unknown";
    }
}
