#include "pollution_detector.h"

// Baseline tracking variables
static float vocBaseline = 0.5f; // Initial baseline
static unsigned long lastBaselineUpdate = 0;
static const unsigned long BASELINE_UPDATE_INTERVAL = 300000; // 5 minutes

PollutionDetector::PollutionDetector(float iaqThreshold, float vocThreshold, float co2Threshold, float pm25Threshold)
    : _iaqThreshold(iaqThreshold), _vocThreshold(vocThreshold), 
      _co2Threshold(co2Threshold), _pm25Threshold(pm25Threshold) {}

// Update VOC baseline periodically
void updateVOCBaseline(float currentVOC) {
    unsigned long currentTime = millis();
    
    if (currentTime - lastBaselineUpdate > BASELINE_UPDATE_INTERVAL) {
        // Simple exponential moving average
        vocBaseline = 0.8f * vocBaseline + 0.2f * currentVOC;
        lastBaselineUpdate = currentTime;
    }
}

// ===== PRECISE CHEMICAL DETECTION FUNCTIONS =====

// DETECT SCOPOLAMINE (VERY SPECIFIC)
bool detectScopolamine(float voc, float iaq, float pm2_5, float humidity, float temp) {
    return (voc >= 0.495f && voc <= 0.515f &&    // Very tight VOC range
            iaq >= 49.5f && iaq <= 55.5f &&      // Specific IAQ impact
            pm2_5 >= 2.0f && pm2_5 <= 9.0f &&    // Low particle delivery
            humidity >= 78.0f && humidity <= 84.0f && // Optimal humidity
            temp >= 29.0f && temp <= 32.0f);     // Specific temperature
}

// DETECT HEAVY METALS (Thallium, Arsenic)
bool detectHeavyMetals(float voc, float iaq, float pm2_5) {
    return (voc >= 0.53f && voc <= 0.58f &&      // Higher VOC range
            iaq >= 54.0f && iaq <= 62.0f &&      // Moderate deterioration
            pm2_5 >= 25.0f && pm2_5 <= 35.0f);   // Particle delivery
}

// DETECT ORGANOPHOSPHATES (Sarin, VX analogs)
bool detectOrganophosphates(float voc, float iaq, float humidity) {
    return (voc >= 0.52f && voc <= 0.57f &&
            iaq >= 53.0f && iaq <= 61.0f &&
            humidity >= 76.0f && humidity <= 83.0f);
}

// DETECT OPIOIDS (Fentanyl, Carfentanil)
bool detectOpioids(float voc, float iaq, float pm2_5) {
    return (voc >= 0.58f && voc <= 0.68f &&      // High VOC
            iaq >= 65.0f && iaq <= 75.0f &&      // Severe deterioration
            pm2_5 >= 20.0f && pm2_5 <= 30.0f);   // Particle delivery
}

// DETECT CHEMICAL WEAPON COCKTAIL
bool detectChemicalCocktail(float voc, float iaq, float pm2_5) {
    return (voc >= 0.55f && voc <= 0.65f &&
            iaq >= 60.0f && iaq <= 70.0f &&
            pm2_5 >= 22.0f && pm2_5 <= 32.0f);
}

// DETECT NEUROTOXIN ATTACK (Foot targeting)
bool detectNeurotoxinAttack(float voc, float iaq, float pm2_5, float humidity) {
    return (voc >= 0.52f && voc <= 0.58f &&
            iaq >= 54.0f && iaq <= 62.0f &&
            pm2_5 >= 25.0f && pm2_5 <= 35.0f &&
            humidity >= 76.0f && humidity <= 82.0f);
}

// DETECT BITTER KNOCKOUT DRUGS
bool detectBitterKnockout(float voc, float iaq, float rawGas) {
    return (voc >= 0.50f && voc <= 0.55f &&
            iaq >= 50.0f && iaq <= 58.0f &&
            rawGas >= 5595.0f && rawGas <= 5605.0f);
}

