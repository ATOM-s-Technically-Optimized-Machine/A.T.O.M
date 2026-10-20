
#ifndef ASSET_REGISTRY_HPP
#define ASSET_REGISTRY_HPP

#include "ResourceIncludes.hpp"
#include "ResourcesUtils.hpp"
#include <initializer_list>
#include <string_view>
#include <filesystem>
#include <unordered_map>
#include <string>

namespace resources {

class AssetRegistry {
public:

    void registerKind(AssetKind kind, std::initializer_list<std::string_view> exts)
    {
        for (std::string_view ext : exts)
            m_by_ext[toLower(std::string(ext))] = kind;
    }

    AssetKind classify(const std::filesystem::path &p) const
    {
        std::string ext = toLower(p.extension().string());

        auto val = m_by_ext.find(ext);
        if (val == m_by_ext.end())
            return AssetKind::Unknown;
        return val->second;
    }

private:
    std::unordered_map<std::string, AssetKind> m_by_ext;
};

}

#endif /* ASSET_REGISTRY_HPP */