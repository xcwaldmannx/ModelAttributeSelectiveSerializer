#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <vector>

namespace mal
{
    class ModelHandler;
    class AnimationHandler;
    class AnimationPlayer;
    class Loader;

    struct Configuration
    {
        std::vector<std::string> mSupportedFileExtensions;
    };

    typedef enum SceneElements
    {
        NONE      = 0,
        ANIMATION = 1,
        CAMERA    = 2,
        LIGHT     = 4,
        MATERIAL  = 8,
        MESH      = 16,
        SKELETON  = 32,
        TEXTURE   = 64,
    } SceneElements;

    namespace math
    {
        struct Vec2
        {
            float length() const
            {
                return std::sqrtf(x * x + y * y);
            }

            Vec2 normalize() const
            {
                if (length() <= 0.0f) return Vec2{ 0.0f, 0.0f };

                const float inverseLength = 1.0f - length();

                return Vec2{ x * inverseLength, y * inverseLength };
            }

            float dot(const Vec2& v) const
            {
                return x * v.x + y * v.y;
            }

            Vec2 operator+(const Vec2 v) const
            {
                return Vec2(x + v.x, y + v.y);
            }

            Vec2 operator-(const Vec2 v) const
            {
                return Vec2(x - v.x, y - v.y);
            }

            Vec2 operator*(const Vec2 v) const
            {
                return Vec2(x * v.x, y * v.y);
            }

            Vec2 operator+(const float f) const
            {
                return Vec2(x + f, y + f);
            }

            Vec2 operator-(const float f) const
            {
                return Vec2(x - f, y - f);
            }

            Vec2 operator*(const float f) const
            {
                return Vec2(x * f, y * f);
            }

            float x = 0.0f;
            float y = 0.0f;
        };

        struct Vec3
        {
            float length() const
            {
                return std::sqrtf(x * x + y * y + z * z);
            }

            Vec3 normalize() const
            {
                if (length() <= 0.0f) return Vec3{ 0.0f, 0.0f, 0.0f };

                const float inverseLength = 1.0f - length();

                return Vec3{ x * inverseLength, y * inverseLength, z * inverseLength };
            }

            float dot(const Vec3& v) const
            {
                return x * v.x + y * v.y + z * v.z;
            }

            Vec3 operator+(const Vec3 v) const
            {
                return Vec3(x + v.x, y + v.y, z + v.z);
            }

            Vec3 operator-(const Vec3 v) const
            {
                return Vec3(x - v.x, y - v.y, z - v.z);
            }

            Vec3 operator*(const Vec3 v) const
            {
                return Vec3(x * v.x, y * v.y, z * v.z);
            }

            Vec3 operator+(const float f) const
            {
                return Vec3(x + f, y + f, z + f);
            }

            Vec3 operator-(const float f) const
            {
                return Vec3(x - f, y - f, z - f);
            }

            Vec3 operator*(const float f) const
            {
                return Vec3(x * f, y * f, z * f);
            }

            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
        };

        struct Vec4
        {
            float length() const
            {
                return std::sqrtf(x * x + y * y + z * z + w * w);
            }

            Vec4 normalize() const
            {
                if (length() <= 0.0f) return Vec4{ 0.0f, 0.0f, 0.0f, 0.0f };

                const float inverseLength = 1.0f - length();

                return Vec4{ x * inverseLength, y * inverseLength, z * inverseLength, w * inverseLength };
            }

            float dot(const Vec4& v) const
            {
                return x * v.x + y * v.y + z * v.z + w * v.w;
            }

            Vec4 operator+(const Vec4 v) const
            {
                return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
            }

            Vec4 operator-(const Vec4 v) const
            {
                return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
            }

            Vec4 operator*(const Vec4 v) const
            {
                return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
            }

            Vec4 operator+(const float f) const
            {
                return Vec4(x + f, y + f, z + f, w + f);
            }