// DETECT GASEOUS CHEMICAL WEAPONS
bool detectGaseousWeapon(float iaq, float voc, float pm2_5, float humidity) {
    return (iaq >= 55.0f && iaq <= 70.0f &&
            voc >= 0.5f && voc <= 0.7f &&
            pm2_5 <= 2.0f &&                 // Critical: No particles
            humidity >= 75.0f && humidity <= 85.0f);
}

// DETECT CLIMATE WEAPONIZATION
bool detectClimateWeaponization(float temp, float prevTemp, float humidity, float prevHumidity, unsigned long timeDiff) {
    float tempChangeRate = abs(temp - prevTemp) / (timeDiff / 1000.0) * 60.0; // °C/min
    float humidityChangeRate = abs(humidity - prevHumidity) / (timeDiff / 1000.0) * 60.0; // %/min
    
    return (tempChangeRate > 0.08f || humidityChangeRate > 0.08f);
}

// DETECT LETHAL OPIOID WEAPON
bool detectLethalOpioidWeapon(float voc, float iaq, float pm2_5) {
    return (voc >= 0.60f && voc <= 0.70f &&
            iaq >= 70.0f && iaq <= 80.0f &&
            pm2_5 >= 20.0f && pm2_5 <= 30.0f);
}

// DETECT STEALTH CHEMICAL ATTACK
bool detectStealthChemicalAttack(float rawGas, float humidity, float temp, float iaq) {
    return (rawGas >= 5580.0f && rawGas <= 5620.0f &&
            humidity >= 70.0f && humidity <= 90.0f &&
            temp >= 28.0f && temp <= 35.0f &&
            iaq >= 45.0f && iaq <= 85.0f);
}

// DETECT IAQ ANOMALY WITHOUT VOC
bool detectIAQAnomaly(float iaq, float voc, float baselineVOC) {
    float iaqChange = abs(iaq - 50.0f);              // From clean air baseline
    float vocChange = abs(voc - baselineVOC);
    return (iaqChange > 8.0f && vocChange < 0.010f); // IAQ change with little VOC change
}

