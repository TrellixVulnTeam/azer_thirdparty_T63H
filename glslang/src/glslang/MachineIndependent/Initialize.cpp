//
//Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
//Copyright (C) 2012-2013 LunarG, Inc.
//
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

//
// Create strings that declare built-in definitions, add built-ins programmatically 
// that cannot be expressed in the strings, and establish mappings between
// built-in functions and operators.
//
// Where to put a built-in:
//   TBuiltIns::initialize(version,profile)       context-independent textual built-ins; add them to the right string
//   TBuiltIns::initialize(resources,...)         context-dependent textual built-ins; add them to the right string
//   IdentifyBuiltIns(...,symbolTable)            context-independent programmatic additions/mappings to the symbol table,
//                                                including identifying what extensions are needed if a version does not allow a symbol
//   IdentifyBuiltIns(...,symbolTable, resources) context-dependent programmatic additions/mappings to the symbol table,
//                                                including identifying what extensions are needed if a version does not allow a symbol
//

#include "../Include/intermediate.h"
#include "Initialize.h"

namespace glslang {

// TODO: ARB_Compatability: do full extension support
bool ARBCompatibility = true;

const bool ForwardCompatibility = false;

inline bool IncludeLegacy(int version, EProfile profile)
{
    return profile != EEsProfile && (version <= 130 || ARBCompatibility || profile == ECompatibilityProfile);
}

TBuiltIns::TBuiltIns()
{
    // Set up textual representations for making all the permutations
    // of texturing/imaging functions.
    prefixes[EbtFloat] =  "";
    prefixes[EbtInt]   = "i";
    prefixes[EbtUint]  = "u";
    postfixes[2] = "2";
    postfixes[3] = "3";
    postfixes[4] = "4";

    // Map from symbolic class of texturing dimension to numeric dimensions.
    dimMap[Esd1D] = 1;
    dimMap[Esd2D] = 2;
    dimMap[EsdRect] = 2;
    dimMap[Esd3D] = 3;
    dimMap[EsdCube] = 3;
    dimMap[EsdBuffer] = 1;
}

TBuiltIns::~TBuiltIns()
{
}

//
// Add all context-independent built-in functions and variables that are present
// for the given version and profile.  Share common ones across stages, otherwise
// make stage-specific entries.
//
// Most built-ins variables can be added as simple text strings.  Some need to
// be added programmatically, which is done later in IdentifyBuiltIns() below.
//
void TBuiltIns::initialize(int version, EProfile profile)
{
    //============================================================================
    //
    // Prototypes for built-in functions seen by both vertex and fragment shaders.
    //
    //============================================================================

    //
    // Angle and Trigonometric Functions.
    //
    commonBuiltins.append(
        "float radians(float degrees);"
        "vec2  radians(vec2  degrees);"
        "vec3  radians(vec3  degrees);"
        "vec4  radians(vec4  degrees);"
                 
        "float degrees(float radians);"
        "vec2  degrees(vec2  radians);"
        "vec3  degrees(vec3  radians);"
        "vec4  degrees(vec4  radians);"
                 
        "float sin(float angle);"
        "vec2  sin(vec2  angle);"
        "vec3  sin(vec3  angle);"
        "vec4  sin(vec4  angle);"
                 
        "float cos(float angle);"
        "vec2  cos(vec2  angle);"
        "vec3  cos(vec3  angle);"
        "vec4  cos(vec4  angle);"
                 
        "float tan(float angle);"
        "vec2  tan(vec2  angle);"
        "vec3  tan(vec3  angle);"
        "vec4  tan(vec4  angle);"
                 
        "float asin(float x);"
        "vec2  asin(vec2  x);"
        "vec3  asin(vec3  x);"
        "vec4  asin(vec4  x);"
                 
        "float acos(float x);"
        "vec2  acos(vec2  x);"
        "vec3  acos(vec3  x);"
        "vec4  acos(vec4  x);"
                 
        "float atan(float y, float x);"
        "vec2  atan(vec2  y, vec2  x);"
        "vec3  atan(vec3  y, vec3  x);"
        "vec4  atan(vec4  y, vec4  x);"
                 
        "float atan(float y_over_x);"
        "vec2  atan(vec2  y_over_x);"
        "vec3  atan(vec3  y_over_x);"
        "vec4  atan(vec4  y_over_x);"
            
        "\n");

    if (version >= 130) {
        commonBuiltins.append(
            "float sinh(float angle);"
            "vec2  sinh(vec2  angle);"
            "vec3  sinh(vec3  angle);"
            "vec4  sinh(vec4  angle);"
                 
            "float cosh(float angle);"
            "vec2  cosh(vec2  angle);"
            "vec3  cosh(vec3  angle);"
            "vec4  cosh(vec4  angle);"
                 
            "float tanh(float angle);"
            "vec2  tanh(vec2  angle);"
            "vec3  tanh(vec3  angle);"
            "vec4  tanh(vec4  angle);"
                 
            "float asinh(float x);"
            "vec2  asinh(vec2  x);"
            "vec3  asinh(vec3  x);"
            "vec4  asinh(vec4  x);"
                 
            "float acosh(float x);"
            "vec2  acosh(vec2  x);"
            "vec3  acosh(vec3  x);"
            "vec4  acosh(vec4  x);"
                 
            "float atanh(float y_over_x);"
            "vec2  atanh(vec2  y_over_x);"
            "vec3  atanh(vec3  y_over_x);"
            "vec4  atanh(vec4  y_over_x);"
            
            "\n");
    }

    //
    // Exponential Functions.
    //
    commonBuiltins.append(
        "float pow(float x, float y);"
        "vec2  pow(vec2  x, vec2  y);"
        "vec3  pow(vec3  x, vec3  y);"
        "vec4  pow(vec4  x, vec4  y);"
                 
        "float exp(float x);"
        "vec2  exp(vec2  x);"
        "vec3  exp(vec3  x);"
        "vec4  exp(vec4  x);"
                 
        "float log(float x);"
        "vec2  log(vec2  x);"
        "vec3  log(vec3  x);"
        "vec4  log(vec4  x);"
                 
        "float exp2(float x);"
        "vec2  exp2(vec2  x);"
        "vec3  exp2(vec3  x);"
        "vec4  exp2(vec4  x);"
                 
        "float log2(float x);"
        "vec2  log2(vec2  x);"
        "vec3  log2(vec3  x);"
        "vec4  log2(vec4  x);"
                 
        "float sqrt(float x);"
        "vec2  sqrt(vec2  x);"
        "vec3  sqrt(vec3  x);"
        "vec4  sqrt(vec4  x);"
                 
        "float inversesqrt(float x);"
        "vec2  inversesqrt(vec2  x);"
        "vec3  inversesqrt(vec3  x);"
        "vec4  inversesqrt(vec4  x);"
            
        "\n");

    //
    // Common Functions.
    //
    commonBuiltins.append(
        "float abs(float x);"
        "vec2  abs(vec2  x);"
        "vec3  abs(vec3  x);"
        "vec4  abs(vec4  x);"
                 
        "float sign(float x);"
        "vec2  sign(vec2  x);"
        "vec3  sign(vec3  x);"
        "vec4  sign(vec4  x);"
                 
        "float floor(float x);"
        "vec2  floor(vec2  x);"
        "vec3  floor(vec3  x);"
        "vec4  floor(vec4  x);"
                 
        "float ceil(float x);"
        "vec2  ceil(vec2  x);"
        "vec3  ceil(vec3  x);"
        "vec4  ceil(vec4  x);"
                 
        "float fract(float x);"
        "vec2  fract(vec2  x);"
        "vec3  fract(vec3  x);"
        "vec4  fract(vec4  x);"
                 
        "float mod(float x, float y);"
        "vec2  mod(vec2  x, float y);"
        "vec3  mod(vec3  x, float y);"
        "vec4  mod(vec4  x, float y);"
        "vec2  mod(vec2  x, vec2  y);"
        "vec3  mod(vec3  x, vec3  y);"
        "vec4  mod(vec4  x, vec4  y);"
                 
        "float min(float x, float y);"
        "vec2  min(vec2  x, float y);"
        "vec3  min(vec3  x, float y);"
        "vec4  min(vec4  x, float y);"
        "vec2  min(vec2  x, vec2  y);"
        "vec3  min(vec3  x, vec3  y);"
        "vec4  min(vec4  x, vec4  y);"
                 
        "float max(float x, float y);"
        "vec2  max(vec2  x, float y);"
        "vec3  max(vec3  x, float y);"
        "vec4  max(vec4  x, float y);"
        "vec2  max(vec2  x, vec2  y);"
        "vec3  max(vec3  x, vec3  y);"
        "vec4  max(vec4  x, vec4  y);"
                 
        "float clamp(float x, float minVal, float maxVal);"
        "vec2  clamp(vec2  x, float minVal, float maxVal);"
        "vec3  clamp(vec3  x, float minVal, float maxVal);"
        "vec4  clamp(vec4  x, float minVal, float maxVal);"
        "vec2  clamp(vec2  x, vec2  minVal, vec2  maxVal);"
        "vec3  clamp(vec3  x, vec3  minVal, vec3  maxVal);"
        "vec4  clamp(vec4  x, vec4  minVal, vec4  maxVal);"
                 
        "float mix(float x, float y, float a);"
        "vec2  mix(vec2  x, vec2  y, float a);"
        "vec3  mix(vec3  x, vec3  y, float a);"
        "vec4  mix(vec4  x, vec4  y, float a);"
        "vec2  mix(vec2  x, vec2  y, vec2  a);"
        "vec3  mix(vec3  x, vec3  y, vec3  a);"
        "vec4  mix(vec4  x, vec4  y, vec4  a);"

        "float step(float edge, float x);"
        "vec2  step(vec2  edge, vec2  x);"
        "vec3  step(vec3  edge, vec3  x);"
        "vec4  step(vec4  edge, vec4  x);"
        "vec2  step(float edge, vec2  x);"
        "vec3  step(float edge, vec3  x);"
        "vec4  step(float edge, vec4  x);"
                 
        "float smoothstep(float edge0, float edge1, float x);"
        "vec2  smoothstep(vec2  edge0, vec2  edge1, vec2  x);"
        "vec3  smoothstep(vec3  edge0, vec3  edge1, vec3  x);"
        "vec4  smoothstep(vec4  edge0, vec4  edge1, vec4  x);"
        "vec2  smoothstep(float edge0, float edge1, vec2  x);"
        "vec3  smoothstep(float edge0, float edge1, vec3  x);"
        "vec4  smoothstep(float edge0, float edge1, vec4  x);"
            
        "\n");

    if (version >= 130) {
        commonBuiltins.append(
            "  int abs(  int x);"
            "ivec2 abs(ivec2 x);"
            "ivec3 abs(ivec3 x);"
            "ivec4 abs(ivec4 x);"

            "  int sign(  int x);"
            "ivec2 sign(ivec2 x);"
            "ivec3 sign(ivec3 x);"
            "ivec4 sign(ivec4 x);"

            "float trunc(float x);"
            "vec2  trunc(vec2  x);"
            "vec3  trunc(vec3  x);"
            "vec4  trunc(vec4  x);"
                     
            "float round(float x);"
            "vec2  round(vec2  x);"
            "vec3  round(vec3  x);"
            "vec4  round(vec4  x);"
                     
            "float roundEven(float x);"
            "vec2  roundEven(vec2  x);"
            "vec3  roundEven(vec3  x);"
            "vec4  roundEven(vec4  x);"
                     
            "float modf(float, out float);"
            "vec2  modf(vec2,  out vec2 );"
            "vec3  modf(vec3,  out vec3 );"
            "vec4  modf(vec4,  out vec4 );"
                     
            "  int min(int    x, int y);"
            "ivec2 min(ivec2  x, int y);"
            "ivec3 min(ivec3  x, int y);"
            "ivec4 min(ivec4  x, int y);"
            "ivec2 min(ivec2  x, ivec2  y);"
            "ivec3 min(ivec3  x, ivec3  y);"
            "ivec4 min(ivec4  x, ivec4  y);"
                     
            " uint min(uint   x, uint y);"
            "uvec2 min(uvec2  x, uint y);"
            "uvec3 min(uvec3  x, uint y);"
            "uvec4 min(uvec4  x, uint y);"
            "uvec2 min(uvec2  x, uvec2  y);"
            "uvec3 min(uvec3  x, uvec3  y);"
            "uvec4 min(uvec4  x, uvec4  y);"
                     
            "  int max(int    x, int y);"
            "ivec2 max(ivec2  x, int y);"
            "ivec3 max(ivec3  x, int y);"
            "ivec4 max(ivec4  x, int y);"
            "ivec2 max(ivec2  x, ivec2  y);"
            "ivec3 max(ivec3  x, ivec3  y);"
            "ivec4 max(ivec4  x, ivec4  y);"
                     
            " uint max(uint   x, uint y);"
            "uvec2 max(uvec2  x, uint y);"
            "uvec3 max(uvec3  x, uint y);"
            "uvec4 max(uvec4  x, uint y);"
            "uvec2 max(uvec2  x, uvec2  y);"
            "uvec3 max(uvec3  x, uvec3  y);"
            "uvec4 max(uvec4  x, uvec4  y);"
                     
            "int    clamp(int x, int minVal, int maxVal);"
            "ivec2  clamp(ivec2  x, int minVal, int maxVal);"
            "ivec3  clamp(ivec3  x, int minVal, int maxVal);"
            "ivec4  clamp(ivec4  x, int minVal, int maxVal);"
            "ivec2  clamp(ivec2  x, ivec2  minVal, ivec2  maxVal);"
            "ivec3  clamp(ivec3  x, ivec3  minVal, ivec3  maxVal);"
            "ivec4  clamp(ivec4  x, ivec4  minVal, ivec4  maxVal);"
                     
            "uint   clamp(uint x, uint minVal, uint maxVal);"
            "uvec2  clamp(uvec2  x, uint minVal, uint maxVal);"
            "uvec3  clamp(uvec3  x, uint minVal, uint maxVal);"
            "uvec4  clamp(uvec4  x, uint minVal, uint maxVal);"
            "uvec2  clamp(uvec2  x, uvec2  minVal, uvec2  maxVal);"
            "uvec3  clamp(uvec3  x, uvec3  minVal, uvec3  maxVal);"
            "uvec4  clamp(uvec4  x, uvec4  minVal, uvec4  maxVal);"
                     
            "float mix(float x, float y, bool  a);"
            "vec2  mix(vec2  x, vec2  y, bvec2 a);"
            "vec3  mix(vec3  x, vec3  y, bvec3 a);"
            "vec4  mix(vec4  x, vec4  y, bvec4 a);"
                     
            "bool  isnan(float x);"
            "bvec2 isnan(vec2  x);"
            "bvec3 isnan(vec3  x);"
            "bvec4 isnan(vec4  x);"
                     
            "bool  isinf(float x);"
            "bvec2 isinf(vec2  x);"
            "bvec3 isinf(vec3  x);"
            "bvec4 isinf(vec4  x);"
            
            "\n");
    }

    if ((profile == EEsProfile && version >= 300) ||
        (profile != EEsProfile && version >= 330)) {
        commonBuiltins.append(
            "int   floatBitsToInt(float value);"
            "ivec2 floatBitsToInt(vec2  value);"
            "ivec3 floatBitsToInt(vec3  value);"
            "ivec4 floatBitsToInt(vec4  value);"
                     
            "uint  floatBitsToUint(float value);"
            "uvec2 floatBitsToUint(vec2  value);"
            "uvec3 floatBitsToUint(vec3  value);"
            "uvec4 floatBitsToUint(vec4  value);"
                     
            "float intBitsToFloat(int   value);"
            "vec2  intBitsToFloat(ivec2 value);"
            "vec3  intBitsToFloat(ivec3 value);"
            "vec4  intBitsToFloat(ivec4 value);"
                     
            "float uintBitsToFloat(uint  value);"
            "vec2  uintBitsToFloat(uvec2 value);"
            "vec3  uintBitsToFloat(uvec3 value);"
            "vec4  uintBitsToFloat(uvec4 value);"
            
            "\n");
    }

    if ((profile == EEsProfile && version >= 300) ||
        (profile != EEsProfile && version >= 400)) {
        commonBuiltins.append(
            "highp uint packSnorm2x16 (vec2);"
            "highp vec2 unpackSnorm2x16 (highp uint);"
            "highp uint packUnorm2x16 (vec2);"
            "highp vec2 unpackUnorm2x16 (highp uint);"
            "highp uint packHalf2x16(mediump vec2);"
            "mediump vec2 unpackHalf2x16(highp uint);"
            
            "\n");
    }

    //
    // Geometric Functions.
    //
    commonBuiltins.append(
        "float length(float x);"
        "float length(vec2  x);"
        "float length(vec3  x);"
        "float length(vec4  x);"
                 
        "float distance(float p0, float p1);"
        "float distance(vec2  p0, vec2  p1);"
        "float distance(vec3  p0, vec3  p1);"
        "float distance(vec4  p0, vec4  p1);"
                 
        "float dot(float x, float y);"
        "float dot(vec2  x, vec2  y);"
        "float dot(vec3  x, vec3  y);"
        "float dot(vec4  x, vec4  y);"
                 
        "vec3 cross(vec3 x, vec3 y);"
        "float normalize(float x);"
        "vec2  normalize(vec2  x);"
        "vec3  normalize(vec3  x);"
        "vec4  normalize(vec4  x);"
                 
        "float faceforward(float N, float I, float Nref);"
        "vec2  faceforward(vec2  N, vec2  I, vec2  Nref);"
        "vec3  faceforward(vec3  N, vec3  I, vec3  Nref);"
        "vec4  faceforward(vec4  N, vec4  I, vec4  Nref);"
                 
        "float reflect(float I, float N);"
        "vec2  reflect(vec2  I, vec2  N);"
        "vec3  reflect(vec3  I, vec3  N);"
        "vec4  reflect(vec4  I, vec4  N);"
                 
        "float refract(float I, float N, float eta);"
        "vec2  refract(vec2  I, vec2  N, float eta);"
        "vec3  refract(vec3  I, vec3  N, float eta);"
        "vec4  refract(vec4  I, vec4  N, float eta);"
            
        "\n");

    //
    // Matrix Functions.
    //
    commonBuiltins.append(
        "mat2 matrixCompMult(mat2 x, mat2 y);"
        "mat3 matrixCompMult(mat3 x, mat3 y);"
        "mat4 matrixCompMult(mat4 x, mat4 y);"
            
        "\n");

    if (version >= 120) {
        commonBuiltins.append(
            "mat2   outerProduct(vec2 c, vec2 r);"
            "mat3   outerProduct(vec3 c, vec3 r);"
            "mat4   outerProduct(vec4 c, vec4 r);"
            "mat2x3 outerProduct(vec3 c, vec2 r);"
            "mat3x2 outerProduct(vec2 c, vec3 r);"
            "mat2x4 outerProduct(vec4 c, vec2 r);"
            "mat4x2 outerProduct(vec2 c, vec4 r);"
            "mat3x4 outerProduct(vec4 c, vec3 r);"
            "mat4x3 outerProduct(vec3 c, vec4 r);"
                     
            "mat2   transpose(mat2   m);"
            "mat3   transpose(mat3   m);"
            "mat4   transpose(mat4   m);"
            "mat2x3 transpose(mat3x2 m);"
            "mat3x2 transpose(mat2x3 m);"
            "mat2x4 transpose(mat4x2 m);"
            "mat4x2 transpose(mat2x4 m);"
            "mat3x4 transpose(mat4x3 m);"
            "mat4x3 transpose(mat3x4 m);"

            "mat2x3 matrixCompMult(mat2x3, mat2x3);"
            "mat2x4 matrixCompMult(mat2x4, mat2x4);"
            "mat3x2 matrixCompMult(mat3x2, mat3x2);"
            "mat3x4 matrixCompMult(mat3x4, mat3x4);"
            "mat4x2 matrixCompMult(mat4x2, mat4x2);"
            "mat4x3 matrixCompMult(mat4x3, mat4x3);"          
            
            "\n");

        if (version >= 150) {
            commonBuiltins.append(
                "float determinant(mat2 m);"
                "float determinant(mat3 m);"
                "float determinant(mat4 m);"
                         
                "mat2 inverse(mat2 m);"
                "mat3 inverse(mat3 m);"
                "mat4 inverse(mat4 m);"
            
                "\n");
        }
    }

    //
    // Vector relational functions.
    //
    commonBuiltins.append(
        "bvec2 lessThan(vec2 x, vec2 y);"
        "bvec3 lessThan(vec3 x, vec3 y);"
        "bvec4 lessThan(vec4 x, vec4 y);"
                 
        "bvec2 lessThan(ivec2 x, ivec2 y);"
        "bvec3 lessThan(ivec3 x, ivec3 y);"
        "bvec4 lessThan(ivec4 x, ivec4 y);"
                 
        "bvec2 lessThanEqual(vec2 x, vec2 y);"
        "bvec3 lessThanEqual(vec3 x, vec3 y);"
        "bvec4 lessThanEqual(vec4 x, vec4 y);"
                 
        "bvec2 lessThanEqual(ivec2 x, ivec2 y);"
        "bvec3 lessThanEqual(ivec3 x, ivec3 y);"
        "bvec4 lessThanEqual(ivec4 x, ivec4 y);"
                 
        "bvec2 greaterThan(vec2 x, vec2 y);"
        "bvec3 greaterThan(vec3 x, vec3 y);"
        "bvec4 greaterThan(vec4 x, vec4 y);"
                 
        "bvec2 greaterThan(ivec2 x, ivec2 y);"
        "bvec3 greaterThan(ivec3 x, ivec3 y);"
        "bvec4 greaterThan(ivec4 x, ivec4 y);"
                 
        "bvec2 greaterThanEqual(vec2 x, vec2 y);"
        "bvec3 greaterThanEqual(vec3 x, vec3 y);"
        "bvec4 greaterThanEqual(vec4 x, vec4 y);"
                 
        "bvec2 greaterThanEqual(ivec2 x, ivec2 y);"
        "bvec3 greaterThanEqual(ivec3 x, ivec3 y);"
        "bvec4 greaterThanEqual(ivec4 x, ivec4 y);"
                 
        "bvec2 equal(vec2 x, vec2 y);"
        "bvec3 equal(vec3 x, vec3 y);"
        "bvec4 equal(vec4 x, vec4 y);"
                 
        "bvec2 equal(ivec2 x, ivec2 y);"
        "bvec3 equal(ivec3 x, ivec3 y);"
        "bvec4 equal(ivec4 x, ivec4 y);"
                 
        "bvec2 equal(bvec2 x, bvec2 y);"
        "bvec3 equal(bvec3 x, bvec3 y);"
        "bvec4 equal(bvec4 x, bvec4 y);"
                 
        "bvec2 notEqual(vec2 x, vec2 y);"
        "bvec3 notEqual(vec3 x, vec3 y);"
        "bvec4 notEqual(vec4 x, vec4 y);"
                 
        "bvec2 notEqual(ivec2 x, ivec2 y);"
        "bvec3 notEqual(ivec3 x, ivec3 y);"
        "bvec4 notEqual(ivec4 x, ivec4 y);"
                 
        "bvec2 notEqual(bvec2 x, bvec2 y);"
        "bvec3 notEqual(bvec3 x, bvec3 y);"
        "bvec4 notEqual(bvec4 x, bvec4 y);"
                 
        "bool any(bvec2 x);"
        "bool any(bvec3 x);"
        "bool any(bvec4 x);"
                 
        "bool all(bvec2 x);"
        "bool all(bvec3 x);"
        "bool all(bvec4 x);"
                 
        "bvec2 not(bvec2 x);"
        "bvec3 not(bvec3 x);"
        "bvec4 not(bvec4 x);"
                 
        "\n");

    if (version >= 130) {
        commonBuiltins.append(
            "bvec2 lessThan(uvec2 x, uvec2 y);"
            "bvec3 lessThan(uvec3 x, uvec3 y);"
            "bvec4 lessThan(uvec4 x, uvec4 y);"
                 
            "bvec2 lessThanEqual(uvec2 x, uvec2 y);"
            "bvec3 lessThanEqual(uvec3 x, uvec3 y);"
            "bvec4 lessThanEqual(uvec4 x, uvec4 y);"
                 
            "bvec2 greaterThan(uvec2 x, uvec2 y);"
            "bvec3 greaterThan(uvec3 x, uvec3 y);"
            "bvec4 greaterThan(uvec4 x, uvec4 y);"
                 
            "bvec2 greaterThanEqual(uvec2 x, uvec2 y);"
            "bvec3 greaterThanEqual(uvec3 x, uvec3 y);"
            "bvec4 greaterThanEqual(uvec4 x, uvec4 y);"
                 
            "bvec2 equal(uvec2 x, uvec2 y);"
            "bvec3 equal(uvec3 x, uvec3 y);"
            "bvec4 equal(uvec4 x, uvec4 y);"

            "bvec2 notEqual(uvec2 x, uvec2 y);"
            "bvec3 notEqual(uvec3 x, uvec3 y);"
            "bvec4 notEqual(uvec4 x, uvec4 y);"                 
            
            "\n");
    }

    //
    // Original-style texture functions existing in all stages.
    // (Per-stage functions below.)
    //
    if ((profile == EEsProfile && version == 100) ||
         profile == ECompatibilityProfile ||
        (profile == ECoreProfile && version < 420) ||
         profile == ENoProfile) {
        commonBuiltins.append(
            "vec4 texture2D(sampler2D, vec2);"

            "vec4 texture2DProj(sampler2D, vec3);"
            "vec4 texture2DProj(sampler2D, vec4);"

            "vec4 texture3D(sampler3D, vec3);"     // OES_texture_3D, but caught by keyword check
            "vec4 texture3DProj(sampler3D, vec4);" // OES_texture_3D, but caught by keyword check

            "vec4 textureCube(samplerCube, vec3);"
            
            "\n");
    }

    if ( profile == ECompatibilityProfile ||
        (profile == ECoreProfile && version < 420) ||
         profile == ENoProfile) {
        commonBuiltins.append(
            "vec4 texture1D(sampler1D, float);"

            "vec4 texture1DProj(sampler1D, vec2);"
            "vec4 texture1DProj(sampler1D, vec4);"
                     
            "vec4 shadow1D(sampler1DShadow, vec3);"
            "vec4 shadow2D(sampler2DShadow, vec3);"
            "vec4 shadow1DProj(sampler1DShadow, vec4);"
            "vec4 shadow2DProj(sampler2DShadow, vec4);"
            
            "\n");
    }

    if (profile == EEsProfile) {        
        commonBuiltins.append(
            "vec4 texture2D(samplerExternalOES, vec2 coord);"  // GL_OES_EGL_image_external, caught by keyword check
            "vec4 texture2DProj(samplerExternalOES, vec3);"    // GL_OES_EGL_image_external, caught by keyword check
            "vec4 texture2DProj(samplerExternalOES, vec4);"    // GL_OES_EGL_image_external, caught by keyword check
            "vec4 texture2DGradEXT(sampler2D, vec2, vec2, vec2);"      // GL_EXT_shader_texture_lod
            "vec4 texture2DProjGradEXT(sampler2D, vec3, vec2, vec2);"  // GL_EXT_shader_texture_lod
            "vec4 texture2DProjGradEXT(sampler2D, vec4, vec2, vec2);"  // GL_EXT_shader_texture_lod
            "vec4 textureCubeGradEXT(samplerCube, vec3, vec3, vec3);"  // GL_EXT_shader_texture_lod

            "\n");
    }

    //
    // Noise functions.
    //
    if (profile != EEsProfile) {
        commonBuiltins.append(
            "float noise1(float x);"
            "float noise1(vec2  x);"
            "float noise1(vec3  x);"
            "float noise1(vec4  x);"
                     
            "vec2 noise2(float x);"
            "vec2 noise2(vec2  x);"
            "vec2 noise2(vec3  x);"
            "vec2 noise2(vec4  x);"
                     
            "vec3 noise3(float x);"
            "vec3 noise3(vec2  x);"
            "vec3 noise3(vec3  x);"
            "vec3 noise3(vec4  x);"
                     
            "vec4 noise4(float x);"
            "vec4 noise4(vec2  x);"
            "vec4 noise4(vec3  x);"
            "vec4 noise4(vec4  x);"
                     
            "\n");
    }

    //============================================================================
    //
    // Prototypes for built-in functions seen by vertex shaders only.
    // (Except legacy lod functions, where it depends which release they are
    // vertex only.)
    //
    //============================================================================

    //
    // Geometric Functions.
    //
    if (IncludeLegacy(version, profile))
        stageBuiltins[EShLangVertex].append("vec4 ftransform();");

    //
    // Original-style texture Functions with lod.
    //
    TString* s;
    if (version < 130)
        s = &stageBuiltins[EShLangVertex];
    else
        s = &commonBuiltins;
    if ((profile == EEsProfile && version == 100) ||
         profile == ECompatibilityProfile ||
        (profile == ECoreProfile && version < 420) ||
         profile == ENoProfile) {             
        s->append(
            "vec4 texture2DLod(sampler2D, vec2, float);"
            "vec4 texture2DProjLod(sampler2D, vec3, float);"
            "vec4 texture2DProjLod(sampler2D, vec4, float);"
            "vec4 texture3DLod(sampler3D, vec3, float);"         // OES_texture_3D, but caught by keyword check
            "vec4 texture3DProjLod(sampler3D, vec4, float);"     // OES_texture_3D, but caught by keyword check
            "vec4 textureCubeLod(samplerCube, vec3, float);"
            
            "\n");
    }
    if ( profile == ECompatibilityProfile ||
        (profile == ECoreProfile && version < 420) ||
         profile == ENoProfile) {
        s->append(
            "vec4 texture1DLod(sampler1D, float, float);"
            "vec4 texture1DProjLod(sampler1D, vec2, float);"
            "vec4 texture1DProjLod(sampler1D, vec4, float);"
            "vec4 shadow1DLod(sampler1DShadow, vec3, float);"
            "vec4 shadow2DLod(sampler2DShadow, vec3, float);"
            "vec4 shadow1DProjLod(sampler1DShadow, vec4, float);"
            "vec4 shadow2DProjLod(sampler2DShadow, vec4, float);"
            
            "\n");
    }

    if (profile != EEsProfile && version >= 150) {
        //============================================================================
        //
        // Prototypes for built-in functions seen by geometry shaders only.
        //
        //============================================================================

        if (version >= 400) {
            stageBuiltins[EShLangGeometry].append(
                "void EmitStreamVertex(int);"
                "void EndStreamPrimitive(int);"
                );
        }
        stageBuiltins[EShLangGeometry].append(
            "void EmitVertex();"
            "void EndPrimitive();"
            
            "\n");
    }
    if (profile != EEsProfile) {
        //============================================================================
        //
        // Prototypes for all control functions.
        //
        //============================================================================

        if (version >= 150)
            stageBuiltins[EShLangTessControl].append(
                "void barrier();"
                );
        if (version >= 430)
            stageBuiltins[EShLangCompute].append(
                "void barrier();"
                );

        if (version >= 420)
            commonBuiltins.append(
                "void memoryBarrier();"
                );
        if (version >= 430) {
            commonBuiltins.append(
                "void memoryBarrierAtomicCounter();"
                "void memoryBarrierBuffer();"
                "void memoryBarrierImage();"
                );
            stageBuiltins[EShLangCompute].append(
                "void memoryBarrierShared();"
                "void groupMemoryBarrier();"
                );
        }
    }
    //============================================================================
    //
    // Prototypes for built-in functions seen by fragment shaders only.
    //
    //============================================================================

    //
    // Original-style texture Functions with bias.
    //
    if (profile != EEsProfile || version == 100) {
        stageBuiltins[EShLangFragment].append(
            "vec4 texture2D(sampler2D, vec2, float);"
            "vec4 texture2DProj(sampler2D, vec3, float);"
            "vec4 texture2DProj(sampler2D, vec4, float);"
            "vec4 texture3D(sampler3D, vec3, float);"        // OES_texture_3D
            "vec4 texture3DProj(sampler3D, vec4, float);"    // OES_texture_3D
            "vec4 textureCube(samplerCube, vec3, float);"
            
            "\n");
    }
    if (profile != EEsProfile && version > 100) {
        stageBuiltins[EShLangFragment].append(
            "vec4 texture1D(sampler1D, float, float);"
            "vec4 texture1DProj(sampler1D, vec2, float);"
            "vec4 texture1DProj(sampler1D, vec4, float);"
            "vec4 shadow1D(sampler1DShadow, vec3, float);"
            "vec4 shadow2D(sampler2DShadow, vec3, float);"
            "vec4 shadow1DProj(sampler1DShadow, vec4, float);"
            "vec4 shadow2DProj(sampler2DShadow, vec4, float);"
            
            "\n");
    }
    if (profile == EEsProfile) {
        stageBuiltins[EShLangFragment].append(
            "vec4 texture2DLodEXT(sampler2D, vec2, float);"      // GL_EXT_shader_texture_lod
            "vec4 texture2DProjLodEXT(sampler2D, vec3, float);"  // GL_EXT_shader_texture_lod
            "vec4 texture2DProjLodEXT(sampler2D, vec4, float);"  // GL_EXT_shader_texture_lod
            "vec4 textureCubeLodEXT(samplerCube, vec3, float);"  // GL_EXT_shader_texture_lod
            
            "\n");
    }

	stageBuiltins[EShLangFragment].append(
        "float dFdx(float p);"
        "vec2  dFdx(vec2  p);"
        "vec3  dFdx(vec3  p);"
        "vec4  dFdx(vec4  p);"
                 
        "float dFdy(float p);"
        "vec2  dFdy(vec2  p);"
        "vec3  dFdy(vec3  p);"
        "vec4  dFdy(vec4  p);"
                 
        "float fwidth(float p);"
        "vec2  fwidth(vec2  p);"
        "vec3  fwidth(vec3  p);"
        "vec4  fwidth(vec4  p);"
            
        "\n");

    //============================================================================
    //
    // Standard Uniforms
    //
    //============================================================================

    //
    // Depth range in window coordinates, p. 33
    //
    commonBuiltins.append(
        "struct gl_DepthRangeParameters {"
        );
    if (profile == EEsProfile) {
        commonBuiltins.append(
            "highp float near;"   // n
            "highp float far;"    // f
            "highp float diff;"   // f - n
            );
    } else {
        commonBuiltins.append(
            "float near;"  // n
            "float far;"   // f
            "float diff;"  // f - n
            );
    }
    commonBuiltins.append(
        "};"
        "uniform gl_DepthRangeParameters gl_DepthRange;"            
        "\n");

    if (IncludeLegacy(version, profile)) {
        //
        // Matrix state. p. 31, 32, 37, 39, 40.
        //
        commonBuiltins.append(
            "uniform mat4  gl_ModelViewMatrix;"
            "uniform mat4  gl_ProjectionMatrix;"
            "uniform mat4  gl_ModelViewProjectionMatrix;"

            //
            // Derived matrix state that provides inverse and transposed versions
            // of the matrices above.
            //
            "uniform mat3  gl_NormalMatrix;"

            "uniform mat4  gl_ModelViewMatrixInverse;"
            "uniform mat4  gl_ProjectionMatrixInverse;"
            "uniform mat4  gl_ModelViewProjectionMatrixInverse;"
                     
            "uniform mat4  gl_ModelViewMatrixTranspose;"
            "uniform mat4  gl_ProjectionMatrixTranspose;"
            "uniform mat4  gl_ModelViewProjectionMatrixTranspose;"
                     
            "uniform mat4  gl_ModelViewMatrixInverseTranspose;"
            "uniform mat4  gl_ProjectionMatrixInverseTranspose;"
            "uniform mat4  gl_ModelViewProjectionMatrixInverseTranspose;"

            //
            // Normal scaling p. 39.
            //
            "uniform float gl_NormalScale;"

            //
            // Point Size, p. 66, 67.
            //
            "struct gl_PointParameters {"
                "float size;"
                "float sizeMin;"
                "float sizeMax;"
                "float fadeThresholdSize;"
                "float distanceConstantAttenuation;"
                "float distanceLinearAttenuation;"
                "float distanceQuadraticAttenuation;"
            "};"

            "uniform gl_PointParameters gl_Point;"

            //
            // Material State p. 50, 55.
            //
            "struct gl_MaterialParameters {"
                "vec4  emission;"    // Ecm
                "vec4  ambient;"     // Acm
                "vec4  diffuse;"     // Dcm
                "vec4  specular;"    // Scm
                "float shininess;"   // Srm
            "};"
            "uniform gl_MaterialParameters  gl_FrontMaterial;"
            "uniform gl_MaterialParameters  gl_BackMaterial;"

            //
            // Light State p 50, 53, 55.
            //
            "struct gl_LightSourceParameters {"
                "vec4  ambient;"             // Acli
                "vec4  diffuse;"             // Dcli
                "vec4  specular;"            // Scli
                "vec4  position;"            // Ppli
                "vec4  halfVector;"          // Derived: Hi
                "vec3  spotDirection;"       // Sdli
                "float spotExponent;"        // Srli
                "float spotCutoff;"          // Crli
                                                        // (range: [0.0,90.0], 180.0)
                "float spotCosCutoff;"       // Derived: cos(Crli)
                                                        // (range: [1.0,0.0],-1.0)
                "float constantAttenuation;" // K0
                "float linearAttenuation;"   // K1
                "float quadraticAttenuation;"// K2
            "};"


            "struct gl_LightModelParameters {"
                "vec4  ambient;"       // Acs
            "};"

            "uniform gl_LightModelParameters  gl_LightModel;"

            //
            // Derived state from products of light and material.
            //
            "struct gl_LightModelProducts {"
                "vec4  sceneColor;"     // Derived. Ecm + Acm * Acs
            "};"

            "uniform gl_LightModelProducts gl_FrontLightModelProduct;"
            "uniform gl_LightModelProducts gl_BackLightModelProduct;"

            "struct gl_LightProducts {"
                "vec4  ambient;"        // Acm * Acli
                "vec4  diffuse;"        // Dcm * Dcli
                "vec4  specular;"       // Scm * Scli
            "};"

            //
            // Fog p. 161
            //
            "struct gl_FogParameters {"
                "vec4  color;"
                "float density;"
                "float start;"
                "float end;"
                "float scale;"   //  1 / (gl_FogEnd - gl_FogStart)
            "};"

            "uniform gl_FogParameters gl_Fog;"
            
            "\n");
    }

    //============================================================================
    //
    // Define the interface to the compute shader.
    //
    //============================================================================

    if (version >= 430) {
        stageBuiltins[EShLangCompute].append(
            "in uvec3 gl_NumWorkGroups;"
            // TODO: 4.3 functionality: compute shader: constant with no initializer                "const uvec3 gl_WorkGroupSize;"

            "in uvec3 gl_WorkGroupID;"
            "in uvec3 gl_LocalInvocationID;"

            "in uvec3 gl_GlobalInvocationID;"
            "in uint gl_LocalInvocationIndex;"
            
            "\n");
    }

    //============================================================================
    //
    // Define the interface to the vertex shader.
    //
    //============================================================================
    
    if (profile != EEsProfile) {
        if (version < 130) {
            stageBuiltins[EShLangVertex].append(
                "attribute vec4  gl_Color;"
                "attribute vec4  gl_SecondaryColor;"
                "attribute vec3  gl_Normal;"
                "attribute vec4  gl_Vertex;"
                "attribute vec4  gl_MultiTexCoord0;"
                "attribute vec4  gl_MultiTexCoord1;"
                "attribute vec4  gl_MultiTexCoord2;"
                "attribute vec4  gl_MultiTexCoord3;"
                "attribute vec4  gl_MultiTexCoord4;"
                "attribute vec4  gl_MultiTexCoord5;"
                "attribute vec4  gl_MultiTexCoord6;"
                "attribute vec4  gl_MultiTexCoord7;"
                "attribute float gl_FogCoord;"            
                "\n");
        } else if (IncludeLegacy(version, profile)) {
            stageBuiltins[EShLangVertex].append(
                "in vec4  gl_Color;"
                "in vec4  gl_SecondaryColor;"
                "in vec3  gl_Normal;"
                "in vec4  gl_Vertex;"
                "in vec4  gl_MultiTexCoord0;"
                "in vec4  gl_MultiTexCoord1;"
                "in vec4  gl_MultiTexCoord2;"
                "in vec4  gl_MultiTexCoord3;"
                "in vec4  gl_MultiTexCoord4;"
                "in vec4  gl_MultiTexCoord5;"
                "in vec4  gl_MultiTexCoord6;"
                "in vec4  gl_MultiTexCoord7;"
                "in float gl_FogCoord;"            
                "\n");
        }

        if (version < 150) {
            if (version < 130) {
                stageBuiltins[EShLangVertex].append(
                    "        vec4  gl_ClipVertex;"       // needs qualifier fixed later
                    "varying vec4  gl_FrontColor;"
                    "varying vec4  gl_BackColor;"
                    "varying vec4  gl_FrontSecondaryColor;"
                    "varying vec4  gl_BackSecondaryColor;"
                    "varying vec4  gl_TexCoord[];"
                    "varying float gl_FogFragCoord;"
                    "\n");
            } else if (IncludeLegacy(version, profile)) {
                stageBuiltins[EShLangVertex].append(
                    "    vec4  gl_ClipVertex;"       // needs qualifier fixed later
                    "out vec4  gl_FrontColor;"
                    "out vec4  gl_BackColor;"
                    "out vec4  gl_FrontSecondaryColor;"
                    "out vec4  gl_BackSecondaryColor;"
                    "out vec4  gl_TexCoord[];"
                    "out float gl_FogFragCoord;"
                    "\n");
            }
            stageBuiltins[EShLangVertex].append(
                "vec4 gl_Position;"   // needs qualifier fixed later
                "float gl_PointSize;" // needs qualifier fixed later
                );

            if (version == 130 || version == 140)
                stageBuiltins[EShLangVertex].append(
                    "out float gl_ClipDistance[];"
                    );
        } else {
            // version >= 150
            stageBuiltins[EShLangVertex].append(
                "out gl_PerVertex {"
                    "vec4 gl_Position;"     // needs qualifier fixed later
                    "float gl_PointSize;"   // needs qualifier fixed later
                    "float gl_ClipDistance[];"
                    );
            if (IncludeLegacy(version, profile))
                stageBuiltins[EShLangVertex].append(
                    "vec4 gl_ClipVertex;"   // needs qualifier fixed later
                    "vec4 gl_FrontColor;"
                    "vec4 gl_BackColor;"
                    "vec4 gl_FrontSecondaryColor;"
                    "vec4 gl_BackSecondaryColor;"
                    "vec4 gl_TexCoord[];"
                    "float gl_FogFragCoord;"
                    );
            stageBuiltins[EShLangVertex].append(
                "};"
                "\n");
        }
        if (version >= 130)
            stageBuiltins[EShLangVertex].append(
                "int gl_VertexID;"     // needs qualifier fixed later
                );
        if (version >= 140)
            stageBuiltins[EShLangVertex].append(
                "int gl_InstanceID;"   // needs qualifier fixed later
                );
    } else {
        // ES profile
        if (version == 100) {
            stageBuiltins[EShLangVertex].append(
                "highp   vec4  gl_Position;"  // needs qualifier fixed later
                "mediump float gl_PointSize;" // needs qualifier fixed later
                );
        } else {
            stageBuiltins[EShLangVertex].append(
                "highp int gl_VertexID;"     // needs qualifier fixed later
                "highp int gl_InstanceID;"   // needs qualifier fixed later

                "highp vec4  gl_Position;"   // needs qualifier fixed later
                "highp float gl_PointSize;"  // needs qualifier fixed later
                );
        }
    }

    //============================================================================
    //
    // Define the interface to the geometry shader.
    //
    //============================================================================

    if (profile == ECoreProfile || profile == ECompatibilityProfile) {
        stageBuiltins[EShLangGeometry].append(
            "in gl_PerVertex {"
                "vec4 gl_Position;"
                "float gl_PointSize;"
                "float gl_ClipDistance[];"
                );
        if (profile == ECompatibilityProfile)
            stageBuiltins[EShLangGeometry].append(
                "vec4 gl_ClipVertex;"
                "vec4 gl_FrontColor;"
                "vec4 gl_BackColor;"
                "vec4 gl_FrontSecondaryColor;"
                "vec4 gl_BackSecondaryColor;"
                "vec4 gl_TexCoord[];"
                "float gl_FogFragCoord;"
                );
        stageBuiltins[EShLangGeometry].append(
            "} gl_in[];"

            "in int gl_PrimitiveIDIn;"
            "out gl_PerVertex {"
                "vec4 gl_Position;"
                "float gl_PointSize;"
                "float gl_ClipDistance[];"
                "\n");
        if (version >= 400 && profile == ECompatibilityProfile)
            stageBuiltins[EShLangGeometry].append(
                "vec4 gl_ClipVertex;"
                "vec4 gl_FrontColor;"
                "vec4 gl_BackColor;"
                "vec4 gl_FrontSecondaryColor;"
                "vec4 gl_BackSecondaryColor;"
                "vec4 gl_TexCoord[];"
                "float gl_FogFragCoord;"
                );
        stageBuiltins[EShLangGeometry].append(
            "};"

            "out int gl_PrimitiveID;"
            "out int gl_Layer;"
            "\n");

        if (version < 400 && profile == ECompatibilityProfile)
            stageBuiltins[EShLangGeometry].append(
            "out vec4 gl_ClipVertex;"
            );

        if (version >= 400)
            stageBuiltins[EShLangGeometry].append(
            "in int gl_InvocationID;"
            );
        if (version >= 410)
            stageBuiltins[EShLangGeometry].append(
            "out int gl_ViewportIndex;"
            );
    }

    //============================================================================
    //
    // Define the interface to the tessellation control shader.
    //
    //============================================================================

    if (version >= 150) {
        // Note:  "in gl_PerVertex {...} gl_in[gl_MaxPatchVertices];" is declared in initialize() below,
        // as it depends on the resource sizing of gl_MaxPatchVertices.

        stageBuiltins[EShLangTessControl].append(
            "in int gl_PatchVerticesIn;"
            "in int gl_PrimitiveID;"
            "in int gl_InvocationID;"

            "out gl_PerVertex {"
                "vec4 gl_Position;"
                "float gl_PointSize;"
                "float gl_ClipDistance[];"
                );
        if (profile == ECompatibilityProfile)
            stageBuiltins[EShLangTessControl].append(
                "vec4 gl_ClipVertex;"
                "vec4 gl_FrontColor;"
                "vec4 gl_BackColor;"
                "vec4 gl_FrontSecondaryColor;"
                "vec4 gl_BackSecondaryColor;"
                "vec4 gl_TexCoord[];"
                "float gl_FogFragCoord;"
                );
        stageBuiltins[EShLangTessControl].append(
            "} gl_out[];"

            "patch out float gl_TessLevelOuter[4];"
            "patch out float gl_TessLevelInner[2];"
            "\n");
    }

    //============================================================================
    //
    // Define the interface to the tessellation evaluation shader.
    //
    //============================================================================

    if (version >= 150) {
        // Note:  "in gl_PerVertex {...} gl_in[gl_MaxPatchVertices];" is declared in initialize() below,
        // as it depends on the resource sizing of gl_MaxPatchVertices.

        stageBuiltins[EShLangTessEvaluation].append(
            "in int gl_PatchVerticesIn;"
            "in int gl_PrimitiveID;"
            "in vec3 gl_TessCoord;"

            "patch in float gl_TessLevelOuter[4];"
            "patch in float gl_TessLevelInner[2];"
                
            "out gl_PerVertex {"
                "vec4 gl_Position;"
                "float gl_PointSize;"
                "float gl_ClipDistance[];"
            );
        if (version >= 400 && profile == ECompatibilityProfile)
            stageBuiltins[EShLangTessEvaluation].append(
                "vec4 gl_ClipVertex;"
                "vec4 gl_FrontColor;"
                "vec4 gl_BackColor;"
                "vec4 gl_FrontSecondaryColor;"
                "vec4 gl_BackSecondaryColor;"
                "vec4 gl_TexCoord[];"
                "float gl_FogFragCoord;"
                );
        stageBuiltins[EShLangTessEvaluation].append(
            "};"
            "\n");
    }

    //============================================================================
    //
    // Define the interface to the fragment shader.
    //
    //============================================================================

    if (profile != EEsProfile) {

        stageBuiltins[EShLangFragment].append(
            "vec4  gl_FragCoord;"   // needs qualifier fixed later
            "bool  gl_FrontFacing;" // needs qualifier fixed later
            "float gl_FragDepth;"   // needs qualifier fixed later
            );
        if (version >= 120)
            stageBuiltins[EShLangFragment].append(
                "vec2 gl_PointCoord;"  // needs qualifier fixed later
                );
        if (IncludeLegacy(version, profile) || (! ForwardCompatibility && version < 420))
            stageBuiltins[EShLangFragment].append(
                "vec4 gl_FragColor;"   // needs qualifier fixed later
                );

        if (version < 130) {
            stageBuiltins[EShLangFragment].append(
                "varying vec4  gl_Color;"
                "varying vec4  gl_SecondaryColor;"
                "varying vec4  gl_TexCoord[];"
                "varying float gl_FogFragCoord;"
                );
        } else {
            stageBuiltins[EShLangFragment].append(
                "in float gl_ClipDistance[];"
                );

            if (IncludeLegacy(version, profile)) {
                if (version < 150)
                    stageBuiltins[EShLangFragment].append(
                        "in float gl_FogFragCoord;"
                        "in vec4  gl_TexCoord[];"
                        "in vec4  gl_Color;"
                        "in vec4  gl_SecondaryColor;"
                        );
                else
                    stageBuiltins[EShLangFragment].append(
                        "in gl_PerFragment {"
                            "in float gl_FogFragCoord;"
                            "in vec4  gl_TexCoord[];"
                            "in vec4  gl_Color;"
                            "in vec4  gl_SecondaryColor;"
                        "};"
                        );
            }
        }

        if (version >= 150)
            stageBuiltins[EShLangFragment].append(
                "flat in int gl_PrimitiveID;"
                );

        if (version >= 400)
            stageBuiltins[EShLangFragment].append(
                "flat in  int  gl_SampleID;"
                "     in  vec2 gl_SamplePosition;"
                "flat in  int  gl_SampleMaskIn[];"
                "     out int  gl_SampleMask[];"
                );

        if (version >= 430)
            stageBuiltins[EShLangFragment].append(
                "flat in int gl_Layer;"
                "flat in int gl_ViewportIndex;"
                );
    } else {
        // ES profile

        if (version == 100)
            stageBuiltins[EShLangFragment].append(
                "mediump vec4 gl_FragCoord;"    // needs qualifier fixed later
                "        bool gl_FrontFacing;"  // needs qualifier fixed later
                "mediump vec4 gl_FragColor;"    // needs qualifier fixed later
                "mediump vec2 gl_PointCoord;"   // needs qualifier fixed later
                );
        else if (version == 300)
            stageBuiltins[EShLangFragment].append(
                "highp   vec4  gl_FragCoord;"    // needs qualifier fixed later
                "        bool  gl_FrontFacing;"  // needs qualifier fixed later
                "mediump vec2  gl_PointCoord;"   // needs qualifier fixed later
                "highp   float gl_FragDepth;"    // needs qualifier fixed later
                );
        stageBuiltins[EShLangFragment].append(
            "highp float gl_FragDepthEXT;"       // GL_EXT_frag_depth
            );
    }
    stageBuiltins[EShLangFragment].append("\n");

    if (version >= 130)
        add2ndGenerationSamplingImaging(version, profile);

    //printf("%s\n", commonBuiltins.c_str();
}

//
// Helper function for initialize(), to add the second set of names for texturing, 
// when adding context-independent built-in functions.
//
void TBuiltIns::add2ndGenerationSamplingImaging(int version, EProfile profile)
{
    //
    // In this function proper, enumerate the types, then calls the next set of functions
    // to enumerate all the uses for that type.
    //

    TBasicType bTypes[3] = { EbtFloat, EbtInt, EbtUint };

    // enumerate all the types
    for (int image = 0; image <= 1; ++image) { // loop over "bool" image vs sampler

        if (image > 0 && version < 420)
            continue;

        for (int shadow = 0; shadow <= 1; ++shadow) { // loop over "bool" shadow or not
            for (int ms = 0; ms <=1; ++ms) {

                if ((ms || image) && shadow)
                    continue;
                if (ms && (profile == EEsProfile || version < 150))
                    continue;

                for (int arrayed = 0; arrayed <= 1; ++arrayed) { // loop over "bool" arrayed or not
                    for (int dim = Esd1D; dim < EsdNumDims; ++dim) { // 1D, 2D, ..., buffer

                        if ((dim == Esd1D || dim == EsdRect) && profile == EEsProfile)
                            continue;
                        if (dim != Esd2D && ms)
                            continue;
                        if ((dim == Esd3D || dim == EsdRect) && arrayed)
                            continue;
                        if (dim == Esd3D && shadow)
                            continue;
                        if (dim == EsdCube && arrayed && (profile == EEsProfile || version < 130))
                            continue;
                        if (dim == EsdBuffer && (profile == EEsProfile || version < 140))
                            continue;
                        if (dim == EsdBuffer && (shadow || arrayed || ms))
                            continue;

                        for (int bType = 0; bType < 3; ++bType) { // float, int, uint results

                            if (shadow && bType > 0)
                                continue;

                            if (dim == EsdRect && version < 140 && bType > 0)
                                continue;

                            //
                            // Now, make all the function prototypes for the type we just built...
                            //

                            TSampler sampler;
                            sampler.set(bTypes[bType], (TSamplerDim)dim, arrayed ? true : false,
                                                                         shadow  ? true : false,
                                                                         ms      ? true : false);
                            if (image)
                                sampler.image = true;

                            TString typeName = sampler.getString();

                            addQueryFunctions(sampler, typeName, version, profile);

                            if (image)
                                addImageFunctions(sampler, typeName, version, profile);
                            else {
                                addSamplingFunctions(sampler, typeName, version, profile);
                                if (version >= 130)
                                    addGatherFunctions(sampler, typeName, version, profile);
                            }
                        }
                    }
                }
            }
        }
    }
}

//
// Helper function for add2ndGenerationSamplingImaging(), 
// when adding context-independent built-in functions.
//
// Add all the query functions for the given type.
//
void TBuiltIns::addQueryFunctions(TSampler sampler, TString& typeName, int version, EProfile profile)
{
    //
    // textureSize
    //

    if (version < 430 && sampler.image)
        return;

    if (profile == EEsProfile)
        commonBuiltins.append("highp ");
    int dims = dimMap[sampler.dim] + (sampler.arrayed ? 1 : 0) - (sampler.dim == EsdCube ? 1 : 0);
    if (dims == 1)
        commonBuiltins.append("int");
    else {
        commonBuiltins.append("ivec");
        commonBuiltins.append(postfixes[dims]);
    }
    if (sampler.image)
        commonBuiltins.append(" imageSize(");
    else
        commonBuiltins.append(" textureSize(");
    commonBuiltins.append(typeName);
    if (! sampler.image && sampler.dim != EsdRect && sampler.dim != EsdBuffer && ! sampler.ms)
        commonBuiltins.append(",int);\n");
    else
        commonBuiltins.append(");\n");
}

//
// Helper function for add2ndGenerationSamplingImaging(), 
// when adding context-independent built-in functions.
//
// Add all the image access functions for the given type.
//
void TBuiltIns::addImageFunctions(TSampler sampler, TString& typeName, int version, EProfile profile)
{
    // TODO: 4.2 Functionality: imaging functions
}

//
// Helper function for add2ndGenerationSamplingImaging(), 
// when adding context-independent built-in functions.
//
// Add all the texture lookup functions for the given type.
//
void TBuiltIns::addSamplingFunctions(TSampler sampler, TString& typeName, int version, EProfile profile)
{
    //
    // texturing
    //
    for (int proj = 0; proj <= 1; ++proj) { // loop over "bool" projective or not

        if (proj && (sampler.dim == EsdCube || sampler.dim == EsdBuffer || sampler.arrayed || sampler.ms))
            continue;

        for (int lod = 0; lod <= 1; ++lod) {

            if (lod && (sampler.dim == EsdBuffer || sampler.dim == EsdRect || sampler.ms))
                continue;
            if (lod && sampler.dim == Esd2D && sampler.arrayed && sampler.shadow)
                continue;
            if (lod && sampler.dim == EsdCube && sampler.shadow)
                continue;

            for (int bias = 0; bias <= 1; ++bias) {

                if (bias && (lod || sampler.ms))
                    continue;
                if (bias && sampler.dim == Esd2D && sampler.shadow && sampler.arrayed)
                    continue;
                if (bias && (sampler.dim == EsdRect || sampler.dim == EsdBuffer))
                    continue;

                for (int offset = 0; offset <= 1; ++offset) { // loop over "bool" offset or not

                    if (proj + offset + bias + lod > 3)
                        continue;
                    if (offset && (sampler.dim == EsdCube || sampler.dim == EsdBuffer || sampler.ms))
                        continue;

                    for (int fetch = 0; fetch <= 1; ++fetch) { // loop over "bool" fetch or not

                        if (proj + offset + fetch + bias + lod > 3)
                            continue;
                        if (fetch && (lod || bias))
                            continue;
                        if (fetch && (sampler.shadow || sampler.dim == EsdCube))
                            continue;
                        if (fetch == 0 && (sampler.ms || sampler.dim == EsdBuffer))
                            continue;

                        for (int grad = 0; grad <= 1; ++grad) { // loop over "bool" grad or not

                            if (grad && (lod || bias || sampler.ms))
                                continue;
                            if (grad && sampler.dim == EsdBuffer)
                                continue;
                            if (proj + offset + fetch + grad + bias + lod > 3)
                                continue;

                            for (int extraProj = 0; extraProj <= 1; ++extraProj) {
                                bool compare = false;
                                int totalDims = dimMap[sampler.dim] + (sampler.arrayed ? 1 : 0);
                                // skip dummy unused second component for 1D non-array shadows
                                if (sampler.shadow && totalDims < 2)
                                    totalDims = 2;
                                totalDims += (sampler.shadow ? 1 : 0) + proj;
                                if (totalDims > 4 && sampler.shadow) {
                                    compare = true;
                                    totalDims = 4;
                                }
                                assert(totalDims <= 4);

                                if (extraProj && ! proj)
                                    continue;
                                if (extraProj && (sampler.dim == Esd3D || sampler.shadow))
                                    continue;

                                TString s;

                                // return type
                                if (sampler.shadow)
                                    s.append("float ");
                                else {
                                    s.append(prefixes[sampler.type]);
                                    s.append("vec4 ");
                                }

                                // name
                                if (fetch)
                                    s.append("texel");
                                else
                                    s.append("texture");
                                if (proj)
                                    s.append("Proj");
                                if (lod)
                                    s.append("Lod");
                                if (grad)
                                    s.append("Grad");
                                if (fetch)
                                    s.append("Fetch");
                                if (offset)
                                    s.append("Offset");
                                s.append("(");

                                // sampler type
                                s.append(typeName);

                                // P coordinate
                                if (extraProj)
                                    s.append(",vec4");
                                else {
                                    s.append(",");
                                    TBasicType t = fetch ? EbtInt : EbtFloat;
                                    if (totalDims == 1)
                                        s.append(TType::getBasicString(t));
                                    else {
                                        s.append(prefixes[t]);
                                        s.append("vec");
                                        s.append(postfixes[totalDims]);
                                    }
                                }

                                if (bias && compare)
                                    continue;

                                // non-optional lod argument (lod that's not driven by lod loop)
                                if (fetch && sampler.dim != EsdBuffer && !sampler.ms)
                                    s.append(",int");

                                // non-optional lod
                                if (lod)
                                    s.append(",float");

                                // gradient arguments
                                if (grad) {
                                    if (dimMap[sampler.dim] == 1)
                                        s.append(",float,float");
                                    else {
                                        s.append(",vec");
                                        s.append(postfixes[dimMap[sampler.dim]]);
                                        s.append(",vec");
                                        s.append(postfixes[dimMap[sampler.dim]]);
                                    }
                                }

                                // offset
                                if (offset) {
                                    if (dimMap[sampler.dim] == 1)
                                        s.append(",int");
                                    else {
                                        s.append(",ivec");
                                        s.append(postfixes[dimMap[sampler.dim]]);
                                    }
                                }

                                // optional bias or non-optional compare
                                if (bias || compare)
                                    s.append(",float");

                                s.append(");\n");

                                // Add to the per-language set of built-ins

                                if (bias)
                                    stageBuiltins[EShLangFragment].append(s);
                                else
                                    commonBuiltins.append(s);
                            }
                        }
                    }
                }
            }
        }
    }
}


//
// Helper function for add2ndGenerationSamplingImaging(), 
// when adding context-independent built-in functions.
//
// Add all the texture gather functions for the given type.
//
void TBuiltIns::addGatherFunctions(TSampler sampler, TString& typeName, int version, EProfile profile)
{
    switch (sampler.dim) {
    case Esd2D:
    case EsdRect:
    case EsdCube:
        break;
    default:
        return;
    }

    if (sampler.ms)
        return;

    if (version < 140 && sampler.dim == EsdRect && sampler.type != EbtFloat)
        return;

    for (int offset = 0; offset < 3; ++offset) { // loop over three forms of offset in the call name:  none, Offset, and Offsets

        for (int comp = 0; comp < 2; ++comp) { // loop over presence of comp argument

            if (comp > 0 && sampler.shadow)
                continue;

            if (offset > 0 && sampler.dim == EsdCube)
                continue;

            TString s;

            // return type
            s.append(prefixes[sampler.type]);
            s.append("vec4 ");

            // name
            s.append("textureGather");
            switch (offset) {
            case 1:
                s.append("Offset");
                break;
            case 2:
                s.append("Offsets");
            default:
                break;
            }
            s.append("(");

            // sampler type argument
            s.append(typeName);

            // P coordinate argument
            s.append(",vec");
            int totalDims = dimMap[sampler.dim] + (sampler.arrayed ? 1 : 0);
            s.append(postfixes[totalDims]);

            // refZ argument
            if (sampler.shadow)
                s.append(",float");

            // offset argument
            if (offset > 0) {
                s.append(",ivec2");
                if (offset == 2)
                    s.append("[4]");
            }

            // comp argument
            if (comp)
                s.append(",int");

            s.append(");\n");
            commonBuiltins.append(s);
            //printf("%s", s.c_str());
        }
    }
}

//
// Add context-dependent built-in functions and variables that are present
// for the given version and profile.  All the results are put into just the
// commonBuiltins, because it is called for just a specific stage.  So,
// add stage-specific entries to the commonBuiltins, and only if that stage
// was requested.
//
void TBuiltIns::initialize(const TBuiltInResource &resources, int version, EProfile profile, EShLanguage language)
{
    //
    // Initialize the context-dependent (resource-dependent) built-in strings for parsing.
    //

    //============================================================================
    //
    // Standard Uniforms
    //
    //============================================================================

    TString& s = commonBuiltins;
	const int maxSize = 80;
    char builtInConstant[maxSize];

    //
    // Build string of implementation dependent constants.
    //

    if (profile == EEsProfile) {
        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxVertexAttribs = %d;", resources.maxVertexAttribs);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxVertexUniformVectors = %d;", resources.maxVertexUniformVectors);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxVertexTextureImageUnits = %d;", resources.maxVertexTextureImageUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxCombinedTextureImageUnits = %d;", resources.maxCombinedTextureImageUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxTextureImageUnits = %d;", resources.maxTextureImageUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxFragmentUniformVectors = %d;", resources.maxFragmentUniformVectors);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxDrawBuffers = %d;", resources.maxDrawBuffers);
        s.append(builtInConstant);

