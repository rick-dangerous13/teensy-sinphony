/*
 * Pazerville Module for ILI9341 Display
 * 
 * This sketch demonstrates a Pazerville-style force-directed graph
 * rendered on an ILI9341 320x240 TFT display connected to a Teensy microcontroller.
 * 
 * Hardware Connections:
 * ILI9341 Pin -> Teensy Pin
 * GND         -> GND
 * VCC         -> 3.3V (with appropriate level shifting if needed)
 * CS          -> Pin 10
 * RST         -> Pin 8
 * DC          -> Pin 9
 * MOSI        -> Pin 11 (SPI MOSI)
 * SCK         -> Pin 13 (SPI SCK)
 * MISO        -> Pin 12 (SPI MISO) - optional for reading
 * 
 * Teensy 4.x recommended for performance
 */

#include "ili9341_display.h"
#include "pazerville_display.h"

// Global display objects
ILI9341Display *tft = nullptr;
PazervilleDisplay *pazerville = nullptr;

// Simulation parameters
float sim_time = 0.0f;
uint32_t last_update = 0;
uint32_t frame_count = 0;
float fps = 0.0f;

// Control parameters
float spring_strength = 0.15f;
float damping_factor = 0.92f;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Initializing Pazerville ILI9341 Display...");
    
    // Initialize display
    tft = new ILI9341Display();
    if (!tft->initialize()) {
        Serial.println("ERROR: Failed to initialize ILI9341 display");
        while (1) {
            delay(100);
        }
    }
    
    Serial.println("Display initialized successfully!");
    
    // Initialize Pazerville
    pazerville = new PazervilleDisplay(tft);
    if (!pazerville->initialize()) {
        Serial.println("ERROR: Failed to initialize Pazerville");
        while (1) {
            delay(100);
        }
    }
    
    Serial.println("Pazerville initialized successfully!");
    
    // Set simulation parameters
    pazerville->setDamping(damping_factor);
    pazerville->setGravity(0.5f);
    pazerville->setTimeStep(0.016f);  // ~60 FPS
    
    // Create a test network graph
    createTestNetwork();
    
    Serial.println("Setup complete!");
    last_update = millis();
}

void loop() {
    uint32_t current_time = millis();
    uint32_t dt = current_time - last_update;
    
    // Update simulation
    for (int i = 0; i < 2; i++) {  // Run physics twice per frame for stability
        pazerville->update();
    }
    
    // Add some interactive forces based on time
    addTimeBasedForces(sim_time);
    
    // Draw the graph
    pazerville->draw();
    
    // Update timing
    frame_count++;
    if (current_time - last_update >= 1000) {
        fps = frame_count / ((current_time - last_update) / 1000.0f);
        frame_count = 0;
        last_update = current_time;
        
        Serial.print("FPS: ");
        Serial.println(fps);
    }
    
    sim_time += 0.016f;
    
    // Small delay to prevent overwhelming the system
    delay(1);
}

// Create a test network with multiple nodes and edges
void createTestNetwork() {
    // Create nodes in a circular pattern
    int num_nodes = 6;
    float radius = 60.0f;
    float center_x = PAZERVILLE_WIDTH / 2.0f;
    float center_y = PAZERVILLE_HEIGHT / 2.0f;
    
    for (int i = 0; i < num_nodes; i++) {
        float angle = (2.0f * PI * i) / num_nodes;
        float x = center_x + radius * cos(angle);
        float y = center_y + radius * sin(angle);
        uint16_t color = getColorFromIndex(i);
        
        pazerville->addNode(x, y, 1.5f, color, 4);
    }
    
    // Add a center node
    pazerville->addNode(center_x, center_y, 2.0f, COLOR_YELLOW, 5);
    
    // Connect all peripheral nodes to the center (node 6)
    int center_node = 6;
    for (int i = 0; i < num_nodes; i++) {
        pazerville->addEdge(i, center_node, spring_strength, 60.0f);
    }
    
    // Connect peripheral nodes to each other in a ring
    for (int i = 0; i < num_nodes; i++) {
        int next = (i + 1) % num_nodes;
        pazerville->addEdge(i, next, spring_strength * 0.5f, 80.0f);
    }
    
    Serial.print("Created network with ");
    Serial.print(pazerville->getNodeCount());
    Serial.println(" nodes");
}

// Get a color based on node index
uint16_t getColorFromIndex(int index) {
    switch (index % 6) {
        case 0: return COLOR_RED;
        case 1: return COLOR_GREEN;
        case 2: return COLOR_BLUE;
        case 3: return COLOR_CYAN;
        case 4: return COLOR_MAGENTA;
        case 5: return COLOR_ORANGE;
        default: return COLOR_WHITE;
    }
}

// Add forces to the network based on time
void addTimeBasedForces(float time) {
    // Create a pulsing effect
    float pulse = sin(time * 2.0f) * 100.0f;
    
    // Apply repulsive force from center periodically
    if ((int)(time * 2) % 2 == 0) {
        for (int i = 0; i < pazerville->getNodeCount() - 1; i++) {
            float center_x = PAZERVILLE_WIDTH / 2.0f;
            float center_y = PAZERVILLE_HEIGHT / 2.0f;
            
            PazervilleNode *node = pazerville->getNode(i);
            if (node) {
                float dx = node->x - center_x;
                float dy = node->y - center_y;
                float dist = sqrt(dx * dx + dy * dy);
                
                if (dist > 0.1f) {
                    float force = pulse * 0.001f;
                    pazerville->repelNode(i, (dx / dist) * force, (dy / dist) * force);
                }
            }
        }
    }
    
    // Apply attracting forces in a rotating pattern
    float angle = time * 2.0f;
    float target_x = PAZERVILLE_WIDTH / 2.0f + 50 * cos(angle);
    float target_y = PAZERVILLE_HEIGHT / 2.0f + 50 * sin(angle);
    
    for (int i = 0; i < pazerville->getNodeCount(); i++) {
        pazerville->attractToPoint(i, target_x, target_y, 50.0f);
    }
}

// Optional: Add user interaction via serial commands
void handleSerialInput() {
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch (cmd) {
            case 'r':
                // Randomize positions
                pazerville->randomizePositions();
                Serial.println("Positions randomized");
                break;
            case 's':
                // Stop simulation (reset velocities)
                pazerville->resetSimulation();
                Serial.println("Simulation reset");
                break;
            case 'd':
                // Increase damping
                damping_factor += 0.01f;
                if (damping_factor > 0.99f) damping_factor = 0.99f;
                pazerville->setDamping(damping_factor);
                Serial.print("Damping: ");
                Serial.println(damping_factor);
                break;
            case 'u':
                // Decrease damping
                damping_factor -= 0.01f;
                if (damping_factor < 0.8f) damping_factor = 0.8f;
                pazerville->setDamping(damping_factor);
                Serial.print("Damping: ");
                Serial.println(damping_factor);
                break;
            case '+':
                // Increase spring strength
                spring_strength += 0.01f;
                Serial.print("Spring strength: ");
                Serial.println(spring_strength);
                break;
            case '-':
                // Decrease spring strength
                spring_strength -= 0.01f;
                if (spring_strength < 0.01f) spring_strength = 0.01f;
                Serial.print("Spring strength: ");
                Serial.println(spring_strength);
                break;
            default:
                break;
        }
    }
}
