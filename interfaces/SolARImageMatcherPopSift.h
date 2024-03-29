/******************************************************************************
 * Author:   Laurent Kneip                                                    *
 * Contact:  kneip.laurent@gmail.com                                          *
 * License:  Copyright (c) 2013 Laurent Kneip, ANU. All rights reserved.      *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions         *
 * are met:                                                                   *
 * * Redistributions of source code must retain the above copyright           *
 *   notice, this list of conditions and the following disclaimer.            *
 * * Redistributions in binary form must reproduce the above copyright        *
 *   notice, this list of conditions and the following disclaimer in the      *
 *   documentation and/or other materials provided with the distribution.     *
 * * Neither the name of ANU nor the names of its contributors may be         *
 *   used to endorse or promote products derived from this software without   *
 *   specific prior written permission.                                       *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"*
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE *
 * ARE DISCLAIMED. IN NO EVENT SHALL ANU OR THE CONTRIBUTORS BE LIABLE        *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT         *
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY  *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF     *
 * SUCH DAMAGE.                                                               *
 ******************************************************************************/

#ifndef SolARImageMatcherPopSift_H
#define SolARImageMatcherPopSift_H
#include <vector>
#include "api/features/IImageMatcher.h"
#include "SolARPopSiftAPI.h"
#include "xpcf/component/ConfigurableBase.h"

#include <popsift/popsift.h>

namespace SolAR {
namespace MODULES {
namespace POPSIFT {

/**
 * @class SolARImageMatcherPopSift
 * @brief <B>find the matches between two input images.</B>
 * <TT>UUID: 3baab95a-ad25-11eb-8529-0242ac130003</TT>
 *
 */

class SOLARMODULEPOPSIFT_EXPORT_API SolARImageMatcherPopSift : public org::bcom::xpcf::ConfigurableBase,
    public api::features::IImageMatcher
{
public:
    ///@brief SolARImageMatcherPopSift constructor;
    SolARImageMatcherPopSift();
    ///@brief SolARImageMatcherPopSift destructor;
    ~SolARImageMatcherPopSift();

    org::bcom::xpcf::XPCFErrorCode onConfigured() override final;

    ///
    /// @brief getType
    /// @return a string describing the type of descriptor used during extraction.
    ///
    //std::string getTypeString() override { return std::string("DescriptorsExtractorType::SIFT"); }

    /// @brief match keypoints between two input images.
    /// @param[in] image1, the first image.
    /// @param[in] image2, the second image.
    /// @param[out] keypoints1, The keypoints detected in the first image.
    /// @param[out] keypoints2, The keypoints detected in the second image.
    /// @param[out] descriptors1, The descriptors of keypoint of image1.
    /// @param[out] descriptors2, The descriptors of keypoint of image1.
    /// @param[out] matches, the keypoint matches between the first and second image
    /// @return FrameworkReturnCode::_SUCCESS_ if images are well matched, else FrameworkReturnCode::_ERROR
    FrameworkReturnCode match( const SRef<datastructure::Image> image1,
                               const SRef<datastructure::Image> image2,
                               std::vector<datastructure::Keypoint> & keypoints1,
                               std::vector<datastructure::Keypoint> & keypoints2,
                               SRef<datastructure::DescriptorBuffer> descriptors1,
                               SRef<datastructure::DescriptorBuffer> descriptors2,
                               std::vector<datastructure::DescriptorMatch> & matches) override;

    void unloadComponent () override final;

private:
    PopSift* m_popSift;
    popsift::Config config;

    std::string m_mode = "PopSift";   // "OpenCV", "VLFeat" also possible.

    std::string m_imageMode = "Float"; // Could be "Unsigned CHar"
    int m_nbOctaves = 0;                // Number of octaves
    int m_nbLevelPerOctave = 0;     // Number of levels per octave
    float m_sigma = 0.0f;                  // Initial Sigma value
    float m_threshold = 0.0f;     // Contrast min threshold
    float m_edgeLimit = 0.0f;      // On-edge limit, Max ratio of Hessian eigenvalues
    float m_downsampling = 0.0f;   // Downscale width and height of input by 2^N, Use original image, or perform an upscale if == -1
    float m_initialBlur = 0.0f;    // Assume initial blur, subtract when blurring first time

    bool m_rootSift = true;         // True, use RootSift(The L1-inspired norm, gives better matching results), otherwise classic (The L2-inspired norm, all descriptors on a hypersphere)

    int m_numScales = 3; // Scales per octave
    float m_edgeThreshold = 10.0f; // Max ratio of Hessian eigenvalues
    float m_peakThreshold = 0.005f; // Min contrast
    float m_relativePeakThreshold = 0.01f; // Min contrast (relative to variance median)

    std::size_t _gridSize = 4;
    uint32_t m_maxTotalKeypoints = 10000;

};

}
}
}

template <> struct org::bcom::xpcf::ComponentTraits<SolAR::MODULES::POPSIFT::SolARImageMatcherPopSift>
{

    static constexpr const char * UUID = "{3baab95a-ad25-11eb-8529-0242ac130003}";
    static constexpr const char * NAME = "SolARImageMatcherPopSift";
    static constexpr const char * DESCRIPTION = "SolARImageMatcherPopSift implements SolAR::api::features::IImageMAtcher interface";
};

#endif // SolARImageMatcherPopSift_H
