#pragma once

#include <tuple>    
#include <vector>   
#include <utility> 
#include <cstddef>  
#include <algorithm> 
#include <functional> 

namespace dodt {

template <typename... Components>
    class World {
    private:

        std::tuple<std::vector<Components>...> storages;    
        size_t entt_count = 0;                   

    public:   

        explicit World(size_t reserve_size = 0) {
            reserve(reserve_size); 
        }
        
        void reserve(size_t cap) {
            std::apply([cap](auto&... vec) { (vec.reserve(cap), ...); }, storages);                     
        }
        
        template <typename... Args>
        void add_entity(Args&&... args) {
            static_assert(sizeof...(Args) == sizeof...(Components),"Must provide values for all components");        
            std::apply([&](auto&... vecs) {((vecs.emplace_back(std::forward<Args>(args))), ...);}, storages);                
            ++entt_count;
        }
        
        template <typename T>
        T* data() {
            return std::get<std::vector<T>>(storages).data();
        }

        template <typename T>
        const T* data() const {
            return std::get<std::vector<T>>(storages).data();
        }

        template <typename... ComponentTypes, typename Func>
        void for_each(Func&& func) {
            auto&& vec_tuple = std::forward_as_tuple(std::get<std::vector<ComponentTypes>>(storages)...);
            
            for (size_t i = 0; i < entt_count; ++i) {                
                std::apply([&](auto&... vecs) {func(vecs[i]...);}, vec_tuple);
            }
        }

        template <typename... ComponentTypes, typename Func>
        void for_each(Func&& func) const {
            const auto& vec_tuple = std::tie(std::get<const std::vector<ComponentTypes>>(storages)...);
            for (size_t i = 0; i < entt_count; ++i) {                
                std::apply([&](const auto&... vecs) { func(vecs[i]...); }, vec_tuple);
            }
        }

        void remove_entity(size_t index) {
            if (index >= entt_count) return;
                        
            size_t last = entt_count - 1;
            if (index != last) {
                std::apply([&](auto&... vecs) {((vecs[index] = std::move(vecs[last])), ...);}, storages);                
            }
            
            std::apply([&](auto&... vecs) {((vecs.pop_back()), ...);}, storages);   
            --entt_count;         
        }
        
        void remove_entities(std::vector<size_t> indices) {
            std::sort(indices.begin(), indices.end(), std::greater<>());
            for (size_t idx : indices) {
                if (idx < entt_count) {
                    remove_entity(idx); 
                }
            }
        }


        size_t size() const { return entt_count; }
    };

} 