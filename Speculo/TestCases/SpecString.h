#pragma once
#include <cstring>

namespace Speculo
{
    class SpecString
    {
    public:
        SpecString(char* value)
        {
            if (value == nullptr)
            {
                m_String = new char[1];
                m_String[0] = '\0';
            }
            else
            {
                m_String = new char[strlen(value) + 1];

                // Copy characters of value using strcpy.
                strcpy(m_String, value);
                m_String[strlen(value)] = '\0';
            }
        }

        void pop_back()
        {
            int stringLength = strlen(m_String);
            char* buffer = new char[stringLength];

            // Copy characters from m_String to the buffer.
            for (int i = 0; i < stringLength - 1; i++)
            {
                buffer[i] = m_String[i];
            }
            buffer[stringLength - 1] = '\0';

            // Assign the new string.
            *this = SpecString{ buffer };

            // Delete the allocated buffer.
            delete[] buffer;
        }

        void push_back(char charToPush)
        {
            int stringLength = strlen(m_String);
            char* buffer = new char[stringLength + 2];

            // Copy characters from m_String to the buffer.
            for (int i = 0; i < stringLength; i++)
            {
                buffer[i] = m_String[i];
            }
            buffer[stringLength] = charToPush;
            buffer[stringLength + 1] = '\0';

            // Assign the new string accordingly.
            *this = SpecString{ buffer };

            // Delete the buffer.
            delete[] buffer;
        }

    private:
        char* m_String;
    };
}