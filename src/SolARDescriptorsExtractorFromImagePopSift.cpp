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


#include "SolARDescriptorsExtractorFromImagePopSift.h"
#include "core/Log.h"

#include <popsift/common/device_prop.h>
#include <popsift/features.h>
#include <popsift/sift_conf.h>
#include <popsift/sift_config.h>
#include <popsift/version.hpp>

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::POPSIFT::SolARDescriptorsExtractorFromImagePopSift);

namespace xpcf  = org::bcom::xpcf;

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace POPSIFT {

SolARDescriptorsExtractorFromImagePopSift::SolARDescriptorsExtractorFromImagePopSift():ConfigurableBase(xpcf::toUUID<SolARDescriptorsExtractorFromImagePopSift>())
{
    addInterface<api::features::IDescriptorsExtractorFromImage>(this);
    declareInjectable<SolAR::api::image::IImageConvertor>(m_imageConvertor);
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

    m_popSift = NULL;


    LOG_DEBUG(" SolARDescriptorsExtractorFromImagePopSift constructor");
}

SolARDescriptorsExtractorFromImagePopSift::~SolARDescriptorsExtractorFromImagePopSift(){
    m_popSift->uninit();
    delete m_popSift;
}

xpcf::XPCFErrorCode SolARDescriptorsExtractorFromImagePopSift::onConfigured()
{
    LOG_INFO(" SolARDescriptorsExtractorFromImagePopSift onConfigured");

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


#ifdef DEBUG
    config.setLogMode(popsift::Config::LogMode::All);
#else
    config.setLogMode(popsift::Config::LogMode::None);
#endif
    LOG_INFO("SolARDescriptorsExtractorFromImagePopSift Create popSift object");
    if (m_popSift != NULL)
        delete m_popSift;

    if (m_imageMode=="Float")
        m_popSift = new PopSift( config,
                                 popsift::Config::ExtractingMode,
                                 PopSift::FloatImages );
    else if (m_imageMode=="Unsigned Char")
        m_popSift = new PopSift( config,
                                 popsift::Config::ExtractingMode,
                                 PopSift::ByteImages );
    else
    {
        LOG_INFO("imageMode for SolARDescriptorsExtractorFromImagePopSift is {}. It should be whether Float or Unsigned Char. It is set by default to Unsigned Char.", m_imageMode);
        m_popSift = new PopSift( config,
                                 popsift::Config::ExtractingMode,
                                 PopSift::ByteImages );
    }
    return xpcf::XPCFErrorCode::_SUCCESS;
}

FrameworkReturnCode SolARDescriptorsExtractorFromImagePopSift::extract(
                           const SRef<datastructure::Image> image,
                           std::vector<datastructure::Keypoint> & keypoints,
                           SRef<SolAR::datastructure::DescriptorBuffer> & descriptors ) 
{
    SRef<Image> greyImage;
    // Convert Image from RGB to grey
    if (image->getNbChannels() != 1)
        m_imageConvertor->convert(image, greyImage, Image::ImageLayout::LAYOUT_GREY);
    else
        greyImage = image->copy();
    LOG_DEBUG("SolARDescriptorsExtractorFromImagePopSift::extract Begin");
    if (greyImage->getDataType() == Image::DataType::TYPE_32U  && m_imageMode != "Float")
    {
        LOG_ERROR("Image format on 32 bits per component, imageMode of PopSift Descriptor extractor should be set to Float");
        return FrameworkReturnCode::_ERROR_;
    }
    else if (greyImage->getDataType() == Image::DataType::TYPE_8U  && m_imageMode != "Unsigned Char")
    {
        LOG_ERROR("Image format on 8 bits per component, imageMode of PopSift Descriptor extractor should be set to Unsigned Char");
        return FrameworkReturnCode::_ERROR_;
    }

    PopSift::AllocTest allocTestError = m_popSift->testTextureFit(image->getWidth(), image->getHeight());
    if (allocTestError!=PopSift::AllocTest::Ok)
        LOG_ERROR("{}",m_popSift->testTextureFitErrorString(allocTestError,image->getWidth(), image->getHeight()));

    SiftJob* job;
    if (m_imageMode == "Unsigned Char")
        job = m_popSift->enqueue(image->getWidth(), image->getHeight(), (unsigned char*)greyImage->data());
    else if (m_imageMode == "Float")
        job = m_popSift->enqueue(image->getWidth(), image->getHeight(), (float*)greyImage->data());
    else
        return FrameworkReturnCode::_ERROR_;

    popsift::FeaturesHost* popFeatures = job->getHost();
    int id(0);	
	std::vector<popsift::Descriptor> descBuffer;
	int descPos(0);
	popsift::Descriptor* descPtr = popFeatures->getDescriptors();
	// Extract the best feature for each position
    for(const auto& popFeat: *popFeatures)
    {
		// position
		float x = popFeat.xpos;
		float y = popFeat.ypos;
		// extract pixel value
		Vector3b pixelValue;
		if (image->getNbChannels() == 3)
			pixelValue = image->getPixel<Vector3b>((int)y, (int)x);
		else {
			uint8_t value = image->getPixel<uint8_t>((int)y, (int)x);
			pixelValue = Vector3b(value, value, value);
		}
		// keypoint
		Keypoint kp(id++,
					x,
					y,
					pixelValue[2],
					pixelValue[1],
					pixelValue[0],
					popFeat.sigma,
					popFeat.orientation[0]);
		keypoints.push_back(kp);		
		// descriptor
		descBuffer.push_back(descPtr[descPos]);
		descPos += popFeat.num_ori;	
    }
    descriptors.reset( new DescriptorBuffer((unsigned char*)descBuffer.data(), DescriptorType::SIFT, DescriptorDataType::TYPE_32F, 128, keypoints.size())) ;	
	descriptors = xpcf::utils::make_shared<DescriptorBuffer>((unsigned char*)descriptors->convertTo(DescriptorDataType::TYPE_8U).data(), DescriptorType::SIFT_UINT8, keypoints.size());
	LOG_DEBUG("{} keypoints were detected by PopSift", descriptors->getNbDescriptors());
    return FrameworkReturnCode::_SUCCESS;
}


}
}
}
