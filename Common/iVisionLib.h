#pragma once

#include "IV_Structure.h"

#include "../iVisionLib/ivDefine.h"

#include "../iVisionLib/IV_dataPlane.h"
#include "../iVisionLib/IV_dataComplex.h"
#include "../iVisionLib/IV_dataEdgeFilter.h"
#include "../iVisionLib/IV_dataEdgeDebugInfo.h"
#include "../iVisionLib/IV_dataEdgeDetectionPara.h"
#include "../iVisionLib/IV_dataEdgePoint.h"

#include "../iVisionLib/IV_dataBlob_Attribute.h"
#include "../iVisionLib/IV_dataBlob_Background.h"
#include "../iVisionLib/IV_dataBlob_Image.h"
#include "../iVisionLib/IV_dataExtremalRegionBuffer.h"
#include "../iVisionLib/IV_dataExtremalRegionSpec.h"

#include "../iVisionLib/IV_dataMatrix.h"

#include "../iVisionLib/iImageProc.h"
#include "../iVisionLib/iGeoTools.h"
#include "../iVisionLib/iScalingTools.h"

#include "../iVisionLib/TemplateMatching.h"
#include "../iVisionLib/TemplateMatchingEx.h"
#include "../iVisionLib/ImageProcessing.h"
#include "../iVisionLib/ImageProcessingEx.h"
#include "../iVisionLib/Mathematics.h"
#include "../iVisionLib/ShapeProcessing.h"

#define iImageProcessing	IntekPlus::VisionMadang::ImageProcessing
#define iImageProcessingEx	IntekPlus::VisionMadang::ImageProcessingEx
#define iTemplateMatching	IntekPlus::VisionMadang::TemplateMatching
#define iTemplateMatchingEx	IntekPlus::VisionMadang::TemplateMatchingEx
#define iMathematics		IntekPlus::VisionMadang::Mathematics
#define iShapeProcessing	IntekPlus::VisionMadang::ShapeProcessing

const float		IV_MATCHERRORPOSITION		= -500000.f;

