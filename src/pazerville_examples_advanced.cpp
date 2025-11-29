/*
 * Advanced Pazerville Examples for ILI9341
 * 
 * This file contains example code snippets showing advanced usage patterns
 * for the Pazerville physics engine with the ILI9341 display.
 */

#include "include/ili9341_display.h"
#include "include/pazerville_display.h"
#include "include/pazerville_examples.h"

// ============================================================================
// EXAMPLE 1: Interactive Network with Real-Time Control
// ============================================================================

void example_interactive_network() {
    ILI9341Display tft;
    PazervilleDisplay pazerville(&tft);
    
    tft.initialize();
    pazerville.initialize();
    
    // Create initial network
    PazervilleExamples::createStarNetwork(&pazerville, 5);
    
    // Set initial physics parameters
    pazerville.setDamping(0.90f);
    pazerville.setGravity(0.3f);
    
    uint32_t last_interaction = 0;
    float angle = 0.0f;
    
    while (1) {
        // Update physics
        pazerville.update();
        pazerville.draw();
        
        // Add rotating attractors
        angle += 0.05f;
        for (int i = 0; i < pazerville.getNodeCount() - 1; i++) {
            float target_x = PAZERVILLE_WIDTH/2 + 40*cos(angle);
            float target_y = PAZERVILLE_HEIGHT/2 + 40*sin(angle);
            pazerville.attractToPoint(i, target_x, target_y, 30.0f);
        }
        
        // Check for serial commands
        if (Serial.available()) {
            char cmd = Serial.read();
            handleControlCommand(&pazerville, cmd);
        }
        
        delay(2);
    }
}

// ============================================================================
// EXAMPLE 2: Animated Network Topology Transitions
// ============================================================================

void example_topology_transitions() {
    ILI9341Display tft;
    PazervilleDisplay pazerville(&tft);
    
    tft.initialize();
    pazerville.initialize();
    
    uint32_t transition_timer = 0;
    int topology_index = 0;
    
    // Create initial topology
    switchTopology(&pazerville, topology_index);
    
    while (1) {
        // Update and draw
        for (int i = 0; i < 3; i++) {
            pazerville.update();
        }
        pazerville.draw();
        
        transition_timer++;
        
        // Switch topology every 5 seconds
        if (transition_timer > 500) {  // 500 * 10ms = 5 seconds
            topology_index = (topology_index + 1) % 5;
            
            // Smoothly transition by resetting
            pazerville.resetSimulation();
            switchTopology(&pazerville, topology_index);
            
            transition_timer = 0;
            Serial.print("Switched to topology: ");
            Serial.println(topology_index);
        }
        
        delay(10);
    }
}

// ============================================================================
// EXAMPLE 3: CV-Driven Physics (Audio/Control Voltage Integration)
// ============================================================================

void example_cv_driven_physics() {
    ILI9341Display tft;
    PazervilleDisplay pazerville(&tft);
    
    tft.initialize();
    pazerville.initialize();
    
    // Create a grid network
    PazervilleExamples::createGridNetwork(&pazerville, 3, 3);
    pazerville.setDamping(0.88f);
    
    // Analog inputs for CV (assuming connected to A0, A1, A2)
    int cv_damping_pin = A0;
    int cv_gravity_pin = A1;
    int cv_repel_pin = A2;
    
    while (1) {
        // Read analog inputs (0-1023)
        int cv_damping = analogRead(cv_damping_pin);
        int cv_gravity = analogRead(cv_gravity_pin);
        int cv_repel = analogRead(cv_repel_pin);
        
        // Map to physics parameters
        float damping = 0.80f + (cv_damping / 1023.0f) * 0.15f;  // 0.80 - 0.95
        float gravity = (cv_gravity / 1023.0f) * 1.0f;            // 0.0 - 1.0
        float repel_strength = (cv_repel / 1023.0f) * 200.0f;    // 0 - 200
        
        pazerville.setDamping(damping);
        pazerville.setGravity(gravity);
        
        // Apply repulsion from center based on CV
        float center_x = PAZERVILLE_WIDTH / 2.0f;
        float center_y = PAZERVILLE_HEIGHT / 2.0f;
        
        for (int i = 0; i < pazerville.getNodeCount(); i++) {
            PazervilleNode *node = pazerville.getNode(i);
            if (node) {
                float dx = node->x - center_x;
                float dy = node->y - center_y;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist > 0.1f) {
                    pazerville.repelNode(i, 
                        (dx/dist) * repel_strength * 0.001f,
                        (dy/dist) * repel_strength * 0.001f);
                }
            }
        }
        
        pazerville.update();
        pazerville.draw();
        
        delay(16);  // ~60 FPS
    }
}

