#include <iostream>
#include <memory>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Application.hpp"
#include "Viewport.hpp"
#include "UiObject.hpp"
#include "RenderComponent.hpp"
#include "Primitive.h"
#include "Color.h"
#include "Clock.h"
#include "Font.h"
#include "TextComponent.h"
#include "BatchRenderer.hpp"

void single_viewport_diagnostic_clock()
{
    try {
        Gui::ApplicationConfig config;
        config.width = 1920;
        config.height = 1080;
        config.title = "Single Viewport Clock Diagnostics";
        
        auto app = Gui::Application::Create(config);

        int currentWindowWidth = 0;
        int currentWindowHeight = 0;
        if (app->GetNativeWindow() != nullptr) {
            glfwGetFramebufferSize(app->GetNativeWindow(), &currentWindowWidth, &currentWindowHeight);
        }
        if (currentWindowWidth == 0 || currentWindowHeight == 0) {
            currentWindowWidth = config.width;
            currentWindowHeight = config.height;
        }

        std::shared_ptr<Gui::Viewport> isolatedViewport = app->CreateViewport(0, 0, currentWindowWidth, currentWindowHeight);

        auto mainFont = std::make_unique<Gui::Font>("C:/Windows/Fonts/arial.ttf", 32);

        // ====================================================================
        // --- FIXED: GROUND THE ANCHOR AT LOCAL SPACE ORIGIN ZERO (0,0) ---
        // ====================================================================
        // Because your diagnosticOrtho camera centers the coordinate origin (0,0) 
        // dead-center on screen, keeping this vector at zero locks it in the middle!
        auto screenCenterAnchor = Gui::UiObject::Create();
        screenCenterAnchor->Move(0.0f, 0.0f); 

        auto diagnosticClock = std::make_unique<Gui::Clock_Widget>(mainFont.get(),"Bangalore");
        diagnosticClock->GetWidget()->Move(0.0f, 0.0f);
        diagnosticClock->GetWidget()->Scale(1.0f); 
        
        screenCenterAnchor->AddChild(diagnosticClock->GetWidget());
        isolatedViewport->AddObject(screenCenterAnchor);

        float halfW = static_cast<float>(currentWindowWidth)  * 0.5f;
        float halfH = static_cast<float>(currentWindowHeight) * 0.5f;
        
        // Maps X: [-halfW to halfW], Y: [halfH to -halfH] to lock orientation right-side up
        glm::mat4 diagnosticOrtho = glm::ortho(-halfW, halfW, halfH, -halfH, -1.0f, 1.0f);
        
        isolatedViewport->m_UseCustomOrtho = true;
        isolatedViewport->m_CustomOrthoMatrix = diagnosticOrtho;

        // MASTER DIAGNOSTIC UPDATE LOOP
        app->OnUpdate([&](float deltaTime) {
            if (diagnosticClock) {
                diagnosticClock->Update(deltaTime);
            }
        });

        app->Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Diagnostic Initialization Fault Caught: " << e.what() << std::endl;
    }
}

static const int GRID_COLS = 3;
static const int GRID_ROWS = 2;

// Persistent cache containers store addresses to manipulate geometry properties fluidly
static std::vector<std::shared_ptr<Gui::Viewport>> g_ViewportGrid;
static std::vector<std::unique_ptr<Gui::Clock_Widget>> g_ClockGrid;
static std::vector<Gui::RectanglePrimitive*> g_OutlinePrimitiveGrid;

// Store original design targets dynamically from the configuration struct
static int g_DesignWindowWidth = 1920;
static int g_DesignWindowHeight = 1080;

// ====================================================================
// --- NATIVE GLFW FRAMEBUFFER DYNAMIC RESIZE MATRIX HOOK CALLBACK ---
// ====================================================================
void global_framebuffer_size_callback(GLFWwindow* window, int pixelWidth, int pixelHeight)
{
    if (pixelWidth == 0 || pixelHeight == 0) return;

    int sectorW = pixelWidth / GRID_COLS;
    int sectorH = pixelHeight / GRID_ROWS;

    // Calculate what the initial setup cell height was dynamically
    int designSectorHeight = g_DesignWindowHeight / GRID_ROWS;

    int trackingIndex = 0;
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            int currentX = c * sectorW;
            int currentY = (GRID_ROWS - 1 - r) * sectorH;

            if (r == 0) {
                currentY -= 1;
            }

            glViewport(currentX, currentY, sectorW, r == 0 ? sectorH - 1 : sectorH);

            if (trackingIndex < static_cast<int>(g_ViewportGrid.size()) && g_ViewportGrid[trackingIndex]) {
                auto& activeViewport = g_ViewportGrid[trackingIndex];
                activeViewport->UpdateLayoutBounds(currentX, currentY, sectorW, r == 0 ? sectorH - 1 : sectorH);
            }

            // ====================================================================
            // --- FIXED: 100% PURE RELATIVE SCALING COMPUTES PROPORTIONS ---------
            // ====================================================================
            // Zero magic numbers! We compare the current physical cell size directly 
            // against the original startup design layout cell size natively!
            float dynamicScaleRatio = static_cast<float>(sectorH) / static_cast<float>(designSectorHeight);
            
            float customScaleFactor = (trackingIndex % 2 == 0) ? 0.75f : 0.5f;

            if (trackingIndex < static_cast<int>(g_ClockGrid.size()) && g_ClockGrid[trackingIndex]) {
                g_ClockGrid[trackingIndex]->SetScale(customScaleFactor * dynamicScaleRatio);
                g_ClockGrid[trackingIndex]->GetWidget()->Move(0.0f, 0.0f);
            }

            if (trackingIndex < static_cast<int>(g_OutlinePrimitiveGrid.size()) && g_OutlinePrimitiveGrid[trackingIndex]) {
                auto* primitiveRect = g_OutlinePrimitiveGrid[trackingIndex];
                if (primitiveRect) {
                    primitiveRect->SetSize(glm::vec2(static_cast<float>(sectorW), static_cast<float>(sectorH)));
                }
            }
            trackingIndex++;
        }
    }
}

