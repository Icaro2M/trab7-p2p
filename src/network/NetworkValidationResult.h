#pragma once

#include <string>
#include <vector>

struct NetworkValidationResult
{
    bool valid = true;
    std::vector<std::string> errors;

    void addError(const std::string& error)
    {
        valid = false;
        errors.push_back(error);
    }
};