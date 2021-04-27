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
    popsift::Config config;
    m_popSift = new PopSift( config,
                     popsift::Config::ExtractingMode,
                     PopSift::ByteImages );
    LOG_DEBUG(" SolARDescriptorsExtractorFromImagePopSift constructor");
}

SolARDescriptorsExtractorFromImagePopSift::~SolARDescriptorsExtractorFromImagePopSift(){
    delete m_popSift;
}

FrameworkReturnCode SolARDescriptorsExtractorFromImagePopSift::extract(
                           const SRef<datastructure::Image> image,
                           std::vector<datastructure::Keypoint> & keypoints,
                           SRef<SolAR::datastructure::DescriptorBuffer> & descriptors ) {
    SiftJob* job;

    job = m_popSift->enqueue(image->getWidth(), image->getHeight(), (unsigned char*)image->data());

    popsift::FeaturesHost* features = job->getHost();

    int id=0;
    uint32_t nbCompPerPixel = image->getNbBitsPerComponent();
    for (popsift::FeaturesHost::F_const_iterator f_it = features->begin(); f_it != features->end(); f_it++)
    {
        Keypoint kp;
        uint32_t posx = (uint32_t)image->getWidth()*f_it->xpos;
        uint32_t posy = (uint32_t)image->getHeight()*f_it->ypos;
        uint32_t pixelPos = (posy * image->getWidth() + posx)*image->getStep();
/*        kp.init(id++,
                posx,
                posy,
                (float)image->data()[pixelPos],
                (float)image->data()[pixelPos+nbCompPerPixel],
                (float)image->data()[pixelPos+(2*nbCompPerPixel)],
                f_it->sigma);
*/
        keypoints.push_back(kp);
    }

    return FrameworkReturnCode::_SUCCESS;
}


}
}
}
