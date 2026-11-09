#ifndef ITOOL_HPP_
#define ITOOL_HPP_

#include <vector>
#include <imgui.h>

#include "core/MathUtils.hpp"
#include "core/ECS/ECS.hpp"

struct ViewportContext;
struct Handle;

class ITool {
    public:
        virtual ~ITool() = default;
        
        virtual const char *name() const = 0;
        virtual ImGuiKey shortcut() const = 0;

        virtual void options() {}
        virtual void onKeys() {}

        virtual void buildHandles(const ViewportContext &, ECS::ECS &, ECS::EntityID, std::vector<Handle> &) const {}

        virtual bool beginDrag(const Handle &, const Ray &, ECS::ECS &, ECS::EntityID) { return false; }
        virtual bool updateDrag(const Ray &, ECS::ECS &, ECS::EntityID) { return false; }
};

#endif /* ITOOL_HPP_ */