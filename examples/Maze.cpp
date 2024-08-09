#include "Car/Application.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Events/WindowEvent.hpp"
#include "imgui.h"
#include <optional>
#define CR_ENTRY
#include <Car/Car.hpp>


#define TOP BIT(1)
#define RIGHT BIT(2)
#define BOTTOM BIT(3)
#define LEFT BIT(4)
#define ALL_SIDES ((TOP | RIGHT | BOTTOM | LEFT))


struct Cell {
    uint32_t i, j;
    uint32_t wallMask;
    bool visited = false;
    std::vector<std::vector<Cell>>* gridPtr;
    
    
    bool isValidIndex(uint32_t x, uint32_t y) {
        return (x >= 0 and y >= 0) && (x < gridPtr[0].size() && y < gridPtr[0][x].size());
    }
        
    std::vector<Cell*> getNotVisitedNeighbors() {
        std::vector<Cell*> ret;
        
        if (isValidIndex(j, i - 1)) {  // top neighbor
            if (!gridPtr[0][i - 1][j].visited)
                ret.push_back(&(gridPtr[0][i - 1][j]));
        }
        if (isValidIndex(j + 1, i)) {  // right neighbor
            if (!gridPtr[0][i][j + 1].visited)
                ret.push_back(&(gridPtr[0][i][j + 1]));
        }
        if (isValidIndex(j, i + 1)) {  // bottom neighbor
            if (!gridPtr[0][i + 1][j].visited)
                ret.push_back(&(gridPtr[0][i + 1][j]));
        }
        if (isValidIndex(j - 1, i)) {  // left neighbor
            if (!gridPtr[0][i][j - 1].visited)
                ret.push_back(&(gridPtr[0][i][j - 1]));
        }
        
        return ret;
    }
    
    std::optional<Cell*> getRandNotVisitedCell() {
        std::vector<Cell*> neighbors = getNotVisitedNeighbors();
        std::optional<Cell*> ret;
        if (neighbors.size() == 0) {
            return ret;
        } else {
            ret = Car::Random::FromVec(neighbors);
            return ret;
        }
    }
    
    void merge(Cell* other) {
        if (other->i + 1 == i && other->j == j) {  // other is on the top
            wallMask &= RIGHT | LEFT | BOTTOM;
            other->wallMask &= RIGHT | TOP | LEFT;
        } else if (other->i == i && other->j - 1 == j) {  // other is on the right
            wallMask &= TOP | LEFT | BOTTOM;
            other->wallMask &= RIGHT | TOP | BOTTOM;
        } else if (other->i - 1 == i && other->j == j) {  // other is on the bottom
            wallMask &= RIGHT | LEFT | TOP;
            other->wallMask &= RIGHT | BOTTOM | LEFT;
        } else if (other->i == i && other->j + 1 == j) {  // other is on the left
            wallMask &= RIGHT | TOP | BOTTOM;
            other->wallMask &= BOTTOM | TOP | LEFT;
        } else {
            CR_APP_INFO("UNREACHABLE");
            CR_DEBUGBREAK();
        }
    }
    
