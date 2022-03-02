#pragma once

namespace Speculo
{
    struct Vector3
    {
        Vector3() = default;
        Vector3(float x, float y, float z) 
        { 
            this->x = x;  
            this->y = y;
            this->z = z;
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct Vector2
    {
        Vector2() = default;
        Vector2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        float x = 0.0f;
        float y = 0.0f;
    };
}