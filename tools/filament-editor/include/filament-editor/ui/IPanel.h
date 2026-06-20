#pragma once

#include "../core/EditorTypes.h"

#include <string_view>

namespace filament::editor {

class EditorContext;

class IPanel {
public:
    virtual ~IPanel() = default;

    virtual void render(EditorContext& ctx) = 0;

    virtual PanelType getType() const = 0;
    virtual std::string_view getTitle() const = 0;
};

} // namespace filament::editor
