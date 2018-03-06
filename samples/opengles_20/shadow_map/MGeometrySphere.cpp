/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2013 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#include "MGeometrySphere.h"

MGeometrySphere::MGeometrySphere()
    :
    BaseClass()
  {
  }

MGeometrySphere::~MGeometrySphere()
  {
  }

void MGeometrySphere::set(
    float aRadius,
    unsigned int aHorizontal,
    unsigned int aVertical)
  {
  MArrayVec3f points;
  MArrayVec3f coords;

  // Evaluate vertices points for the whole sphere
  float radius = aRadius;
  float rad = 0.0f;
  MVector3f tmp(0.0f, 0.0f, 0.0f);
  //
  for (unsigned int iy = 0; iy <= aVertical; iy++)
    {
    float alpha = (float)iy * (180.f / (float)aVertical);
    rad = (float)(alpha * 0.01745329251994329576923690768489);
    //
    float fx = 0.0f;
    float fy = radius * cosf(rad);
    float fz = radius * sinf(rad);
    //
    for (unsigned int ix = 0; ix <= aHorizontal; ix++)
      {
      float beta = (float)ix * (360.f / (float)(aHorizontal));
      float rad = (float)(beta * 0.01745329251994329576923690768489);
      //
      tmp[0] = fz * sinf(rad);
      tmp[1] = fy;
      tmp[2] = fz * cosf(rad);
      points.append(tmp);
      //
      tmp[0] = (float)ix / (float)aHorizontal;
      tmp[1] = (float)(aVertical - iy) / (float)aVertical;
      tmp[2] = 0.0f;
      coords.append(tmp);
      }
    }

#define MDR_DRAWING_ELEMENTS
#ifdef MDR_DRAWING_ELEMENTS
  buildPrimitveTSI(aHorizontal + 1,
                   aVertical + 1,
                   points,
                   coords,
                   getPrimitive());
#else
  buildPrimitveTS(aHorizontal,
                  aVertical,
                  points,
                  getPrimitive());
#endif

  }

void MGeometrySphere::buildPrimitveTSI(
    unsigned int aHorizontal,
    unsigned int aVertical,
    const MArrayVec3f& aPoints,
    const MArrayVec3f& aCoords,
    MRendererPrimitive& aOutPrimitive) const
  {
  aOutPrimitive.clearAll();
  aOutPrimitive.setMode(MRendererPrimitive::MODE_TRIANGLE_STRIP);
  aOutPrimitive.setDrawingMode(MRendererPrimitive::DRAWING_ELEMENTS);

  // Copy points one-to-one
  unsigned int count = aPoints.getCount();
  for (unsigned int index = 0; index < count; index++)
    {
    const MVector3f& point = aPoints[index];
    //
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, point);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, point);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, point + 0.5f);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_TEXCOORDS_0, aCoords[index]);
    }

  // Build indices
  for (unsigned int iy = 1; iy < aVertical; iy++)
    {
    for (unsigned int ix = 1; ix < aHorizontal; ix++)
      {
      /*

              vPrev
         [ix - 1, iy - 1]
                o
                  \
                    \
                      \
                        \
                          o
                       [ix, iy]
                         vCurr

      */
      unsigned int iPrev = (iy - 1) * aHorizontal + (ix - 1);
      unsigned int iCurr = iy * aHorizontal + ix;
      //
      aOutPrimitive.appendIndex(iPrev);
      aOutPrimitive.appendIndex(iCurr);
      }
    /*

            vLast                vFirstPrev
    [aHorizontal - 1, iy - 1]   [0, iy - 1]
              o                   o
                  \               |   \
                      \           |       \
                          \       |           \
                              \   |               \
                                  o                   o
                               [0, iy]              [1, iy]
                              vFirstCurr           vSecondCurr

    */
    // Close the slice
    unsigned int iLast       = (iy - 1) * aHorizontal + (aHorizontal - 1);
    unsigned int iFirstCurr  = iy * aHorizontal + 0;
    unsigned int iFirstPrev  = (iy - 1) * aHorizontal + 0;
    unsigned int iSecondCurr = iy * aHorizontal + 1;

    aOutPrimitive.appendIndex(iLast);
    aOutPrimitive.appendIndex(iFirstCurr);
    aOutPrimitive.appendIndex(iFirstPrev);
    aOutPrimitive.appendIndex(iSecondCurr);

    // Check if next slice is available and set vertex position in appropriate place to
    // start drawing triangles for next slice (row)
//    if (iy < (aVertical - 1))
      {
      // In this case the cursor of drawing should be move to the position of drawing next slices without
      // drawing triangles and without necessary of switching drawing modes nor by calling
      // glDrawArrays or glDrawElements for an each slice

      // Draw invisible line from vFirstPrev to vSecondCurr
      aOutPrimitive.appendIndex(iSecondCurr);
      // Draw invisible line from vSecondCurr to vFirstCurr
      aOutPrimitive.appendIndex(iFirstCurr);
      // Draw point in vFirstCurr
      aOutPrimitive.appendIndex(iFirstCurr);
      // Draw the point once again to correct face culling
      aOutPrimitive.appendIndex(iFirstCurr);
      }
    // That's all, lets try!!!
    }
  }

