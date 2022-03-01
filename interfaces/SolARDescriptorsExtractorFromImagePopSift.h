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

#ifndef SolARDescriptorsExtractorFromImagePopSift_H
#define SolARDescriptorsExtractorFromImagePopSift_H
#include <vector>
#include "api/features/IDescriptorsExtractorFromImage.h"
#include "api/image/IImageConvertor.h"
#include "SolARPopSiftAPI.h"
#include "xpcf/component/ConfigurableBase.h"

#include <popsift/popsift.h>

namespace SolAR {
namespace MODULES {
/**
 * @namespace SolAR::MODULES::POPSIFT
 * @brief <B>Provides a CUDA optimized SIFT feature detection and extraction based on PopSift library: https://github.com/alicevision/popsift</B>
 * <TT>UUID: 4a43732c-a1b2-11eb-bcbc-0242ac130002</TT>
 *
 */
namespace POPSIFT {

/**
 * @class SolARDescriptorsExtractorFromImagePopSift
 * @brief <B>Detect keypoints and compute the descriptors from an image.</B>
 * <TT>UUID: 7fb2aace-a1b1-11eb-bcbc-0242ac130002</TT>
 *
 * @SolARComponentInjectablesBegin
 * @SolARComponentInjectable{SolAR::api::image::IImageConvertor}
 * @SolARComponentInjectablesEnd
 */

class SOLARMODULEPOPSIFT_EXPORT_API SolARDescriptorsExtractorFromImagePopSift : public org::bcom::xpcf::ConfigurableBase,
    public api::features::IDescriptorsExtractorFromImage
{
public:
    ///@brief SolARDescriptorsExtractorFromImagePopSift constructor;
    SolARDescriptorsExtractorFromImagePopSift();
    ///@brief SolARDescriptorsExtractorFromImagePopSift destructor;
    ~SolARDescriptorsExtractorFromImagePopSift();

    org::bcom::xpcf::XPCFErrorCode onConfigured() override final;

    /// @brief getType
    /// @return a string describing the type of descriptor used during extraction.
    std::string getTypeString() override { return std::string("DescriptorsExtractorType::SIFT_UINT8"); }

    /// @brief detect keypoints and compute the descriptors.
    /// @param[in] image image on which the keypoint and their descriptor will be detected and extracted.
    /// @param[out] keypoints The keypoints detected in the input image.
    /// @param[out] descriptors The descriptors of keypoints of the input image.
    /// @return FrameworkReturnCode::_SUCCESS_ if images are well matched, else FrameworkReturnCode::_ERROR
    FrameworkReturnCode extract(const SRef<SolAR::datastructure::Image> image,
                                std::vector<SolAR::datastructure::Keypoint> &keypoints,
                                SRef<SolAR::datastructure::DescriptorBuffer> & descriptors) override;

    void unloadComponent () override final;

private:
    SRef<SolAR::api::image::IImageConvertor> m_imageConvertor;
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

template <> struct org::bcom::xpcf::ComponentTraits<SolAR::MODULES::POPSIFT::SolARDescriptorsExtractorFromImagePopSift>
{

    static constexpr const char * UUID = "{7fb2aace-a1b1-11eb-bcbc-0242ac130002}";
    static constexpr const char * NAME = "SolARDescriptorsExtractorFromImagePopSift";
    static constexpr const char * DESCRIPTION = "SolARDescriptorsExtractorFromImagePopSift implements SolAR::api::features::IImageMAtcher interface";
};

#endif // SolARDescriptorsExtractorFromImagePopSift_H
