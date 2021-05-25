/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "SolARImageMatcherPopSift.h"
#include "core/Log.h"

#include <popsift/common/device_prop.h>
#include <popsift/features.h>
#include <popsift/sift_conf.h>
#include <popsift/sift_config.h>
#include <popsift/version.hpp>

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::POPSIFT::SolARImageMatcherPopSift);

namespace xpcf  = org::bcom::xpcf;

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace POPSIFT {

SolARImageMatcherPopSift::SolARImageMatcherPopSift():ConfigurableBase(xpcf::toUUID<SolARImageMatcherPopSift>())
{
    addInterface<api::features::IImageMatcher>(this);
    declareProperty("mode",m_mode);
    declareProperty("imageMode", m_imageMode);
    declareProperty("nbOctaves",m_nbOctaves);
    declareProperty("nbLevelPerOctave",m_nbLevelPerOctave);
    declareProperty("sigma",m_sigma);
    declareProperty("threshold",m_threshold);
    declareProperty("edgeLimit",m_edgeLimit);
    declareProperty("downsampling",m_downsampling);
    declareProperty("initialBlur",m_initialBlur);
    declareProperty("maxTotalKeypoints",m_maxTotalKeypoints);


    LOG_DEBUG(" SolARImageMatcherPopSift constructor");
}

SolARImageMatcherPopSift::~SolARImageMatcherPopSift(){
    m_popSift->uninit();
    delete m_popSift;
}

xpcf::XPCFErrorCode SolARImageMatcherPopSift::onConfigured()
{
    LOG_DEBUG(" SolARImageMatcherPopSift onConfigured");

    popsift::cuda::device_prop_t deviceInfo;
    deviceInfo.set(0, true);

    // reset configuration

    if (m_nbOctaves >0)
        config.setOctaves(m_nbOctaves);
    if (m_nbLevelPerOctave >0)
        config.setLevels(m_nbLevelPerOctave);
    if (m_downsampling >0)
        config.setDownsampling(m_downsampling);
    if (m_threshold >0)
        config.setThreshold(m_threshold);
    if (m_edgeLimit >0)
        config.setEdgeLimit(m_edgeLimit);
    if (m_initialBlur>0)
        config.setInitialBlur(m_initialBlur);
    if (m_maxTotalKeypoints >0)
        config.setFilterMaxExtrema((size_t)m_maxTotalKeypoints);
    config.setNormalizationMultiplier(9); // 2^9 = 512
    config.setNormMode(m_rootSift ? popsift::Config::RootSift : popsift::Config::Classic);
    config.setFilterSorting(popsift::Config::LargestScaleFirst);

    if (m_mode=="PopSift")
        config.setMode(popsift::Config::SiftMode::PopSift);
    else if (m_mode=="OpenCV")
        config.setMode(popsift::Config::SiftMode::OpenCV);
    else if (m_mode=="VLFeat")
        config.setMode(popsift::Config::SiftMode::VLFeat);
    else
    {
        LOG_INFO("{} is not a valid mode for PopSift Descriptor Extractor. Set to PopSift default mode. Valid values are PopSift, OpenCV, VLFeat", m_mode);
        config.setMode(popsift::Config::SiftMode::PopSift);
    }

    config.setNormMode(popsift::Config::NormMode::RootSift);


#ifdef DEBUG
    config.setLogMode(popsift::Config::LogMode::All);
#else
    config.setLogMode(popsift::Config::LogMode::None);
#endif

    if (m_imageMode=="Float")
        m_popSift = new PopSift( config,
                                 popsift::Config::MatchingMode,
                                 PopSift::FloatImages );
    else if (m_imageMode=="Unsigned Char")
        m_popSift = new PopSift( config,
                                 popsift::Config::MatchingMode,
                                 PopSift::ByteImages );
    else
    {
        LOG_INFO("imageMode for SolARImageMatcherPopSift is {}. It should be whether Float or Unsigned Char. It is set by default to Unsigned Char.", m_imageMode);
        m_popSift = new PopSift( config,
                                 popsift::Config::MatchingMode,
                                 PopSift::ByteImages );
    }
    return xpcf::XPCFErrorCode::_SUCCESS;
}

FrameworkReturnCode SolARImageMatcherPopSift::match(
                           const SRef<datastructure::Image> image1,
                           const SRef<datastructure::Image> image2,
                           std::vector<datastructure::Keypoint> & keypoints1,
                           std::vector<datastructure::Keypoint> & keypoints2,
                           SRef<datastructure::DescriptorBuffer> descriptors1,
                           SRef<datastructure::DescriptorBuffer> descriptors2,
                           std::vector<datastructure::DescriptorMatch> & matches)
{
    if ((image1->getDataType() == Image::DataType::TYPE_32U || image2->getDataType() == Image::DataType::TYPE_32U ) && m_imageMode != "Float")
    {
        LOG_ERROR("Image format on 32 bits per component, imageMode of PopSift Descriptor extractor should be set to Float");
        return FrameworkReturnCode::_ERROR_;
    }
    else if ((image1->getDataType() == Image::DataType::TYPE_8U || image2->getDataType() == Image::DataType::TYPE_8U ) && m_imageMode != "Unsigned Char")
    {
        LOG_ERROR("Image format on 8 bits per component, imageMode of PopSift Descriptor extractor should be set to Unsigned Char");
        return FrameworkReturnCode::_ERROR_;
    }

    SiftJob* job1;
    SiftJob* job2;

    if (m_imageMode == "Unsigned Char")
    {
        job1 = m_popSift->enqueue(image1->getWidth(), image1->getHeight(), (unsigned char*)image1->data());
        job2 = m_popSift->enqueue(image2->getWidth(), image2->getHeight(), (unsigned char*)image2->data());
    }
    else if (m_imageMode == "Float")
    {
        job1 = m_popSift->enqueue(image1->getWidth(), image1->getHeight(), (float*)image1->data());
        job2 = m_popSift->enqueue(image2->getWidth(), image2->getHeight(), (float*)image2->data());
    }
    else
        return FrameworkReturnCode::_ERROR_;

    popsift::FeaturesDev* features1 = job1->getDev();
    popsift::FeaturesDev* features2 = job2->getDev();

    features1->match(features2);

    int id=0;
    for(int i=0; i< features1->getFeatureCount(); i++)
    {
        popsift::Feature feature=features1->getFeatures()[i];
        for(int orientationIndex = 0; orientationIndex < feature.num_ori; ++orientationIndex)
        {
          Keypoint kp;
          kp.init(id++,
                  feature.xpos,
                  feature.ypos,
                  0.0f,  //(float)image->data()[pixelPos],
                  0.0f, //(float)image->data()[pixelPos+nbCompPerPixel],
                  0.0f, //(float)image->data()[pixelPos+(2*nbCompPerPixel)],
                  feature.sigma,
                  feature.orientation[orientationIndex]);

          keypoints1.push_back(kp);
        }
    }

    descriptors1.reset( new DescriptorBuffer((unsigned char*)features1->getDescriptors(), DescriptorType::SIFT, DescriptorDataType::TYPE_32F, 128, features1->getDescriptorCount())) ;

    id=0;
    for(int i=0; i< features2->getFeatureCount(); i++)
    {
        popsift::Feature feature=features2->getFeatures()[i];
        for(int orientationIndex = 0; orientationIndex < feature.num_ori; ++orientationIndex)
        {
          Keypoint kp;
          kp.init(id++,
                  feature.xpos,
                  feature.ypos,
                  0.0f,  //(float)image->data()[pixelPos],
                  0.0f, //(float)image->data()[pixelPos+nbCompPerPixel],
                  0.0f, //(float)image->data()[pixelPos+(2*nbCompPerPixel)],
                  feature.sigma,
                  feature.orientation[orientationIndex]);

          keypoints2.push_back(kp);
        }
    }

    descriptors2.reset( new DescriptorBuffer((unsigned char*)features2->getDescriptors(), DescriptorType::SIFT, DescriptorDataType::TYPE_32F, 128, features2->getDescriptorCount())) ;

    int* matches1 = features1->getReverseMap();
    //int* matches2 = features2->getReverseMap();
    for (int i = 0; i < features1->getDescriptorCount(); i++)
        matches.push_back(DescriptorMatch(i, matches1[i], 1.0f));

    return FrameworkReturnCode::_SUCCESS;
}


}
}
}
