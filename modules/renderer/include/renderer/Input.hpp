#ifndef INPUT_HPP_
    #define INPUT_HPP_

enum Key { W, A, S, D, Space, LShift, Escape, PlusSign, MinusSign, Enter, _Count};

struct InputState {
    bool keys[(int)Key::_Count] = {};
    bool last_keys[(int)Key::_Count] = {};
    float mouseDx = 0.0f;
    float mouseDy = 0.0f;

    bool isPressed(Key k) const { return last_keys[(int)k] && !keys[(int)k]; }
    bool isDown(Key k) const { return keys[(int)k]; }
    bool isUp(Key k) const { return !keys[(int)k]; }
};

class IInputProvider {
    public:
        virtual ~IInputProvider() = default;
        virtual void fetch(InputState &state) = 0;

    protected:
    private:
};

#endif /* !INPUT_HPP_ */
