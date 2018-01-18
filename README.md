# protobuf-unrealscript 

This is a very early implementation of Google Protocol Buffers for UnrealScript and UDK 3.

# Supported Types

- float
- int32
- uint32
- sint32
- fixed32
- sfixed32
- bool
- string
- nested types
- repeated types

# Known Issues

- Floats are not properly supported due to limitations in 
  UnrealScript.
- Required types are not enforced.
- None of the *64 types are supported because UnrealScript does
  not contain any 64-bit types.
- Enumerations are not supported.
- Default values are not supported.
- Groups are not supported but this was by design since they are 
  deprecated.
- Extensions are not supported.

# Note

This code is provided as is with no warrant in the unlikely chance that someone might find it interesting. It was used in a real game so it does work fairly well but I only implemented the features we needed and any issues were just worked around. I no longer have any interest in working on this anymore (especially since UDK has moved to C++).