        if (version == 100) {
            snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxVaryingVectors = %d;", resources.maxVaryingVectors);
            s.append(builtInConstant);
        } else {
            snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxVertexOutputVectors = %d;", resources.maxVertexOutputVectors);
            s.append(builtInConstant);

            snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxFragmentInputVectors = %d;", resources.maxFragmentInputVectors);
            s.append(builtInConstant);

            snprintf(builtInConstant, maxSize, "const mediump int  gl_MinProgramTexelOffset = %d;", resources.minProgramTexelOffset);
            s.append(builtInConstant);

            snprintf(builtInConstant, maxSize, "const mediump int  gl_MaxProgramTexelOffset = %d;", resources.maxProgramTexelOffset);
            s.append(builtInConstant);
        }
    } else {
        // non-ES profile

        snprintf(builtInConstant, maxSize, "const int  gl_MaxVertexAttribs = %d;", resources.maxVertexAttribs);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxVertexTextureImageUnits = %d;", resources.maxVertexTextureImageUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxCombinedTextureImageUnits = %d;", resources.maxCombinedTextureImageUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxTextureImageUnits = %d;", resources.maxTextureImageUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxDrawBuffers = %d;", resources.maxDrawBuffers);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxLights = %d;", resources.maxLights);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxClipPlanes = %d;", resources.maxClipPlanes);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxTextureUnits = %d;", resources.maxTextureUnits);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxTextureCoords = %d;", resources.maxTextureCoords);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxVertexUniformComponents = %d;", resources.maxVertexUniformComponents);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxVaryingFloats = %d;", resources.maxVaryingFloats);
        s.append(builtInConstant);

        snprintf(builtInConstant, maxSize, "const int  gl_MaxFragmentUniformComponents = %d;", resources.maxFragmentUniformComponents);
        s.append(builtInConstant);

        if (IncludeLegacy(version, profile)) {
            //
            // OpenGL'uniform' state.  Page numbers are in reference to version
            // 1.4 of the OpenGL specification.
            //

            //
            // Matrix state. p. 31, 32, 37, 39, 40.
            //
            s.append("uniform mat4  gl_TextureMatrix[gl_MaxTextureCoords];"

            //
            // Derived matrix state that provides inverse and transposed versions
            // of the matrices above.
            //
                        "uniform mat4  gl_TextureMatrixInverse[gl_MaxTextureCoords];"

                        "uniform mat4  gl_TextureMatrixTranspose[gl_MaxTextureCoords];"

                        "uniform mat4  gl_TextureMatrixInverseTranspose[gl_MaxTextureCoords];"

            //
            // Clip planes p. 42.
            //
                        "uniform vec4  gl_ClipPlane[gl_MaxClipPlanes];"

            //
            // Light State p 50, 53, 55.
            //
                        "uniform gl_LightSourceParameters  gl_LightSource[gl_MaxLights];"

            //
            // Derived state from products of light.
            //
                        "uniform gl_LightProducts gl_FrontLightProduct[gl_MaxLights];"
                        "uniform gl_LightProducts gl_BackLightProduct[gl_MaxLights];"

            //
            // Texture Environment and Generation, p. 152, p. 40-42.
            //
                        "uniform vec4  gl_TextureEnvColor[gl_MaxTextureImageUnits];"
                        "uniform vec4  gl_EyePlaneS[gl_MaxTextureCoords];"
                        "uniform vec4  gl_EyePlaneT[gl_MaxTextureCoords];"
                        "uniform vec4  gl_EyePlaneR[gl_MaxTextureCoords];"
                        "uniform vec4  gl_EyePlaneQ[gl_MaxTextureCoords];"
                        "uniform vec4  gl_ObjectPlaneS[gl_MaxTextureCoords];"
                        "uniform vec4  gl_ObjectPlaneT[gl_MaxTextureCoords];"
                        "uniform vec4  gl_ObjectPlaneR[gl_MaxTextureCoords];"
                        "uniform vec4  gl_ObjectPlaneQ[gl_MaxTextureCoords];");
        }

        if (version >= 130) {
            snprintf(builtInConstant, maxSize, "const int gl_MaxClipDistances = %d;", resources.maxClipDistances);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxVaryingComponents = %d;", resources.maxVaryingComponents);
            s.append(builtInConstant);
        }

        // geometry
        if (version >= 150) {
            snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryInputComponents = %d;", resources.maxGeometryInputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryOutputComponents = %d;", resources.maxGeometryOutputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryOutputVertices = %d;", resources.maxGeometryOutputVertices);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryTotalOutputComponents = %d;", resources.maxGeometryTotalOutputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryUniformComponents = %d;", resources.maxGeometryUniformComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryVaryingComponents = %d;", resources.maxGeometryVaryingComponents);
            s.append(builtInConstant);

        }

        if (version >= 150) {
            snprintf(builtInConstant, maxSize, "const int gl_MaxVertexOutputComponents = %d;", resources.maxVertexOutputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxFragmentInputComponents = %d;", resources.maxFragmentInputComponents);
            s.append(builtInConstant);
        }

        // tessellation
        if (version >= 150) {
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlInputComponents = %d;", resources.maxTessControlInputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlOutputComponents = %d;", resources.maxTessControlOutputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlTextureImageUnits = %d;", resources.maxTessControlTextureImageUnits);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlUniformComponents = %d;", resources.maxTessControlUniformComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlTotalOutputComponents = %d;", resources.maxTessControlTotalOutputComponents);
            s.append(builtInConstant);
                
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationInputComponents = %d;", resources.maxTessEvaluationInputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationOutputComponents = %d;", resources.maxTessEvaluationOutputComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationTextureImageUnits = %d;", resources.maxTessEvaluationTextureImageUnits);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationUniformComponents = %d;", resources.maxTessEvaluationUniformComponents);
            s.append(builtInConstant);
                
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessPatchComponents = %d;", resources.maxTessPatchComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTessGenLevel = %d;", resources.maxTessGenLevel);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxPatchVertices = %d;", resources.maxPatchVertices);
            s.append(builtInConstant);

            if (language == EShLangTessControl || language == EShLangTessEvaluation) {
                s.append(
                    "in gl_PerVertex {"
                        "vec4 gl_Position;"
                        "float gl_PointSize;"
                        "float gl_ClipDistance[];"
                    );
                if (profile == ECompatibilityProfile)
                    s.append(
                        "vec4 gl_ClipVertex;"
                        "vec4 gl_FrontColor;"
                        "vec4 gl_BackColor;"
                        "vec4 gl_FrontSecondaryColor;"
                        "vec4 gl_BackSecondaryColor;"
                        "vec4 gl_TexCoord[];"
                        "float gl_FogFragCoord;"
                        );
                s.append(
                    "} gl_in[gl_MaxPatchVertices];"
                    "\n");
            }
        }

        if (version >= 410) {
            snprintf(builtInConstant, maxSize, "const int gl_MaxViewports = %d;", resources.maxViewports);
            s.append(builtInConstant);
        }

        // atomic counters
        if (version >= 420) {
            //snprintf(builtInConstant, maxSize, "const int gl_MaxVertexAtomicCounters = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlAtomicCounters = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationAtomicCounters = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryAtomicCounters = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxFragmentAtomicCounters = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxCombinedAtomicCounters = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxAtomicCounterBindings = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxVertexAtomicCounterBuffers = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlAtomicCounterBuffers = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationAtomicCounterBuffers = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryAtomicCounterBuffers = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxFragmentAtomicCounterBuffers = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxCombinedAtomicCounterBuffers = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxAtomicCounterBufferSize = %d;", resources.);
        }

        // images
        if (version >= 420) {
            //snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryImageUniforms = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxGeometryTextureImageUnits = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxTessControlImageUniforms = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxTessEvaluationImageUniforms = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxImageUnits = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxCombinedImageUnitsAndFragmentOutputs = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxImageSamples = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxVertexImageUniforms = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxFragmentImageUniforms = %d;", resources.);
            //snprintf(builtInConstant, maxSize, "const int gl_MaxCombinedImageUniforms = %d;", resources.);
        }

        // compute
        if (version >= 430) {
            snprintf(builtInConstant, maxSize, "const ivec3 gl_MaxComputeWorkGroupCount = {%d,%d,%d};", resources.maxComputeWorkGroupCountX,
                                                                                                        resources.maxComputeWorkGroupCountY,
                                                                                                        resources.maxComputeWorkGroupCountZ);                
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const ivec3 gl_MaxComputeWorkGroupSize = {%d,%d,%d};", resources.maxComputeWorkGroupSizeX,
                                                                                                        resources.maxComputeWorkGroupSizeY,
                                                                                                        resources.maxComputeWorkGroupSizeZ);
            s.append(builtInConstant);

            snprintf(builtInConstant, maxSize, "const int gl_MaxComputeUniformComponents = %d;", resources.maxComputeUniformComponents);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxComputeTextureImageUnits = %d;", resources.maxComputeTextureImageUnits);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxComputeImageUniforms = %d;", resources.maxComputeImageUniforms);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxComputeAtomicCounters = %d;", resources.maxComputeAtomicCounters);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxComputeAtomicCounterBuffers = %d;", resources.maxComputeAtomicCounterBuffers);
            s.append(builtInConstant);
        }

        // enhanced layouts
        if (version >= 430) {
            snprintf(builtInConstant, maxSize, "const int gl_MaxTransformFeedbackBuffers = %d;", resources.maxTransformFeedbackBuffers);
            s.append(builtInConstant);
            snprintf(builtInConstant, maxSize, "const int gl_MaxTransformFeedbackInterleavedComponents = %d;", resources.maxTransformFeedbackInterleavedComponents);
            s.append(builtInConstant);
        }
    }

    s.append("\n");
}

