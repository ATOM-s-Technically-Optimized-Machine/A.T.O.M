#ifndef IPanel_HPP_
    #define IPanel_HPP_

#include <type_traits>

class IPanel;

template<typename T>
concept PanelClass = std::is_base_of_v<IPanel, T>;

class IPanel {
    public:
        virtual ~IPanel() = default;

        virtual const char *name() const = 0;
        virtual void render(int view_id) = 0;
        virtual void contextMenu(int view_id) = 0;
        virtual bool capturesMouse() const = 0;
        virtual void onViewClose(int view_id) = 0;

    protected:
    private:
};

namespace editor {
    class Editor;
};

#endif /* !IPanel_HPP_ */
