#pragma once

#include <tuple>    
#include <vector>   
#include <utility> 
#include <cstddef>  

namespace dod {

template <typename... Components>
    class World {
        std::tuple<std::vector<Components>...> storages;
        size_t entity_count = 0;

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
            std::apply([&](auto&... vecs) {
                ((vecs.emplace_back(std::forward<Args>(args))), ...);
            }, storages);
            ++entity_count;
        }
        
        template <typename T>
        T* data() {
            return std::get<std::vector<T>>(storages).data();
        }

        template <typename T>
        const T* data() const {
            return std::get<std::vector<T>>(storages).data();
        }

        size_t size() const { return entity_count; }
    };

} 