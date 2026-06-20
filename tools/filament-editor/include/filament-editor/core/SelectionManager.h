#pragma once

#include "EditorTypes.h"

#include <functional>
#include <vector>

namespace filament::editor {

class SelectionManager {
public:
    using Callback = std::function<void()>;

    void select(EntityID entity);
    void deselect(EntityID entity);
    void toggle(EntityID entity);
    void clear();

    bool isSelected(EntityID entity) const;
    bool empty() const;
    size_t count() const;

    EntityID getPrimary() const;
    const std::vector<EntityID>& getSelection() const { return mSelection; }

    void setOnChanged(Callback cb) { mOnChanged = std::move(cb); }

private:
    void notify();

    std::vector<EntityID> mSelection;
    Callback mOnChanged;
};

} // namespace filament::editor
