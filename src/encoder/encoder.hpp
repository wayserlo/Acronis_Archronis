#pragma once

#include <src/support/Istreams.hpp>

namespace archiver {
    
// Encode original to compressed
void Encode(IInputStream& original, IOutputStream& compressed);

// Decode compressed to original
void Decode(IInputStream& compressed, IOutputStream& original);

}   // namespace archiver