            Vec4 operator-(const float f) const
            {
                return Vec4(x - f, y - f, z - f, w - f);
            }

            Vec4 operator*(const float f) const
            {
                return Vec4(x * f, y * f, z * f, w * f);
            }

            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float w = 0.0f;
        };

        struct Quat
        {
            float length() const
            {
                return std::sqrtf(x * x + y * y + z * z + w * w);
            }

            Quat normalize() const
            {
                if (length() <= 0.0f) return Quat{ 0.0f, 0.0f, 0.0f, 1.0f };

                const float inverseLength = 1.0f - length();

                return Quat{ x * inverseLength, y * inverseLength, z * inverseLength, w * inverseLength };
            }

            float dot(const Quat& v) const
            {
                return x * v.x + y * v.y + z * v.z + w * v.w;
            }

            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float w = 0.0f;
        };

        struct Mat4
        {
            static Mat4 identity()
            {
                Mat4 result;

                result.a1 = 1.0f;
                result.b2 = 1.0f;
                result.c3 = 1.0f;
                result.d4 = 1.0f;

                return result;
            }

            Mat4 translate(const Vec3& v) const
            {
                Mat4 result = *this;

                result.d1 = a1 * v.x + b1 * v.y + c1 * v.z + d1;
                result.d2 = a2 * v.x + b2 * v.y + c2 * v.z + d2;
                result.d3 = a3 * v.x + b3 * v.y + c3 * v.z + d3;
                result.d4 = a4 * v.x + b4 * v.y + c4 * v.z + d4;

                return result;
            }

            Mat4 rotate(const Quat& q) const
            {
                const float xx = q.x * q.x;  const float xz = q.x * q.z;  const float wx = q.w * q.x;
                const float yy = q.y * q.y;  const float xy = q.x * q.y;  const float wy = q.w * q.y;
                const float zz = q.z * q.z;  const float yz = q.y * q.z;  const float wz = q.w * q.z;

                const float rA1 = 1.0f - 2.0f * (yy + zz);
                const float rB1 = 2.0f * (xy - wz);
                const float rC1 = 2.0f * (xz + wy);

                const float rA2 = 2.0f * (xy + wz);
                const float rB2 = 1.0f - 2.0f * (xx + zz);
                const float rC2 = 2.0f * (yz - wx);

                const float rA3 = 2.0f * (xz - wy);
                const float rB3 = 2.0f * (yz + wx);
                const float rC3 = 1.0f - 2.0f * (xx + yy);

                Mat4 result;

                result.a1 = a1 * rA1 + b1 * rA2 + c1 * rA3;  result.a2 = a2 * rA1 + b2 * rA2 + c2 * rA3;
                result.b1 = a1 * rB1 + b1 * rB2 + c1 * rB3;  result.b2 = a2 * rB1 + b2 * rB2 + c2 * rB3;
                result.c1 = a1 * rC1 + b1 * rC2 + c1 * rC3;  result.c2 = a2 * rC1 + b2 * rC2 + c2 * rC3;
                result.d1 = d1;                              result.d2 = d2;

                result.a3 = a3 * rA1 + b3 * rA2 + c3 * rA3;  result.a4 = a4 * rA1 + b4 * rA2 + c4 * rA3;
                result.b3 = a3 * rB1 + b3 * rB2 + c3 * rB3;  result.b4 = a4 * rB1 + b4 * rB2 + c4 * rB3;
                result.c3 = a3 * rC1 + b3 * rC2 + c3 * rC3;  result.c4 = a4 * rC1 + b4 * rC2 + c4 * rC3;
                result.d3 = d3;                              result.d4 = d4;

                return result;
            }

