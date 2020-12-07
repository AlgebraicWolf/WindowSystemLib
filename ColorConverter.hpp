#ifndef COLOR_CONVERTER_HPP
#define COLOR_CONVERTER_HPP

#include <stdint.h>

#include <cmath>

// uint16_t abs(uint16_t x) {
//     if (x < 0) return -x;
//     return x;
// }

// Function that converts HSV to RGB: [0..360) x [0..100] x [0x100] -> uint32_t
uint32_t HSVtoHEX(uint16_t hue, uint8_t saturation, uint8_t value) {
    hue %= 360;

    double S = static_cast<double>(saturation) / 100.0;
    double V = static_cast<double>(value) / 100.0;
    double H = static_cast<double>(hue);

    double C = S * V;
    double X = C * (1 - std::abs(fmod((H / 60.0), 2) - 1));
    double m = V - C;

    double R = m;
    double G = m;
    double B = m;

    if (hue < 60) {
        R += C;
        G += X;
    } else if (hue < 120) {
        R += X;
        G += C;
    } else if (hue < 180) {
        G += C;
        B += X;
    } else if (hue < 240) {
        G += X;
        B += C;
    } else if (hue < 300) {
        B += C;
        R += X;
    } else if (hue < 360) {
        B += X;
        R += C;
    }

    // fprintf(stderr, "R: %lf, G: %lf, B: %lf\n", R, G, B);

    uint8_t r = R * 255;
    uint8_t g = G * 255;
    uint8_t b = B * 255;

    return r | (g << 8) | (b << 16) | (0xFF << 24);
}

#endif