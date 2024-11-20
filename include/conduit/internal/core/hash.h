#ifndef CNDT_HASH
#define CNDT_HASH

namespace cndt {

// Combine the hash of the given type with an existing hash
template <typename T>
void hash_combine(std::size_t& seed, const T& value) {
    std::hash<T> hasher;
    seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

} // namespace cndt

#endif