//
// To support special built-ins that have a special qualifier that cannot be declared textually
// in a shader, like gl_Position.
//
// This lets the type of the built-in be declared textually, and then have just its qualifier be
// updated afterward.
//
// Safe to call even if name is not present.
//
// N.B.: longer term, having special qualifiers is probably not the way to go, but this is keeping
// in place the legacy ones.
//
void SpecialQualifier(const char* name, TStorageQualifier qualifier, TSymbolTable& symbolTable)
{
    TSymbol* symbol = symbolTable.find(name);
    if (symbol)
        symbol->getWritableType().getQualifier().storage = qualifier;
}

//
// Finish adding/processing context-independent built-in symbols.
// 1) Programmatically add symbols that could not be added by simple text strings above.
// 2) Map built-in functions to operators, for those that will turn into an operation node
//    instead of remaining a function call.
// 3) Tag extension-related symbols added to their base version with their extensions, so
//    that if an early version has the extension turned off, there is an error reported on use.
//
void IdentifyBuiltIns(int version, EProfile profile, EShLanguage language, TSymbolTable& symbolTable)
{
    //
    // Tag built-in variables and functions with additional qualifier and extension information
    // that cannot be declared with the text strings.
    //
    switch(language) {
    case EShLangVertex:
    case EShLangTessControl:
    case EShLangTessEvaluation:
    case EShLangGeometry:
        SpecialQualifier("gl_Position",   EvqPosition, symbolTable);
        SpecialQualifier("gl_PointSize",  EvqPointSize, symbolTable);
        SpecialQualifier("gl_ClipVertex", EvqClipVertex, symbolTable);
        SpecialQualifier("gl_VertexID",   EvqVertexId, symbolTable);
        SpecialQualifier("gl_InstanceID", EvqInstanceId, symbolTable);
        break;

    case EShLangFragment:
        SpecialQualifier("gl_FrontFacing",  EvqFace, symbolTable);
        SpecialQualifier("gl_FragCoord",    EvqFragCoord, symbolTable);
        SpecialQualifier("gl_PointCoord",   EvqPointCoord, symbolTable);
        SpecialQualifier("gl_FragColor",    EvqFragColor, symbolTable);
        SpecialQualifier("gl_FragDepth",    EvqFragDepth, symbolTable);
        SpecialQualifier("gl_FragDepthEXT", EvqFragDepth, symbolTable);
        if (version == 100) {
            symbolTable.setFunctionExtensions("dFdx",   1, &GL_OES_standard_derivatives);
            symbolTable.setFunctionExtensions("dFdy",   1, &GL_OES_standard_derivatives);
            symbolTable.setFunctionExtensions("fwidth", 1, &GL_OES_standard_derivatives);
        }
        if (profile == EEsProfile) {
            symbolTable.setFunctionExtensions("texture2DLodEXT",     1, &GL_EXT_shader_texture_lod);
            symbolTable.setFunctionExtensions("texture2DProjLodEXT", 1, &GL_EXT_shader_texture_lod);
            symbolTable.setFunctionExtensions("textureCubeLodEXT",   1, &GL_EXT_shader_texture_lod);
        }
        symbolTable.setVariableExtensions("gl_FragDepthEXT", 1, &GL_EXT_frag_depth);
        break;

    case EShLangCompute:
        // TODO: 4.3 desktop functionality: compute special variables
        break;

    default:
        assert(false && "Language not supported");
        break;
    }

    if (profile == EEsProfile) {
        symbolTable.setFunctionExtensions("texture2DGradEXT",     1, &GL_EXT_shader_texture_lod);
        symbolTable.setFunctionExtensions("texture2DProjGradEXT", 1, &GL_EXT_shader_texture_lod);
        symbolTable.setFunctionExtensions("textureCubeGradEXT",   1, &GL_EXT_shader_texture_lod);
    }

    //
    // Next, identify which built-ins have a mapping to an operator.
    // Those that are not identified as such are
    // expected to be resolved through a library of functions, versus as
    // operations.
    //
    symbolTable.relateToOperator("not",              EOpVectorLogicalNot);

    symbolTable.relateToOperator("matrixCompMult",   EOpMul);
    if (version >= 120) {
        symbolTable.relateToOperator("outerProduct", EOpOuterProduct);
        symbolTable.relateToOperator("transpose", EOpTranspose);
        if (version >= 150) {
            symbolTable.relateToOperator("determinant", EOpDeterminant);
            symbolTable.relateToOperator("inverse", EOpMatrixInverse);
        }
    }

    symbolTable.relateToOperator("mod",              EOpMod);
    symbolTable.relateToOperator("modf",             EOpModf);

    symbolTable.relateToOperator("equal",            EOpVectorEqual);
    symbolTable.relateToOperator("notEqual",         EOpVectorNotEqual);
    symbolTable.relateToOperator("lessThan",         EOpLessThan);
    symbolTable.relateToOperator("greaterThan",      EOpGreaterThan);
    symbolTable.relateToOperator("lessThanEqual",    EOpLessThanEqual);
    symbolTable.relateToOperator("greaterThanEqual", EOpGreaterThanEqual);

    symbolTable.relateToOperator("radians",      EOpRadians);
    symbolTable.relateToOperator("degrees",      EOpDegrees);
    symbolTable.relateToOperator("sin",          EOpSin);
    symbolTable.relateToOperator("cos",          EOpCos);
    symbolTable.relateToOperator("tan",          EOpTan);
    symbolTable.relateToOperator("asin",         EOpAsin);
    symbolTable.relateToOperator("acos",         EOpAcos);
    symbolTable.relateToOperator("atan",         EOpAtan);
    symbolTable.relateToOperator("sinh",         EOpSinh);
    symbolTable.relateToOperator("cosh",         EOpCosh);
    symbolTable.relateToOperator("tanh",         EOpTanh);
    symbolTable.relateToOperator("asinh",        EOpAsinh);
    symbolTable.relateToOperator("acosh",        EOpAcosh);
    symbolTable.relateToOperator("atanh",        EOpAtanh);

    symbolTable.relateToOperator("pow",          EOpPow);
    symbolTable.relateToOperator("exp2",         EOpExp2);
    symbolTable.relateToOperator("log",          EOpLog);
    symbolTable.relateToOperator("exp",          EOpExp);
    symbolTable.relateToOperator("log2",         EOpLog2);
    symbolTable.relateToOperator("sqrt",         EOpSqrt);
    symbolTable.relateToOperator("inversesqrt",  EOpInverseSqrt);

    symbolTable.relateToOperator("abs",          EOpAbs);
    symbolTable.relateToOperator("sign",         EOpSign);
    symbolTable.relateToOperator("floor",        EOpFloor);
    symbolTable.relateToOperator("trunc",        EOpTrunc);
    symbolTable.relateToOperator("round",        EOpRound);
    symbolTable.relateToOperator("roundEven",    EOpRoundEven);
    symbolTable.relateToOperator("ceil",         EOpCeil);
    symbolTable.relateToOperator("fract",        EOpFract);
    symbolTable.relateToOperator("min",          EOpMin);
    symbolTable.relateToOperator("max",          EOpMax);
    symbolTable.relateToOperator("clamp",        EOpClamp);
    symbolTable.relateToOperator("mix",          EOpMix);
    symbolTable.relateToOperator("step",         EOpStep);
    symbolTable.relateToOperator("smoothstep",   EOpSmoothStep);

    symbolTable.relateToOperator("isnan",  EOpIsNan);
    symbolTable.relateToOperator("isinf",  EOpIsInf);

    symbolTable.relateToOperator("floatBitsToInt",  EOpFloatBitsToInt);
    symbolTable.relateToOperator("floatBitsToUint", EOpFloatBitsToUint);
    symbolTable.relateToOperator("intBitsToFloat",  EOpIntBitsToFloat);
    symbolTable.relateToOperator("uintBitsToFloat", EOpUintBitsToFloat);
    symbolTable.relateToOperator("packSnorm2x16",   EOpPackSnorm2x16);
    symbolTable.relateToOperator("unpackSnorm2x16", EOpUnpackSnorm2x16);
    symbolTable.relateToOperator("packUnorm2x16",   EOpPackUnorm2x16);
    symbolTable.relateToOperator("unpackUnorm2x16", EOpUnpackUnorm2x16);
    symbolTable.relateToOperator("packHalf2x16",    EOpPackHalf2x16);
    symbolTable.relateToOperator("unpackHalf2x16",  EOpUnpackHalf2x16);

    symbolTable.relateToOperator("length",       EOpLength);
    symbolTable.relateToOperator("distance",     EOpDistance);
    symbolTable.relateToOperator("dot",          EOpDot);
    symbolTable.relateToOperator("cross",        EOpCross);
    symbolTable.relateToOperator("normalize",    EOpNormalize);
    symbolTable.relateToOperator("faceforward",  EOpFaceForward);
    symbolTable.relateToOperator("reflect",      EOpReflect);
    symbolTable.relateToOperator("refract",      EOpRefract);

    symbolTable.relateToOperator("any",          EOpAny);
    symbolTable.relateToOperator("all",          EOpAll);

    symbolTable.relateToOperator("barrier",                    EOpBarrier);
    symbolTable.relateToOperator("memoryBarrier",              EOpMemoryBarrier);
    symbolTable.relateToOperator("memoryBarrierAtomicCounter", EOpMemoryBarrierAtomicCounter);
    symbolTable.relateToOperator("memoryBarrierBuffer",        EOpMemoryBarrierBuffer);
    symbolTable.relateToOperator("memoryBarrierImage",         EOpMemoryBarrierImage);

    switch(language) {
    case EShLangVertex:
        break;

    case EShLangTessControl:
    case EShLangTessEvaluation:
        break;

    case EShLangGeometry:
        symbolTable.relateToOperator("EmitStreamVertex",   EOpEmitStreamVertex);
        symbolTable.relateToOperator("EndStreamPrimitive", EOpEndStreamPrimitive);
        symbolTable.relateToOperator("EmitVertex",         EOpEmitVertex);
        symbolTable.relateToOperator("EndPrimitive",       EOpEndPrimitive);
        break;

    case EShLangFragment:
        symbolTable.relateToOperator("dFdx",         EOpDPdx);
        symbolTable.relateToOperator("dFdy",         EOpDPdy);
        symbolTable.relateToOperator("fwidth",       EOpFwidth);
        break;

    case EShLangCompute:
        symbolTable.relateToOperator("memoryBarrierShared", EOpMemoryBarrierShared);
        symbolTable.relateToOperator("groupMemoryBarrier",  EOpGroupMemoryBarrier);
        break;

	default:
        assert(false && "Language not supported");
    }
}