void MGeometrySphere::buildPrimitveTS(
    unsigned int aHorizontal,
    unsigned int aVertical,
    const MArrayVec3f& aPoints,
    MRendererPrimitive& aOutPrimitive) const
  {
  aOutPrimitive.clearAll();
  aOutPrimitive.setMode(MRendererPrimitive::MODE_TRIANGLE_STRIP);
  aOutPrimitive.setDrawingMode(MRendererPrimitive::DRAWING_ARRAYS);

  // Build primitive based on previously created points with STRIP mode
    {
    for (unsigned int iy = 1; iy < aVertical; iy++)
      {
      for (unsigned int ix = 1; ix < aHorizontal; ix++)
        {
        /*

                vPrev
           [ix - 1, iy - 1]
                  o
                    \
                      \
                        \
                          \
                            o
                         [ix, iy]
                           vCurr

        */
        const MVector3f& vPrev = aPoints[(iy - 1) * aHorizontal + (ix - 1)];
        const MVector3f& vCurr = aPoints[iy * aHorizontal + ix];
        //
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vPrev);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vCurr);
        //
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vPrev);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vCurr);
        //
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vPrev[0] + 0.5f, vPrev[1] + 0.5f, vPrev[2] + 0.5f));
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vCurr[0] + 0.5f, vCurr[1] + 0.5f, vCurr[2] + 0.5f));
        }
      /*

              vLast                vFirstPrev
      [aHorizontal - 1, iy - 1]   [0, iy - 1]
                o                   o
                    \               |   \
                        \           |       \
                            \       |           \
                                \   |               \
                                    o                   o
                                 [0, iy]              [1, iy]
                                vFirstCurr           vSecondCurr

      */
      // Close the slice
      const MVector3f& vLast       = aPoints[(iy - 1) * aHorizontal + (aHorizontal - 1)];
      const MVector3f& vFirstCurr  = aPoints[iy * aHorizontal + 0];
      const MVector3f& vFirstPrev  = aPoints[(iy - 1) * aHorizontal + 0];
      const MVector3f& vSecondCurr = aPoints[iy * aHorizontal + 1];
      //
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vLast);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstPrev);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vSecondCurr);
      //
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vLast);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vFirstCurr);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vFirstPrev);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vSecondCurr);
      //
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vLast[0] + 0.5f, vLast[1] + 0.5f, vLast[2] + 0.5f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vFirstCurr[0] + 0.5f, vFirstCurr[1] + 0.5f, vFirstCurr[2] + 0.5f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vFirstPrev[0] + 0.5f, vFirstPrev[1] + 0.5f, vFirstPrev[2] + 0.5f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vSecondCurr[0] + 0.5f, vSecondCurr[1] + 0.5f, vSecondCurr[2] + 0.5f));

      // Check if next slice is available and set vertex position in appropriate place to
      // start drawing triangles for next slice (row)
      if (iy < (aVertical - 1))
        {
        // In this case the cursor of drawing should be move to the position of drawing next slices without
        // drawing triangles and without necessary of switching drawing modes nor by calling
        // glDrawArrays or glDrawElements for an each slice

        // Draw invisible line from vFirstPrev to vSecondCurr
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vSecondCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vSecondCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 1.0f));
        // Draw invisible line from vSecondCurr to vFirstCurr
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vFirstCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 1.0f));
        // Draw point in vFirstCurr
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vFirstCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 1.0f));
        // Draw the point once again to correct face culling
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, vFirstCurr);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 1.0f));
        }
      // That's all, lets try!!!
      }
    }
  }