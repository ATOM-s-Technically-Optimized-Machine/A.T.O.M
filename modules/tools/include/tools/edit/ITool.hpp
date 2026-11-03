#ifndef ITOOL_HPP_
#define ITOOL_HPP_

#include <imgui.h>

class ITool {
    public:
        virtual ~ITool() = default;
        
        virtual const char *name() const = 0;
        virtual ImGuiKey shortcut() const = 0;

        virtual void options() {}
        virtual void onKeys() {}
};

#endif /* ITOOL_HPP_ */