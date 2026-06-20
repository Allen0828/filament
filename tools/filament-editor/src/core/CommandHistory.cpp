#include <filament-editor/core/CommandHistory.h>

namespace filament::editor {

void CommandHistory::execute(std::unique_ptr<ICommand> cmd) {
    if (!cmd) return;
    cmd->execute();
    mUndoStack.push_back(std::move(cmd));
    mRedoStack.clear();
}

void CommandHistory::undo() {
    if (mUndoStack.empty()) return;
    auto cmd = std::move(mUndoStack.back());
    mUndoStack.pop_back();
    cmd->undo();
    mRedoStack.push_back(std::move(cmd));
}

void CommandHistory::redo() {
    if (mRedoStack.empty()) return;
    auto cmd = std::move(mRedoStack.back());
    mRedoStack.pop_back();
    cmd->execute();
    mUndoStack.push_back(std::move(cmd));
}

bool CommandHistory::canUndo() const {
    return !mUndoStack.empty();
}

bool CommandHistory::canRedo() const {
    return !mRedoStack.empty();
}

void CommandHistory::clear() {
    mUndoStack.clear();
    mRedoStack.clear();
}

} // namespace filament::editor
