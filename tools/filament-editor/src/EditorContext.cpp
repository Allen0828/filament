#include <filament-editor/EditorContext.h>

namespace filament::editor {

EditorContext::EditorContext() = default;

EditorContext::~EditorContext() = default;

void EditorContext::initialize(filament::Engine* engine,
                               filament::Scene* scene,
                               filament::View* mainView) {
    mEngine = engine;
    mScene = scene;
    mMainView = mainView;
}

void EditorContext::addPanel(std::unique_ptr<IPanel> panel) {
    mPanels.push_back(std::move(panel));
}

void EditorContext::renderUI() {
    for (auto& panel : mPanels) {
        panel->render(*this);
    }
}

} // namespace filament::editor
