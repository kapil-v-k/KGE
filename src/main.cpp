// #include "Application.hpp"
// #include "UiObject.hpp"
// #include "RenderComponent.hpp"
// #include "TextComponent.h"
// #include "TextureComponent.h"
// #include "RotationComponent.h"
// #include "ClipContainerComponent.h"
// #include "Primitive.h"
// #include <glm/glm.hpp>
// #include <iostream>
// #include <memory>
// #include <cmath>
// #include "Color.h"
// #include "Clock.h"
// #include <chrono>
// #include <ctime>

// void first_app_demo()
// {
//     try {
//         // 1. Initialize Master Core Framework via its explicit ApplicationConfig struct
//         Gui::ApplicationConfig config;
//         config.width = 1280;
//         config.height = 720;
//         config.title = "Radar Console";
        
//         auto app = Gui::Application::Create(config); 
        
//         // 2. Segment the window into viewports
//         std::shared_ptr<Gui::Viewport> sidebarScreen = app->CreateViewport(0, 0, 300, 720);
//         std::shared_ptr<Gui::Viewport> canvasScreen  = app->CreateViewport(300, 0, 980, 720);

//         // ====================================================================
//         // LEFT SIDEBAR MONITOR SECTOR
//         // ====================================================================
//         auto rect_widget = Gui::UiObject::Create();
//         auto rect_rendrer = Gui::RenderComponent::Create(); 
//         auto rect_drawer = Gui::RectanglePrimitive::Create(glm::vec2(100.0f, 100.0f), 5.0f);
//         rect_rendrer->AddPrimitives(rect_drawer);
//         rect_widget->Move(150.0f, 360.0f);
//         rect_widget->AddComponents(std::move(rect_rendrer));
//         sidebarScreen->AddObject(rect_widget);

//         // --- HARDWARE SCISSOR LOG FEED WINDOW ---
//         auto scrollWindowWidget = Gui::UiObject::Create();
//         scrollWindowWidget->Move(150.0f, 150.0f); 

//         auto scissorMask = Gui::ClipContainerComponent::Create(220.0f, 150.0f); 
//         scrollWindowWidget->AddComponents(std::move(scissorMask));
//         sidebarScreen->AddObject(scrollWindowWidget);

//         auto rollingLogsWidget = Gui::UiObject::Create();
//         rollingLogsWidget->Move(0.0f, 0.0f); 

//         auto mainFont = std::make_unique<Gui::Font>("C:/Windows/Fonts/arial.ttf", 32);
//         auto logText = Gui::TextComponent::Create(); 
//         logText->SetFont(mainFont.get());
//         logText->SetText("LOG: RADAR ACTIVE\nLOG: LINK ESTABLISHED\nLOG: SCANNING GRIDS\nLOG: SECURE SYNC OK");
//         logText->SetLocalScale(0.95f);
//         logText->SetLocalPosition(-65.0f, -40.0f); 

//         rollingLogsWidget->AddComponents(std::move(logText)); 
//         scrollWindowWidget->AddChild(rollingLogsWidget);

//         //
//         // CANVAS SCREEN
//         //
//         //CLOCK WIDGET
//         auto masterRadarClock = std::make_unique<Gui::Clock_Widget>(mainFont.get());
//         canvasScreen->AddObject(masterRadarClock->GetWidget());

//         // ====================================================================
//         // HIGH-PRECISION RUNTIME TIME SYNC CONTROLLER
//         // ====================================================================
//         static float logVerticalDrift = 0.0f;
        
//         app->OnUpdate([&](float deltaTime) {
//             // A. Drive sidebar animation independently
//             logVerticalDrift += deltaTime * 20.0f;
//             if (logVerticalDrift > 120.0f) logVerticalDrift = -20.0f;
//             rollingLogsWidget->Move(0.0f, logVerticalDrift);

//             // B. Drive your high-precision clock engine automatically inside your ticker callback!
//             if (masterRadarClock) {
//                 masterRadarClock->Update(deltaTime);
//             }
//         });

//         app->Run();
//     } 
//     catch (const std::exception& e) {
//         std::cerr << "Engine Initialization Fault Caught: " << e.what() << std::endl;
        
//     }
// }

// #include <iostream>
// #include <memory>
// #include <vector>
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include "Application.hpp"
// #include "Viewport.hpp"
// #include "UiObject.hpp"
// #include "RenderComponent.hpp"
// #include "Primitive.h"
// #include "Color.h"
// #include "Clock.h"
// #include "Font.h"
// #include "BatchRenderer.hpp"
// #include "Shader.hpp"

// // Static reference hooks capture the true running graphics framework assets
// static Gui::BatchRenderer* s_MasterBatcher = nullptr;
// static Gui::Shader*        s_MasterShader  = nullptr;

// class PipelineInterceptor : public Gui::Component {
// public:
//     PipelineInterceptor(Gui::UiObject* owner) : Gui::Component(owner) {}
    
//     // --- FIXED: IMPLEMENTED VIA THE TRUE METHOD TO ASSIGN RENDERING LAYERS NATIVELY ---
//     Gui::RenderLayer GetRenderLayer() const override { return Gui::RenderLayer::Background; }

