#pragma once

#define UNITFIGURE 10.f

float Meters(float value) { return value * UNITFIGURE; }
float Centimeters(float value) { return Meters(value) / 100.f; }