// Fixed pollution_signatures.h with proper class declaration
#ifndef POLLUTION_SIGNATURES_H
#define POLLUTION_SIGNATURES_H

#include <Arduino.h>

struct PollutionPattern {
    String name;
    int priority;
    float minIAQ, maxIAQ;
    float minVOC, maxVOC; // VOC range in ppm
    float minCO2, maxCO2;
    float minTemp, maxTemp;
    String description;
    bool isThreat;
};

class PollutionSignatures {
public:
    static const PollutionPattern* getSignatures();
    static int getNumSignatures();
    
    // Optional: Keep this method if you want it, but it's not used in the new detector
    static String detectPollutionSignature(float iaq, float voc, float co2, float temp, float humidity, bool inSpike) {
        // Simple fallback implementation
        return "LEGACY_DETECTION";
    }
};

#endif