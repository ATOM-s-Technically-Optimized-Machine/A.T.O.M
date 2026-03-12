/**
 * @file KeyCodes.hpp
 * @brief Key codes, mouse buttons, and modifier flag definitions for the
 *        ATOM input system.
 */

#ifndef ATOM_INPUT_KEYCODES_HPP
#define ATOM_INPUT_KEYCODES_HPP

#include <cstdint>
#include <type_traits>

namespace Atom::Input
{

/**
 * @brief Keyboard key identifiers.
 *
 * Numeric values are kept stable across builds so they can be serialised
 * and used as array indices.
 */
enum class KeyCode : uint16_t
{
    Unknown = 0, ///< No key / unrecognised key.

    /* ---- Letters (1-26) ---- */
    A = 1,  ///< Letter A.
    B = 2,  ///< Letter B.
    C = 3,  ///< Letter C.
    D = 4,  ///< Letter D.
    E = 5,  ///< Letter E.
    F = 6,  ///< Letter F.
    G = 7,  ///< Letter G.
    H = 8,  ///< Letter H.
    I = 9,  ///< Letter I.
    J = 10, ///< Letter J.
    K = 11, ///< Letter K.
    L = 12, ///< Letter L.
    M = 13, ///< Letter M.
    N = 14, ///< Letter N.
    O = 15, ///< Letter O.
    P = 16, ///< Letter P.
    Q = 17, ///< Letter Q.
    R = 18, ///< Letter R.
    S = 19, ///< Letter S.
    T = 20, ///< Letter T.
    U = 21, ///< Letter U.
    V = 22, ///< Letter V.
    W = 23, ///< Letter W.
    X = 24, ///< Letter X.
    Y = 25, ///< Letter Y.
    Z = 26, ///< Letter Z.

    /* ---- Top-row digits (27-36) ---- */
    Num0 = 27, ///< Digit 0 (top row).
    Num1 = 28, ///< Digit 1 (top row).
    Num2 = 29, ///< Digit 2 (top row).
    Num3 = 30, ///< Digit 3 (top row).
    Num4 = 31, ///< Digit 4 (top row).
    Num5 = 32, ///< Digit 5 (top row).
    Num6 = 33, ///< Digit 6 (top row).
    Num7 = 34, ///< Digit 7 (top row).
    Num8 = 35, ///< Digit 8 (top row).
    Num9 = 36, ///< Digit 9 (top row).

    /* ---- Function keys (37-48) ---- */
    F1 = 37,  ///< Function key F1.
    F2 = 38,  ///< Function key F2.
    F3 = 39,  ///< Function key F3.
    F4 = 40,  ///< Function key F4.
    F5 = 41,  ///< Function key F5.
    F6 = 42,  ///< Function key F6.
    F7 = 43,  ///< Function key F7.
    F8 = 44,  ///< Function key F8.
    F9 = 45,  ///< Function key F9.
    F10 = 46, ///< Function key F10.
    F11 = 47, ///< Function key F11.
    F12 = 48, ///< Function key F12.

    /* ---- Common keys (49-63) ---- */
    Space = 49,     ///< Space bar.
    Enter = 50,     ///< Enter / Return.
    Escape = 51,    ///< Escape.
    Tab = 52,       ///< Tab.
    Backspace = 53, ///< Backspace.
    Delete = 54,    ///< Delete.
    Insert = 55,    ///< Insert.

    /* ---- Navigation (56-63) ---- */
    Up = 56,       ///< Arrow up.
    Down = 57,     ///< Arrow down.
    Left = 58,     ///< Arrow left.
    Right = 59,    ///< Arrow right.
    Home = 60,     ///< Home.
    End = 61,      ///< End.
    PageUp = 62,   ///< Page up.
    PageDown = 63, ///< Page down.

    /* ---- Modifier keys (64-69) ---- */
    LShift = 64, ///< Left Shift.
    RShift = 65, ///< Right Shift.
    LCtrl = 66,  ///< Left Control.
    RCtrl = 67,  ///< Right Control.
    LAlt = 68,   ///< Left Alt.
    RAlt = 69,   ///< Right Alt.

    /* ---- Lock keys (70-72) ---- */
    CapsLock = 70,   ///< Caps Lock.
    NumLock = 71,    ///< Num Lock.
    ScrollLock = 72, ///< Scroll Lock.

    /* ---- Punctuation / symbols (73-83) ---- */
    Minus = 73,        ///< Minus / hyphen (-).
    Equals = 74,       ///< Equals (=).
    LeftBracket = 75,  ///< Left bracket ([).
    RightBracket = 76, ///< Right bracket (]).
    Backslash = 77,    ///< Backslash (\).
    Semicolon = 78,    ///< Semicolon (;).
    Apostrophe = 79,   ///< Apostrophe (').
    Grave = 80,        ///< Grave accent / back-tick (`).
    Comma = 81,        ///< Comma (,).
    Period = 82,       ///< Period (.).
    Slash = 83,        ///< Forward slash (/).

