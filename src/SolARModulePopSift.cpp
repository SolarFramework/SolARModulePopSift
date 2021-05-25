/**
 * @copyright Copyright (c) 2015 All Right Reserved, B-com http://www.b-com.com/
 *
 * This file is subject to the B<>Com License.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 */

#include <iostream>

#include "xpcf/module/ModuleFactory.h"
#include "SolARDescriptorsExtractorFromImagePopSift.h"
#include "SolARImageMatcherPopSift.h"


namespace xpcf=org::bcom::xpcf;

XPCF_DECLARE_MODULE("4a43732c-a1b2-11eb-bcbc-0242ac130002", "SolARModulePopSift", "SolARModulePopSift module description")

extern "C" XPCF_MODULEHOOKS_API xpcf::XPCFErrorCode XPCF_getComponent(const boost::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)
{
    xpcf::XPCFErrorCode errCode = xpcf::XPCFErrorCode::_FAIL;
   
     if (errCode != xpcf::XPCFErrorCode::_SUCCESS)
     {

        errCode =  xpcf::tryCreateComponent<SolAR::MODULES::POPSIFT::SolARDescriptorsExtractorFromImagePopSift>(componentUUID,interfaceRef);
     }
     if (errCode != xpcf::XPCFErrorCode::_SUCCESS)
     {

        errCode =  xpcf::tryCreateComponent<SolAR::MODULES::POPSIFT::SolARImageMatcherPopSift>(componentUUID,interfaceRef);
     }

    return errCode;
}

XPCF_BEGIN_COMPONENTS_DECLARATION
XPCF_ADD_COMPONENT(SolAR::MODULES::POPSIFT::SolARDescriptorsExtractorFromImagePopSift)
XPCF_ADD_COMPONENT(SolAR::MODULES::POPSIFT::SolARImageMatcherPopSift)
XPCF_END_COMPONENTS_DECLARATION
