#ifndef PAZERVILLE_DISPLAY_H
#define PAZERVILLE_DISPLAY_H

#include "ili9341_display.h"
#include <math.h>

// Pazerville module configuration for ILI9341
#define PAZERVILLE_WIDTH  320
#define PAZERVILLE_HEIGHT 240
#define PAZERVILLE_MAX_NODES 8
#define PAZERVILLE_MAX_EDGES 32

// Pazerville graph node structure
typedef struct {
    float x;
    float y;
    float vx;  // velocity x
    float vy;  // velocity y
    float mass;
    uint16_t color;
    uint8_t radius;
    bool active;
    int id;
} PazervilleNode;

// Pazerville graph edge structure
typedef struct {
    int node1;
    int node2;
    float spring_constant;
    float rest_length;
    uint16_t color;
    bool active;
} PazervilleEdge;

// Pazerville renderer class
class PazervilleDisplay {
private:
    ILI9341Display *display;
    PazervilleNode nodes[PAZERVILLE_MAX_NODES];
    PazervilleEdge edges[PAZERVILLE_MAX_EDGES];
    int node_count;
    int edge_count;
    float damping;
    float gravity;
    float time_step;
    bool is_initialized;
    
    // Physics simulation
    void updateNodePhysics();
    void applySpringForces();
    void applyDamping();
    void constrainNodes();
    void drawNode(const PazervilleNode &node);
    void drawEdge(const PazervilleNode &n1, const PazervilleNode &n2, const PazervilleEdge &edge);
    
public:
    PazervilleDisplay(ILI9341Display *tft_display);
    ~PazervilleDisplay();
    
    bool initialize();
    void addNode(float x, float y, float mass, uint16_t color = COLOR_WHITE, uint8_t radius = 3);
    void addEdge(int node1, int node2, float spring_constant = 0.01f, float rest_length = 50.0f);
    void update();
    void draw();
    void setDamping(float d) { damping = d; }
    void setGravity(float g) { gravity = g; }
    void setTimeStep(float ts) { time_step = ts; }
    
    // Interactive controls
    void repelNode(int node_id, float force_x, float force_y);
    void attractToPoint(int node_id, float target_x, float target_y, float strength);
    void randomizePositions();
    void resetSimulation();
    
    int getNodeCount() const { return node_count; }
    PazervilleNode* getNode(int idx) { return (idx >= 0 && idx < node_count) ? &nodes[idx] : nullptr; }
};

#endif // PAZERVILLE_DISPLAY_H