//
// Add context-dependent (resource-specific) built-ins not yet handled.  These
// would be ones that need to be programmatically added because they cannot 
// be added by simple text strings.  For these, also
// 1) Map built-in functions to operators, for those that will turn into an operation node
//    instead of remaining a function call.
// 2) Tag extension-related symbols added to their base version with their extensions, so
//    that if an early version has the extension turned off, there is an error reported on use.
//
void IdentifyBuiltIns(int version, EProfile profile, EShLanguage language, TSymbolTable& symbolTable, const TBuiltInResource &resources)
{
    if (version >= 430 && version < 440) {
        symbolTable.setVariableExtensions("gl_MaxTransformFeedbackBuffers", 1, &GL_ARB_enhanced_layouts);
        symbolTable.setVariableExtensions("gl_MaxTransformFeedbackInterleavedComponents", 1, &GL_ARB_enhanced_layouts);
    }

    switch(language) {

    case EShLangFragment:
        // Set up gl_FragData based on current array size.
        if (version == 100 || IncludeLegacy(version, profile) || (! ForwardCompatibility && profile != EEsProfile && version < 420)) {
            TPrecisionQualifier pq = profile == EEsProfile ? EpqMedium : EpqNone;
            TType fragData(EbtFloat, EvqFragColor, pq, 4);
            TArraySizes* arraySizes = new TArraySizes;
            arraySizes->setSize(resources.maxDrawBuffers);
            fragData.setArraySizes(arraySizes);
            symbolTable.insert(*new TVariable(NewPoolTString("gl_FragData"), fragData));
        }
        break;

	default:
        break;
    }
}

} // end namespace glslang
