/*
** EPITECH PROJECT, 2026
** main
** File description:
** IWindow
*/

#ifndef IWINDOW_HPP_
    #define IWINDOW_HPP_

#include <concepts>
#include <type_traits>

class IWindow;

template<typename T>
concept WindowClass = std::is_base_of_v<IWindow, T>;

class IWindow {
    public:
        virtual ~IWindow() = default;

        virtual const char *name() const = 0;
        virtual void render() = 0;
        virtual void contextMenu() = 0;

    protected:
    private:
};

#endif /* !IWINDOW_HPP_ */
