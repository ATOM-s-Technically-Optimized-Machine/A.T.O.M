/**
 * @file input_event.h
 * @brief Input event system for ATOM game engine
 *
 * Provides event types, queue management, and callback system for
 * keyboard, mouse, and window events.
 */

#ifndef ATOM_INPUT_EVENT_H_
#define ATOM_INPUT_EVENT_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * Key Codes
     * ============================================================================ */

    typedef enum atom_keycode
    {
        ATOM_KEY_UNKNOWN = 0,

        /* Letters */
        ATOM_KEY_A,
        ATOM_KEY_B,
        ATOM_KEY_C,
        ATOM_KEY_D,
        ATOM_KEY_E,
        ATOM_KEY_F,
        ATOM_KEY_G,
        ATOM_KEY_H,
        ATOM_KEY_I,
        ATOM_KEY_J,
        ATOM_KEY_K,
        ATOM_KEY_L,
        ATOM_KEY_M,
        ATOM_KEY_N,
        ATOM_KEY_O,
        ATOM_KEY_P,
        ATOM_KEY_Q,
        ATOM_KEY_R,
        ATOM_KEY_S,
        ATOM_KEY_T,
        ATOM_KEY_U,
        ATOM_KEY_V,
        ATOM_KEY_W,
        ATOM_KEY_X,
        ATOM_KEY_Y,
        ATOM_KEY_Z,

        /* Numbers */
        ATOM_KEY_0,
        ATOM_KEY_1,
        ATOM_KEY_2,
        ATOM_KEY_3,
        ATOM_KEY_4,
        ATOM_KEY_5,
        ATOM_KEY_6,
        ATOM_KEY_7,
        ATOM_KEY_8,
        ATOM_KEY_9,

        /* Function keys */
        ATOM_KEY_F1,
        ATOM_KEY_F2,
        ATOM_KEY_F3,
        ATOM_KEY_F4,
        ATOM_KEY_F5,
        ATOM_KEY_F6,
        ATOM_KEY_F7,
        ATOM_KEY_F8,
        ATOM_KEY_F9,
        ATOM_KEY_F10,
        ATOM_KEY_F11,
        ATOM_KEY_F12,

        /* Modifiers */
        ATOM_KEY_LSHIFT,
        ATOM_KEY_RSHIFT,
        ATOM_KEY_LCTRL,
        ATOM_KEY_RCTRL,
        ATOM_KEY_LALT,
        ATOM_KEY_RALT,

        /* Special */
        ATOM_KEY_SPACE,
        ATOM_KEY_ENTER,
        ATOM_KEY_ESCAPE,
        ATOM_KEY_TAB,
        ATOM_KEY_BACKSPACE,
        ATOM_KEY_DELETE,
        ATOM_KEY_INSERT,

        /* Navigation */
        ATOM_KEY_UP,
        ATOM_KEY_DOWN,
        ATOM_KEY_LEFT,
        ATOM_KEY_RIGHT,
        ATOM_KEY_HOME,
        ATOM_KEY_END,
        ATOM_KEY_PAGEUP,
        ATOM_KEY_PAGEDOWN,

        /* Punctuation */
        ATOM_KEY_MINUS,
        ATOM_KEY_EQUALS,
        ATOM_KEY_LEFTBRACKET,
        ATOM_KEY_RIGHTBRACKET,
        ATOM_KEY_BACKSLASH,
        ATOM_KEY_SEMICOLON,
        ATOM_KEY_APOSTROPHE,
        ATOM_KEY_GRAVE,
        ATOM_KEY_COMMA,
        ATOM_KEY_PERIOD,
        ATOM_KEY_SLASH,

        /* Numpad */
        ATOM_KEY_KP_0,
        ATOM_KEY_KP_1,
        ATOM_KEY_KP_2,
        ATOM_KEY_KP_3,
        ATOM_KEY_KP_4,
        ATOM_KEY_KP_5,
        ATOM_KEY_KP_6,
        ATOM_KEY_KP_7,
        ATOM_KEY_KP_8,
        ATOM_KEY_KP_9,
        ATOM_KEY_KP_DECIMAL,
        ATOM_KEY_KP_DIVIDE,
        ATOM_KEY_KP_MULTIPLY,
        ATOM_KEY_KP_SUBTRACT,
        ATOM_KEY_KP_ADD,
        ATOM_KEY_KP_ENTER,

        /* Lock keys */
        ATOM_KEY_CAPSLOCK,
        ATOM_KEY_NUMLOCK,
        ATOM_KEY_SCROLLLOCK,

        ATOM_KEY_COUNT
    } atom_keycode_t;

    /* ============================================================================
     * Mouse Buttons
     * ============================================================================ */

    typedef enum atom_mouse_button
    {
        ATOM_MOUSE_NONE = -1,
        ATOM_MOUSE_LEFT = 0,
        ATOM_MOUSE_MIDDLE,
        ATOM_MOUSE_RIGHT,
        ATOM_MOUSE_X1,
        ATOM_MOUSE_X2,
        ATOM_MOUSE_BUTTON_COUNT
    } atom_mouse_button_t;

    /* ============================================================================
     * Event Types
     * ============================================================================ */

    typedef enum atom_input_event_type
    {
        ATOM_EVENT_NONE = 0,

        /* Keyboard */
        ATOM_EVENT_KEY_DOWN,
        ATOM_EVENT_KEY_UP,
        ATOM_EVENT_KEY_REPEAT,

        /* Mouse */
        ATOM_EVENT_MOUSE_BUTTON_DOWN,
        ATOM_EVENT_MOUSE_BUTTON_UP,
        ATOM_EVENT_MOUSE_MOVE,
        ATOM_EVENT_MOUSE_SCROLL,

        /* Window */
        ATOM_EVENT_WINDOW_FOCUS_GAINED,
        ATOM_EVENT_WINDOW_FOCUS_LOST,
        ATOM_EVENT_WINDOW_RESIZE,
        ATOM_EVENT_WINDOW_CLOSE,

        ATOM_EVENT_TYPE_COUNT
    } atom_input_event_type_t;

    /* ============================================================================
     * Modifier Key Flags
     * ============================================================================ */

