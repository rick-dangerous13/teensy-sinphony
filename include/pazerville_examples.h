#ifndef PAZERVILLE_EXAMPLES_H
#define PAZERVILLE_EXAMPLES_H

#include "include/pazerville_display.h"

class PazervilleExamples {
public:
    // Create a complete network (fully connected graph)
    static void createCompleteNetwork(PazervilleDisplay *pazerville, int num_nodes) {
        // Create nodes in a circle
        float radius = 50.0f;
        float center_x = PAZERVILLE_WIDTH / 2.0f;
        float center_y = PAZERVILLE_HEIGHT / 2.0f;
        
        for (int i = 0; i < num_nodes; i++) {
            float angle = (2.0f * PI * i) / num_nodes;
            float x = center_x + radius * cos(angle);
            float y = center_y + radius * sin(angle);
            pazerville->addNode(x, y, 1.0f, getColorFromIndex(i), 4);
        }
        
        // Connect all pairs
        for (int i = 0; i < num_nodes; i++) {
            for (int j = i + 1; j < num_nodes; j++) {
                pazerville->addEdge(i, j, 0.05f, 80.0f);
            }
        }
    }
    
    // Create a star topology (one center, all connected to center)
    static void createStarNetwork(PazervilleDisplay *pazerville, int num_outer_nodes) {
        float radius = 60.0f;
        float center_x = PAZERVILLE_WIDTH / 2.0f;
        float center_y = PAZERVILLE_HEIGHT / 2.0f;
        
        // Add outer nodes
        for (int i = 0; i < num_outer_nodes; i++) {
            float angle = (2.0f * PI * i) / num_outer_nodes;
            float x = center_x + radius * cos(angle);
            float y = center_y + radius * sin(angle);
            pazerville->addNode(x, y, 1.0f, getColorFromIndex(i), 4);
        }
        
        // Add center node
        pazerville->addNode(center_x, center_y, 2.0f, COLOR_YELLOW, 5);
        int center = num_outer_nodes;
        
        // Connect all to center
        for (int i = 0; i < num_outer_nodes; i++) {
            pazerville->addEdge(i, center, 0.2f, 60.0f);
        }
    }
    
    // Create a line/chain topology
    static void createChainNetwork(PazervilleDisplay *pazerville, int num_nodes) {
        float spacing = PAZERVILLE_WIDTH / (num_nodes + 1);
        
        for (int i = 0; i < num_nodes; i++) {
            float x = spacing * (i + 1);
            float y = PAZERVILLE_HEIGHT / 2.0f;
            pazerville->addNode(x, y, 1.0f, getColorFromIndex(i), 4);
        }
        
        // Connect in a chain
        for (int i = 0; i < num_nodes - 1; i++) {
            pazerville->addEdge(i, i + 1, 0.3f, 50.0f);
        }
    }
    
    // Create a ring topology
    static void createRingNetwork(PazervilleDisplay *pazerville, int num_nodes) {
        float radius = 60.0f;
        float center_x = PAZERVILLE_WIDTH / 2.0f;
        float center_y = PAZERVILLE_HEIGHT / 2.0f;
        
        // Add nodes in circle
        for (int i = 0; i < num_nodes; i++) {
            float angle = (2.0f * PI * i) / num_nodes;
            float x = center_x + radius * cos(angle);
            float y = center_y + radius * sin(angle);
            pazerville->addNode(x, y, 1.0f, getColorFromIndex(i), 4);
        }
        
        // Connect in a ring (each to next)
        for (int i = 0; i < num_nodes; i++) {
            int next = (i + 1) % num_nodes;
            pazerville->addEdge(i, next, 0.25f, 70.0f);
        }
    }
    