// ============================================================================
// EXAMPLE 4: Visual Music - Frequency Analysis Display
// ============================================================================

void example_music_visualization() {
    ILI9341Display tft;
    PazervilleDisplay pazerville(&tft);
    
    tft.initialize();
    pazerville.initialize();
    
    // Create ring network for frequency bands
    PazervilleExamples::createRingNetwork(&pazerville, 8);
    pazerville.setDamping(0.85f);
    pazerville.setGravity(0.0f);
    
    // Audio input would come from ADC or FFT processor
    // For this example, we'll simulate with sine waves
    
    float time = 0.0f;
    
    while (1) {
        time += 0.016f;
        
        // Simulate frequency bands (8 frequency bins)
        for (int i = 0; i < 8; i++) {
            float freq = 100.0f + i * 150.0f;  // 100Hz, 250Hz, 400Hz, etc.
            float magnitude = (sin(time * freq) + 1.0f) / 2.0f * 100.0f;
            
            // Attract node inward based on frequency magnitude
            float center_x = PAZERVILLE_WIDTH / 2.0f;
            float center_y = PAZERVILLE_HEIGHT / 2.0f;
            
            pazerville.attractToPoint(i, center_x, center_y, magnitude);
        }
        
        pazerville.update();
        pazerville.draw();
        
        delay(16);
    }
}

// ============================================================================
// EXAMPLE 5: Particle Burst Animation
// ============================================================================

void example_particle_burst() {
    ILI9341Display tft;
    PazervilleDisplay pazerville(&tft);
    
    tft.initialize();
    pazerville.initialize();
    
    // Create single central node
    pazerville.addNode(PAZERVILLE_WIDTH/2, PAZERVILLE_HEIGHT/2, 1.0f, COLOR_WHITE, 5);
    
    // Add particles in a circle around it
    int num_particles = 6;
    for (int i = 0; i < num_particles; i++) {
        float angle = (2.0f * PI * i) / num_particles;
        float x = PAZERVILLE_WIDTH/2 + 20*cos(angle);
        float y = PAZERVILLE_HEIGHT/2 + 20*sin(angle);
        pazerville.addNode(x, y, 1.0f, getColorFromFreq(i), 3);
        
        // Connect to center
        pazerville.addEdge(0, i+1, 0.1f, 20.0f);
    }
    
    pazerville.setDamping(0.90f);
    
    uint32_t burst_time = millis();
    
    while (1) {
        uint32_t elapsed = millis() - burst_time;
        
        // Every 2 seconds, create a burst
        if (elapsed % 2000 < 100) {
            // Repel all particles
            for (int i = 1; i <= num_particles; i++) {
                PazervilleNode *node = pazerville.getNode(i);
                if (node) {
                    float dx = node->x - PAZERVILLE_WIDTH/2;
                    float dy = node->y - PAZERVILLE_HEIGHT/2;
                    float dist = sqrt(dx*dx + dy*dy);
                    if (dist > 0.1f) {
                        pazerville.repelNode(i, (dx/dist)*200, (dy/dist)*200);
                    }
                }
            }
        }
        
        pazerville.update();
        pazerville.draw();
        
        delay(16);
    }
}

// ============================================================================
// EXAMPLE 6: Dynamic Network Growth
// ============================================================================

