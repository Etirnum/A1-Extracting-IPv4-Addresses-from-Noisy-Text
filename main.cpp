#include <iostream>
#include <string>

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort);

bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool isTokenChar(char c)
{
    return isDigit(c) || c == '.' || c == ':';
}

// Reads a decimal number from token[index].
// maxDigits controls how many digits are allowed.
bool readNumber(const std::string& token, int& index,
                int maxDigits, int maxValue, int& value)
{
    int start = index;
    value = 0;

    while (index < (int)token.length() && isDigit(token[index]))
    {
        if (index - start >= maxDigits)
            return false;

        value = value * 10 + (token[index] - '0');

        if (value > maxValue)
            return false;

        index++;
    }

    int digits = index - start;

    if (digits == 0)
        return false;

    // Leading zero is only allowed when the number itself is 0.
    if (digits > 1 && token[start] == '0')
        return false;

    return true;
}

bool validToken(const std::string& token,
                unsigned long& address, int& port)
{
    int index = 0;
    int octets[4];

    // Read exactly four octets.
    for (int i = 0; i < 4; i++)
    {
        if (!readNumber(token, index, 3, 255, octets[i]))
            return false;

        if (i < 3)
        {
            if (index >= (int)token.length() || token[index] != '.')
                return false;

            index++;
        }
    }

    port = -1;

    // Anything after the fourth octet must be a valid :port.
    if (index < (int)token.length())
    {
        if (token[index] != ':')
            return false;

        index++;

        if (!readNumber(token, index, 5, 65535, port))
            return false;
    }

    // The whole token must have been consumed.
    if (index != (int)token.length())
        return false;

    address = 0;

    for (int i = 0; i < 4; i++)
    {
        address = address * 256 + octets[i];
    }

    return true;
}

bool extractIPv4(const std::string& str,
                 unsigned long& outAddress, int& outPort)
{
    outAddress = 0;
    outPort = -1;

    int i = 0;
    int validCount = 0;

    unsigned long foundAddress = 0;
    int foundPort = -1;

    while (i < (int)str.length())
    {
        // Skip garbage characters.
        if (!isTokenChar(str[i]))
        {
            i++;
            continue;
        }

        // Grab the entire candidate token.
        int start = i;

        while (i < (int)str.length() && isTokenChar(str[i]))
            i++;

        std::string token = str.substr(start, i - start);

        unsigned long address;
        int port;

        if (validToken(token, address, port))
        {
            validCount++;

            foundAddress = address;
            foundPort = port;

            // Only one address is allowed in a line.
            if (validCount > 1)
            {
                outAddress = 0;
                outPort = -1;
                return false;
            }
        }
    }

    if (validCount == 1)
    {
        outAddress = foundAddress;
        outPort = foundPort;
        return true;
    }

    return false;
}

int main()
{
    std::string input;

    while (true)
    {
        std::cout << "Enter a string (or 'END' to quit): ";
        std::getline(std::cin, input);

        if (input == "END")
        {
            std::cout << "Program terminated." << std::endl;
            break;
        }

        unsigned long address;
        int port;

        if (extractIPv4(input, address, port))
        {
            std::cout << "Extracted IPv4 address: "
                      << ((address >> 24) & 255) << "."
                      << ((address >> 16) & 255) << "."
                      << ((address >> 8) & 255) << "."
                      << (address & 255)
                      << " (decimal value: " << address
                      << ", port: ";

            if (port == -1)
                std::cout << "none";
            else
                std::cout << port;

            std::cout << ")" << std::endl;
        }
        else
        {
            std::cout << "Invalid input: no valid IPv4 address found"
                      << std::endl;
        }
    }

    return 0;
}