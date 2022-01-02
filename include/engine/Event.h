#ifndef EVENT_H_
#define EVENT_H_

#include <queue>
#include <cassert>
enum class EventType
{
    NONE,
    WINDOW_RESIZE,
    KEY_PRESSED,
    MOUSE_BUTTON_PRESSED,
};

struct WindowEvent
{
    int width;
    int height;
};

struct KeyboardEvent
{
    int keyId;
};

struct MouseEvent
{
};

struct Event
{
    EventType type;
    union
    {
        WindowEvent windowEvent;
        KeyboardEvent keyboardEvent;
        MouseEvent mouseEvent;
    };

    Event(EventType ty, WindowEvent e);
    Event(EventType ty, KeyboardEvent e);
};

class EventQueue : public std::queue<Event>
{
public:
    void pushWindowResize(int w, int h);
    void pushKeyboardPress(int key);
};

#endif
