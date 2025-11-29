#include "../include/pazerville_display.h"

// Constructor
PazervilleDisplay::PazervilleDisplay(ILI9341Display *tft_display) {
    display = tft_display;
    node_count = 0;
    edge_count = 0;
    damping = 0.95f;
    gravity = 0.0f;
    time_step = 0.01f;
    is_initialized = false;
    
    // Initialize all nodes and edges
    for (int i = 0; i < PAZERVILLE_MAX_NODES; i++) {
        nodes[i].active = false;
    }
    for (int i = 0; i < PAZERVILLE_MAX_EDGES; i++) {
        edges[i].active = false;
    }
}

// Destructor
PazervilleDisplay::~PazervilleDisplay() {
    // Nothing to clean up currently
}

// Initialize Pazerville display
bool PazervilleDisplay::initialize() {
    if (!display || !display->getFramebuffer()) {
        return false;
    }
    
    display->fillScreen(COLOR_BLACK);
    is_initialized = true;
    return true;
}

// Add a node to the graph
void PazervilleDisplay::addNode(float x, float y, float mass, uint16_t color, uint8_t radius) {
    if (node_count >= PAZERVILLE_MAX_NODES) {
        return;
    }
    
    nodes[node_count].x = x;
    nodes[node_count].y = y;
    nodes[node_count].vx = 0.0f;
    nodes[node_count].vy = 0.0f;
    nodes[node_count].mass = mass;
    nodes[node_count].color = color;
    nodes[node_count].radius = radius;
    nodes[node_count].active = true;
    nodes[node_count].id = node_count;
    
    node_count++;
}

// Add an edge between two nodes
void PazervilleDisplay::addEdge(int node1, int node2, float spring_constant, float rest_length) {
    if (edge_count >= PAZERVILLE_MAX_EDGES) {
        return;
    }
    if (node1 < 0 || node1 >= node_count || node2 < 0 || node2 >= node_count) {
        return;
    }
    
    edges[edge_count].node1 = node1;
    edges[edge_count].node2 = node2;
    edges[edge_count].spring_constant = spring_constant;
    edges[edge_count].rest_length = rest_length;
    edges[edge_count].color = COLOR_GRAY;
    edges[edge_count].active = true;
    
    edge_count++;
}

// Update physics simulation
void PazervilleDisplay::updateNodePhysics() {
    // Apply spring forces
    applySpringForces();
    
    // Apply gravity
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].active) {
            nodes[i].vy += gravity * time_step;
        }
    }
    
    // Update velocities and positions using Verlet integration
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].active) {
            nodes[i].vx *= damping;
            nodes[i].vy *= damping;
            
            nodes[i].x += nodes[i].vx * time_step;
            nodes[i].y += nodes[i].vy * time_step;
        }
    }
    
    // Apply damping
    applyDamping();
    
    // Constrain nodes to screen
    constrainNodes();
}

// Apply spring forces between connected nodes
void PazervilleDisplay::applySpringForces() {
    for (int i = 0; i < edge_count; i++) {
        if (!edges[i].active) continue;
        
        PazervilleNode &n1 = nodes[edges[i].node1];
        PazervilleNode &n2 = nodes[edges[i].node2];
        
        if (!n1.active || !n2.active) continue;
        
        // Calculate distance between nodes
        float dx = n2.x - n1.x;
        float dy = n2.y - n1.y;
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist < 0.1f) continue;
        
        // Calculate spring force
        float force = edges[i].spring_constant * (dist - edges[i].rest_length);
        float fx = (force * dx) / dist;
        float fy = (force * dy) / dist;
        
        // Apply force to nodes
        n1.vx += (fx / n1.mass) * time_step;
        n1.vy += (fy / n1.mass) * time_step;
        n2.vx -= (fx / n2.mass) * time_step;
        n2.vy -= (fy / n2.mass) * time_step;
    }
}

// Apply damping to reduce oscillation
void PazervilleDisplay::applyDamping() {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].active) {
            nodes[i].vx *= 0.995f;
            nodes[i].vy *= 0.995f;
        }
    }
}

