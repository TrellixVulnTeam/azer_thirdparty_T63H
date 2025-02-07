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

#include "localintermediate.h"
#include "../Include/InfoSink.h"

namespace glslang {

//
// Two purposes:
// 1.  Show an example of how to iterate tree.  Functions can
//     also directly call Traverse() on children themselves to
//     have finer grained control over the process than shown here.
//     See the last function for how to get started.
// 2.  Print out a text based description of the tree.
//

//
// Use this class to carry along data from node to node in
// the traversal
//
class TOutputTraverser : public TIntermTraverser {
public:
    TOutputTraverser(TInfoSink& i) : infoSink(i) { }

    virtual bool visitBinary(TVisit, TIntermBinary* node);
    virtual bool visitUnary(TVisit, TIntermUnary* node);
    virtual bool visitAggregate(TVisit, TIntermAggregate* node);
    virtual bool visitSelection(TVisit, TIntermSelection* node);
    virtual void visitConstantUnion(TIntermConstantUnion* node);
    virtual void visitSymbol(TIntermSymbol* node);
    virtual bool visitLoop(TVisit, TIntermLoop* node);
    virtual bool visitBranch(TVisit, TIntermBranch* node);
    virtual bool visitSwitch(TVisit, TIntermSwitch* node);

    TInfoSink& infoSink;
};

//
// Helper functions for printing, not part of traversing.
//

void OutputTreeText(TInfoSink& infoSink, const TIntermNode* node, const int depth)
{
    int i;

    infoSink.debug << node->getLoc().string << ":";
    if (node->getLoc().line)
        infoSink.debug << node->getLoc().line;
    else
        infoSink.debug << "? ";

    for (i = 0; i < depth; ++i)
        infoSink.debug << "  ";
}

//
// The rest of the file are the traversal functions.  The last one
// is the one that starts the traversal.
//
// Return true from interior nodes to have the external traversal
// continue on to children.  If you process children yourself,
// return false.
//

bool TOutputTraverser::visitBinary(TVisit /* visit */, TIntermBinary* node)
{
    TInfoSink& out = infoSink;

    OutputTreeText(out, node, depth);

    switch (node->getOp()) {
    case EOpAssign:                   out.debug << "move second child to first child";           break;
    case EOpAddAssign:                out.debug << "add second child into first child";          break;
    case EOpSubAssign:                out.debug << "subtract second child into first child";     break;
    case EOpMulAssign:                out.debug << "multiply second child into first child";     break;
    case EOpVectorTimesMatrixAssign:  out.debug << "matrix mult second child into first child";  break;
    case EOpVectorTimesScalarAssign:  out.debug << "vector scale second child into first child"; break;
    case EOpMatrixTimesScalarAssign:  out.debug << "matrix scale second child into first child"; break;
    case EOpMatrixTimesMatrixAssign:  out.debug << "matrix mult second child into first child";  break;
    case EOpDivAssign:                out.debug << "divide second child into first child";       break;
    case EOpModAssign:                out.debug << "mod second child into first child";          break;
    case EOpAndAssign:                out.debug << "and second child into first child";          break;
    case EOpInclusiveOrAssign:        out.debug << "or second child into first child";           break;
    case EOpExclusiveOrAssign:        out.debug << "exclusive or second child into first child"; break;
    case EOpLeftShiftAssign:          out.debug << "left shift second child into first child";   break;
    case EOpRightShiftAssign:         out.debug << "right shift second child into first child";  break;

    case EOpIndexDirect:   out.debug << "direct index";   break;
    case EOpIndexIndirect: out.debug << "indirect index"; break;
    case EOpIndexDirectStruct:
        out.debug << (*node->getLeft()->getType().getStruct())[node->getRight()->getAsConstantUnion()->getConstArray()[0].getIConst()].type->getFieldName();
        out.debug << ": direct index for structure";      break;
    case EOpVectorSwizzle: out.debug << "vector swizzle"; break;

    case EOpAdd:    out.debug << "add";                     break;
    case EOpSub:    out.debug << "subtract";                break;
    case EOpMul:    out.debug << "component-wise multiply"; break;
    case EOpDiv:    out.debug << "divide";                  break;
    case EOpMod:    out.debug << "mod";                     break;
    case EOpRightShift:  out.debug << "right-shift";  break;
    case EOpLeftShift:   out.debug << "left-shift";   break;
    case EOpAnd:         out.debug << "bitwise and";  break;
    case EOpInclusiveOr: out.debug << "inclusive-or"; break;
    case EOpExclusiveOr: out.debug << "exclusive-or"; break;
    case EOpEqual:            out.debug << "Compare Equal";                 break;
    case EOpNotEqual:         out.debug << "Compare Not Equal";             break;
    case EOpLessThan:         out.debug << "Compare Less Than";             break;
    case EOpGreaterThan:      out.debug << "Compare Greater Than";          break;
    case EOpLessThanEqual:    out.debug << "Compare Less Than or Equal";    break;
    case EOpGreaterThanEqual: out.debug << "Compare Greater Than or Equal"; break;

    case EOpVectorTimesScalar: out.debug << "vector-scale";          break;
    case EOpVectorTimesMatrix: out.debug << "vector-times-matrix";   break;
    case EOpMatrixTimesVector: out.debug << "matrix-times-vector";   break;
    case EOpMatrixTimesScalar: out.debug << "matrix-scale";          break;
    case EOpMatrixTimesMatrix: out.debug << "matrix-multiply";       break;

    case EOpLogicalOr:  out.debug << "logical-or";   break;
    case EOpLogicalXor: out.debug << "logical-xor"; break;
    case EOpLogicalAnd: out.debug << "logical-and"; break;
    default: out.debug << "<unknown op>";
    }

    out.debug << " (" << node->getCompleteString() << ")";

    out.debug << "\n";

    return true;
}

bool TOutputTraverser::visitUnary(TVisit /* visit */, TIntermUnary* node)
{
    TInfoSink& out = infoSink;

    OutputTreeText(out, node, depth);

    switch (node->getOp()) {
    case EOpNegative:       out.debug << "Negate value";         break;
    case EOpVectorLogicalNot:
    case EOpLogicalNot:     out.debug << "Negate conditional";   break;
    case EOpBitwiseNot:     out.debug << "Bitwise not";          break;

    case EOpPostIncrement:  out.debug << "Post-Increment";       break;
    case EOpPostDecrement:  out.debug << "Post-Decrement";       break;
    case EOpPreIncrement:   out.debug << "Pre-Increment";        break;
    case EOpPreDecrement:   out.debug << "Pre-Decrement";        break;

    case EOpConvIntToBool:     out.debug << "Convert int to bool";     break;
    case EOpConvUintToBool:    out.debug << "Convert uint to bool";    break;
    case EOpConvFloatToBool:   out.debug << "Convert float to bool";   break;
    case EOpConvDoubleToBool:  out.debug << "Convert double to bool";  break;
    case EOpConvIntToFloat:    out.debug << "Convert int to float";    break;
    case EOpConvUintToFloat:   out.debug << "Convert uint to float";   break;
    case EOpConvDoubleToFloat: out.debug << "Convert double to float"; break;
    case EOpConvBoolToFloat:   out.debug << "Convert bool to float";   break;
    case EOpConvUintToInt:     out.debug << "Convert uint to int";     break;
    case EOpConvFloatToInt:    out.debug << "Convert float to int";    break;
    case EOpConvDoubleToInt:   out.debug << "Convert double to int";   break;
    case EOpConvBoolToInt:     out.debug << "Convert bool to int";     break;
    case EOpConvIntToUint:     out.debug << "Convert int to uint";     break;
    case EOpConvFloatToUint:   out.debug << "Convert float to uint";   break;
    case EOpConvDoubleToUint:  out.debug << "Convert double to uint";  break;
    case EOpConvBoolToUint:    out.debug << "Convert bool to uint";    break;
    case EOpConvIntToDouble:   out.debug << "Convert int to double";   break;
    case EOpConvUintToDouble:  out.debug << "Convert uint to double";  break;
    case EOpConvFloatToDouble: out.debug << "Convert float to double"; break;
    case EOpConvBoolToDouble:  out.debug << "Convert bool to double";  break;

    case EOpRadians:        out.debug << "radians";              break;
    case EOpDegrees:        out.debug << "degrees";              break;
    case EOpSin:            out.debug << "sine";                 break;
    case EOpCos:            out.debug << "cosine";               break;
    case EOpTan:            out.debug << "tangent";              break;
    case EOpAsin:           out.debug << "arc sine";             break;
    case EOpAcos:           out.debug << "arc cosine";           break;
    case EOpAtan:           out.debug << "arc tangent";          break;
    case EOpSinh:           out.debug << "hyp. sine";            break;
    case EOpCosh:           out.debug << "hyp. cosine";          break;
    case EOpTanh:           out.debug << "hyp. tangent";         break;
    case EOpAsinh:          out.debug << "arc hyp. sine";        break;
    case EOpAcosh:          out.debug << "arc hyp. cosine";      break;
    case EOpAtanh:          out.debug << "arc hyp. tangent";     break;

    case EOpExp:            out.debug << "exp";                  break;
    case EOpLog:            out.debug << "log";                  break;
    case EOpExp2:           out.debug << "exp2";                 break;
    case EOpLog2:           out.debug << "log2";                 break;
    case EOpSqrt:           out.debug << "sqrt";                 break;
    case EOpInverseSqrt:    out.debug << "inverse sqrt";         break;

    case EOpAbs:            out.debug << "Absolute value";       break;
    case EOpSign:           out.debug << "Sign";                 break;
    case EOpFloor:          out.debug << "Floor";                break;
    case EOpTrunc:          out.debug << "trunc";                break;
    case EOpRound:          out.debug << "round";                break;
    case EOpRoundEven:      out.debug << "roundEven";            break;
    case EOpCeil:           out.debug << "Ceiling";              break;
    case EOpFract:          out.debug << "Fraction";             break;

    case EOpIsNan:          out.debug << "isnan";                break;
    case EOpIsInf:          out.debug << "isinf";                break;

    case EOpFloatBitsToInt: out.debug << "floatBitsToInt";       break;
    case EOpFloatBitsToUint:out.debug << "floatBitsToUint";      break;
    case EOpIntBitsToFloat: out.debug << "intBitsToFloat";       break;
    case EOpUintBitsToFloat:out.debug << "uintBitsToFloat";      break;
    case EOpPackSnorm2x16:  out.debug << "packSnorm2x16";        break;
    case EOpUnpackSnorm2x16:out.debug << "unpackSnorm2x16";      break;
    case EOpPackUnorm2x16:  out.debug << "packUnorm2x16";        break;
    case EOpUnpackUnorm2x16:out.debug << "unpackUnorm2x16";      break;
    case EOpPackHalf2x16:   out.debug << "packHalf2x16";         break;
    case EOpUnpackHalf2x16: out.debug << "unpackHalf2x16";       break;

    case EOpLength:         out.debug << "length";               break;
    case EOpNormalize:      out.debug << "normalize";            break;
    case EOpDPdx:           out.debug << "dPdx";                 break;
    case EOpDPdy:           out.debug << "dPdy";                 break;
    case EOpFwidth:         out.debug << "fwidth";               break;
    case EOpDeterminant:    out.debug << "determinant";          break;
    case EOpMatrixInverse:  out.debug << "inverse";              break;
    case EOpTranspose:      out.debug << "transpose";            break;

    case EOpAny:            out.debug << "any";                  break;
    case EOpAll:            out.debug << "all";                  break;

    case EOpEmitStreamVertex:   out.debug << "EmitStreamVertex";   break;
    case EOpEndStreamPrimitive: out.debug << "EndStreamPrimitive"; break;

    default: out.debug.message(EPrefixError, "Bad unary op");
    }

    out.debug << " (" << node->getCompleteString() << ")";

    out.debug << "\n";

    return true;
}

bool TOutputTraverser::visitAggregate(TVisit /* visit */, TIntermAggregate* node)
{
    TInfoSink& out = infoSink;

    if (node->getOp() == EOpNull) {
        out.debug.message(EPrefixError, "node is still EOpNull!");
        return true;
    }

    OutputTreeText(out, node, depth);

    switch (node->getOp()) {
    case EOpSequence:      out.debug << "Sequence\n";       return true;
    case EOpLinkerObjects: out.debug << "Linker Objects\n"; return true;
    case EOpComma:         out.debug << "Comma";            break;
    case EOpFunction:      out.debug << "Function Definition: " << node->getName(); break;
    case EOpFunctionCall:  out.debug << "Function Call: "       << node->getName(); break;
    case EOpParameters:    out.debug << "Function Parameters: ";                    break;

    case EOpConstructFloat: out.debug << "Construct float"; break;
    case EOpConstructVec2:  out.debug << "Construct vec2";  break;
    case EOpConstructVec3:  out.debug << "Construct vec3";  break;
    case EOpConstructVec4:  out.debug << "Construct vec4";  break;
    case EOpConstructBool:  out.debug << "Construct bool";  break;
    case EOpConstructBVec2: out.debug << "Construct bvec2"; break;
    case EOpConstructBVec3: out.debug << "Construct bvec3"; break;
    case EOpConstructBVec4: out.debug << "Construct bvec4"; break;
    case EOpConstructInt:   out.debug << "Construct int";   break;
    case EOpConstructIVec2: out.debug << "Construct ivec2"; break;
    case EOpConstructIVec3: out.debug << "Construct ivec3"; break;
    case EOpConstructIVec4: out.debug << "Construct ivec4"; break;
    case EOpConstructUint:    out.debug << "Construct uint";    break;
    case EOpConstructUVec2:   out.debug << "Construct uvec2";   break;
    case EOpConstructUVec3:   out.debug << "Construct uvec3";   break;
    case EOpConstructUVec4:   out.debug << "Construct uvec4";   break;
    case EOpConstructMat2x2:  out.debug << "Construct mat2";    break;
    case EOpConstructMat2x3:  out.debug << "Construct mat2x3";  break;
    case EOpConstructMat2x4:  out.debug << "Construct mat2x4";  break;
    case EOpConstructMat3x2:  out.debug << "Construct mat3x2";  break;
    case EOpConstructMat3x3:  out.debug << "Construct mat3";    break;
    case EOpConstructMat3x4:  out.debug << "Construct mat3x4";  break;
    case EOpConstructMat4x2:  out.debug << "Construct mat4x2";  break;
    case EOpConstructMat4x3:  out.debug << "Construct mat4x3";  break;
    case EOpConstructMat4x4:  out.debug << "Construct mat4";    break;
    case EOpConstructDMat2x2: out.debug << "Construct dmat2";   break;
    case EOpConstructDMat2x3: out.debug << "Construct dmat2x3"; break;
    case EOpConstructDMat2x4: out.debug << "Construct dmat2x4"; break;
    case EOpConstructDMat3x2: out.debug << "Construct dmat3x2"; break;
    case EOpConstructDMat3x3: out.debug << "Construct dmat3";   break;
    case EOpConstructDMat3x4: out.debug << "Construct dmat3x4"; break;
    case EOpConstructDMat4x2: out.debug << "Construct dmat4x2"; break;
    case EOpConstructDMat4x3: out.debug << "Construct dmat4x3"; break;
    case EOpConstructDMat4x4: out.debug << "Construct dmat4";  break;
    case EOpConstructStruct:  out.debug << "Construct structure";  break;

    case EOpLessThan:         out.debug << "Compare Less Than";             break;
    case EOpGreaterThan:      out.debug << "Compare Greater Than";          break;
    case EOpLessThanEqual:    out.debug << "Compare Less Than or Equal";    break;
    case EOpGreaterThanEqual: out.debug << "Compare Greater Than or Equal"; break;
    case EOpVectorEqual:      out.debug << "Equal";                         break;
    case EOpVectorNotEqual:   out.debug << "NotEqual";                      break;

    case EOpMod:           out.debug << "mod";         break;
    case EOpModf:          out.debug << "modf";        break;
    case EOpPow:           out.debug << "pow";         break;

    case EOpAtan:          out.debug << "arc tangent"; break;

    case EOpMin:           out.debug << "min";         break;
    case EOpMax:           out.debug << "max";         break;
    case EOpClamp:         out.debug << "clamp";       break;
    case EOpMix:           out.debug << "mix";         break;
    case EOpStep:          out.debug << "step";        break;
    case EOpSmoothStep:    out.debug << "smoothstep";  break;

    case EOpDistance:      out.debug << "distance";                break;
    case EOpDot:           out.debug << "dot-product";             break;
    case EOpCross:         out.debug << "cross-product";           break;
    case EOpFaceForward:   out.debug << "face-forward";            break;
    case EOpReflect:       out.debug << "reflect";                 break;
    case EOpRefract:       out.debug << "refract";                 break;
    case EOpMul:           out.debug << "component-wise multiply"; break;
    case EOpOuterProduct:  out.debug << "outer product";           break;

    case EOpEmitVertex:    out.debug << "EmitVertex";              break;
    case EOpEndPrimitive:  out.debug << "EndPrimitive";            break;

    case EOpBarrier:                    out.debug << "Barrier";                    break;
    case EOpMemoryBarrier:              out.debug << "MemoryBarrier";              break;
    case EOpMemoryBarrierAtomicCounter: out.debug << "MemoryBarrierAtomicCounter"; break;
    case EOpMemoryBarrierBuffer:        out.debug << "MemoryBarrierBuffer";        break;
    case EOpMemoryBarrierImage:         out.debug << "MemoryBarrierImage";         break;
    case EOpMemoryBarrierShared:        out.debug << "MemoryBarrierShared";        break;
    case EOpGroupMemoryBarrier:         out.debug << "GroupMemoryBarrier";         break;

    default: out.debug.message(EPrefixError, "Bad aggregation op");
    }

    if (node->getOp() != EOpSequence && node->getOp() != EOpParameters)
        out.debug << " (" << node->getCompleteString() << ")";

    out.debug << "\n";

    return true;
}

bool TOutputTraverser::visitSelection(TVisit /* visit */, TIntermSelection* node)
{
    TInfoSink& out = infoSink;

    OutputTreeText(out, node, depth);

    out.debug << "Test condition and select";
    out.debug << " (" << node->getCompleteString() << ")\n";

    ++depth;

    OutputTreeText(out, node, depth);
    out.debug << "Condition\n";
    node->getCondition()->traverse(this);

    OutputTreeText(out, node, depth);
    if (node->getTrueBlock()) {
        out.debug << "true case\n";
        node->getTrueBlock()->traverse(this);
    } else
        out.debug << "true case is null\n";

    if (node->getFalseBlock()) {
        OutputTreeText(out, node, depth);
        out.debug << "false case\n";
        node->getFalseBlock()->traverse(this);
    }

    --depth;

    return false;
}

void OutputConstantUnion(TInfoSink& out, const TIntermTyped* node, const TConstUnionArray& constUnion, int depth)
{
    int size = node->getType().computeNumComponents();

    for (int i = 0; i < size; i++) {
        OutputTreeText(out, node, depth);
        switch (constUnion[i].getType()) {
        case EbtBool:
            if (constUnion[i].getBConst())
                out.debug << "true";
            else
                out.debug << "false";

            out.debug << " (" << "const bool" << ")";

            out.debug << "\n";
            break;
        case EbtFloat:
        case EbtDouble:
            {
				const int maxSize = 300;
                char buf[maxSize];
                snprintf(buf, maxSize, "%f", constUnion[i].getDConst());

                out.debug << buf << "\n";
            }
            break;
        case EbtInt:
            {
				const int maxSize = 300;
                char buf[maxSize];
                snprintf(buf, maxSize, "%d (%s)", constUnion[i].getIConst(), "const int");

                out.debug << buf << "\n";
            }
            break;
        case EbtUint:
            {
				const int maxSize = 300;
                char buf[maxSize];
                snprintf(buf, maxSize, "%u (%s)", constUnion[i].getUConst(), "const uint");

                out.debug << buf << "\n";
            }
            break;
        default:
            out.info.message(EPrefixInternalError, "Unknown constant", node->getLoc());
            break;
        }
    }
}

void TOutputTraverser::visitConstantUnion(TIntermConstantUnion* node)
{
    OutputTreeText(infoSink, node, depth);
    infoSink.debug << "Constant:\n";

    OutputConstantUnion(infoSink, node, node->getConstArray(), depth + 1);
}

void TOutputTraverser::visitSymbol(TIntermSymbol* node)
{
    OutputTreeText(infoSink, node, depth);

    const int maxSize = GlslangMaxTypeLength + GlslangMaxTokenLength;
    char buf[maxSize];
    snprintf(buf, maxSize, "'%s' (%s)\n",
             node->getName().c_str(),
             node->getCompleteString().c_str());
    infoSink.debug << buf;

    if (! node->getConstArray().empty())
        OutputConstantUnion(infoSink, node, node->getConstArray(), depth + 1);
}

bool TOutputTraverser::visitLoop(TVisit /* visit */, TIntermLoop* node)
{
    TInfoSink& out = infoSink;

    OutputTreeText(out, node, depth);

    out.debug << "Loop with condition ";
    if (! node->testFirst())
        out.debug << "not ";
    out.debug << "tested first\n";

    ++depth;

    OutputTreeText(infoSink, node, depth);
    if (node->getTest()) {
        out.debug << "Loop Condition\n";
        node->getTest()->traverse(this);
    } else
        out.debug << "No loop condition\n";

    OutputTreeText(infoSink, node, depth);
    if (node->getBody()) {
        out.debug << "Loop Body\n";
        node->getBody()->traverse(this);
    } else
        out.debug << "No loop body\n";

    if (node->getTerminal()) {
        OutputTreeText(infoSink, node, depth);
        out.debug << "Loop Terminal Expression\n";
        node->getTerminal()->traverse(this);
    }

    --depth;

    return false;
}

bool TOutputTraverser::visitBranch(TVisit /* visit*/, TIntermBranch* node)
{
    TInfoSink& out = infoSink;

    OutputTreeText(out, node, depth);

    switch (node->getFlowOp()) {
    case EOpKill:      out.debug << "Branch: Kill";           break;
    case EOpBreak:     out.debug << "Branch: Break";          break;
    case EOpContinue:  out.debug << "Branch: Continue";       break;
    case EOpReturn:    out.debug << "Branch: Return";         break;
    case EOpCase:      out.debug << "case: ";                 break;
    case EOpDefault:   out.debug << "default: ";              break;
    default:               out.debug << "Branch: Unknown Branch"; break;
    }

    if (node->getExpression()) {
        out.debug << " with expression\n";
        ++depth;
        node->getExpression()->traverse(this);
        --depth;
    } else
        out.debug << "\n";

    return false;
}

bool TOutputTraverser::visitSwitch(TVisit /* visit */, TIntermSwitch* node)
{
    TInfoSink& out = infoSink;

    OutputTreeText(out, node, depth);
    out.debug << "switch\n";

    OutputTreeText(out, node, depth);
    out.debug << "condition\n";
    ++depth;
    node->getCondition()->traverse(this);

    --depth;
    OutputTreeText(out, node, depth);
    out.debug << "body\n";
    ++depth;
    node->getBody()->traverse(this);

    --depth;

    return false;
}

//
// This function is the one to call externally to start the traversal.
// Individual functions can be initialized to 0 to skip processing of that
// type of node.  It's children will still be processed.
//
void TIntermediate::output(TInfoSink& infoSink, bool tree)
{
    if (xfbMode)
        infoSink.debug << "in xfb mode\n";

    switch (language) {
    case EShLangVertex:
        break;

    case EShLangTessControl:
        infoSink.debug << "vertices = " << vertices << "\n";
        break;

    case EShLangTessEvaluation:
        infoSink.debug << "input primitive = " << TQualifier::getGeometryString(inputPrimitive) << "\n";
        infoSink.debug << "vertex spacing = " << TQualifier::getVertexSpacingString(vertexSpacing) << "\n";
        infoSink.debug << "triangle order = " << TQualifier::getVertexOrderString(vertexOrder) << "\n";
        if (pointMode)
            infoSink.debug << "using point mode\n";
        break;

    case EShLangGeometry:
        infoSink.debug << "invocations = " << invocations << "\n";
        infoSink.debug << "max_vertices = " << vertices << "\n";
        infoSink.debug << "input primitive = " << TQualifier::getGeometryString(inputPrimitive) << "\n";
        infoSink.debug << "output primitive = " << TQualifier::getGeometryString(outputPrimitive) << "\n";
        break;

    case EShLangFragment:
        if (pixelCenterInteger)
            infoSink.debug << "gl_FragCoord pixel center is integer\n";
        if (originUpperLeft)
            infoSink.debug << "gl_FragCoord origin is upper left\n";
        break;

    case EShLangCompute:
        break;

	default:
        break;
    }

    if (treeRoot == 0 || ! tree)
        return;

    TOutputTraverser it(infoSink);

    treeRoot->traverse(&it);
}

} // end namespace glslang