    // Create a binary tree topology
    static void createBinaryTreeNetwork(PazervilleDisplay *pazerville) {
        // Root
        pazerville->addNode(PAZERVILLE_WIDTH / 2.0f, 30.0f, 1.5f, COLOR_RED, 4);
        
        // Level 1 (2 nodes)
        pazerville->addNode(PAZERVILLE_WIDTH / 4.0f, 80.0f, 1.2f, COLOR_ORANGE, 4);
        pazerville->addNode(3.0f * PAZERVILLE_WIDTH / 4.0f, 80.0f, 1.2f, COLOR_ORANGE, 4);
        
        // Level 2 (4 nodes)
        pazerville->addNode(PAZERVILLE_WIDTH / 8.0f, 130.0f, 1.0f, COLOR_YELLOW, 4);
        pazerville->addNode(3.0f * PAZERVILLE_WIDTH / 8.0f, 130.0f, 1.0f, COLOR_YELLOW, 4);
        pazerville->addNode(5.0f * PAZERVILLE_WIDTH / 8.0f, 130.0f, 1.0f, COLOR_YELLOW, 4);
        pazerville->addNode(7.0f * PAZERVILLE_WIDTH / 8.0f, 130.0f, 1.0f, COLOR_YELLOW, 4);
        
        // Connect root to level 1
        pazerville->addEdge(0, 1, 0.2f, 50.0f);
        pazerville->addEdge(0, 2, 0.2f, 50.0f);
        
        // Connect level 1 to level 2
        pazerville->addEdge(1, 3, 0.2f, 50.0f);
        pazerville->addEdge(1, 4, 0.2f, 50.0f);
        pazerville->addEdge(2, 5, 0.2f, 50.0f);
        pazerville->addEdge(2, 6, 0.2f, 50.0f);
    }
    
    // Create a grid/mesh topology
    static void createGridNetwork(PazervilleDisplay *pazerville, int cols, int rows) {
        float dx = PAZERVILLE_WIDTH / (cols + 1);
        float dy = PAZERVILLE_HEIGHT / (rows + 1);
        
        // Create nodes in grid
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                float px = dx * (x + 1);
                float py = dy * (y + 1);
                pazerville->addNode(px, py, 1.0f, getColorFromIndex(x + y), 3);
            }
        }
        
        // Connect adjacent nodes
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                int idx = y * cols + x;
                
                // Connect to right neighbor
                if (x < cols - 1) {
                    pazerville->addEdge(idx, idx + 1, 0.2f, dx);
                }
                
                // Connect to bottom neighbor
                if (y < rows - 1) {
                    pazerville->addEdge(idx, idx + cols, 0.2f, dy);
                }
            }
        }
    }
    
    // Create a random graph
    static void createRandomNetwork(PazervilleDisplay *pazerville, int num_nodes, int num_edges) {
        // Create random nodes
        for (int i = 0; i < num_nodes; i++) {
            float x = 20 + (rand() % (PAZERVILLE_WIDTH - 40));
            float y = 20 + (rand() % (PAZERVILLE_HEIGHT - 40));
            pazerville->addNode(x, y, 1.0f, getColorFromIndex(i), 4);
        }
        
        // Create random edges
        for (int i = 0; i < num_edges && i < num_nodes * num_nodes; i++) {
            int n1 = rand() % num_nodes;
            int n2 = rand() % num_nodes;
            
            if (n1 != n2) {
                pazerville->addEdge(n1, n2, 0.15f, 70.0f);
            }
        }
    }
    
    // Helper: Convert angle to position
    static void polarToCartesian(float angle, float radius, float center_x, float center_y,
                                 float &x, float &y) {
        x = center_x + radius * cos(angle);
        y = center_y + radius * sin(angle);
    }
    
private:
    static uint16_t getColorFromIndex(int index) {
        switch (index % 9) {
            case 0: return COLOR_RED;
            case 1: return COLOR_GREEN;
            case 2: return COLOR_BLUE;
            case 3: return COLOR_CYAN;
            case 4: return COLOR_MAGENTA;
            case 5: return COLOR_YELLOW;
            case 6: return COLOR_ORANGE;
            case 7: return COLOR_WHITE;
            case 8: return COLOR_GRAY;
            default: return COLOR_WHITE;
        }
    }
};

#endif // PAZERVILLE_EXAMPLES_H
