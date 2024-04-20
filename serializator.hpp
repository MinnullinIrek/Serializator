#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <list>
#include <set>
#include <unordered_set>




template <typename T>
struct is_container {
    static const bool value = false;
};

template <typename T, typename Alloc>
struct is_container<std::vector<T, Alloc> > {
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_container<std::list<T, Alloc> > {
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_container<std::map<T, Alloc> > {
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_container<std::unordered_map<T, Alloc> > {
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_container<std::unordered_set<T, Alloc> > {
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_container<std::set<T, Alloc> > {
    static const bool value = true;
};

template<>
struct is_container<std::string> {
    static const bool value = true;
};


//template <typename T>
//struct is_pair {
//    static const bool value = false;
//};

//template <typename T, typename V>
//struct is_container<std::pair<T, V> > {
//    static const bool value = true;
//};

template <typename T>
size_t serialize(std::byte* buffer, const T& value) {
    if constexpr(is_container<T>::value) {
        auto count = value.size();
        std::cout << "is_container" <<std::endl;
                
        size_t ret = serialize(buffer, count);
        for (auto it = value.cbegin(); it != value.cend(); it = std::next(it)) {
            ret += serialize(buffer + ret, *it);
        }
        return ret;
    }
    std::memcpy(buffer, &value, sizeof(value));
    return sizeof(value);
}

//template <>
//size_t serialize<std::string>(std::byte* buffer, const std::string& value) {
//    std::memcpy(buffer, &value, sizeof(value));
//    std::memcpy(buffer + sizeof(int32_t), &value, value.size());
//    return sizeof(int32_t) + value.size();
//}

template <typename K, typename V>
size_t serialize(std::byte* buffer, const std::pair<K, V>& value) {
    std::cout << "serialize buffer";
    
    auto ret = serialize<K>(buffer, value.first);
    ret += serialize<V>(buffer + sizeof(value.first), value.second);
    return ret;
}

template <typename T>
size_t deserialize(const std::byte* buffer, T& value) {
    if constexpr (is_container<T>::value) {
        size_t readedCount = 0;
        auto count = value.size();
        //std::memcpy(&count, buffer, sizeof(count));
        readedCount = deserialize(buffer, count);

        auto back = std::inserter(value, value.end());
        for (int i = 0; i < count; ++i) {
            typename T::value_type element;
            readedCount += deserialize(buffer + readedCount, element);
            back = element;
            
            ++back;
            
            
        }
        return readedCount;
    }
    std::memcpy(&value, buffer, sizeof(value));
    return sizeof(value);
}