// Constrain nodes to stay within screen bounds with bounce
void PazervilleDisplay::constrainNodes() {
    for (int i = 0; i < node_count; i++) {
        if (!nodes[i].active) continue;
        
        // Left and right bounds
        if (nodes[i].x - nodes[i].radius < 0) {
            nodes[i].x = nodes[i].radius;
            nodes[i].vx *= -0.8f;  // Bounce with energy loss
        }
        if (nodes[i].x + nodes[i].radius > PAZERVILLE_WIDTH) {
            nodes[i].x = PAZERVILLE_WIDTH - nodes[i].radius;
            nodes[i].vx *= -0.8f;
        }
        
        // Top and bottom bounds
        if (nodes[i].y - nodes[i].radius < 0) {
            nodes[i].y = nodes[i].radius;
            nodes[i].vy *= -0.8f;
        }
        if (nodes[i].y + nodes[i].radius > PAZERVILLE_HEIGHT) {
            nodes[i].y = PAZERVILLE_HEIGHT - nodes[i].radius;
            nodes[i].vy *= -0.8f;
        }
    }
}

// Draw a node on the display
void PazervilleDisplay::drawNode(const PazervilleNode &node) {
    if (!node.active || !display) return;
    
    int x = (int)node.x;
    int y = (int)node.y;
    int r = node.radius;
    
    display->drawCircle(x, y, r, node.color);
    display->fillRect(x - r/2, y - r/2, r, r, node.color);
}

// Draw an edge between two nodes
void PazervilleDisplay::drawEdge(const PazervilleNode &n1, const PazervilleNode &n2, const PazervilleEdge &edge) {
    if (!display) return;
    
    int x1 = (int)n1.x;
    int y1 = (int)n1.y;
    int x2 = (int)n2.x;
    int y2 = (int)n2.y;
    
    display->drawLine(x1, y1, x2, y2, edge.color);
}

// Update the simulation
void PazervilleDisplay::update() {
    if (!is_initialized) return;
    
    updateNodePhysics();
}

// Draw the Pazerville graph
void PazervilleDisplay::draw() {
    if (!is_initialized || !display) return;
    
    // Clear screen
    display->fillScreen(COLOR_BLACK);
    
    // Draw edges
    for (int i = 0; i < edge_count; i++) {
        if (!edges[i].active) continue;
        
        PazervilleNode &n1 = nodes[edges[i].node1];
        PazervilleNode &n2 = nodes[edges[i].node2];
        
        if (n1.active && n2.active) {
            drawEdge(n1, n2, edges[i]);
        }
    }
    
    // Draw nodes
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].active) {
            drawNode(nodes[i]);
        }
    }
    
    // Update display
    display->updateDisplay();
}

// Repel a node with a force
void PazervilleDisplay::repelNode(int node_id, float force_x, float force_y) {
    if (node_id < 0 || node_id >= node_count) return;
    if (!nodes[node_id].active) return;
    
    nodes[node_id].vx += (force_x / nodes[node_id].mass) * time_step;
    nodes[node_id].vy += (force_y / nodes[node_id].mass) * time_step;
}

// Attract a node to a point
void PazervilleDisplay::attractToPoint(int node_id, float target_x, float target_y, float strength) {
    if (node_id < 0 || node_id >= node_count) return;
    if (!nodes[node_id].active) return;
    
    float dx = target_x - nodes[node_id].x;
    float dy = target_y - nodes[node_id].y;
    float dist = sqrt(dx * dx + dy * dy);
    
    if (dist > 0.1f) {
        float force = strength / dist;
        nodes[node_id].vx += (force * dx / dist) * time_step;
        nodes[node_id].vy += (force * dy / dist) * time_step;
    }
}

// Randomize node positions
void PazervilleDisplay::randomizePositions() {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].active) {
            nodes[i].x = 20 + (rand() % (PAZERVILLE_WIDTH - 40));
            nodes[i].y = 20 + (rand() % (PAZERVILLE_HEIGHT - 40));
            nodes[i].vx = (rand() % 100 - 50) * 0.01f;
            nodes[i].vy = (rand() % 100 - 50) * 0.01f;
        }
    }
}

// Reset simulation to initial state
void PazervilleDisplay::resetSimulation() {
    for (int i = 0; i < node_count; i++) {
        nodes[i].vx = 0.0f;
        nodes[i].vy = 0.0f;
    }
}
