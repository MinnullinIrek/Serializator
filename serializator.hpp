#pragma once
//#ifdef __cpp_lib_ranges 
#ifdef __cpp_lib_ranges
    #include <concepts>

    template<class P>
    concept SimplePair = requires(P p) {
	    p.first;
	    p.second;
    };

    template<typename T> size_t serialize(std::byte* buffer, const T& value);
    template<std::ranges::common_range T> size_t serialize(std::byte* buffer, const T& value);
    template<std::ranges::common_range T> size_t serialize(std::byte* buffer, const T& value);

    template<typename T> size_t deserialize(const std::byte* buffer, T& value);
    template<SimplePair T> size_t deserialize(const std::byte* buffer, T& value);
    template<std::ranges::common_range T> size_t deserialize(const std::byte* buffer, T& value);


    template<typename T>
    size_t serialize(std::byte* buffer, const T& value) {
	    auto valSize = sizeof(value);
	    std::memcpy(buffer, &value, sizeof(value));
	
	    return sizeof(value);
    }

    template<SimplePair T>
    size_t serialize(std::byte* buffer, const T& value) {
	    auto sz = serialize(buffer, value.first);
	    sz += serialize(buffer + sz, value.second);
	    return sz;
    }

    template<std::ranges::common_range T>
    size_t serialize(std::byte* buffer, const T& value) {
	    auto sz = value.size();
	    auto writedSize = serialize(buffer, value.size());
	    for (auto i = value.cbegin(); i != value.cend(); ++i)
	    {
		    writedSize += serialize(buffer + writedSize, *i);
	    }
	    return writedSize;
    }

    template<typename T>
    size_t deserialize(const std::byte* buffer, T& value) {
	    auto valSize = sizeof(value);
	    std::memcpy((void*)( & value), buffer, sizeof(value));
	    return sizeof(value);
    }

    template<SimplePair T>
    size_t deserialize(const std::byte* buffer, T& value) {
	    auto sz = deserialize(buffer, value.first);
	    sz += deserialize(buffer + sz, value.second);
	    return sz;
    }

    template<std::ranges::common_range T>
    size_t deserialize(const std::byte* buffer,  T& value) {
	    decltype(value.size()) sz = 0;
	    deserialize(buffer, sz);
	    decltype(value.size()) writedSize = sizeof(sz);
	    auto i = std::inserter(value, std::end(value));
	    for (auto j = 0; j < sz; ++j) {
		    typename T::value_type val;
		    writedSize += deserialize(buffer + writedSize, val);
		    i = val;
		    ++i;
	    }
	    return writedSize;
    }

#else

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


    template <typename T>
    struct is_pair {
        static const bool value = false;
    };

    template <typename T, typename V>
    struct is_pair<std::pair<T, V> > {
        static const bool value = true;
    };

    template <typename K, typename V>
    size_t serialize(std::byte* buffer, const std::pair<K, V>& value);
    template <typename T>
    size_t serialize(std::byte* buffer, const T& value);
    template <typename T>
    size_t deserialize(const std::byte* buffer, T& value);
    template <typename K, typename V>
    size_t deserialize(const std::byte* buffer, std::pair<K, V>& value);

    template <typename T>
    size_t serialize(std::byte* buffer, const T& value) {
        if constexpr (is_container<T>::value) {
            auto count = value.size();

            size_t ret = serialize(buffer, count);
            for (auto it = value.cbegin(); it != value.cend(); it = std::next(it)) {
                ret += serialize(buffer + ret, *it);
            }
            return ret;
        }
        std::memcpy(buffer, &value, sizeof(value));
        return sizeof(value);
    }

    template <typename K, typename V>
    size_t serialize(std::byte* buffer, const std::pair<K, V>& value) {
        auto ret = serialize<K>(buffer, value.first);
        ret += serialize<V>(buffer + ret, value.second);
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
        std::memcpy((void*)(&value), buffer, sizeof(value));
        return sizeof(value);
    }

    template <typename K, typename V>
    size_t deserialize(const std::byte* buffer, std::pair<K, V>& value) {
        auto sz = deserialize(buffer, value.first);
        sz += deserialize(buffer + sz, value.second);
        return sz;
    }

#endif