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

        // Enable docking on first frame
        static bool s_firstFrame = true;
        if (s_firstFrame) {
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            s_firstFrame = false;
        }

        // Full-window dockspace
        ImGuiViewport const* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::SetNextWindowViewport(vp->ID);

        ImGuiWindowFlags const windowFlags =
                ImGuiWindowFlags_MenuBar |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("DockSpaceWindow", nullptr, windowFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceId = ImGui::GetID("EditorDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0, 0),
                ImGuiDockNodeFlags_PassthruCentralNode);

        // Menu bar
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

        ImGui::End();

        // Set up default docking layout on first frame
        static bool s_layoutDone = false;
        if (!s_layoutDone) {
            s_layoutDone = true;

            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId,
                    ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, vp->WorkSize);

            ImGuiID dockLeft  = ImGui::DockBuilderSplitNode(dockspaceId,
                    ImGuiDir_Left, 0.20f, nullptr, &dockspaceId);
            ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockspaceId,
                    ImGuiDir_Right, 0.25f, nullptr, &dockspaceId);
            ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockspaceId,
                    ImGuiDir_Down, 0.25f, nullptr, &dockspaceId);

            ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
            ImGui::DockBuilderDockWindow("Viewport",  dockspaceId);
            ImGui::DockBuilderDockWindow("Inspector", dockRight);
            ImGui::DockBuilderDockWindow("Console",   dockBottom);
            ImGui::DockBuilderFinish(dockspaceId);
        }

        // Hierarchy panel
        ImGui::Begin("Hierarchy");
        ImGui::TextUnformatted("Scene Entities");
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

        // Viewport panel
        ImGui::Begin("Viewport");
        ImGui::TextUnformatted("Scene Viewport");
        if (editorCtx.getMainView()) {
            auto const& vp = editorCtx.getMainView()->getViewport();
            ImGui::TextDisabled("%dx%d", (int)vp.width, (int)vp.height);
        }
        ImGui::End();

        // Inspector panel
        ImGui::Begin("Inspector");
        ImGui::TextUnformatted("Properties");
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

        // Console panel
        ImGui::Begin("Console");
        ImGui::TextUnformatted("Output");
        ImGui::End();
    };

    FilamentApp::get().run(config, setup, cleanup, imgui);

    return 0;
}
