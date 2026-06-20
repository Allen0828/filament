#pragma once

#include "core/SelectionManager.h"
#include "core/CommandHistory.h"
#include "ui/IPanel.h"
#include "core/EditorTypes.h"

#include <memory>
#include <vector>

namespace filament {
class Engine;
class Scene;
class View;
}

namespace filament::editor {

class EditorContext {
public:
    EditorContext();
    ~EditorContext();

    void initialize(filament::Engine* engine,
                    filament::Scene* scene,
                    filament::View* mainView);

    filament::Engine* getEngine() const { return mEngine; }
    filament::Scene* getScene() const { return mScene; }
    filament::View* getMainView() const { return mMainView; }

    SelectionManager& getSelection() { return mSelection; }
    const SelectionManager& getSelection() const { return mSelection; }

    CommandHistory& getHistory() { return mHistory; }
    const CommandHistory& getHistory() const { return mHistory; }

    void addPanel(std::unique_ptr<IPanel> panel);
    void renderUI();

private:
    filament::Engine* mEngine = nullptr;
    filament::Scene* mScene = nullptr;
    filament::View* mMainView = nullptr;

    SelectionManager mSelection;
    CommandHistory mHistory;

    std::vector<std::unique_ptr<IPanel>> mPanels;
};

} // namespace filament::editor
