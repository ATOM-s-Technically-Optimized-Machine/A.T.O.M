/**
 * @file test_event.c
 * @brief Unit tests for input_event: event queue, listeners, utility functions
 */

#include "test_helpers.h"
#include <inputs/input_event.h>

/* ========================================================================== *
 * Event Queue Tests
 * ========================================================================== */

static void test_queue_init(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);
    ASSERT_TRUE(atom_event_queue_is_empty(&queue));
    ASSERT_FALSE(atom_event_queue_is_full(&queue));
    ASSERT_EQ(atom_event_queue_count(&queue), 0u);
}

static void test_queue_push_pop(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_DOWN;
    event.timestamp_ms = 1000;
    event.data.key.key = ATOM_KEY_A;

    ASSERT_TRUE(atom_event_queue_push(&queue, &event));
    ASSERT_EQ(atom_event_queue_count(&queue), 1u);
    ASSERT_FALSE(atom_event_queue_is_empty(&queue));

    atom_input_event_t popped = {0};
    ASSERT_TRUE(atom_event_queue_pop(&queue, &popped));
    ASSERT_EQ(popped.type, ATOM_EVENT_KEY_DOWN);
    ASSERT_EQ(popped.data.key.key, ATOM_KEY_A);
    ASSERT_EQ(popped.timestamp_ms, 1000u);
    ASSERT_TRUE(atom_event_queue_is_empty(&queue));
}

static void test_queue_peek(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_UP;
    atom_event_queue_push(&queue, &event);

    atom_input_event_t peeked = {0};
    ASSERT_TRUE(atom_event_queue_peek(&queue, &peeked));
    ASSERT_EQ(peeked.type, ATOM_EVENT_KEY_UP);
    /* Peek should not remove the event */
    ASSERT_EQ(atom_event_queue_count(&queue), 1u);
}

static void test_queue_fifo_order(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    /* Push events with different keys */
    for (int i = 0; i < 10; i++)
    {
        atom_input_event_t event = {0};
        event.type = ATOM_EVENT_KEY_DOWN;
        event.data.key.key = (atom_keycode_t)(ATOM_KEY_A + i);
        atom_event_queue_push(&queue, &event);
    }

    /* Pop should return in FIFO order */
    for (int i = 0; i < 10; i++)
    {
        atom_input_event_t popped = {0};
        ASSERT_TRUE(atom_event_queue_pop(&queue, &popped));
        ASSERT_EQ(popped.data.key.key, (atom_keycode_t)(ATOM_KEY_A + i));
    }
}

static void test_queue_full(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    /* Fill the queue completely */
    for (int i = 0; i < ATOM_EVENT_QUEUE_SIZE; i++)
    {
        atom_input_event_t event = {0};
        event.type = ATOM_EVENT_KEY_DOWN;
        ASSERT_TRUE(atom_event_queue_push(&queue, &event));
    }

    ASSERT_TRUE(atom_event_queue_is_full(&queue));
    ASSERT_EQ(atom_event_queue_count(&queue), (uint32_t)ATOM_EVENT_QUEUE_SIZE);

    /* Next push should fail */
    atom_input_event_t overflow = {0};
    ASSERT_FALSE(atom_event_queue_push(&queue, &overflow));
}

static void test_queue_circular_wrap(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    /* Push and pop many times to wrap around */
    for (int round = 0; round < 3; round++)
    {
        for (int i = 0; i < ATOM_EVENT_QUEUE_SIZE; i++)
        {
            atom_input_event_t event = {0};
            event.type = ATOM_EVENT_KEY_DOWN;
            event.data.key.key = (atom_keycode_t)(i % ATOM_KEY_COUNT);
            ASSERT_TRUE(atom_event_queue_push(&queue, &event));
        }

        for (int i = 0; i < ATOM_EVENT_QUEUE_SIZE; i++)
        {
            atom_input_event_t popped = {0};
            ASSERT_TRUE(atom_event_queue_pop(&queue, &popped));
        }

        ASSERT_TRUE(atom_event_queue_is_empty(&queue));
    }
}

static void test_queue_pop_empty(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    atom_input_event_t event = {0};
    ASSERT_FALSE(atom_event_queue_pop(&queue, &event));
}

static void test_queue_clear(void)
{
    atom_event_queue_t queue;
    atom_event_queue_init(&queue);

    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_DOWN;
    atom_event_queue_push(&queue, &event);
    atom_event_queue_push(&queue, &event);

    atom_event_queue_clear(&queue);
    ASSERT_TRUE(atom_event_queue_is_empty(&queue));
    ASSERT_EQ(atom_event_queue_count(&queue), 0u);
}

/* ========================================================================== *
 * Listener Tests
 * ========================================================================== */

static int g_callback_count = 0;
static atom_input_event_type_t g_last_event_type = ATOM_EVENT_NONE;

static void test_callback(const atom_input_event_t *event, void *user_data)
{
    (void)user_data;
    g_callback_count++;
    g_last_event_type = event->type;
}

static void test_callback2(const atom_input_event_t *event, void *user_data)
{
    (void)event;
    int *counter = (int *)user_data;
    (*counter)++;
}

