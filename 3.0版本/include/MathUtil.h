#pragma once

#include "raylib.h"

float clampFloat(float value, float minValue, float maxValue);
float lerpFloat(float a, float b, float t);
float distanceX(Vector2 a, Vector2 b);
bool circleHitsRect(Vector2 center, float radius, Rectangle rect);
bool pointInRect(Vector2 point, Rectangle rect);
