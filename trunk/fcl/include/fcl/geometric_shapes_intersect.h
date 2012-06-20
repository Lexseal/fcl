/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Jia Pan */


#ifndef FCL_GEOMETRIC_SHAPES_INTERSECT_H
#define FCL_GEOMETRIC_SHAPES_INTERSECT_H

#include "fcl/geometric_shapes.h"
#include "fcl/transform.h"

#include <ccd/ccd.h>
#include <ccd/quat.h>

namespace fcl
{

/** \brief recall function used by GJK algorithm */
typedef void (*GJKSupportFunction)(const void* obj, const ccd_vec3_t* dir_, ccd_vec3_t* v);
typedef void (*GJKCenterFunction)(const void* obj, ccd_vec3_t* c);

/** \brief initialize GJK stuffs */
template<typename T>
class GJKInitializer
{
public:
  /** \brief Get GJK support function */
  static GJKSupportFunction getSupportFunction() { return NULL; }

  /** \brief Get GJK center function */
  static GJKCenterFunction getCenterFunction() { return NULL; }

  /** \brief Get GJK object from a shape
   * Notice that only local transformation is applied.
   * Gloal transformation are considered later
   */
  static void* createGJKObject(const T& s, const SimpleTransform& tf) { return NULL; }

  /** \brief Delete GJK object */
  static void deleteGJKObject(void* o) {}
};

/** \brief initialize GJK Cylinder */
template<>
class GJKInitializer<Cylinder>
{
public:
  static GJKSupportFunction getSupportFunction();
  static GJKCenterFunction getCenterFunction();
  static void* createGJKObject(const Cylinder& s, const SimpleTransform& tf);
  static void deleteGJKObject(void* o);
};

/** \brief initialize GJK Sphere */
template<>
class GJKInitializer<Sphere>
{
public:
  static GJKSupportFunction getSupportFunction();
  static GJKCenterFunction getCenterFunction();
  static void* createGJKObject(const Sphere& s, const SimpleTransform& tf);
  static void deleteGJKObject(void* o);
};

/** \brief initialize GJK Box */
template<>
class GJKInitializer<Box>
{
public:
  static GJKSupportFunction getSupportFunction();
  static GJKCenterFunction getCenterFunction();
  static void* createGJKObject(const Box& s, const SimpleTransform& tf);
  static void deleteGJKObject(void* o);
};

/** \brief initialize GJK Capsule */
template<>
class GJKInitializer<Capsule>
{
public:
  static GJKSupportFunction getSupportFunction();
  static GJKCenterFunction getCenterFunction();
  static void* createGJKObject(const Capsule& s, const SimpleTransform& tf);
  static void deleteGJKObject(void* o);
};

/** \brief initialize GJK Cone */
template<>
class GJKInitializer<Cone>
{
public:
  static GJKSupportFunction getSupportFunction();
  static GJKCenterFunction getCenterFunction();
  static void* createGJKObject(const Cone& s, const SimpleTransform& tf);
  static void deleteGJKObject(void* o);
};

/** \brief initialize GJK Convex */
template<>
class GJKInitializer<Convex>
{
public:
  static GJKSupportFunction getSupportFunction();
  static GJKCenterFunction getCenterFunction();
  static void* createGJKObject(const Convex& s, const SimpleTransform& tf);
  static void deleteGJKObject(void* o);
};

/** \brief initialize GJK Triangle */
GJKSupportFunction triGetSupportFunction();

GJKCenterFunction triGetCenterFunction();

void* triCreateGJKObject(const Vec3f& P1, const Vec3f& P2, const Vec3f& P3);

void* triCreateGJKObject(const Vec3f& P1, const Vec3f& P2, const Vec3f& P3, const Matrix3f& R, const Vec3f& T);

void triDeleteGJKObject(void* o);

/** \brief GJK collision algorithm */
bool GJKCollide(void* obj1, ccd_support_fn supp1, ccd_center_fn cen1,
               void* obj2, ccd_support_fn supp2, ccd_center_fn cen2,
               Vec3f* contact_points, BVH_REAL* penetration_depth, Vec3f* normal);


/** intersection checking between two shapes */
template<typename S1, typename S2>
bool shapeIntersect(const S1& s1, const SimpleTransform& tf1,
                    const S2& s2, const SimpleTransform& tf2,
                    Vec3f* contact_points = NULL, BVH_REAL* penetration_depth = NULL, Vec3f* normal = NULL)
{
  void* o1 = GJKInitializer<S1>::createGJKObject(s1, tf1);
  void* o2 = GJKInitializer<S2>::createGJKObject(s2, tf2);

  bool res =  GJKCollide(o1, GJKInitializer<S1>::getSupportFunction(), GJKInitializer<S1>::getCenterFunction(),
                         o2, GJKInitializer<S2>::getSupportFunction(), GJKInitializer<S2>::getCenterFunction(),
                         contact_points, penetration_depth, normal);

  GJKInitializer<S1>::deleteGJKObject(o1);
  GJKInitializer<S2>::deleteGJKObject(o2);

  return res;
}

/** \brief intersection checking between one shape and a triangle */
template<typename S>
bool shapeTriangleIntersect(const S& s, const SimpleTransform& tf,
                            const Vec3f& P1, const Vec3f& P2, const Vec3f& P3, Vec3f* contact_points = NULL, BVH_REAL* penetration_depth = NULL, Vec3f* normal = NULL)
{
  void* o1 = GJKInitializer<S>::createGJKObject(s, tf);
  void* o2 = triCreateGJKObject(P1, P2, P3);

  bool res = GJKCollide(o1, GJKInitializer<S>::getSupportFunction(), GJKInitializer<S>::getCenterFunction(),
                        o2, triGetSupportFunction(), triGetCenterFunction(),
                        contact_points, penetration_depth, normal);

  GJKInitializer<S>::deleteGJKObject(o1);
  triDeleteGJKObject(o2);

  return res;
}

/** \brief intersection checking between one shape and a triangle with transformation */
template<typename S>
bool shapeTriangleIntersect(const S& s, const SimpleTransform& tf,
                            const Vec3f& P1, const Vec3f& P2, const Vec3f& P3, const Matrix3f& R, const Vec3f& T,
                            Vec3f* contact_points = NULL, BVH_REAL* penetration_depth = NULL, Vec3f* normal = NULL)
{
  void* o1 = GJKInitializer<S>::createGJKObject(s, tf);
  void* o2 = triCreateGJKObject(P1, P2, P3, R, T);

  bool res = GJKCollide(o1, GJKInitializer<S>::getSupportFunction(), GJKInitializer<S>::getCenterFunction(),
                        o2, triGetSupportFunction(), triGetCenterFunction(),
                        contact_points, penetration_depth, normal);

  GJKInitializer<S>::deleteGJKObject(o1);
  triDeleteGJKObject(o2);

  return res;
}


bool GJKDistance(void* obj1, ccd_support_fn supp1,
                 void* obj2, ccd_support_fn supp2,
                 BVH_REAL* dist);


/** \brief distance computation between two shapes */
template<typename S1, typename S2>
bool shapeDistance(const S1& s1, const SimpleTransform& tf1,
                   const S2& s2, const SimpleTransform& tf2,
                   BVH_REAL* dist)
{
  void* o1 = GJKInitializer<S1>::createGJKObject(s1, tf1);
  void* o2 = GJKInitializer<S2>::createGJKObject(s2, tf2);

  bool res =  GJKDistance(o1, GJKInitializer<S1>::getSupportFunction(),
                          o2, GJKInitializer<S2>::getSupportFunction(),
                          dist);

  if(*dist > 0) *dist = std::sqrt(*dist);

  GJKInitializer<S1>::deleteGJKObject(o1);
  GJKInitializer<S2>::deleteGJKObject(o2);

  return res;
}

}

#endif
