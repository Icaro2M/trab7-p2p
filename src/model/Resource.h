#pragma once

#include <string>
#include <utility>

struct Resource
{
    std::string id;
    std::string content;

    Resource() = default;

    Resource(std::string id, std::string content)
        : id(std::move(id)),
        content(std::move(content))
    {
    }
};