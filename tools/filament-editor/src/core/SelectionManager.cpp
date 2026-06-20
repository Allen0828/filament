#include <filament-editor/core/SelectionManager.h>

#include <algorithm>

namespace filament::editor {

void SelectionManager::select(EntityID entity) {
    if (!entity) return;
    auto it = std::find(mSelection.begin(), mSelection.end(), entity);
    if (it != mSelection.end()) return;
    mSelection.push_back(entity);
    notify();
}

void SelectionManager::deselect(EntityID entity) {
    auto it = std::find(mSelection.begin(), mSelection.end(), entity);
    if (it != mSelection.end()) {
        mSelection.erase(it);
        notify();
    }
}

void SelectionManager::toggle(EntityID entity) {
    if (!entity) return;
    if (isSelected(entity)) {
        deselect(entity);
    } else {
        select(entity);
    }
}

void SelectionManager::clear() {
    if (mSelection.empty()) return;
    mSelection.clear();
    notify();
}

bool SelectionManager::isSelected(EntityID entity) const {
    return std::find(mSelection.begin(), mSelection.end(), entity) != mSelection.end();
}

bool SelectionManager::empty() const {
    return mSelection.empty();
}

size_t SelectionManager::count() const {
    return mSelection.size();
}

EntityID SelectionManager::getPrimary() const {
    return mSelection.empty() ? EntityID{} : mSelection.front();
}

void SelectionManager::notify() {
    if (mOnChanged) {
        mOnChanged();
    }
}

} // namespace filament::editor
