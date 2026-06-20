#pragma once

#include <utils/Entity.h>

#include <cstdint>

namespace filament::editor {

using EntityID = utils::Entity;

enum class PanelType : uint8_t {
    Hierarchy,
    Viewport,
    Inspector,
    AssetBrowser,
    Console,
};

} // namespace filament::editor
