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

void multi_viewport_clock_matrix()
{
    try {
        Gui::ApplicationConfig config;
        config.width = 1920;
        config.height = 1080;
        config.title = "3x2 Multi-Viewport Global Command Matrix";
        
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

        int columns = 3;
        int rows = 2;
        int sectorWidth  = currentWindowWidth / columns;  
        int sectorHeight = currentWindowHeight / rows;   

        glm::vec4 boundaryColors[] = {
            Gui::Color::Cyan,          // Sector 0 (New York)
            Gui::Color::NeonGreen,     // Sector 1 (London)
            Gui::Color::Blue,          // Sector 2 (New Delhi)
            Gui::Color::WarningOrange, // Sector 3 (Dubai)
            Gui::Color::CrimsonBlood,  // Sector 4 (Tokyo)
            Gui::Color::LaserYellow    // Sector 5 (Sydney)
        };

        // Complete international time zone offset list configuration values
        float timezoneOffsets[] = {
            -4.0f,  // New York (EDT: UTC-4)
            1.0f,   // London (BST: UTC+1)
            5.5f,   // New Delhi (IST: UTC+5:30)
            4.0f,   // Dubai (GST: UTC+4)
            9.0f,   // Tokyo (JST: UTC+9)
            10.0f   // Sydney (AEST: UTC+10)
        };

        auto mainFont = std::make_unique<Gui::Font>("C:/Windows/Fonts/arial.ttf", 32);

        // Store our 6 clock instances in a tracking array map to update them live per tick
        std::vector<std::unique_ptr<Gui::Clock_Widget>> liveClocksCollection;

        int sectorIndex = 0;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < columns; ++col) {
                int startX = col * sectorWidth;
                int startY = (rows - 1 - row) * sectorHeight;

                // A. CREATE THE INDEPENDENT VIEWPORT SECTOR
                auto viewportSector = app->CreateViewport(startX, startY, sectorWidth, sectorHeight);

                float halfW = static_cast<float>(sectorWidth)  * 0.5f;
                float halfH = static_cast<float>(sectorHeight) * 0.5f;
                glm::mat4 rightSideUpOrtho = glm::ortho(-halfW, halfW, halfH, -halfH, -1.0f, 1.0f);
                
                viewportSector->m_UseCustomOrtho = true;
                viewportSector->m_CustomOrthoMatrix = rightSideUpOrtho;

                // B. INSTANTIATE THE DYNAMIC RADAR WIDGET FOR THIS SPECIFIC SECTOR
                auto worldClockInstance = std::make_unique<Gui::Clock_Widget>(mainFont.get(),"");
                
                worldClockInstance->GetWidget()->Scale(0.5f);
                // Inject the specific international city timezone hour offset parameters
                worldClockInstance->SetTimezoneOffset(timezoneOffsets[sectorIndex]);
                
                // Locking its position at (0,0) centers it inside this viewport box automatically
               // worldClockInstance->GetWidget()->Move(0.0f, 0.0f);
                worldClockInstance->GetWidget()->Scale(1.0f);

                // Attach the clock widget cleanly to this independent viewport container scene tree
                viewportSector->AddObject(worldClockInstance->GetWidget());

                // C. GENERATE THE HIGHLIGHT SECTOR DEBUG OUTLINE BORDER BOX
                auto layoutBoundaryAnchor = Gui::UiObject::Create();
                layoutBoundaryAnchor->Move(0.0f, 0.0f); 

                auto boundaryRenderer = Gui::RenderComponent::Create(layoutBoundaryAnchor.get());
                auto outlineBox = boundaryRenderer->template AddPrimitives<Gui::RectanglePrimitive>(
                    glm::vec2(static_cast<float>(sectorWidth), static_cast<float>(sectorHeight)), 0.0f
                );
                
                if (outlineBox) {
                    outlineBox->SetColor(Gui::Color::WithAlpha(Gui::Color::PureDark, 0.0f)); 
                    outlineBox->SetOutlineColor(boundaryColors[sectorIndex]);
                    outlineBox->SetBorderThickness(4.0f); 
                    outlineBox->SetFilled(false);
                }

                layoutBoundaryAnchor->AddComponents(boundaryRenderer);
                viewportSector->AddObject(layoutBoundaryAnchor);

                // Cache the tracker address to update hand animations per frame tick
                liveClocksCollection.push_back(std::move(worldClockInstance));
                sectorIndex++;
            }
        }

        // ====================================================================
        // MASTER DYNAMIC ANIMATION UPDATER LOOP
        // ====================================================================
        app->OnUpdate([&](float deltaTime) {
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_SCISSOR_TEST);
            glStencilMask(0xFF); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // Cascade updates across all 6 international clocks simultaneously
            for (auto& clock : liveClocksCollection) {
                if (clock) {
                    clock->Update(deltaTime);
                }
            }
        });

        app->Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Command Matrix Fault Caught: " << e.what() << std::endl;
    }
}

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
#include "TextComponent.h" 
#include "Primitive.h"
#include "Color.h"
#include "Clock.h"
#include "Font.h"

static const int GRID_COLS = 3;
static const int GRID_ROWS = 2;

