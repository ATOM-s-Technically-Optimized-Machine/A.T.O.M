#ifndef ENTITYHIERARCHYPANEL_HPP_
    #define ENTITYHIERARCHYPANEL_HPP_

#include "core/DefaultComponents.hpp"
#include "core/ECS/Component.hpp"
#include "core/ECS/Includes.hpp"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"

class EntityHierarchyPanel : public APanel {
    public:
        EntityHierarchyPanel(editor::Editor &editor, ECS::ECS &ecs);
        ~EntityHierarchyPanel();

        void render(int view_id) override;
        void renderEntities(const ECS::DenseComponent<HierarchyNode> &data, ECS::EntityID curr_entity);
    protected:
    private:
        ECS::ECS &m_ecs;
};

#endif /* !ENTITYHIERARCHYPANEL_HPP_ */