#define ATOM_MOD_NONE   0x0000
#define ATOM_MOD_LSHIFT 0x0001
#define ATOM_MOD_RSHIFT 0x0002
#define ATOM_MOD_SHIFT  (ATOM_MOD_LSHIFT | ATOM_MOD_RSHIFT)
#define ATOM_MOD_LCTRL  0x0004
#define ATOM_MOD_RCTRL  0x0008
#define ATOM_MOD_CTRL   (ATOM_MOD_LCTRL | ATOM_MOD_RCTRL)
#define ATOM_MOD_LALT   0x0010
#define ATOM_MOD_RALT   0x0020
#define ATOM_MOD_ALT    (ATOM_MOD_LALT | ATOM_MOD_RALT)
#define ATOM_MOD_CAPS   0x0040
#define ATOM_MOD_NUM    0x0080

    /* ============================================================================
     * Event Data Structures
     * ============================================================================ */

    typedef struct atom_key_event
    {
        atom_keycode_t key;
        bool is_repeat;
        uint16_t modifiers;
    } atom_key_event_t;

    typedef struct atom_mouse_button_event
    {
        atom_mouse_button_t button;
        int32_t x;
        int32_t y;
        uint8_t clicks;
    } atom_mouse_button_event_t;

    typedef struct atom_mouse_move_event
    {
        int32_t x;
        int32_t y;
        int32_t delta_x;
        int32_t delta_y;
    } atom_mouse_move_event_t;

    typedef struct atom_mouse_scroll_event
    {
        float delta_x;
        float delta_y;
    } atom_mouse_scroll_event_t;

    typedef struct atom_window_resize_event
    {
        int32_t width;
        int32_t height;
    } atom_window_resize_event_t;

    /* ============================================================================
     * Unified Input Event
     * ============================================================================ */

    typedef struct atom_input_event
    {
        atom_input_event_type_t type;
        uint64_t timestamp_ms;

        union
        {
            atom_key_event_t key;
            atom_mouse_button_event_t mouse_button;
            atom_mouse_move_event_t mouse_move;
            atom_mouse_scroll_event_t mouse_scroll;
            atom_window_resize_event_t window_resize;
        } data;
    } atom_input_event_t;

    /* ============================================================================
     * Event Queue (Circular Buffer)
     * ============================================================================ */

#define ATOM_EVENT_QUEUE_SIZE 256

    typedef struct atom_event_queue
    {
        atom_input_event_t events[ATOM_EVENT_QUEUE_SIZE];
        uint32_t head;
        uint32_t tail;
        uint32_t count;
    } atom_event_queue_t;

    /* Event queue API */
    void atom_event_queue_init(atom_event_queue_t *queue);
    void atom_event_queue_clear(atom_event_queue_t *queue);
    bool atom_event_queue_push(atom_event_queue_t *queue, const atom_input_event_t *event);
    bool atom_event_queue_pop(atom_event_queue_t *queue, atom_input_event_t *event);
    bool atom_event_queue_peek(const atom_event_queue_t *queue, atom_input_event_t *event);
    bool atom_event_queue_is_empty(const atom_event_queue_t *queue);
    bool atom_event_queue_is_full(const atom_event_queue_t *queue);
    uint32_t atom_event_queue_count(const atom_event_queue_t *queue);

    /* ============================================================================
     * Event Callback System
     * ============================================================================ */

    typedef void (*atom_event_callback_t)(const atom_input_event_t *event, void *user_data);

    typedef struct atom_event_listener
    {
        atom_input_event_type_t type;
        atom_event_callback_t callback;
        void *user_data;
        bool active;
    } atom_event_listener_t;

    /* ============================================================================
     * Listener Manager
     * ============================================================================ */

#define ATOM_MAX_LISTENERS 64

    typedef struct atom_listener_manager
    {
        atom_event_listener_t listeners[ATOM_MAX_LISTENERS];
        uint32_t count;
    } atom_listener_manager_t;

    /* Listener manager API */
    void atom_listener_manager_init(atom_listener_manager_t *manager);
    void atom_listener_manager_clear(atom_listener_manager_t *manager);

    bool atom_listener_add(atom_listener_manager_t *manager, atom_input_event_type_t type,
                           atom_event_callback_t callback, void *user_data);

    bool atom_listener_remove(atom_listener_manager_t *manager, atom_event_callback_t callback);

    bool atom_listener_remove_all_for_type(atom_listener_manager_t *manager,
                                           atom_input_event_type_t type);

    void atom_listener_dispatch(atom_listener_manager_t *manager, const atom_input_event_t *event);

    /* ============================================================================
     * Utility Functions
     * ============================================================================ */

    /* Get human-readable name for a key */
    const char *atom_keycode_name(atom_keycode_t key);

    /* Get human-readable name for a mouse button */
    const char *atom_mouse_button_name(atom_mouse_button_t button);

    /* Get human-readable name for an event type */
    const char *atom_event_type_name(atom_input_event_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* !ATOM_INPUT_EVENT_H_ */