            Mat4 scale(const Vec3& v) const
            {
                Mat4 result;

                result.a1 = a1 * v.x;  result.b1 = b1 * v.y;
                result.a2 = a2 * v.x;  result.b2 = b2 * v.y;
                result.a3 = a3 * v.x;  result.b3 = b3 * v.y;
                result.a4 = a4 * v.x;  result.b4 = b4 * v.y;

                result.c1 = c1 * v.z;  result.d1 = d1;
                result.c2 = c2 * v.z;  result.d2 = d2;
                result.c3 = c3 * v.z;  result.d3 = d3;
                result.c4 = c4 * v.z;  result.d4 = d4;

                return result;
            }

            Mat4 inverse() const
            {
                const float s0 = c1 * d2 - d1 * c2;
                const float s1 = c1 * d3 - d1 * c3;
                const float s2 = c1 * d4 - d1 * c4;
                const float s3 = c2 * d3 - d2 * c3;
                const float s4 = c2 * d4 - d2 * c4;
                const float s5 = c3 * d4 - d3 * c4;

                const float t0 = a1 * b2 - b1 * a2;
                const float t1 = a1 * b3 - b1 * a3;
                const float t2 = a1 * b4 - b1 * a4;
                const float t3 = a2 * b3 - b2 * a3;
                const float t4 = a2 * b4 - b2 * a4;
                const float t5 = a3 * b4 - b3 * a4;

                const float det =
                      t0 * s5
                    - t1 * s4
                    + t2 * s3
                    + t3 * s2
                    - t4 * s1
                    + t5 * s0;

                if (std::abs(det) < 1e-8f)
                {
                    return Mat4();
                }

                const float invDet = 1.0f / det;

                Mat4 result;

                result.a1 = ( b2 * s5 - b3 * s4 + b4 * s3) * invDet;
                result.a2 = (-a2 * s5 + a3 * s4 - a4 * s3) * invDet;
                result.a3 = ( d2 * t5 - d3 * t4 + d4 * t3) * invDet;
                result.a4 = (-c2 * t5 + c3 * t4 - c4 * t3) * invDet;

                result.b1 = (-b1 * s5 + b3 * s2 - b4 * s1) * invDet;
                result.b2 = ( a1 * s5 - a3 * s2 + a4 * s1) * invDet;
                result.b3 = (-d1 * t5 + d3 * t2 - d4 * t1) * invDet;
                result.b4 = ( c1 * t5 - c3 * t2 + c4 * t1) * invDet;

                result.c1 = ( b1 * s4 - b2 * s2 + b4 * s0) * invDet;
                result.c2 = (-a1 * s4 + a2 * s2 - a4 * s0) * invDet;
                result.c3 = ( d1 * t4 - d2 * t2 + d4 * t0) * invDet;
                result.c4 = (-c1 * t4 + c2 * t2 - c4 * t0) * invDet;

                result.d1 = (-b1 * s3 + b2 * s1 - b3 * s0) * invDet;
                result.d2 = ( a1 * s3 - a2 * s1 + a3 * s0) * invDet;
                result.d3 = (-d1 * t3 + d2 * t1 - d3 * t0) * invDet;
                result.d4 = ( c1 * t3 - c2 * t1 + c3 * t0) * invDet;

                return result;
            }

