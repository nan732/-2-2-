#include "MathUtil.h"

#include <algorithm>
#include <cmath>

float clampFloat(float value, float minValue, float maxValue)
{
    return std::max(minValue, std::min(value, maxValue));
}

float lerpFloat(float a, float b, float t)
{
    return a + (b - a) * clampFloat(t, 0.0f, 1.0f);
}

float distanceX(Vector2 a, Vector2 b)
{
    return std::fabs(a.x - b.x);
}

bool circleHitsRect(Vector2 center, float radius, Rectangle rect)
{
    const float closestX = clampFloat(center.x, rect.x, rect.x + rect.width);
    const float closestY = clampFloat(center.y, rect.y, rect.y + rect.height);
    const float dx = center.x - closestX;
    const float dy = center.y - closestY;
    return dx * dx + dy * dy <= radius * radius;
}

bool pointInRect(Vector2 point, Rectangle rect)
{
    return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y && point.y <= rect.y + rect.height;
}
