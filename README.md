This is a very early implementation of Google Protocol Buffers
for UnrealScript and UDK 3.

===============
SUPPORTED TYPES
===============

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

============
KNOWN ISSUES
============

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
