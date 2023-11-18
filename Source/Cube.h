/*
  ==============================================================================

    Cube.h
    Created: 29 Mar 2021 9:07:53am
    Author:  corbin

  ==============================================================================
*/

#pragma once
#include "CubeData.h"

class Cube
{
public:
    static constexpr float SCALE {2.0f};
    Cube(int cubemapID, int shapeID, CubeData& data);
    ~Cube();
    
    int cubemapID;
    int shapeID;
    
    const int getStatefulPosition();
    const juce::Vector3D<float> getPosition();
    void setPosition(int index, int state);
    const juce::Matrix3D<float> getViewMatrix();
    const juce::Quaternion<float>& getRotation();
    void setRotation(const juce::Quaternion<float>& rot);
    //void commitRotation(const bool hardCommit = true);
    void snapRotation();
    
    const bool checkBounds(const juce::Rectangle<int>& scale, const juce::Point<int>& position);
    
    #define IMG(name) juce::ImageCache::getFromMemory(Resources::name ## _png, Resources::name ## _pngSize)
    static const std::array<const juce::Image, 6>& TypeToFaces(int type) {
        static const std::array<const std::array<const juce::Image, 6>, 5> map {{
            {IMG(three),IMG(one),IMG(four),IMG(zero),IMG(two),IMG(five)},
            {IMG(seven),IMG(one),IMG(eight),IMG(zero),IMG(two),IMG(six)},
            {IMG(null),IMG(April),IMG(March),IMG(January),IMG(February),IMG(null)},
            {IMG(null),IMG(August),IMG(July),IMG(May),IMG(June),IMG(null)},
            {IMG(null),IMG(December),IMG(November),IMG(September),IMG(October),IMG(null)}
        }};
        return map[type];
    }
    
    #define IMGC(name, zoom, offset) IMG(name).getClippedImage(juce::Rectangle<int>(offset, 0, zoom == 0 ? 250 : 200, 100)).rescaled(800, 400, juce::Graphics::highResamplingQuality)
    static const juce::Image& GetClippedFace(int index) {
        static const std::array<const juce::Image, 18> map {
            IMG(null),IMGC(April, 1, 145),IMGC(March, 0, 102),IMGC(January, 1, 50),IMGC(February, 1, 20),IMG(null),
            IMG(null),IMGC(August, 0, 50),IMGC(July, 0, 163),IMGC(May, 0, 160),IMGC(June, 0, 185),IMG(null),
            IMG(null),IMGC(December, 1, 29),IMGC(November, 0, 7),IMGC(September, 1, 4),IMGC(October, 0, 30),IMG(null)
        };
        return map[index];
    }
    #undef IMGC
    #undef IMG
    
    static const juce::Vector3D<float> GetShape(int type) {
        static const juce::Vector3D<float> normal {SCALE,SCALE,SCALE};
        static const juce::Vector3D<float> month {2.0f*SCALE,0.5f*SCALE,0.5f*SCALE};

        return (type < 2) ? normal : month;
    }
    
    static const juce::Image& GetImage(const CubeData& cube, juce::AffineTransform& transform) {
        int face = -1;
        int rotation = -1;
        /*
        // Up = bank -90; Right = heading +90; Left = heading -90
        // Right, Up, UpUp, 0, UpUpUp, Left
        const float scal = cube.quat.scalar;
        const juce::Vector3D<float> vect = cube.quat.vector;
        if (scal == 0.0f) {
            
        } else {
            const bank = vect.x / scale; // -1 = Up, 1 = Down, 0 = none
            if (vect.x / scal)
        }*/
        // w=1, 1; w=0.707, 6; w=0.5, 8; w=0, 9;
        // 3 1 4 0 2 5
        float w = cube.quat.scalar;
        juce::Vector3D<float> v = cube.quat.vector;
        int i = -1;
        int r = -1;
        if (w < 0.6f) {
            if (w < 0.25f) {
                /*w = 0.0f*/
                if (v.x > 0.8f) {
                    i = 2;
                    r = 0;
                }
                else if (v.y > 0.8f) {
                    i = 2;
                    r = 2;
                }
                else if (v.z > 0.8f) {
                    i = 3;
                    r = 2;
                }
                else if (v.x < 0.5f && v.x > -0.5f) {
                    if (v.z > 0.0f) {
                        i = 1;
                        r = 2;
                    } else {
                        i = 4;
                        r = 2;
                    }
                }
                else if (v.y < 0.5f && v.y > -0.5f) {
                    if (v.z > 0.0f) {
                        i = 0;
                        r = 2;
                    } else {
                        i = 5;
                        r = 2;
                    }
                }
                else if (v.z < 0.5f && v.z > -0.5f) {
                    if (v.x > 0.0f) {
                        i = 2;
                        r = 3;
                    } else {
                        i = 2;
                        r = 1;
                    }
                }
            } else {
                /*w = 0.5f*/
                if (v.x + v.y + v.z > 0.0f) {
                    if (v.x < 0.0f) {
                        i = 1;
                        r = 1;
                    }
                    else if (v.y < 0.0f) {
                        i = 4;
                        r = 1;
                    }
                    else if (v.z < 0.0f) {
                        i = 4;
                        r = 3;
                    }
                    else {
                        i = 0;
                        r = 1;
                    }
                } else {
                    if (v.x > 0.0f) {
                        i = 5;
                        r = 3;
                    }
                    else if (v.y > 0.0f) {
                        i = 0;
                        r = 3;
                    }
                    else if (v.z > 0.0f) {
                        i = 5;
                        r = 1;
                    }
                    else {
                        i = 1;
                        r = 3;
                    }
                }
            }
        } else {
            if (w < 0.8f) {
                /*w = 0.707f*/
                if (v.x > 0.0f) {
                    i = 4;
                    r = 0;
                }
                else if (v.x < 0.0f) {
                    i = 1;
                    r = 0;
                }
                else if (v.y > 0.0f) {
                    i = 0;
                    r = 0;
                }
                else if (v.y < 0.0f) {
                    i = 5;
                    r = 0;
                }
                else if (v.z > 0.0f) {
                    i = 3;
                    r = 1;
                }
                else if (v.z < 0.0f) {
                    i = 3;
                    r = 3;
                }
            } else {
                /*w = 1.0f*/
                i = 3;
                r = 0;
            }
        }
        if (r > 0)
            transform = transform.rotated(((float)r) * M_PI/2.0f);
        
        const int t = static_cast<int>(cube.type);
        return (t < 2) ? TypeToFaces(t)[i] : GetClippedFace(((t - 2) * 6) + i);
        /*
        return (t < 2) ? ret : 
        if (t < 2)
            return ret;
        
        const float SCALE = 4.0f;
        transform.scaled(SCALE, 4.0f);
        return ret.getClippedImage(juce::Rectangle<int>(offsets[3], 0, 800 / SCALE, 100));
        */
    }
    
private:
    CubeData* data;
    //juce::Vector3D<float> position;
    //juce::Quaternion<float>* rotation_ptr;
    
