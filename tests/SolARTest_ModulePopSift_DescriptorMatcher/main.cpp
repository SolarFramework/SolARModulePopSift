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

#include "xpcf/xpcf.h"

#include "api/image/IImageLoader.h"
#include "api/features/IDescriptorsExtractorFromImage.h"
#include "api/display/IImageViewer.h"
#include "api/display/IMatchesOverlay.h"
#include "api/features/IDescriptorMatcher.h"
#include "core/Log.h"

#include <iostream>
#include <boost/log/core.hpp>
#include <string>
#include <vector>

using namespace SolAR;
using namespace SolAR::datastructure;
using namespace SolAR::api;

namespace xpcf  = org::bcom::xpcf;

int main()
{
#if NDEBUG
    boost::log::core::get()->set_logging_enabled(false);
#endif
    try {
        LOG_ADD_LOG_TO_CONSOLE();

        /* instantiate component manager*/
        /* this is needed in dynamic mode */
        SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();

        if(xpcfComponentManager->load("SolARTest_ModulePopSift_DescriptorMatcher_conf.xml")!=org::bcom::xpcf::_SUCCESS)
        {
            LOG_ERROR("Failed to load the configuration file SolARTest_ModulePopSift_DescriptorMatcher_conf.xml")
            return -1;
        }

        // declare and create components
        LOG_INFO("Start creating components");

        SRef<image::IImageLoader> imageLoaderImage1 = xpcfComponentManager->resolve<image::IImageLoader>("image1");
        SRef<image::IImageLoader> imageLoaderImage2 = xpcfComponentManager->resolve<image::IImageLoader>("image2");

        SRef<features::IDescriptorsExtractorFromImage> extractor = xpcfComponentManager->resolve<features::IDescriptorsExtractorFromImage>();

        SRef<features::IDescriptorMatcher> matcher = xpcfComponentManager->resolve<features::IDescriptorMatcher>();

        SRef<display::IMatchesOverlay> overlay = xpcfComponentManager->resolve<display::IMatchesOverlay>();

        SRef<display::IImageViewer> viewer = xpcfComponentManager->resolve<display::IImageViewer>();

        if (!imageLoaderImage1  || !imageLoaderImage2 || !extractor || !matcher || !overlay || !viewer )
        {
            LOG_ERROR("One or more component creations have failed");
            return -1;
        }

        SRef<Image>                     image1;
        SRef<Image>                     image2;
        std::vector<Keypoint>           keypoints1;
        std::vector<Keypoint>           keypoints2;
        SRef<DescriptorBuffer>          descriptors1;
        SRef<DescriptorBuffer>          descriptors2;
        std::vector<DescriptorMatch>    matches;
        SRef<Image>                     viewerImage;

     // Start
        // Get the first image (the path of this image is defined in the conf_DetectorMatcher.xml)
        if (imageLoaderImage1->getImage(image1) != FrameworkReturnCode::_SUCCESS)
        {
            LOG_WARNING("First image {} cannot be loaded", imageLoaderImage1->bindTo<xpcf::IConfigurable>()->getProperty("filePath")->getStringValue());
            return 0;
        }

        // Get the second image (the path of this image is defined in the conf_DetectorMatcher.xml)
        if (imageLoaderImage2->getImage(image2) != FrameworkReturnCode::_SUCCESS)
        {
            LOG_WARNING("Second image {} cannot be loaded", imageLoaderImage2->bindTo<xpcf::IConfigurable>()->getProperty("filePath")->getStringValue());
            return 0;
        }

        // SIFT
        // --------
        // Extract the SIFT keypoints and descriptors from the first image
        extractor->extract(image1, keypoints1, descriptors1);

        // Extract the SIFT keypoints and descriptors from from the first image
        extractor->extract(image2, keypoints2, descriptors2);

        // Compute the matches between the keypoints of the first image and the keypoints of the second image
        matcher->match(descriptors1, descriptors2, matches);

        // Draw the matches in a dedicated image
        overlay->draw(image1, image2, viewerImage, keypoints1, keypoints2, matches);

        while (true)
        {
            // Display the image with matches in a viewer. If escape key is pressed, exit the loop.
            if (viewer->display(viewerImage) == FrameworkReturnCode::_STOP )
            {
                LOG_INFO("End of DescriptorMatcherPopSiftTest");
                break;
            }
        }
    }
    catch (xpcf::Exception e)
    {
        LOG_ERROR ("The following exception has been catch : {}", e.what());
        return -1;
    }
    return 0;
}