//     void Render(Gui::BatchRenderer& batcher, const glm::mat4&, Gui::Viewport*) override {
//         s_MasterBatcher = &batcher;
//     }
// };

// void multi_viewport_clock()
// {
//     try {
//         Gui::ApplicationConfig config;
//         config.width = 1920;
//         config.height = 1080;
//         config.title = "Global Radar Console Matrix";
        
//         auto app = Gui::Application::Create(config);

//         int cellWidth  = static_cast<int>(config.width / 3);  
//         int cellHeight = static_cast<int>(config.height / 2); 

//         std::vector<std::shared_ptr<Gui::Viewport>> viewports;
//         viewports.push_back(app->CreateViewport(0,             0,          cellWidth, cellHeight)); 
//         viewports.push_back(app->CreateViewport(cellWidth,     0,          cellWidth, cellHeight)); 
//         viewports.push_back(app->CreateViewport(cellWidth * 2, 0,          cellWidth, cellHeight)); 
//         viewports.push_back(app->CreateViewport(0,             cellHeight, cellWidth, cellHeight)); 
//         viewports.push_back(app->CreateViewport(cellWidth,     cellHeight, cellWidth, cellHeight)); 
//         viewports.push_back(app->CreateViewport(cellWidth * 2, cellHeight, cellWidth, cellHeight)); 

//         auto mainFont = std::make_unique<Gui::Font>("C:/Windows/Fonts/arial.ttf", 32);
//         std::vector<std::unique_ptr<Gui::Clock_Widget>> globalClocks(6);

//         for (int i = 0; i < 6; ++i) {
//             auto cell_rect_widget = Gui::UiObject::Create();
//             auto rect_rendrer = Gui::RenderComponent::Create(cell_rect_widget.get()); 
            
//             auto rect_drawer = rect_rendrer->template AddPrimitives<Gui::RectanglePrimitive>(glm::vec2(cellWidth, cellHeight), 5.0f);
//             if (rect_drawer) {
//                 rect_drawer->SetOutlineColor(Gui::Color::WithAlpha(Gui::Color::Cyan, 0.3f));
//                 rect_drawer->SetBorderThickness(2.0f);
//             }
//             cell_rect_widget->AddComponents(rect_rendrer);

//             if (i == 0) {
//                 auto interceptor = std::make_shared<PipelineInterceptor>(cell_rect_widget.get());
//                 cell_rect_widget->AddComponents(interceptor);
//             }

//             cell_rect_widget->Move(0.0f, 0.0f); 
//             viewports[i]->AddObject(cell_rect_widget);

//             globalClocks[i] = std::make_unique<Gui::Clock_Widget>(mainFont.get());
//             globalClocks[i]->GetWidget()->Move(0.0f, 0.0f);
//             globalClocks[i]->GetWidget()->Scale(0.72f);

//             float halfW = (static_cast<float>(cellWidth)  / 0.72f) * 0.5f;
//             float halfH = (static_cast<float>(cellHeight) / 0.72f) * 0.5f;
//             glm::mat4 customOrtho = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
            
//             // --- FIXED: THESE NOW MAP SUCCESSFULLY TO PROPER CODE TARGETS ---
//             viewports[i]->m_UseCustomOrtho = true;
//             viewports[i]->m_CustomOrthoMatrix = customOrtho;

//             viewports[i]->AddObject(globalClocks[i]->GetWidget());
//         }

//         // ====================================================================
//         // MASTER TIMELINE RENDERING INTERACTION TICK LOOP
//         // ====================================================================
//         app->OnUpdate([&](float deltaTime) {
//             glDisable(GL_STENCIL_TEST);
//             glDisable(GL_SCISSOR_TEST);
//             glStencilMask(0xFF); 
//             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

//             for (int i = 0; i < 6; ++i) {
//                 if (globalClocks[i]) {
//                     globalClocks[i]->Update(deltaTime);
//                 }
//             }

//             int physicalWidth = 0, physicalHeight = 0;
//             if (app->GetNativeWindow() != nullptr) {
//                 glfwGetFramebufferSize(app->GetNativeWindow(), &physicalWidth, &physicalHeight);
//             }
//             if (physicalWidth == 0 || physicalHeight == 0) {
//                 physicalWidth = config.width;
//                 physicalHeight = config.height;
//             }

//             // Capture the shader handle dynamically from the running interceptor on frame 1
//             if (s_MasterBatcher && s_MasterShader == nullptr) {
//                 // (If your local engine maps a shader getter inside the batcher, assign it here)
//             }

//             // Render all 6 isolated viewports using the 4 core reference parameters
//             for (int i = 0; i < 6; ++i) {
//                 // If s_MasterShader is resolved via your internal loops, pass here:
//                 // viewports[i]->Render(physicalWidth, physicalHeight, *s_MasterBatcher, *s_MasterShader);
//             }
//         });

//         app->Run();
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Initialization Fault: " << e.what() << std::endl;
//     }
// }

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

        auto diagnosticClock = std::make_unique<Gui::Clock_Widget>(mainFont.get());
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

int main() {
    single_viewport_diagnostic_clock();
    return 0;
}