static std::vector<std::shared_ptr<Gui::Viewport>> g_ViewportGrid;
static std::vector<std::unique_ptr<Gui::Clock_Widget>> g_ClockGrid;
static std::vector<Gui::RectanglePrimitive*> g_OutlinePrimitiveGrid;

// ====================================================================
// --- NATIVE GLFW FRAMEBUFFER DYNAMIC RESIZE MATRIX HOOK CALLBACK ---
// ====================================================================
void global_framebuffer_size_callback(GLFWwindow* window, int pixelWidth, int pixelHeight)
{
    if (pixelWidth == 0 || pixelHeight == 0) return;

    // Divide the physical framebuffer dimensions evenly across our columns and rows
    int sectorW = pixelWidth / GRID_COLS;
    int sectorH = pixelHeight / GRID_ROWS;

    float halfW = static_cast<float>(sectorW) * 0.5f;
    float halfH = static_cast<float>(sectorH) * 0.5f;
    glm::mat4 updatedProjection = glm::ortho(-halfW, halfW, halfH, -halfH, -1.0f, 1.0f);

    int trackingIndex = 0;
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            int currentX = c * sectorW;
            
            // Map the vertical coordinates natively relative to the OpenGL bottom-left axis
            int currentY = (GRID_ROWS - 1 - r) * sectorH;

            // ====================================================================
            // --- FIXED: PURE HARDWARE INSET CLIPS PREVENT TITLE BAR DROPOUTS ---
            // ====================================================================
            // Shifting the top row downward by exactly 1 pixel and compressing its height
            // by 1 pixel safely pins the border strokes inside the visible monitor space!
            if (r == 0) {
                currentY -= 1;
            }

            glViewport(currentX, currentY, sectorW, r == 0 ? sectorH - 1 : sectorH);

            if (trackingIndex < static_cast<int>(g_ViewportGrid.size()) && g_ViewportGrid[trackingIndex]) {
                auto& activeViewport = g_ViewportGrid[trackingIndex];
                activeViewport->UpdateLayoutBounds(currentX, currentY, sectorW, r == 0 ? sectorH - 1 : sectorH);
                activeViewport->m_CustomOrthoMatrix = updatedProjection;
            }

            if (trackingIndex < static_cast<int>(g_ClockGrid.size()) && g_ClockGrid[trackingIndex]) {
                g_ClockGrid[trackingIndex]->GetWidget()->Move(0.0f, 0.0f);
            }

            if (trackingIndex < static_cast<int>(g_OutlinePrimitiveGrid.size()) && g_OutlinePrimitiveGrid[trackingIndex]) {
                auto* primitiveRect = g_OutlinePrimitiveGrid[trackingIndex];
                if (primitiveRect) {
                    primitiveRect->SetSize(glm::vec2(static_cast<float>(sectorW), static_cast<float>(r == 0 ? sectorH - 1 : sectorH)));
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
        config.title = "Native 3x2 Viewport Engine Matrix - Locked Aspect Hub";
        
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

                float halfW = static_cast<float>(sectorWidth)  * 0.5f;
                float halfH = static_cast<float>(sectorHeight) * 0.5f;
                glm::mat4 rightSideUpOrtho = glm::ortho(-halfW, halfW, halfH, -halfH, -1.0f, 1.0f);
                
                viewportSector->m_UseCustomOrtho = true;
                viewportSector->m_CustomOrthoMatrix = rightSideUpOrtho;

                auto worldClockInstance = std::make_unique<Gui::Clock_Widget>(mainFont.get(), cityNames[sectorIndex]);
                worldClockInstance->SetTimezoneOffset(timezoneOffsets[sectorIndex]);
                
                float customScale = (sectorIndex % 2 == 0) ? 0.75f : 0.5f;
                worldClockInstance->SetScale(customScale);
                worldClockInstance->GetWidget()->Move(0.0f, 0.0f); 

                viewportSector->AddObject(worldClockInstance->GetWidget());

                // C. GENERATE THE HIGHLIGHT SECTOR DEBUG OUTLINE BORDER BOX
                auto layoutBoundaryAnchor = Gui::UiObject::Create();
                layoutBoundaryAnchor->Move(0.0f, 0.0f); 
                
                auto boundaryRenderer = Gui::RenderComponent::Create(layoutBoundaryAnchor.get());
                auto outlineBox = boundaryRenderer->template AddPrimitives<Gui::RectanglePrimitive>(
                    glm::vec2(static_cast<float>(sectorWidth), static_cast<float>(row == 0 ? sectorHeight - 1 : sectorHeight)), 0.0f
                );
                if (outlineBox) {
                    outlineBox->SetColor(Gui::Color::WithAlpha(Gui::Color::PureDark, 0.0f)); 
                    outlineBox->SetOutlineColor(boundaryColors[sectorIndex]);
                    outlineBox->SetBorderThickness(4.0f); 
                    outlineBox->SetFilled(false);
                    
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

int main() {
    //single_viewport_diagnostic_clock();
    //multi_viewport_clock_matrix();
    native_multi_viewport_matrix();
    return 0;
}