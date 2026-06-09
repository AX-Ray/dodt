#pragma once

#include <tuple>    
#include <vector>   
#include <utility> 
#include <cstddef>  

namespace dod {

template <typename... Components>
    class World {
    private:

        std::tuple<std::vector<Components>...> storages;        
        std::vector<bool> alive; //list? 

    public:   

        explicit World(size_t reserve_size = 0) {
            reserve(reserve_size); 
        }
        
        void reserve(size_t cap) {
            std::apply([cap](auto&... vec) { (vec.reserve(cap), ...); }, storages);
            alive.reserve(cap);
        }
        
        template <typename... Args>
        void add_entity(Args&&... args) {
            static_assert(sizeof...(Args) == sizeof...(Components),"Must provide values for all components");        
            std::apply([&](auto&... vecs) {
                ((vecs.emplace_back(std::forward<Args>(args))), ...);
            }, storages);   
            alive.push_back(true); 
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
            auto& vec_tuple = std::forward_as_tuple(
                std::get<std::vector<ComponentTypes>>(storages)...
            );
            
            for (size_t i = 0; i < alive.size(); ++i) {
                if (!alive[i]) continue;
                std::apply([&](auto&... vecs) {
                    func(vecs[i]...);
                }, vec_tuple);
            }
        }

        //void delete

        size_t size() const { return alive.size(); }
    };

} 