static void test_listener_add_dispatch(void)
{
    atom_listener_manager_t mgr;
    atom_listener_manager_init(&mgr);

    g_callback_count = 0;
    g_last_event_type = ATOM_EVENT_NONE;

    ASSERT_TRUE(atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL));

    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_DOWN;
    atom_listener_dispatch(&mgr, &event);

    ASSERT_EQ(g_callback_count, 1);
    ASSERT_EQ(g_last_event_type, ATOM_EVENT_KEY_DOWN);
}

static void test_listener_type_filter(void)
{
    atom_listener_manager_t mgr;
    atom_listener_manager_init(&mgr);

    g_callback_count = 0;
    atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL);

    /* Dispatch a KEY_UP event - should NOT trigger the KEY_DOWN listener */
    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_UP;
    atom_listener_dispatch(&mgr, &event);

    ASSERT_EQ(g_callback_count, 0);
}

static void test_listener_remove(void)
{
    atom_listener_manager_t mgr;
    atom_listener_manager_init(&mgr);

    g_callback_count = 0;
    atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL);
    ASSERT_TRUE(atom_listener_remove(&mgr, test_callback));

    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_DOWN;
    atom_listener_dispatch(&mgr, &event);

    ASSERT_EQ(g_callback_count, 0);
}

static void test_listener_remove_all_for_type(void)
{
    atom_listener_manager_t mgr;
    atom_listener_manager_init(&mgr);

    g_callback_count = 0;
    atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL);
    atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL);
    atom_listener_add(&mgr, ATOM_EVENT_KEY_UP, test_callback, NULL);

    ASSERT_TRUE(atom_listener_remove_all_for_type(&mgr, ATOM_EVENT_KEY_DOWN));

    atom_input_event_t down = {0};
    down.type = ATOM_EVENT_KEY_DOWN;
    atom_listener_dispatch(&mgr, &down);
    ASSERT_EQ(g_callback_count, 0);

    /* KEY_UP listener should still work */
    atom_input_event_t up = {0};
    up.type = ATOM_EVENT_KEY_UP;
    atom_listener_dispatch(&mgr, &up);
    ASSERT_EQ(g_callback_count, 1);
}

static void test_listener_user_data(void)
{
    atom_listener_manager_t mgr;
    atom_listener_manager_init(&mgr);

    int counter = 0;
    atom_listener_add(&mgr, ATOM_EVENT_MOUSE_MOVE, test_callback2, &counter);

    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_MOUSE_MOVE;
    atom_listener_dispatch(&mgr, &event);
    atom_listener_dispatch(&mgr, &event);

    ASSERT_EQ(counter, 2);
}

static void test_listener_max_capacity(void)
{
    atom_listener_manager_t mgr;
    atom_listener_manager_init(&mgr);

    /* Fill all listener slots */
    for (int i = 0; i < ATOM_MAX_LISTENERS; i++)
    {
        ASSERT_TRUE(atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL));
    }

    /* Next add should fail */
    ASSERT_FALSE(atom_listener_add(&mgr, ATOM_EVENT_KEY_DOWN, test_callback, NULL));
}

/* ========================================================================== *
 * Utility Tests
 * ========================================================================== */

static void test_keycode_name(void)
{
    const char *name = atom_keycode_name(ATOM_KEY_A);
    ASSERT_TRUE(name != NULL);
    ASSERT_TRUE(strlen(name) > 0);

    const char *space_name = atom_keycode_name(ATOM_KEY_SPACE);
    ASSERT_TRUE(space_name != NULL);
}

static void test_mouse_button_name(void)
{
    const char *name = atom_mouse_button_name(ATOM_MOUSE_LEFT);
    ASSERT_TRUE(name != NULL);
    ASSERT_TRUE(strlen(name) > 0);
}

static void test_event_type_name(void)
{
    const char *name = atom_event_type_name(ATOM_EVENT_KEY_DOWN);
    ASSERT_TRUE(name != NULL);
    ASSERT_TRUE(strlen(name) > 0);
}

/* ========================================================================== *
 * Main
 * ========================================================================== */

int main(void)
{
    printf("=== input_event tests ===\n\n");

    printf("[Event Queue]\n");
    RUN_TEST(test_queue_init);
    RUN_TEST(test_queue_push_pop);
    RUN_TEST(test_queue_peek);
    RUN_TEST(test_queue_fifo_order);
    RUN_TEST(test_queue_full);
    RUN_TEST(test_queue_circular_wrap);
    RUN_TEST(test_queue_pop_empty);
    RUN_TEST(test_queue_clear);

    printf("\n[Listeners]\n");
    RUN_TEST(test_listener_add_dispatch);
    RUN_TEST(test_listener_type_filter);
    RUN_TEST(test_listener_remove);
    RUN_TEST(test_listener_remove_all_for_type);
    RUN_TEST(test_listener_user_data);
    RUN_TEST(test_listener_max_capacity);

    printf("\n[Utilities]\n");
    RUN_TEST(test_keycode_name);
    RUN_TEST(test_mouse_button_name);
    RUN_TEST(test_event_type_name);

    TEST_SUMMARY();
}
