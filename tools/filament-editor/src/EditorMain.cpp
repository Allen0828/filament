/*
 * Filament Editor - Entry point using FilamentApp + ImGui Docking
 */

#include <filament-editor/EditorContext.h>

#include <filamentapp/FilamentApp.h>
#include <filamentapp/Config.h>

#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/View.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace filament;
using namespace filament::editor;

// ------------------------------------------------------------------------------------------------
// Concrete ImGui Panels
// ------------------------------------------------------------------------------------------------

class HierarchyPanel final : public IPanel {
public:
    void render(EditorContext& ctx) override {
        ImGui::Text("Scene Entities");
        ImGui::Separator();

        auto const& sel = ctx.getSelection();
        if (sel.empty()) {
            ImGui::TextDisabled("No entities in scene");
        } else {
            for (auto e : sel.getSelection()) {
                ImGui::BulletText("Entity %u", e.getId());
            }
        }
    }

    PanelType getType() const override { return PanelType::Hierarchy; }
    std::string_view getTitle() const override { return "Hierarchy"; }
};

class ViewportPanel final : public IPanel {
public:
    void render(EditorContext& ctx) override {
        auto const* view = ctx.getMainView();
        if (view) {
            auto const& vp = view->getViewport();
            ImVec2 const avail = ImGui::GetContentRegionAvail();
            if (avail.x > 0 && avail.y > 0) {
                ImGui::TextColored({0.4f, 0.4f, 0.4f, 1.0f},
                        "Viewport %dx%d", (int)vp.width, (int)vp.height);
            }
        }
    }

    PanelType getType() const override { return PanelType::Viewport; }
    std::string_view getTitle() const override { return "Viewport"; }
};

class InspectorPanel final : public IPanel {
public:
    void render(EditorContext& ctx) override {
        auto const& sel = ctx.getSelection();
        auto primary = sel.getPrimary();

        if (!primary) {
            ImGui::TextDisabled("Nothing selected");
            return;
        }

        ImGui::Text("Entity %u", primary.getId());
        ImGui::Separator();

        ImGui::Text("Transform");
        static float pos[3] = {0, 0, 0};
        static float rot[3] = {0, 0, 0};
        static float scl[3] = {1, 1, 1};

        ImGui::DragFloat3("Position", pos, 0.1f);
        ImGui::DragFloat3("Rotation", rot, 1.0f);
        ImGui::DragFloat3("Scale",    scl, 0.1f);
    }

    PanelType getType() const override { return PanelType::Inspector; }
    std::string_view getTitle() const override { return "Inspector"; }
};

// ------------------------------------------------------------------------------------------------
// Main
// ------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    Config config;
    config.title = "Filament Editor";
    config.backend = filament::Engine::Backend::OPENGL;

    EditorContext editorCtx;

    auto setup = [&](Engine* engine, View* view, Scene* scene) {
        editorCtx.initialize(engine, scene, view);

        editorCtx.addPanel(std::make_unique<HierarchyPanel>());
        editorCtx.addPanel(std::make_unique<ViewportPanel>());
        editorCtx.addPanel(std::make_unique<InspectorPanel>());
    };

    auto cleanup = [&](Engine* engine, View*, Scene*) {
        (void)engine;
    };

    auto imgui = [&](Engine* engine, View* view) {
        (void)engine;
        (void)view;

        // Simple menu bar and sidebar layout (no docking)
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 const displaySize = io.DisplaySize;

        // Main menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    FilamentApp::get().close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                bool canUndo = editorCtx.getHistory().canUndo();
                bool canRedo = editorCtx.getHistory().canRedo();
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canUndo)) {
                    editorCtx.getHistory().undo();
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, canRedo)) {
                    editorCtx.getHistory().redo();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        float const sidebarWidth = 250.0f;
        float const inspectorWidth = 250.0f;
        float const bottomHeight = 150.0f;

        // Hierarchy panel (left sidebar)
        ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y * 0.05f));
        ImGui::SetNextWindowSize(
                ImVec2(sidebarWidth, displaySize.y - bottomHeight));
        ImGui::Begin("##Hierarchy", nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::TextUnformatted("Hierarchy");
        ImGui::Separator();
        {
            auto const& sel = editorCtx.getSelection();
            if (sel.empty()) {
                ImGui::TextDisabled("No entities in scene");
            } else {
                for (auto e : sel.getSelection()) {
                    ImGui::BulletText("Entity %u", e.getId());
                }
            }
        }
        ImGui::End();

        // Viewport panel (center)
        ImGui::SetNextWindowPos(
                ImVec2(sidebarWidth, io.DisplaySize.y * 0.05f));
        ImGui::SetNextWindowSize(ImVec2(displaySize.x - sidebarWidth
                - inspectorWidth, displaySize.y - bottomHeight));
        ImGui::Begin("##Viewport", nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::TextUnformatted("Viewport");
        if (editorCtx.getMainView()) {
            auto const& vp = editorCtx.getMainView()->getViewport();
            ImGui::TextDisabled("%dx%d", (int)vp.width, (int)vp.height);
        }
        ImGui::End();

        // Inspector panel (right sidebar)
        ImGui::SetNextWindowPos(ImVec2(displaySize.x - inspectorWidth,
                io.DisplaySize.y * 0.05f));
        ImGui::SetNextWindowSize(
                ImVec2(inspectorWidth, displaySize.y - bottomHeight));
        ImGui::Begin("##Inspector", nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::TextUnformatted("Inspector");
        ImGui::Separator();
        {
            auto primary = editorCtx.getSelection().getPrimary();
            if (!primary) {
                ImGui::TextDisabled("Nothing selected");
            } else {
                ImGui::Text("Entity %u", primary.getId());
                ImGui::TextUnformatted("Transform");
                static float pos[3] = {0, 0, 0};
                static float scl[3] = {1, 1, 1};
                ImGui::DragFloat3("Position", pos, 0.1f);
                ImGui::DragFloat3("Scale", scl, 0.1f);
            }
        }
        ImGui::End();

        // Console panel (bottom)
        ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - bottomHeight));
        ImGui::SetNextWindowSize(
                ImVec2(displaySize.x, bottomHeight));
        ImGui::Begin("##Console", nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::TextUnformatted("Console");
        ImGui::End();
    };

    FilamentApp::get().run(config, setup, cleanup, imgui);

    return 0;
}