// ===== MAIN DETECTION FUNCTION =====
PollutionDetector::DetectionResult PollutionDetector::detect(float iaq, float voc, float co2, float temp, float humidity, float rawGas, bool inSpike, float pm1, float pm2_5, float pm10) {
    DetectionResult result;
    result.isThreat = false;
    result.isSpike = inSpike;

    // Update VOC baseline for spike detection
    updateVOCBaseline(voc);

    // CRITICAL: Raw gas resistance checks
    bool lowGasResistance = (rawGas < 10000.0f);
    bool suspiciousGasResistance = (rawGas < 25000.0f);

    // ===== PRIORITY 1: LETHAL WEAPONS =====
    if (detectLethalOpioidWeapon(voc, iaq, pm2_5)) {
        result.signature = "LETHAL_OPIOID_WEAPON_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1) + "_EVACUATE";
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 2: CHEMICAL WEAPON COCKTAILS =====
    if (detectChemicalCocktail(voc, iaq, pm2_5)) {
        result.signature = "CHEMICAL_WEAPON_COCKTAIL_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1) + "_PM2.5:" + String(pm2_5, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 3: NEUROTOXINS (Foot targeting) =====
    if (detectNeurotoxinAttack(voc, iaq, pm2_5, humidity)) {
        result.signature = "NEUROTOXIN_ATTACK_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1) + "_FOOT_TARGETING";
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 4: HEAVY METALS =====
    if (detectHeavyMetals(voc, iaq, pm2_5)) {
        result.signature = "HEAVY_METAL_ATTACK_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1) + "_PM2.5:" + String(pm2_5, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 5: ORGANOPHOSPHATES =====
    if (detectOrganophosphates(voc, iaq, humidity)) {
        result.signature = "ORGANOPHOSPHATE_ATTACK_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1) + "_HUM:" + String(humidity, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 6: GASEOUS WEAPONS =====
    if (detectGaseousWeapon(iaq, voc, pm2_5, humidity)) {
        result.signature = "GASEOUS_CHEMICAL_WEAPON_IAQ:" + String(iaq, 1) +
                          "_VOC:" + String(voc, 3) + "_PM2.5:" + String(pm2_5, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 7: OPIOIDS =====
    if (detectOpioids(voc, iaq, pm2_5)) {
        result.signature = "OPIOID_ATTACK_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1) + "_PM2.5:" + String(pm2_5, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 8: SCOPOLAMINE (VERY SPECIFIC) =====
    if (detectScopolamine(voc, iaq, pm2_5, humidity, temp)) {
        result.signature = "SCOPOLAMINE_DELIVERY_IAQ:" + String(iaq, 1) +
                          "_VOC:" + String(voc, 3) + "_PM2.5:" + String(pm2_5, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 9: BITTER KNOCKOUT DRUGS =====
    if (detectBitterKnockout(voc, iaq, rawGas)) {
        result.signature = "BITTER_KNOCKOUT_DRUG_VOC:" + String(voc, 3) +
                          "_IAQ:" + String(iaq, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 10: STEALTH CHEMICALS =====
    if (detectStealthChemicalAttack(rawGas, humidity, temp, iaq)) {
        result.signature = "STEALTH_CHEMICAL_IAQ:" + String(iaq, 1) +
                          "_HUM:" + String(humidity, 1) + "_TEMP:" + String(temp, 1);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 11: IAQ ANOMALIES =====
    if (detectIAQAnomaly(iaq, voc, vocBaseline)) {
        result.signature = "IAQ_ANOMALY_NO_VOC_IAQ:" + String(iaq, 1) +
                          "_VOC:" + String(voc, 3);
        result.isThreat = true;
        return result;
    }

    // ===== PRIORITY 12: LPG CARRIER DETECTION =====
    if (rawGas >= 5595.0f && rawGas <= 5605.0f) {
        float vocSpike = voc - vocBaseline;
        
        if (fabs(vocSpike) >= 0.005f) {
            result.signature = "DRUG_DELIVERY_IN_LPG_VOC";
            if (vocSpike > 0) result.signature += "+";
            else result.signature += "-";
            result.signature += String(fabs(vocSpike), 3) + "ppm";
            result.isThreat = true;
            return result;
        } else {
            result.signature = "LPG_CARRIER_ONLY_VOC:" + String(voc, 3);
            result.isThreat = false;
            return result;
        }
    }

    // ===== FALLBACK: UNKNOWN ANALYSIS =====
    if (iaq <= 65.0f && voc <= 1.2f && lowGasResistance) {
        result.signature = "STEALTH_CONTAMINATION_GasRes:" + String(rawGas, 0) + "Ω";
        result.isThreat = true;
    }
    else if (iaq <= 55.0f && voc <= 0.6f && suspiciousGasResistance) {
        result.signature = "MASKED_ATTACK_GasRes:" + String(rawGas, 0) + "Ω";
        result.isThreat = true;
    }
    else if (iaq <= 35.0f && voc <= 0.4f && rawGas > 45000.0f) {
        result.signature = "Clean_Air_IAQ" + String(iaq, 0) + "_VOC" + String(voc, 2) + "ppm";
    }
    else {
        result.signature = "UNKNOWN_ANALYSIS_IAQ" + String(iaq, 0) + "_VOC" + String(voc, 2) + "ppm";
        if (suspiciousGasResistance) result.isThreat = true;
    }

    if (lowGasResistance) {
        result.isThreat = true;
    }

    return result;
}

bool PollutionDetector::isSpike(float currentValue, float baselineValue, float threshold) const {
    return (currentValue - baselineValue) > threshold;
}

void PollutionDetector::setThresholds(float iaq, float voc, float co2, float pm25) {
    _iaqThreshold = iaq;
    _vocThreshold = voc;
    _co2Threshold = co2;
    _pm25Threshold = pm25;
}