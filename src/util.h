#pragma once

//! Inspect the content of the @p buffer and remove any `/.` and `/..` parts.
//! @param buffer The buffer that will be inspected and modified in-place.
//! @param length The length of the buffer.
//! @return The new length of the buffer.
__attribute__((visibility("hidden"))) int buffer_derelativize(char* buffer, int length);