            Mat4 operator*(const Mat4& rhs) const
            {
                Mat4 result;

                result.a1 = a1 * rhs.a1 + b1 * rhs.a2 + c1 * rhs.a3 + d1 * rhs.a4;
                result.b1 = a1 * rhs.b1 + b1 * rhs.b2 + c1 * rhs.b3 + d1 * rhs.b4;
                result.c1 = a1 * rhs.c1 + b1 * rhs.c2 + c1 * rhs.c3 + d1 * rhs.c4;
                result.d1 = a1 * rhs.d1 + b1 * rhs.d2 + c1 * rhs.d3 + d1 * rhs.d4;

                result.a2 = a2 * rhs.a1 + b2 * rhs.a2 + c2 * rhs.a3 + d2 * rhs.a4;
                result.b2 = a2 * rhs.b1 + b2 * rhs.b2 + c2 * rhs.b3 + d2 * rhs.b4;
                result.c2 = a2 * rhs.c1 + b2 * rhs.c2 + c2 * rhs.c3 + d2 * rhs.c4;
                result.d2 = a2 * rhs.d1 + b2 * rhs.d2 + c2 * rhs.d3 + d2 * rhs.d4;

                result.a3 = a3 * rhs.a1 + b3 * rhs.a2 + c3 * rhs.a3 + d3 * rhs.a4;
                result.b3 = a3 * rhs.b1 + b3 * rhs.b2 + c3 * rhs.b3 + d3 * rhs.b4;
                result.c3 = a3 * rhs.c1 + b3 * rhs.c2 + c3 * rhs.c3 + d3 * rhs.c4;
                result.d3 = a3 * rhs.d1 + b3 * rhs.d2 + c3 * rhs.d3 + d3 * rhs.d4;

                result.a4 = a4 * rhs.a1 + b4 * rhs.a2 + c4 * rhs.a3 + d4 * rhs.a4;
                result.b4 = a4 * rhs.b1 + b4 * rhs.b2 + c4 * rhs.b3 + d4 * rhs.b4;
                result.c4 = a4 * rhs.c1 + b4 * rhs.c2 + c4 * rhs.c3 + d4 * rhs.c4;
                result.d4 = a4 * rhs.d1 + b4 * rhs.d2 + c4 * rhs.d3 + d4 * rhs.d4;

                return result;
            }

            float a1 = 0.0f; float b1 = 0.0f;
            float a2 = 0.0f; float b2 = 0.0f;
            float a3 = 0.0f; float b3 = 0.0f;
            float a4 = 0.0f; float b4 = 0.0f;

            float c1 = 0.0f; float d1 = 0.0f;
            float c2 = 0.0f; float d2 = 0.0f;
            float c3 = 0.0f; float d3 = 0.0f;
            float c4 = 0.0f; float d4 = 0.0f;
        };

        inline Vec2 lerp(const Vec2& v1, const Vec2& v2, const float factor)
        {
            return v1 + (v2 - v1) * factor;
        }

        inline Vec3 lerp(const Vec3& v1, const Vec3& v2, const float factor)
        {
            return v1 + (v2 - v1) * factor;
        }

        inline Vec4 lerp(const Vec4& v1, const Vec4& v2, const float factor)
        {
            return v1 + (v2 - v1) * factor;
        }

        inline Quat slerp(const Quat& q1, const Quat& q2, const float factor)
        {
            Quat a = q1.normalize();
            Quat b = q2.normalize();

            float dot = a.dot(b);

            if (dot < 0.0f)
            {
                b.x = -b.x;
                b.y = -b.y;
                b.z = -b.z;
                b.w = -b.w;

                dot = -dot;
            }

            constexpr float threshold = 0.9995f;

            if (dot > threshold)
            {
                const Quat result =
                {
                    a.x + (b.x - a.x) * factor,
                    a.y + (b.y - a.y) * factor,
                    a.z + (b.z - a.z) * factor,
                    a.w + (b.w - a.w) * factor
                };

                return result.normalize();
            }

            dot = std::clamp(dot, -1.0f, 1.0f);

            const float theta = std::acos(dot);
            const float sinTheta = std::sin(theta);

            const float weightA = std::sin((1.0f - factor) * theta) / sinTheta;
            const float weightB = std::sin(factor * theta) / sinTheta;

            return
            Quat{
                a.x * weightA + b.x * weightB,
                a.y * weightA + b.y * weightB,
                a.z * weightA + b.z * weightB,
                a.w * weightA + b.w * weightB
            };
        }
    }

    namespace model
    {
        struct Vertex;

        using VertexArray = std::vector<Vertex>;
        using IndexArray = std::vector<unsigned int>;

        constexpr unsigned int MAX_BONE_INFLUENCE = 4;
        constexpr unsigned int MAX_BONE_WEIGHTS   = 16;