    void draw(const glm::ivec2& cellSize) {
        if (!visited) {
            Car::Renderer2D::DrawRect(
                {float(i * cellSize.x), float(j * cellSize.y), 
                    float(cellSize.x), float(cellSize.y)}, 
                {1.0f, 1.0f, 1.0f}
            );
        } else {
            Car::Renderer2D::DrawRect(
                {float(i * cellSize.x), float(j * cellSize.y), 
                    float(cellSize.x), float(cellSize.y)}, 
                {0.0f, 1.0f, 0.0f}
            );
        }
        
        if (wallMask & TOP) {
            Car::Renderer2D::DrawLine(
                {float(i * cellSize.x), float(j * cellSize.y)}, 
                {float(i * cellSize.x + cellSize.x), float(j * cellSize.y)},
                {0.0f, 0.0f, 0.0f}
            );
        }
        if (wallMask & LEFT) {
            Car::Renderer2D::DrawLine(
                {float(i * cellSize.x), float(j * cellSize.y)}, 
                {float(i * cellSize.x), float(j * cellSize.y + cellSize.y)},
                {0.0f, 0.0f, 0.0f}
            );
        }
        if (wallMask & BOTTOM) {
            Car::Renderer2D::DrawLine(
                {float(i * cellSize.x), float(j * cellSize.y + cellSize.y)}, 
                {float(i * cellSize.x + cellSize.x), float(j * cellSize.y + cellSize.y)},
                {0.0f, 0.0f, 0.0f}
            );
        }
        if (wallMask & RIGHT) {
            Car::Renderer2D::DrawLine(
                {float(i * cellSize.x + cellSize.x), float(j * cellSize.y)}, 
                {float(i * cellSize.x + cellSize.x), float(j * cellSize.y + cellSize.y)},
                {0.0f, 0.0f, 0.0f}
            );
        }
        
    }
};


class MazeLayer : public Car::Layer {
public:
    MazeLayer() : Car::Layer("Maze Layer") {}

    virtual void onAttach() override {
        mCellSize = {20, 20};
        mRunning = false;
    }

    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");
        ImGui::Text("[FPS]: %f", 1 / dt);
        ImGui::End();
        
        ImGui::Begin("Settings");
        
        ImGui::DragInt2("Cell size", glm::value_ptr(mCellSize), 1, 1, 40);
        
        if (ImGui::Button("Start")) {
            startAlgorithm();
        }
        
        ImGui::End();
    }
    
    void startAlgorithm() {
        mRunning = true;
        mIsFinished = false;
        
        mGrid.clear();
        
        const auto window = Car::Application::Get()->getWindow();
        
        mRows = window->getWidth() / mCellSize.x;
        mCollumns = window->getHeight() / mCellSize.y;
        
        mGrid.resize(mRows);
        for (uint32_t i = 0; i < mRows; i++) {
            mGrid[i].resize(mCollumns);
            for (uint32_t j = 0; j < mCollumns; j++) {
                mGrid[i][j].i = i;
                mGrid[i][j].j = j;
                mGrid[i][j].wallMask = ALL_SIDES;
                mGrid[i][j].gridPtr = &mGrid;
            }
        }
        
        mCurrentCell = &(mGrid[0][0]);
    }
    
    virtual bool onWindowResizeEvent(Car::WindowResizeEvent&) override {
        if (mRunning) {
            startAlgorithm();
        }
        
        return false;
    }
    
    void nextIteration() {
        if (mIsFinished) {
            return;
        }
        
        mGrid[0][0].merge(&mGrid[1][0]);
    }
    
    virtual void onRender() override {
        if (not mRunning) {
            return;
        }
        
        nextIteration();
        
        for (uint32_t i = 0; i < mRows; i++) {
            for (uint32_t j = 0; j < mCollumns; j++) {
                mGrid[i][j].draw(mCellSize);
            }
        }
    }

private:
    glm::ivec2 mCellSize;
    uint32_t mCollumns;
    uint32_t mRows;
    std::vector<std::vector<Cell>> mGrid;
    Cell* mCurrentCell;
    std::vector<Cell*> mPath;
    bool mRunning = false;
    bool mIsFinished = false;
};

class RayCastingApplication : public Car::Application {
public:
    RayCastingApplication() {
        mMazeLayer = new MazeLayer();
        pushLayer(mMazeLayer);
    }
    virtual ~RayCastingApplication() override {}

    virtual void onRender() override { Car::Renderer::Clear(); }

private:
    MazeLayer* mMazeLayer;
};

Car::Application* Car::createApplication() {
    Car::Application::Specification spec{};
    spec.width = 1280;
    spec.height = 720;
    spec.title = "Maze";
    spec.vsync = false;
    spec.resizable = true;
    spec.useImGui = true;
    Car::Application::SetSpecification(spec);

    return new RayCastingApplication();
}