    static const juce::Vector3D<float> InitPosition(int type, int state = 0);
    
    
    /*
    mat[0]  = m00;  mat[1]  = m10;  mat[2]  = m20;  mat[3]  = m30;
    mat[4]  = m01;  mat[5]  = m11;  mat[6]  = m21;  mat[7]  = m31;
    mat[8]  = m02;  mat[9]  = m12;  mat[10] = m22;  mat[11] = m32;
    mat[12] = m03;  mat[13] = m13;  mat[14] = m23;  mat[15] = m33;
    */
    #define SIGN(x) std::copysign(1.0f,x) //(float)((x > 0) - (x < 0))
    #define M(a,b) m.mat[a + b*4]
    static const juce::Quaternion<float> MatrixToQuat(const juce::Matrix3D<float>& m) {
        if (M(0,0)+M(1,1)+M(2,2) < 0 && M(0,0)*M(1,1)*M(2,2) == 0)
            return juce::Quaternion<float>(
                SIGN(M(0,1)) * sqrt( 1 + M(0,0) - M(1,1) - M(2,2) ) / 2,
                sqrt( 1 - M(0,0) + M(1,1) - M(2,2) ) / 2,
                SIGN(M(0,2) + M(1,2)) * sqrt( 1 - M(0,0) - M(1,1) + M(2,2) ) / 2,
                0
        );
        return juce::Quaternion<float>(
            SIGN(M(1,2) - M(2,1)) * sqrt( 1 + M(0,0) - M(1,1) - M(2,2) ) / 2,
            SIGN(M(2,0) - M(0,2)) * sqrt( 1 - M(0,0) + M(1,1) - M(2,2) ) / 2,
            SIGN(M(0,1) - M(1,0)) * sqrt( 1 - M(0,0) - M(1,1) + M(2,2) ) / 2,
            sqrt( 1 + M(0,0) + M(1,1) + M(2,2) ) / 2
        );
    }
    #undef M
    
    //Find the highest magnitude element of the 3x3 matrix (highest abs value). Make that element +1 or -1, and zero out its row & column. Find the second highest value (not counting the one we just replaced. Make that one +/- 1, and zero rest of its row & column. Same for last
    static const void RoundMatrix(juce::Matrix3D<float>& m) {
        //Find the 3 highest magnitude elements of the 3x3 matrix
        float highest;
        std::array<int, 3> ind {-1, -1, -1};
        for (size_t i = 0; i < 3; i++) {
            highest = 0;
            for (size_t k = 0; k < 11; k++) {
                float v = fabs(m.mat[k]);
                if (v > highest && k != ind[0] && k != ind[1]) {
                    highest = v;
                    ind[i] = k;
                }
            }
            
            m.mat[ind[i]] = SIGN(m.mat[ind[i]]);
            for (size_t k = 0; k < 11; k++) {
                if (k != ind[i] && ((k % 4 == ind[i] % 4) || (k / 4 == ind[i] / 4))){
                    m.mat[k] = 0;
                }
            }
        }
    }
    #undef SIGN
};