void native_multi_viewport_matrix()
{
    try {
        Gui::ApplicationConfig config;
        config.width = 1920;
        config.height = 1080;
        config.title = "Native 3x2 Viewport Engine Matrix - Universal Grid Hub";
        
        // Cache our original design configuration limits dynamically
        g_DesignWindowWidth = config.width;
        g_DesignWindowHeight = config.height;

        auto app = Gui::Application::Create(config);

        g_ViewportGrid.clear();
        g_ClockGrid.clear();
        g_OutlinePrimitiveGrid.clear();

        int currentWindowWidth = 0;
        int currentWindowHeight = 0;
        if (app->GetNativeWindow() != nullptr) {
            glfwGetFramebufferSize(app->GetNativeWindow(), &currentWindowWidth, &currentWindowHeight);
        }
        if (currentWindowWidth == 0 || currentWindowHeight == 0) {
            currentWindowWidth = config.width;
            currentWindowHeight = config.height;
        }

        int sectorWidth  = currentWindowWidth / GRID_COLS;  
        int sectorHeight = currentWindowHeight / GRID_ROWS;   

        glm::vec4 boundaryColors[] = {
            Gui::Color::Cyan, Gui::Color::NeonGreen, Gui::Color::Blue,
            Gui::Color::WarningOrange, Gui::Color::CrimsonBlood, Gui::Color::LaserYellow
        };

        float timezoneOffsets[] = { -7.0f, 1.0f, 5.5f, 4.0f, 9.0f, 10.0f };
        const std::string cityNames[] = { "PHOENIX", "LONDON", "BANGALORE", "DUBAI", "TOKYO", "SYDNEY" };

        auto mainFont = std::make_unique<Gui::Font>("C:/Windows/Fonts/arial.ttf", 32);

        int sectorIndex = 0;
        for (int row = 0; row < GRID_ROWS; ++row) {
            for (int col = 0; col < GRID_COLS; ++col) {
                int startX = col * sectorWidth;
                int startY = (GRID_ROWS - 1 - row) * sectorHeight;

                if (row == 0) {
                    startY -= 1;
                }

                auto viewportSector = app->CreateViewport(startX, startY, sectorWidth, row == 0 ? sectorHeight - 1 : sectorHeight);
                viewportSector->m_UseCustomOrtho = false; 

                auto worldClockInstance = std::make_unique<Gui::Clock_Widget>(mainFont.get(), cityNames[sectorIndex]);
                worldClockInstance->SetTimezoneOffset(timezoneOffsets[sectorIndex]);
                
                // Initialize setup scale directly using relative ratios
                float dynamicScaleRatio = static_cast<float>(sectorHeight) / static_cast<float>(g_DesignWindowHeight / GRID_ROWS);
                float customScaleFactor = (sectorIndex % 2 == 0) ? 0.75f : 0.5f;
                
                worldClockInstance->SetScale(customScaleFactor * dynamicScaleRatio);
                worldClockInstance->GetWidget()->Move(0.0f, 0.0f); 

                viewportSector->AddObject(worldClockInstance->GetWidget());

                // C. GENERATE THE HIGHLIGHT SECTOR DEBUG OUTLINE BORDER BOX
                auto layoutBoundaryAnchor = Gui::UiObject::Create();
                layoutBoundaryAnchor->Move(0.0f, 0.0f); 
                
                auto boundaryRenderer = Gui::RenderComponent::Create(layoutBoundaryAnchor.get());
                auto outlineBox =Gui::RectanglePrimitive::Create(glm::vec2(static_cast<float>(sectorWidth), static_cast<float>(sectorHeight)), 0.0f);
                // auto outlineBox = boundaryRenderer->template AddPrimitives<Gui::RectanglePrimitive>(
                //     glm::vec2(static_cast<float>(sectorWidth), static_cast<float>(sectorHeight)), 0.0f
                // );
                if (outlineBox) {
                    outlineBox->SetColor(Gui::Color::WithAlpha(Gui::Color::PureDark, 0.0f)); 
                    outlineBox->SetOutlineColor(boundaryColors[sectorIndex]);
                    outlineBox->SetBorderThickness(4.0f); 
                    outlineBox->SetFilled(false);
                    boundaryRenderer->AddPrimitives(outlineBox);
                    g_OutlinePrimitiveGrid.push_back(outlineBox.get());
                }
                
                layoutBoundaryAnchor->AddComponents(boundaryRenderer);
                viewportSector->AddObject(layoutBoundaryAnchor);

                g_ViewportGrid.push_back(viewportSector);
                g_ClockGrid.push_back(std::move(worldClockInstance));
                sectorIndex++;
            }
        }

        if (app->GetNativeWindow() != nullptr) {
            glfwSetWindowAspectRatio(app->GetNativeWindow(), 1920, 1080);
            glfwSetFramebufferSizeCallback(app->GetNativeWindow(), global_framebuffer_size_callback);
        }

        app->OnUpdate([&](float deltaTime) {
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_SCISSOR_TEST);
            glStencilMask(0xFF); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            for (auto& clock : g_ClockGrid) {
                if (clock) clock->Update(deltaTime);
            }
        });

        app->Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Command Matrix Fault Caught: " << e.what() << std::endl;
    }
}

