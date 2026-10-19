#ifndef ENTITYVIEWERPANEL_HPP_
    #define ENTITYVIEWERPANEL_HPP_

#include "core/ECS/Component.hpp"
#include "core/ECS/ECS.hpp"
#include "core/ECS/Includes.hpp"
#include "core/CoreUtils.hpp"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"
#include <cstdint>
#include <unordered_map>

struct ComponentDescriptor {
    std::string name;
    std::vector<FieldDescriptor> fields;
};

using InspectorFn = std::function<void(ComponentDescriptor &descriptor, void *component)>;

class InspectorRegistry {
    public:
        InspectorRegistry();
        ~InspectorRegistry();

        template<typename T>
        void registerInspector(InspectorFn fn)
        {
            m_registry[ECS::typenameId::get<T>()] = std::move(fn);
        }

        std::unordered_map<uint16_t, InspectorFn> &registry();
    private:
        std::unordered_map<uint16_t, InspectorFn> m_registry;
};

class EntityViewerPanel : public APanel {
    public:
        EntityViewerPanel(editor::Editor &editor, ECS::ECS &ecs);
        ~EntityViewerPanel();

        void render(int view_id) override;

        void setTargetEntity(ECS::EntityID e);
        void showEntityComponents();
        void displayFieldEntry(FieldDescriptor entry, int id);

        InspectorRegistry &inspector();
    protected:
    private:
        ECS::ECS &m_ecs;
        InspectorRegistry m_inspector;
        ECS::EntityID m_selected_entity = ECS::NULL_ENTITY;
        int m_cached_input = -1;
};

#endif /* !ENTITYVIEWERPANEL_HPP_ */
