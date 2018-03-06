/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2013 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef M_SHADOWMAPDR_RENDERER_PRIMITIVE_HPP
#define M_SHADOWMAPDR_RENDERER_PRIMITIVE_HPP

//------------------------------------------
// INCLUDES

#include "mCommon.h"

#include "MArray.h"
#include "MVector3.h"
#include "MBox.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * The class represents data of vertices and theirs attributes.
 */
class MRendererPrimitive
  {
public:

  // ----- Types -----

  ///
  enum MMode
    {
    ///
    MODE_TRIANGLES = GL_TRIANGLES,
    ///
    MODE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    ///
    MODE_POINTS = GL_POINTS,
    ///
    MODE_LINE_STRIP = GL_LINE_STRIP
    };

  ///
  enum MDRDrawingMode
    {
    ///
    DRAWING_ARRAYS,
    ///
    DRAWING_ELEMENTS
    };

  ///
  enum MDRAttrib
    {
    ///
    ATTRIB_VERTICES,
    ///
    ATTRIB_COLORS,
    ///
    ATTRIB_NORMALS,
    ///
    ATTRIB_TEXCOORDS_0,
    ///
    ATTRIB_TEXCOORDS_1,
    ///
    ATTRIB_TEXCOORDS_2,
    ///
    ATTRIB_TEXCOORDS_3,
    ///
    ATTRIB_TEXCOORDS_4,
    ///
    ATTRIB_TEXCOORDS_5,
    ///
    ATTRIB_TEXCOORDS_6,
    ///
    ATTRIB_TEXCOORDS_7,
    ///
    ATTRIBS_COUNT
    };

  // ----- Constructors and destructors -----

  ///
  MRendererPrimitive();

  ///
  ~MRendererPrimitive();

  // ----- Accessors and mutators -----

  /// The method sets primitive mode such as: triangles, triangle strip, points etc.
  void setMode(MMode aMode)
    { theMode = aMode; }

  /// The method sets a drawing mode either Arrays or Elements
  void setDrawingMode(MDRDrawingMode aDrawingMode)
    { theDrawingMode = aDrawingMode; }

  // ----- Miscellaneous -----

  /// The method adds a specified attribute value to the list of attributes.
  void append(MDRAttrib aAttrib,
              const MVector3f& aBox);

  /// The method appends a single index to the list of indices.
  void appendIndex(unsigned short aIndex)
    { theIndices.append(aIndex); }

  /// The method adds four values for a specified attribute. Very useful for defining quads.
  void appendBox(MDRAttrib aAttrib,
                 const MBoxf& aBox);

  /// Append other primitive with a caveat the other primitive must have the same mode as "this" one.
  bool append(const MRendererPrimitive& aOther);

  /// The method clears/removes all attributes
  void clearAll();

  /// The method sets mapping of attributes indices reported by shader program.
  bool setAttribIndex(MDRAttrib aAttrib,
                      int aIndex);

  /// The method sets vertices pointers and calls either glDrawElements or glDrawArrays.
  void render();

  /// The method transforms all vertices by aPosition vector
  /// All the work is done on a CPU
  void transform(const MVector3f& aPosition);

private:

  // ----- Types -----

  ///
  typedef MArray<MVector3f> MArrayf;
  typedef MArray<unsigned short> MArrayus;

  // ----- Fields -----

  //
  MMode theMode;
  MDRDrawingMode theDrawingMode;
  GLuint theBufferHandle;
  //
  MArrayf theData[ATTRIBS_COUNT];
  MArrayus theIndices;
  //
  int theAttribs[ATTRIBS_COUNT];

  };

#endif
