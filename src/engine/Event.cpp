#include <engine/Event.h>

Event::Event(EventType ty, WindowEvent e)
{
    assert(ty == EventType::WINDOW_RESIZE);
    type = ty;
    windowEvent = e;
}

Event::Event(EventType ty, KeyboardEvent e)
{
    assert(ty == EventType::KEY_PRESSED);
    type = ty;
    keyboardEvent = e;
}

void EventQueue::pushWindowResize(int w, int h)
{
    push(Event(EventType::WINDOW_RESIZE, WindowEvent{w, h}));
}

void EventQueue::pushKeyboardPress(int key)
{
    push(Event(EventType::KEY_PRESSED, KeyboardEvent{key}));
}