    /* ---- Keypad (84-99) ---- */
    KP_0 = 84,        ///< Keypad 0.
    KP_1 = 85,        ///< Keypad 1.
    KP_2 = 86,        ///< Keypad 2.
    KP_3 = 87,        ///< Keypad 3.
    KP_4 = 88,        ///< Keypad 4.
    KP_5 = 89,        ///< Keypad 5.
    KP_6 = 90,        ///< Keypad 6.
    KP_7 = 91,        ///< Keypad 7.
    KP_8 = 92,        ///< Keypad 8.
    KP_9 = 93,        ///< Keypad 9.
    KP_Decimal = 94,  ///< Keypad decimal point.
    KP_Divide = 95,   ///< Keypad divide.
    KP_Multiply = 96, ///< Keypad multiply.
    KP_Subtract = 97, ///< Keypad subtract.
    KP_Add = 98,      ///< Keypad add.
    KP_Enter = 99,    ///< Keypad enter.

    Count = 100 ///< Total number of key codes (not a valid key).
};

// ---------------------------------------------------------------------------

/**
 * @brief Mouse button identifiers.
 */
enum class MouseButton : int8_t
{
    None = -1, ///< No button.
    Left = 0,  ///< Left mouse button.
    Middle,    ///< Middle mouse button (wheel click).
    Right,     ///< Right mouse button.
    X1,        ///< Extra button 1 (side).
    X2,        ///< Extra button 2 (side).
    Count      ///< Total number of mouse buttons (not a valid button).
};

// ---------------------------------------------------------------------------

/**
 * @brief Bitwise modifier-key flags.
 *
 * Combined flags (Shift, Ctrl, Alt) cover both left and right variants.
 */
enum class Modifier : uint16_t
{
    None = 0x0000,   ///< No modifier active.
    LShift = 0x0001, ///< Left Shift held.
    RShift = 0x0002, ///< Right Shift held.
    Shift = 0x0003,  ///< Either Shift held (LShift | RShift).
    LCtrl = 0x0004,  ///< Left Control held.
    RCtrl = 0x0008,  ///< Right Control held.
    Ctrl = 0x000C,   ///< Either Control held (LCtrl | RCtrl).
    LAlt = 0x0010,   ///< Left Alt held.
    RAlt = 0x0020,   ///< Right Alt held.
    Alt = 0x0030,    ///< Either Alt held (LAlt | RAlt).
    Caps = 0x0040,   ///< Caps Lock active.
    Num = 0x0080     ///< Num Lock active.
};

// ---------------------------------------------------------------------------
// Bitwise operators for Modifier
// ---------------------------------------------------------------------------

/**
 * @brief Bitwise OR for two Modifier values.
 * @param lhs Left-hand operand.
 * @param rhs Right-hand operand.
 * @return Combined modifier flags.
 */
[[nodiscard]] constexpr Modifier operator|(Modifier lhs, Modifier rhs) noexcept
{
    using U = std::underlying_type_t<Modifier>;
    return static_cast<Modifier>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

/**
 * @brief Bitwise AND for two Modifier values.
 * @param lhs Left-hand operand.
 * @param rhs Right-hand operand.
 * @return Intersection of modifier flags.
 */
[[nodiscard]] constexpr Modifier operator&(Modifier lhs, Modifier rhs) noexcept
{
    using U = std::underlying_type_t<Modifier>;
    return static_cast<Modifier>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

/**
 * @brief Bitwise NOT (complement) for a Modifier value.
 * @param m Modifier to invert.
 * @return Inverted modifier flags.
 */
[[nodiscard]] constexpr Modifier operator~(Modifier m) noexcept
{
    using U = std::underlying_type_t<Modifier>;
    return static_cast<Modifier>(~static_cast<U>(m));
}

/**
 * @brief Bitwise OR-assign for Modifier.
 * @param lhs Modifier to update.
 * @param rhs Flags to add.
 * @return Reference to the updated modifier.
 */
constexpr Modifier &operator|=(Modifier &lhs, Modifier rhs) noexcept
{
    lhs = lhs | rhs;
    return lhs;
}

/**
 * @brief Bitwise AND-assign for Modifier.
 * @param lhs Modifier to update.
 * @param rhs Mask to apply.
 * @return Reference to the updated modifier.
 */
constexpr Modifier &operator&=(Modifier &lhs, Modifier rhs) noexcept
{
    lhs = lhs & rhs;
    return lhs;
}

// ---------------------------------------------------------------------------
// Name look-up helpers (implemented in Event.cpp)
// ---------------------------------------------------------------------------

/**
 * @brief Return a human-readable name for a keyboard key.
 * @param key The key code to look up.
 * @return Null-terminated string such as "A" or "Space". Never null.
 */
const char *keyCodeName(KeyCode key) noexcept;

/**
 * @brief Return a human-readable name for a mouse button.
 * @param button The mouse button to look up.
 * @return Null-terminated string such as "Left" or "X2". Never null.
 */
const char *mouseButtonName(MouseButton button) noexcept;

} // namespace Atom::Input

#endif // ATOM_INPUT_KEYCODES_HPP