void example_dynamic_growth() {
    ILI9341Display tft;
    PazervilleDisplay pazerville(&tft);
    
    tft.initialize();
    pazerville.initialize();
    
    pazerville.setDamping(0.92f);
    
    // Start with single node
    pazerville.addNode(PAZERVILLE_WIDTH/2, PAZERVILLE_HEIGHT/2, 1.0f, COLOR_RED, 4);
    
    uint32_t growth_timer = 0;
    int max_nodes = 8;
    
    while (1) {
        pazerville.update();
        pazerville.draw();
        
        growth_timer++;
        
        // Add new node every second
        if (growth_timer > 100 && pazerville.getNodeCount() < max_nodes) {
            float x = 50 + (rand() % (PAZERVILLE_WIDTH - 100));
            float y = 50 + (rand() % (PAZERVILLE_HEIGHT - 100));
            pazerville.addNode(x, y, 1.0f, getColorFromIndex(pazerville.getNodeCount()), 3);
            
            // Connect to a random existing node
            if (pazerville.getNodeCount() > 1) {
                int existing = rand() % (pazerville.getNodeCount() - 1);
                pazerville.addEdge(existing, pazerville.getNodeCount() - 1, 0.2f, 60.0f);
            }
            
            growth_timer = 0;
            Serial.print("Added node ");
            Serial.println(pazerville.getNodeCount());
        }
        
        delay(16);
    }
}

// ============================================================================
// Helper Functions
// ============================================================================

void switchTopology(PazervilleDisplay *pazerville, int index) {
    // Clear by recreating (not implemented in current version)
    // In a real implementation, you'd have a reset function
    
    switch (index) {
        case 0:
            PazervilleExamples::createStarNetwork(pazerville, 5);
            break;
        case 1:
            PazervilleExamples::createRingNetwork(pazerville, 6);
            break;
        case 2:
            PazervilleExamples::createChainNetwork(pazerville, 5);
            break;
        case 3:
            PazervilleExamples::createGridNetwork(pazerville, 2, 2);
            break;
        case 4:
            PazervilleExamples::createBinaryTreeNetwork(pazerville);
            break;
    }
}

void handleControlCommand(PazervilleDisplay *pazerville, char cmd) {
    static float damping = 0.92f;
    static float spring_strength = 0.15f;
    
    switch (cmd) {
        case 'r':
            pazerville->randomizePositions();
            Serial.println("Positions randomized");
            break;
        case 's':
            pazerville->resetSimulation();
            Serial.println("Simulation reset");
            break;
        case 'd':
            damping += 0.01f;
            if (damping > 0.99f) damping = 0.99f;
            pazerville->setDamping(damping);
            Serial.print("Damping: ");
            Serial.println(damping);
            break;
        case 'u':
            damping -= 0.01f;
            if (damping < 0.80f) damping = 0.80f;
            pazerville->setDamping(damping);
            Serial.print("Damping: ");
            Serial.println(damping);
            break;
        case 'g':
            pazerville->setGravity(0.5f);
            Serial.println("Gravity enabled");
            break;
        case 'n':
            pazerville->setGravity(0.0f);
            Serial.println("Gravity disabled");
            break;
    }
}

uint16_t getColorFromFreq(int freq_index) {
    switch (freq_index % 6) {
        case 0: return COLOR_RED;
        case 1: return COLOR_ORANGE;
        case 2: return COLOR_YELLOW;
        case 3: return COLOR_GREEN;
        case 4: return COLOR_CYAN;
        case 5: return COLOR_BLUE;
        default: return COLOR_WHITE;
    }
}

// ============================================================================
// Usage in main():
// ============================================================================
/*
 * To use these examples, call them from setup() or main():
 * 
 * void setup() {
 *     Serial.begin(115200);
 *     delay(1000);
 *     
 *     // Choose one:
 *     // example_interactive_network();
 *     // example_topology_transitions();
 *     // example_cv_driven_physics();
 *     // example_music_visualization();
 *     // example_particle_burst();
 *     // example_dynamic_growth();
 * }
 */