void two_viewport_display()
{
    try {
        Gui::ApplicationConfig config;
        config.width = 1920;
        config.height = 1080;
        config.title = "Native 3x2 Viewport Engine Matrix - Universal Grid Hub";
        
        // Cache our original design configuration limits dynamically
        g_DesignWindowWidth = config.width;
        g_DesignWindowHeight = config.height;

        auto app = Gui::Application::Create(config);

        auto mainFont = std::make_unique<Gui::Font>("C:/Windows/Fonts/arial.ttf", 32);
        auto viewport1 = app->CreateViewport(0,0,config.width/2,config.height);
        auto viewport2 = app->CreateViewport(config.width/2,0,config.width/2,config.height);

        //
        // VIEWPORT 1
        //
        auto viewport1_boundary = Gui::UiObject::Create();
        auto boundary_render = Gui::RenderComponent::Create(viewport1_boundary.get());
        //auto boundary_rect = Gui::RectanglePrimitive::Create(glm::vec2(static_cast<float>(config.width/2.0f),static_cast<float>(config.height)),0.0f);
        auto rect_primitive = Gui::RectanglePrimitive::Create(glm::vec2(static_cast<float>(config.width/2.0f), static_cast<float>(config.height)), 0.0f);
        // auto boundary_rect = boundary_render->template AddPrimitives<Gui::RectanglePrimitive>(
        //             glm::vec2(static_cast<float>(config.width/2.0f), static_cast<float>(config.height)), 0.0f
        //         );
        
        rect_primitive->SetOutlineColor(Gui::Color::Cyan);
        rect_primitive->SetBorderThickness(4.0f);
        rect_primitive->SetFilled(true);
        rect_primitive->SetColor(Gui::Color::Black);
        boundary_render->AddPrimitives(rect_primitive);
        viewport1_boundary->AddComponents(boundary_render);
        viewport1->AddObject(viewport1_boundary);

        auto clock1 = std::make_unique<Gui::Clock_Widget>(mainFont.get(), "Bangalore");
        clock1->SetTimezoneOffset(5.5f);
        viewport1->AddObject(clock1->GetWidget());

        //
        // VIEWPORT 2
        //
        auto viewport2_boundary = Gui::UiObject::Create();
        auto boundary_render2 = Gui::RenderComponent::Create(viewport2_boundary.get());
        auto boundary_rect2 = Gui::RectanglePrimitive::Create(glm::vec2(static_cast<float>(config.width/2.0f), static_cast<float>(config.height)), 0.0f);
        // auto boundary_rect2 = boundary_render2->template AddPrimitives<Gui::RectanglePrimitive>(
        //             glm::vec2(static_cast<float>(config.width/2.0f), static_cast<float>(config.height)), 0.0f
        //         );
        boundary_rect2->SetOutlineColor(Gui::Color::Cyan);
        boundary_rect2->SetBorderThickness(4.0f);
        boundary_rect2->SetFilled(true);
        boundary_rect2->SetColor(Gui::Color::Black);
        boundary_render2->AddPrimitives(boundary_rect2);
        viewport2_boundary->AddComponents(boundary_render2);
        viewport2->AddObject(viewport2_boundary);


        app->OnUpdate([&](float deltaTime) {
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_SCISSOR_TEST);
            glStencilMask(0xFF); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // for (auto& clock : g_ClockGrid) {
            //     if (clock) clock->Update(deltaTime);
            // }
            clock1->Update(deltaTime);
        });

        app->Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Command Matrix Fault Caught: " << e.what() << std::endl;
    }
}

int main() {
    //single_viewport_diagnostic_clock();
   //native_multi_viewport_matrix();
    two_viewport_display();
    return 0;
}