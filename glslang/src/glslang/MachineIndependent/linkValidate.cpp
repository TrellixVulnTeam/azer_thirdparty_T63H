//
//Copyright (C) 2013 LunarG, Inc.
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
// Do link-time merging and validation of intermediate representations.
//
// Basic model is that during compilation, each compilation unit (shader) is
// compiled into one TIntermediate instance.  Then, at link time, multiple
// units for the same stage can be merged together, which can generate errors.
// Then, after all merging, a single instance of TIntermediate represents
// the whole stage.  A final error check can be done on the resulting stage,
// even if no merging was done (i.e., the stage was only one compilation unit).
//

#include "localintermediate.h"
#include "../Include/InfoSink.h"

namespace glslang {
    
//
// Link-time error emitter.
//
void TIntermediate::error(TInfoSink& infoSink, const char* message)
{
    infoSink.info.prefix(EPrefixError);
    infoSink.info << "Linking " << StageName(language) << " stage: " << message << "\n";

    ++numErrors;
}

// TODO: 4.4 offset/align:  "Two blocks linked together in the same program with the same block 
// name must have the exact same set of members qualified with offset and their integral-constant 
// expression values must be the same, or a link-time error results."

//
// Merge the information from 'unit' into 'this'
//
void TIntermediate::merge(TInfoSink& infoSink, TIntermediate& unit)
{
    numMains += unit.numMains;
    numErrors += unit.numErrors;
    callGraph.insert(callGraph.end(), unit.callGraph.begin(), unit.callGraph.end());

    if ((profile != EEsProfile && unit.profile == EEsProfile) ||
        (profile == EEsProfile && unit.profile != EEsProfile))
        error(infoSink, "Cannot mix ES profile with non-ES profile shaders\n");

    if (originUpperLeft != unit.originUpperLeft || pixelCenterInteger != unit.pixelCenterInteger)
        error(infoSink, "gl_FragCoord redeclarations must match across shaders\n");

    if (inputPrimitive == ElgNone)
        inputPrimitive = unit.inputPrimitive;
    else if (inputPrimitive != unit.inputPrimitive)
        error(infoSink, "Contradictory input layout primitives");
    
    if (outputPrimitive == ElgNone)
        outputPrimitive = unit.outputPrimitive;
    else if (outputPrimitive != unit.outputPrimitive)
        error(infoSink, "Contradictory output layout primitives");
    
    if (vertices == 0)
        vertices = unit.vertices;
    else if (vertices != unit.vertices) {
        if (language == EShLangGeometry)
            error(infoSink, "Contradictory layout max_vertices values");
        else if (language == EShLangTessControl)
            error(infoSink, "Contradictory layout vertices values");
        else
            assert(0);
    }

    if (vertexSpacing == EvsNone)
        vertexSpacing = unit.vertexSpacing;
    else if (vertexSpacing != unit.vertexSpacing)
        error(infoSink, "Contradictory input vertex spacing");

    if (vertexOrder == EvoNone)
        vertexOrder = unit.vertexOrder;
    else if (vertexOrder != unit.vertexOrder)
        error(infoSink, "Contradictory triangle ordering");

    if (unit.pointMode)
        pointMode = true;

    if (unit.xfbMode)
        xfbMode = true;
    for (size_t b = 0; b < xfbBuffers.size(); ++b) {
        if (xfbBuffers[b].stride == TQualifier::layoutXfbStrideEnd)
            xfbBuffers[b].stride = unit.xfbBuffers[b].stride;
        else if (xfbBuffers[b].stride != unit.xfbBuffers[b].stride)
            error(infoSink, "Contradictory xfb_stride");
        xfbBuffers[b].implicitStride = std::max(xfbBuffers[b].implicitStride, unit.xfbBuffers[b].implicitStride);
        if (unit.xfbBuffers[b].containsDouble)
            xfbBuffers[b].containsDouble = true;
        // TODO: 4.4 link: enhanced layouts: compare ranges
    }

    if (unit.treeRoot == 0)
        return;

    if (treeRoot == 0) {
        version = unit.version;
        treeRoot = unit.treeRoot;
        return;
    } else
        version = std::max(version, unit.version);

    // Get the top-level globals of each unit
    TIntermSequence& globals = treeRoot->getAsAggregate()->getSequence();
    TIntermSequence& unitGlobals = unit.treeRoot->getAsAggregate()->getSequence();

    // Get the linker-object lists
    TIntermSequence& linkerObjects = findLinkerObjects();
    TIntermSequence& unitLinkerObjects = unit.findLinkerObjects();

    mergeBodies(infoSink, globals, unitGlobals);
    mergeLinkerObjects(infoSink, linkerObjects, unitLinkerObjects);

    ioAccessed.insert(unit.ioAccessed.begin(), unit.ioAccessed.end());
}

//
// Merge the function bodies and global-level initalizers from unitGlobals into globals.
// Will error check duplication of function bodies for the same signature.
//
void TIntermediate::mergeBodies(TInfoSink& infoSink, TIntermSequence& globals, const TIntermSequence& unitGlobals)
{
    // TODO: link-time performance: Processing in alphabetical order will be faster

    // Error check the global objects, not including the linker objects
    for (unsigned int child = 0; child < globals.size() - 1; ++child) {
        for (unsigned int unitChild = 0; unitChild < unitGlobals.size() - 1; ++unitChild) {
            TIntermAggregate* body = globals[child]->getAsAggregate();
            TIntermAggregate* unitBody = unitGlobals[unitChild]->getAsAggregate();
            if (body && unitBody && body->getOp() == EOpFunction && unitBody->getOp() == EOpFunction && body->getName() == unitBody->getName()) {
                error(infoSink, "Multiple function bodies in multiple compilation units for the same signature in the same stage:");
                infoSink.info << "    " << globals[child]->getAsAggregate()->getName() << "\n";
            }
        }
    }

    // Merge the global objects, just in front of the linker objects
    globals.insert(globals.end() - 1, unitGlobals.begin(), unitGlobals.end() - 1);
}

//
// Merge the linker objects from unitLinkerObjects into linkerObjects.
// Duplication is expected and filtered out, but contradictions are an error.
//
void TIntermediate::mergeLinkerObjects(TInfoSink& infoSink, TIntermSequence& linkerObjects, const TIntermSequence& unitLinkerObjects)
{
    // Error check and merge the linker objects (duplicates should not be merged)
    std::size_t initialNumLinkerObjects = linkerObjects.size();
    for (unsigned int unitLinkObj = 0; unitLinkObj < unitLinkerObjects.size(); ++unitLinkObj) {
        bool merge = true;
        for (std::size_t linkObj = 0; linkObj < initialNumLinkerObjects; ++linkObj) {
            TIntermSymbol* symbol = linkerObjects[linkObj]->getAsSymbolNode();
            TIntermSymbol* unitSymbol = unitLinkerObjects[unitLinkObj]->getAsSymbolNode();
            assert(symbol && unitSymbol);
            if (symbol->getName() == unitSymbol->getName()) {
                // filter out copy
                merge = false;

                // but if one has an initializer and the other does not, update
                // the initializer
                if (symbol->getConstArray().empty() && ! unitSymbol->getConstArray().empty())
                    symbol->setConstArray(unitSymbol->getConstArray());

                // Similarly for binding
                if (! symbol->getQualifier().hasBinding() && unitSymbol->getQualifier().hasBinding())
                    symbol->getQualifier().layoutBinding = unitSymbol->getQualifier().layoutBinding;
                
                // Check for consistent types/qualification/initializers etc.
                mergeErrorCheck(infoSink, *symbol, *unitSymbol, false);
            }
        }
        if (merge)
            linkerObjects.push_back(unitLinkerObjects[unitLinkObj]);
    }
}

//
// Compare two global objects from two compilation units and see if they match
// well enough.  Rules can be different for intra- vs. cross-stage matching.
//
// This function only does one of intra- or cross-stage matching per call.
//
void TIntermediate::mergeErrorCheck(TInfoSink& infoSink, const TIntermSymbol& symbol, const TIntermSymbol& unitSymbol, bool crossStage)
{
    bool writeTypeComparison = false;

    // Types have to match
    if (symbol.getType() != unitSymbol.getType()) {
        error(infoSink, "Types must match:");
        writeTypeComparison = true;
    }

    // Qualifiers have to (almost) match

    // Storage...
    if (symbol.getQualifier().storage != unitSymbol.getQualifier().storage) {
        error(infoSink, "Storage qualifiers must match:");
        writeTypeComparison = true;
    }

    // Precision...
    if (symbol.getQualifier().precision != unitSymbol.getQualifier().precision) {
        error(infoSink, "Precision qualifiers must match:");
        writeTypeComparison = true;
    }

    // Invariance...
    if (! crossStage && symbol.getQualifier().invariant != unitSymbol.getQualifier().invariant) {
        error(infoSink, "Presence of invariant qualifier must match:");
        writeTypeComparison = true;
    }

    // Auxiliary and interpolation...
    if (symbol.getQualifier().centroid  != unitSymbol.getQualifier().centroid ||
        symbol.getQualifier().smooth    != unitSymbol.getQualifier().smooth ||
        symbol.getQualifier().flat      != unitSymbol.getQualifier().flat ||
        symbol.getQualifier().sample    != unitSymbol.getQualifier().sample ||
        symbol.getQualifier().patch     != unitSymbol.getQualifier().patch ||
        symbol.getQualifier().nopersp   != unitSymbol.getQualifier().nopersp) {
        error(infoSink, "Interpolation and auxiliary storage qualifiers must match:");
        writeTypeComparison = true;
    }

    // Memory...
    if (symbol.getQualifier().shared    != unitSymbol.getQualifier().shared ||
        symbol.getQualifier().coherent  != unitSymbol.getQualifier().coherent ||
        symbol.getQualifier().volatil   != unitSymbol.getQualifier().volatil ||
        symbol.getQualifier().restrict  != unitSymbol.getQualifier().restrict ||
        symbol.getQualifier().readonly  != unitSymbol.getQualifier().readonly ||
        symbol.getQualifier().writeonly != unitSymbol.getQualifier().writeonly) {
        error(infoSink, "Memory qualifiers must match:");
        writeTypeComparison = true;
    }

    // Layouts... 
    // TODO: 4.4 enhanced layouts: Generalize to include offset/align: currrent spec 
    //       requires separate user-supplied offset from actual computed offset, but 
    //       current implementation only has one offset.
    if (symbol.getQualifier().layoutMatrix   != unitSymbol.getQualifier().layoutMatrix ||
        symbol.getQualifier().layoutPacking  != unitSymbol.getQualifier().layoutPacking ||
        symbol.getQualifier().layoutLocation != unitSymbol.getQualifier().layoutLocation ||
        symbol.getQualifier().layoutBinding  != unitSymbol.getQualifier().layoutBinding) {
        error(infoSink, "Layout qualification must match:");
        writeTypeComparison = true;
    }

    // Initializers have to match, if both are present, and if we don't already know the types don't match
    if (! writeTypeComparison) {
        if (! symbol.getConstArray().empty() && ! unitSymbol.getConstArray().empty()) {
            if (symbol.getConstArray() != unitSymbol.getConstArray()) {
                error(infoSink, "Initializers must match:");
                infoSink.info << "    " << symbol.getName() << "\n";
            }
        }
    }

    if (writeTypeComparison)
        infoSink.info << "    " << symbol.getName() << ": \"" << symbol.getType().getCompleteString() << "\" versus \"" <<
                                                             unitSymbol.getType().getCompleteString() << "\"\n";
}

//
// Do final link-time error checking of a complete (merged) intermediate representation.
// (Much error checking was done during merging).
//
// Also, lock in defaults of things not set.
//
void TIntermediate::finalCheck(TInfoSink& infoSink)
{   
    if (numMains < 1)
        error(infoSink, "Missing entry point: Each stage requires one \"void main()\" entry point");

    // recursion checking
    checkCallGraphCycles(infoSink);

    // overlap/alias/missing I/O, etc.
    inOutLocationCheck(infoSink);

    if (inIoAccessed("gl_ClipDistance") && inIoAccessed("gl_ClipVertex"))
        error(infoSink, "Can only use one of gl_ClipDistance or gl_ClipVertex (gl_ClipDistance is preferred)");

    if (userOutputUsed() && (inIoAccessed("gl_FragColor") || inIoAccessed("gl_FragData")))
        error(infoSink, "Cannot use gl_FragColor or gl_FragData when using user-defined outputs");
    if (inIoAccessed("gl_FragColor") && inIoAccessed("gl_FragData"))
        error(infoSink, "Cannot use both gl_FragColor and gl_FragData");

    for (size_t b = 0; b < xfbBuffers.size(); ++b) {
        if (xfbBuffers[b].containsDouble)
            RoundToPow2(xfbBuffers[b].implicitStride, 8);

        // "It is a compile-time or link-time error to have 
        // any xfb_offset that overflows xfb_stride, whether stated on declarations before or after the xfb_stride, or
        // in different compilation units. While xfb_stride can be declared multiple times for the same buffer, it is a
        // compile-time or link-time error to have different values specified for the stride for the same buffer."
        if (xfbBuffers[b].stride != TQualifier::layoutXfbStrideEnd && xfbBuffers[b].implicitStride > xfbBuffers[b].stride) {
            error(infoSink, "xfb_stride is too small to hold all buffer entries:");
            infoSink.info.prefix(EPrefixError);
            infoSink.info << "    xfb_buffer " << (unsigned int)b << ", xfb_stride " << xfbBuffers[b].stride << ", minimum stride needed: " << xfbBuffers[b].implicitStride << "\n";
        }
        if (xfbBuffers[b].stride == TQualifier::layoutXfbStrideEnd)
            xfbBuffers[b].stride = xfbBuffers[b].implicitStride;

        // "If the buffer is capturing any 
        // outputs with double-precision components, the stride must be a multiple of 8, otherwise it must be a 
        // multiple of 4, or a compile-time or link-time error results."
        if (xfbBuffers[b].containsDouble && ! IsMultipleOfPow2(xfbBuffers[b].stride, 8)) {
            error(infoSink, "xfb_stride must be multiple of 8 for buffer holding a double:");
            infoSink.info.prefix(EPrefixError);
            infoSink.info << "    xfb_buffer " << (unsigned int)b << ", xfb_stride " << xfbBuffers[b].stride << "\n";
        } else if (! IsMultipleOfPow2(xfbBuffers[b].stride, 4)) {
            error(infoSink, "xfb_stride must be multiple of 4:");
            infoSink.info.prefix(EPrefixError);
            infoSink.info << "    xfb_buffer " << (unsigned int)b << ", xfb_stride " << xfbBuffers[b].stride << "\n";
        }

        // "The resulting stride (implicit or explicit), when divided by 4, must be less than or equal to the 
        // implementation-dependent constant gl_MaxTransformFeedbackInterleavedComponents."
        if (xfbBuffers[b].stride > (unsigned int)(4 * resources.maxTransformFeedbackInterleavedComponents)) {
            error(infoSink, "xfb_stride is too large:");
            infoSink.info.prefix(EPrefixError);
            infoSink.info << "    xfb_buffer " << (unsigned int)b << ", components (1/4 stride) needed are " << xfbBuffers[b].stride/4 << ", gl_MaxTransformFeedbackInterleavedComponents is " << resources.maxTransformFeedbackInterleavedComponents << "\n";
        }
    }

    switch (language) {
    case EShLangVertex:
        break;
    case EShLangTessControl:
        if (vertices == 0)
            error(infoSink, "At least one shader must specify an output layout(vertices=...)");
        break;
    case EShLangTessEvaluation:
        if (inputPrimitive == ElgNone)
            error(infoSink, "At least one shader must specify an input layout primitive");
        if (vertexSpacing == EvsNone)
            vertexSpacing = EvsEqual;
        if (vertexOrder == EvoNone)
            vertexOrder = EvoCcw;
        break;
    case EShLangGeometry:
        if (inputPrimitive == ElgNone)
            error(infoSink, "At least one shader must specify an input layout primitive");
        if (outputPrimitive == ElgNone)
            error(infoSink, "At least one shader must specify an output layout primitive");
        if (vertices == 0)
            error(infoSink, "At least one shader must specify a layout(max_vertices = value)");
        break;
    case EShLangFragment:
        break;
    case EShLangCompute:
        break;
    }
}

//
// See if the call graph contains any static recursion, which is disallowed
// by the specification.
//
void TIntermediate::checkCallGraphCycles(TInfoSink& infoSink)
{
    // Reset everything, once.
    for (TGraph::iterator call = callGraph.begin(); call != callGraph.end(); ++call) {
        call->visited = false;
        call->currentPath = false;
        call->errorGiven = false;
    }

    //
    // Loop, looking for a new connected subgraph.  One subgraph is handled per loop iteration.
    //

    TCall* newRoot;
    do {
        // See if we have unvisited parts of the graph.
        newRoot = 0;
        for (TGraph::iterator call = callGraph.begin(); call != callGraph.end(); ++call) {
            if (! call->visited) {
                newRoot = &(*call);
                break;
            }
        }

        // If not, we are done.
        if (! newRoot)
            break;

        // Otherwise, we found a new subgraph, process it:
        // See what all can be reached by this new root, and if any of 
        // that is recursive.  This is done by depth-first traversals, seeing
        // if a new call is found that was already in the currentPath (a back edge),
        // thereby detecting recursion.
        std::list<TCall*> stack;
        newRoot->currentPath = true; // currentPath will be true iff it is on the stack
        stack.push_back(newRoot);
        while (! stack.empty()) {
            // get a caller
            TCall* call = stack.back();

            // Add to the stack just one callee.
            // This algorithm always terminates, because only !visited and !currentPath causes a push
            // and all pushes change currentPath to true, and all pops change visited to true.
            TGraph::iterator child = callGraph.begin();
            for (; child != callGraph.end(); ++child) {

                // If we already visited this node, its whole subgraph has already been processed, so skip it.
                if (child->visited)
                    continue;

                if (call->callee == child->caller) {
                    if (child->currentPath) {
                        // Then, we found a back edge
                        if (! child->errorGiven) {
                            error(infoSink, "Recursion detected:");
                            infoSink.info << "    " << call->callee << " calling " << child->callee << "\n";
                            child->errorGiven = true;
                            recursive = true;
                        }
                    } else {
                        child->currentPath = true;
                        stack.push_back(&(*child));
                        break;
                    }
                }
            }
            if (child == callGraph.end()) {
                // no more callees, we bottomed out, never look at this node again
                stack.back()->currentPath = false;
                stack.back()->visited = true;
                stack.pop_back();
            }
        }  // end while, meaning nothing left to process in this subtree

    } while (newRoot);  // redundant loop check; should always exit via the 'break' above
}

//
// Satisfy rules for location qualifiers on inputs and outputs
//
void TIntermediate::inOutLocationCheck(TInfoSink& infoSink)
{
    // ES 3.0 requires all outputs to have location qualifiers if there is more than one output
    bool fragOutHasLocation = false;
    bool fragOutWithNoLocation = false;
    int numFragOut = 0;

    // TODO: linker functionality: location collision checking

    TIntermSequence& linkObjects = findLinkerObjects();
    for (size_t i = 0; i < linkObjects.size(); ++i) {
        const TType& type = linkObjects[i]->getAsTyped()->getType();
        const TQualifier& qualifier = type.getQualifier();
        if (language == EShLangFragment) {
            if (qualifier.storage == EvqVaryingOut) {
                ++numFragOut;
                if (qualifier.hasLocation())
                    fragOutHasLocation = true;
                else
                    fragOutWithNoLocation = true;
            }
        }
    }

    if (profile == EEsProfile) {
        if (numFragOut > 1 && fragOutWithNoLocation)
            error(infoSink, "when more than one fragment shader output, all must have location qualifiers");
    }        
}

TIntermSequence& TIntermediate::findLinkerObjects() const
{
    // Get the top-level globals
    TIntermSequence& globals = treeRoot->getAsAggregate()->getSequence();

    // Get the last member of the sequences, expected to be the linker-object lists
    assert(globals.back()->getAsAggregate()->getOp() == EOpLinkerObjects);

    return globals.back()->getAsAggregate()->getSequence();
}

// See if a variable was both a user-declared output and used.
// Note: the spec discusses writing to one, but this looks at read or write, which 
// is more useful, and perhaps the spec should be changed to reflect that.
bool TIntermediate::userOutputUsed() const
{
    const TIntermSequence& linkerObjects = findLinkerObjects();

    bool found = false;
    for (size_t i = 0; i < linkerObjects.size(); ++i) {
        const TIntermSymbol& symbolNode = *linkerObjects[i]->getAsSymbolNode();
        if (symbolNode.getQualifier().storage == EvqVaryingOut &&
            symbolNode.getName().compare(0, 3, "gl_") != 0 &&
            inIoAccessed(symbolNode.getName())) {            
            found = true;
            break;
        }
    }

    return found;
}

// Accumulate locations used for inputs, outputs, and uniforms, and check for collisions
// as the accumulation is done.
//
// Returns < 0 if no collision, >= 0 if collision and the value returned is a colliding value.
//
// typeCollision is set to true if there is no direct collision, but the types in the same location
// are different.
//
int TIntermediate::addUsedLocation(const TQualifier& qualifier, const TType& type, bool& typeCollision)
{
    typeCollision = false;

    int set;
    if (qualifier.isPipeInput())
        set = 0;
    else if (qualifier.isPipeOutput())
        set = 1;
    else if (qualifier.isUniform())
        set = 2;
    else
        return -1;

    int size;
    if (qualifier.isUniform()) {
        if (type.isArray())
            size = type.getArraySize();
        else
            size = 1;
    } else {
        // Strip off the outer array dimension for those having an extra one.
        if (type.isArray() && ! qualifier.patch &&
            (language == EShLangGeometry && qualifier.isPipeInput()) ||
            language == EShLangTessControl                           ||
            (language == EShLangTessEvaluation && qualifier.isPipeInput())) {
            TType elementType(type, 0);
            size = computeTypeLocationSize(elementType);
        } else
            size = computeTypeLocationSize(type);
    }

    TRange locationRange(qualifier.layoutLocation, qualifier.layoutLocation + size - 1);
    TRange componentRange(0, 3);
    if (qualifier.layoutComponent != TQualifier::layoutComponentEnd) {
        componentRange.start = qualifier.layoutComponent;
        componentRange.last = componentRange.start + type.getVectorSize() - 1;
    }
    TIoRange range(locationRange, componentRange, type.getBasicType());

    // check for collisions, except for vertex inputs on desktop
    if (! (profile != EEsProfile && language == EShLangVertex && qualifier.isPipeInput())) {
        for (size_t r = 0; r < usedIo[set].size(); ++r) {
            if (range.overlap(usedIo[set][r])) {
                // there is a collision; pick one
                return std::max(locationRange.start, usedIo[set][r].location.start);
            } else if (locationRange.overlap(usedIo[set][r].location) && type.getBasicType() != usedIo[set][r].basicType) {
                // aliased-type mismatch
                typeCollision = true;
                return std::max(locationRange.start, usedIo[set][r].location.start);
            }
        }
    }

    usedIo[set].push_back(range);

    return -1; // no collision
}

// Recursively figure out how many locations are used up by an input or output type.
// Return the size of type, as measured by "locations".
int TIntermediate::computeTypeLocationSize(const TType& type)
{
    // "If the declared input is an array of size n and each element takes m locations, it will be assigned m * n 
    // consecutive locations..."
    if (type.isArray()) {
        TType elementType(type, 0);
        if (type.getArraySize() == 0) {
            // TODO: are there valid cases of having an unsized array with a location?  If so, running this code too early.
            return computeTypeLocationSize(elementType);
        } else
            return type.getArraySize() * computeTypeLocationSize(elementType);
    }

    // "The locations consumed by block and structure members are determined by applying the rules above 
    // recursively..."    
    if (type.isStruct()) {
        int size = 0;
        for (int member = 0; member < (int)type.getStruct()->size(); ++member) {
            TType memberType(type, member);
            size += computeTypeLocationSize(memberType);
        }
        return size;
    }

    // "If a vertex shader input is any scalar or vector type, it will consume a single location. If a non-vertex 
    // shader input is a scalar or vector type other than dvec3 or dvec4, it will consume a single location, while 
    // types dvec3 or dvec4 will consume two consecutive locations. Inputs of type double and dvec2 will 
    // consume only a single location, in all stages."
    if (type.isScalar())
        return 1;
    if (type.isVector()) {
        if (language == EShLangVertex && type.getQualifier().isPipeInput())
            return 1;
        if (type.getBasicType() == EbtDouble && type.getVectorSize() > 2)
            return 2;
        else
            return 1;
    }

    // "If the declared input is an n x m single- or double-precision matrix, ...
    // The number of locations assigned for each matrix will be the same as 
    // for an n-element array of m-component vectors..."
    if (type.isMatrix()) {
        TType columnType(type, 0);
        return type.getMatrixCols() * computeTypeLocationSize(columnType);
    }

    assert(0);
    return 1;
}

// Accumulate xfb buffer ranges and check for collisions as the accumulation is done.
//
// Returns < 0 if no collision, >= 0 if collision and the value returned is a colliding value.
//
int TIntermediate::addXfbBufferOffset(const TType& type)
{
    const TQualifier& qualifier = type.getQualifier();

    assert(qualifier.hasXfbOffset() && qualifier.hasXfbBuffer());
    TXfbBuffer& buffer = xfbBuffers[qualifier.layoutXfbBuffer];

    // compute the range
    unsigned int size = computeTypeXfbSize(type, buffer.containsDouble);
    buffer.implicitStride = std::max(buffer.implicitStride, qualifier.layoutXfbOffset + size);
    TRange range(qualifier.layoutXfbOffset, qualifier.layoutXfbOffset + size - 1);

    // check for collisions
    for (size_t r = 0; r < buffer.ranges.size(); ++r) {
        if (range.overlap(buffer.ranges[r])) {
            // there is a collision; pick an example to return
            return std::max(range.start, buffer.ranges[r].start);
        }
    }

    buffer.ranges.push_back(range);

    return -1;  // no collision
}

// Recursively figure out how many bytes of xfb buffer are used by the given type.
// Return the size of type, in bytes.
// Sets containsDouble to true if the type contains a double.
// N.B. Caller must set containsDouble to false before calling.
unsigned int TIntermediate::computeTypeXfbSize(const TType& type, bool& containsDouble) const
{
    // "...if applied to an aggregate containing a double, the offset must also be a multiple of 8, 
    // and the space taken in the buffer will be a multiple of 8.
    // ...within the qualified entity, subsequent components are each 
    // assigned, in order, to the next available offset aligned to a multiple of
    // that component's size.  Aggregate types are flattened down to the component
    // level to get this sequence of components."

    if (type.isArray()) {
        assert(type.getArraySize() > 0);
        TType elementType(type, 0);
        return type.getArraySize() * computeTypeXfbSize(elementType, containsDouble);
    }

    if (type.isStruct()) {
        unsigned int size = 0;
        bool structContainsDouble = false;
        for (int member = 0; member < (int)type.getStruct()->size(); ++member) {
            TType memberType(type, member);
            // "... if applied to 
            // an aggregate containing a double, the offset must also be a multiple of 8, 
            // and the space taken in the buffer will be a multiple of 8."
            bool memberContainsDouble = false;
            int memberSize = computeTypeXfbSize(memberType, memberContainsDouble);
            if (memberContainsDouble) {
                structContainsDouble = true;
                RoundToPow2(size, 8);
            }
            size += memberSize;
        }

        if (structContainsDouble) {
            containsDouble = true;
            RoundToPow2(size, 8);
        }
        return size;
    }

    int numComponents;
    if (type.isScalar())
        numComponents = 1;
    else if (type.isVector())
        numComponents = type.getVectorSize();
    else if (type.isMatrix())
        numComponents = type.getMatrixCols() * type.getMatrixRows();
    else {
        assert(0);
        numComponents = 1;
    }

    if (type.getBasicType() == EbtDouble) {
        containsDouble = true;
        return 8 * numComponents;
    } else
        return 4 * numComponents;
}

const int baseAlignmentVec4Std140 = 16;

// Return the size and alignment of a scalar.
// The size is returned in the 'size' parameter
// Return value is the alignment of the type.
int TIntermediate::getBaseAlignmentScalar(const TType& type, int& size) const
{
    switch (type.getBasicType()) {
    case EbtDouble:  size = 8; return 8;
    default:         size = 4; return 4;
    }
}

// Implement base-alignment and size rules from section 7.6.2.2 Standard Uniform Block Layout
// Operates recursively.
//
// If std140 is true, it does the rounding up to vec4 size required by std140, 
// otherwise it does not, yielding std430 rules.
//
// The size is returned in the 'size' parameter
// Return value is the alignment of the type.
int TIntermediate::getBaseAlignment(const TType& type, int& size, bool std140) const
{
    int alignment;

    // When using the std140 storage layout, structures will be laid out in buffer
    // storage with its members stored in monotonically increasing order based on their
    // location in the declaration. A structure and each structure member have a base
    // offset and a base alignment, from which an aligned offset is computed by rounding
    // the base offset up to a multiple of the base alignment. The base offset of the first
    // member of a structure is taken from the aligned offset of the structure itself. The
    // base offset of all other structure members is derived by taking the offset of the
    // last basic machine unit consumed by the previous member and adding one. Each
    // structure member is stored in memory at its aligned offset. The members of a top-
    // level uniform block are laid out in buffer storage by treating the uniform block as
    // a structure with a base offset of zero.
    //
    //   1. If the member is a scalar consuming N basic machine units, the base alignment is N.
    //
    //   2. If the member is a two- or four-component vector with components consuming N basic 
    //      machine units, the base alignment is 2N or 4N, respectively.
    //
    //   3. If the member is a three-component vector with components consuming N
    //      basic machine units, the base alignment is 4N.
    //
    //   4. If the member is an array of scalars or vectors, the base alignment and array
    //      stride are set to match the base alignment of a single array element, according
    //      to rules (1), (2), and (3), and rounded up to the base alignment of a vec4. The
    //      array may have padding at the end; the base offset of the member following
    //      the array is rounded up to the next multiple of the base alignment.
    //
    //   5. If the member is a column-major matrix with C columns and R rows, the
    //      matrix is stored identically to an array of C column vectors with R 
    //      components each, according to rule (4).
    //
    //   6. If the member is an array of S column-major matrices with C columns and
    //      R rows, the matrix is stored identically to a row of S  C column vectors
    //      with R components each, according to rule (4).
    //
    //   7. If the member is a row-major matrix with C columns and R rows, the matrix
    //      is stored identically to an array of R row vectors with C components each,
    //      according to rule (4).
    //
    //   8. If the member is an array of S row-major matrices with C columns and R
    //      rows, the matrix is stored identically to a row of S  R row vectors with C
    //      components each, according to rule (4).
    //
    //   9. If the member is a structure, the base alignment of the structure is N , where
    //      N is the largest base alignment value of any    of its members, and rounded
    //      up to the base alignment of a vec4. The individual members of this substructure 
    //      are then assigned offsets by applying this set of rules recursively,
    //      where the base offset of the first member of the sub-structure is equal to the
    //      aligned offset of the structure. The structure may have padding at the end;
    //      the base offset of the member following the sub-structure is rounded up to
    //      the next multiple of the base alignment of the structure.
    //
    //   10. If the member is an array of S structures, the S elements of the array are laid
    //       out in order, according to rule (9).

    // rules 4, 6, and 8
    if (type.isArray()) {
        TType derefType(type, 0);
        alignment = getBaseAlignment(derefType, size, std140);
        if (std140)
            alignment = std::max(baseAlignmentVec4Std140, alignment);
        RoundToPow2(size, alignment);
        size *= type.getArraySize();
        return alignment;
    }

    // rule 9
    if (type.getBasicType() == EbtStruct) {
        const TTypeList& memberList = *type.getStruct();

        size = 0;
        int maxAlignment = std140 ? baseAlignmentVec4Std140 : 0;
        for (size_t m = 0; m < memberList.size(); ++m) {
            int memberSize;
            int memberAlignment = getBaseAlignment(*memberList[m].type, memberSize, std140);
            maxAlignment = std::max(maxAlignment, memberAlignment);
            RoundToPow2(size, memberAlignment);         
            size += memberSize;
        }

        return maxAlignment;
    }

    // rule 1
    if (type.isScalar())
        return getBaseAlignmentScalar(type, size);

    // rules 2 and 3
    if (type.isVector()) {
        int scalarAlign = getBaseAlignmentScalar(type, size);
        switch (type.getVectorSize()) {
        case 2:
            size *= 2;
            return 2 * scalarAlign;
        default: 
            size *= type.getVectorSize();
            return 4 * scalarAlign;
        }
    }

    // rules 5 and 7
    if (type.isMatrix()) {
        TType derefType(type, 0);
            
        // rule 5: deref to row, not to column, meaning the size of vector is num columns instead of num rows
        if (type.getQualifier().layoutMatrix == ElmRowMajor)
            derefType.setElementType(derefType.getBasicType(), type.getMatrixCols(), 0, 0, 0);
            
        alignment = getBaseAlignment(derefType, size, std140);
        if (std140)
            alignment = std::max(baseAlignmentVec4Std140, alignment);
        RoundToPow2(size, alignment);
        if (type.getQualifier().layoutMatrix == ElmRowMajor)
            size *= type.getMatrixRows();
        else
            size *= type.getMatrixCols();

        return alignment;
    }

    assert(0);  // all cases should be covered above
    size = baseAlignmentVec4Std140;
    return baseAlignmentVec4Std140;
}

} // end namespace glslang
