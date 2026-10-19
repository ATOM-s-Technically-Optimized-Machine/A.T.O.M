/*
** EPITECH PROJECT, 2026
** main
** File description:
** AssetTree
*/

#ifndef ASSETTREE_HPP_
    #define ASSETTREE_HPP_

#include <filesystem>

namespace editor {
namespace asset_tree {

    class AssetTree {
        public:
            AssetTree();
            ~AssetTree();

            void renderDirTree(const std::filesystem::path &path);

        protected:
        private:
    };

}}
#endif /* !ASSETTREE_HPP_ */
