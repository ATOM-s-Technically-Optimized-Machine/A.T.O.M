#ifndef ITOOL_HPP_
#define ITOOL_HPP_

#include <vector>
#include <imgui.h>

struct ViewportContext;
struct TransformComponent;
struct Handle;

class ITool {
    public:
        virtual ~ITool() = default;
        
        virtual const char *name() const = 0;
        virtual ImGuiKey shortcut() const = 0;

        virtual void options() {}
        virtual void onKeys() {}

        virtual void buildHandles(const ViewportContext &, const TransformComponent &, std::vector<Handle> &) const {}
};

#endif /* ITOOL_HPP_ */