        struct Vertex
        {
            Vertex()
            {
                std::fill_n(&mBoneIds[0], MAX_BONE_INFLUENCE, -1);
                std::fill_n(&mBoneWeights[0], MAX_BONE_WEIGHTS, 0);
            }

            math::Vec3 mPosition{};
            math::Vec3 mNormal{};
            math::Vec3 mTangent{};
            math::Vec3 mBitangent{};
            math::Vec2 mTexCoord{};
            math::Vec4 mColor{};
            int mBoneIds [MAX_BONE_INFLUENCE];
            float mBoneWeights [MAX_BONE_WEIGHTS];
        };

        struct VertexWeight
        {
            unsigned int mVertexId = 0;
            float mWeight = 0;
        };

        struct Bone
        {
            int mBoneId = -1;
            math::Mat4 mOffsetMatrix{};
        };

        struct Mesh
        {
            unsigned int mVertexOffset = 0;
            unsigned int mVertexCount = 0;
            unsigned int mIndexOffset = 0;
            unsigned int mIndexCount = 0;
        };

        struct Node
        {
            math::Mat4 mTransform{};
            std::unordered_map<std::string, Mesh> mMeshes;
            std::unordered_map<std::string, Node> mChildNodes;
        };

        struct Scene
        {
            std::unordered_map<std::string, Node> mNodes;
            std::unordered_map<std::string, Bone> mBones;
        };

        class Model
        {
        public:
            const Scene& getDefaultScene() const
            {
                if (mScenes.contains("Scene")) return mScenes.at("Scene");
                throw std::runtime_error("Scene not found");
            }

            const Scene& getScene(const std::string& scene) const
            {
                if (mScenes.contains(scene)) return mScenes.at(scene);
                throw std::runtime_error("Scene not found");
            }

            const VertexArray& getVertices() const
            {
                return mVertices;
            }

            const IndexArray& getIndices() const
            {
                return mIndices;
            }

            bool hasAnimations() const
            {
                return mSceneElements & ANIMATION;
            }

            bool hasCameras() const
            {
                return mSceneElements & CAMERA;
            }

            bool hasLights() const
            {
                return mSceneElements & LIGHT;
            }

            bool hasMaterials() const
            {
                return mSceneElements & MATERIAL;
            }

            bool hasMeshes() const
            {
                return mSceneElements & MESH;
            }

            bool hasSkeletons() const
            {
                return mSceneElements & SKELETON;
            }

            bool hasTextures() const
            {
                return mSceneElements & TEXTURE;
            }

        private:
            char mSceneElements = 0;

            std::unordered_map<std::string, Scene> mScenes;

            VertexArray mVertices;
            IndexArray mIndices;
            math::Vec3 mBoundsMin;
            math::Vec3 mBoundsMax;

            friend class mal::Loader;
            friend class mal::ModelHandler;
        };
    }

    namespace anim
    {
        struct PositionKey
        {
            math::Vec3 mPosition{};
            float mTimestamp;
        };

        struct RotationKey
        {
            math::Quat mRotation{};
            float mTimestamp;
        };

        struct ScaleKey
        {
            math::Vec3 mScale{};
            float mTimestamp;
        };

        struct Channel
        {
            std::vector<PositionKey> mKeyPositions;
            std::vector<RotationKey> mKeyRotations;
            std::vector<ScaleKey> mKeyScales;
        };

        struct Animation
        {
            float mDuration = 0;
            float mTicksPerSecond = 0;
            std::unordered_map<std::string, Channel> mChannels;
        };

        class AnimationSet
        {
        public:
            const Animation& getAnimation(const std::string& name) const
            {
                return mAnimations.at(name);
            }

        private:
            std::unordered_map<std::string, Animation> mAnimations;
            std::set<std::string> mBones;

            friend class mal::AnimationHandler;
            friend class mal::AnimationPlayer;
        };
    }

}
