#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace filament::editor {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string_view description() const = 0;
};

class CommandHistory {
public:
    void execute(std::unique_ptr<ICommand> cmd);
    void undo();
    void redo();

    bool canUndo() const;
    bool canRedo() const;

    void clear();

private:
    std::vector<std::unique_ptr<ICommand>> mUndoStack;
    std::vector<std::unique_ptr<ICommand>> mRedoStack;
};

} // namespace filament::editor
