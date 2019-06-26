#include "pch.h"

// Define WIN32 to get a correct definition of ULONG_PTR
#define WIN32
#define M_PI 3.14159265358979323846
#include "..\src\GdiPlusFlat.h"

#define STARTUP \
    ULONG_PTR gdiplusToken; \
    GdiplusStartupInput gdiplusStartupInput; \
    gdiplusStartupInput.GdiplusVersion = 1; \
    gdiplusStartupInput.DebugEventCallback = NULL; \
    gdiplusStartupInput.SuppressBackgroundThread = FALSE; \
    gdiplusStartupInput.SuppressExternalCodecs = FALSE; \
    GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL); \

#define SHUTDOWN GdiplusShutdown (gdiplusToken);

TEST(RegionTests, GetRegionScans_CustomMatrix_TransformsRegionScans) {
	STARTUP

		GpMatrix* matrix = NULL;

	ASSERT_EQ(0, GdipCreateMatrix(&matrix));
	ASSERT_EQ(0, GdipTranslateMatrix(matrix, 10, 11, MatrixOrderPrepend));
	ASSERT_EQ(0, GdipScaleMatrix(matrix, 5, 6, MatrixOrderPrepend));

	GpRect rect = { 1, 2, 3, 4 };

	GpRegion* region = NULL;
	ASSERT_EQ(0, GdipCreateRegionRectI(&rect, &region));

	UINT count;
	ASSERT_EQ(0, GdipGetRegionScansCount(region, &count, matrix));
	ASSERT_EQ(1, count);

	GpRectF rects = { 0, 0, 0, 0 };
	int scansCount;
	ASSERT_EQ(0, GdipGetRegionScans(region, &rects, &scansCount, matrix));

	SHUTDOWN
}

TEST(RegionTests, Xor_GraphicsPath_Success) {
	STARTUP

		GpRegion* region = NULL;
	ASSERT_EQ(0, GdipCreateRegion(&region));

	GpRectF rectangles[] =
	{
		{ 520, 40, 60, 80 }
	};

	GpRectF expectedScans[] =
	{
		{ -1 * 0x400000, -1 * 0x400000, 0x800000, 0x400000 + 40 },
		{ -1 * 0x400000, 40, 0x400000 + 520, 80 },
		{ 580, 40, 0x400000 - 580, 80 },
		{ -1 * 0x400000, 120, 0x800000, 0x400000 - 120 }
	};

	GpPath* path = NULL;
	ASSERT_EQ(0, GdipCreatePath(FillModeAlternate, &path));
	ASSERT_EQ(0, GdipAddPathRectangle(path, rectangles[0].X, rectangles[0].Y, rectangles[0].Width, rectangles[0].Height));
	ASSERT_EQ(0, GdipCombineRegionPath(region, path, CombineModeXor));

	SHUTDOWN
}

TEST(RegionTests, GdipGetPathWorldBounds) {
	STARTUP

		GpRegion* region = NULL;
	ASSERT_EQ(0, GdipCreateRegion(&region));

	GpRectF rectangles[] =
	{
		{ 520, 40, 60, 80 }
	};

	GpPath* path = NULL;
	ASSERT_EQ(0, GdipCreatePath(FillModeAlternate, &path));
	ASSERT_EQ(0, GdipAddPathRectangle(path, rectangles[0].X, rectangles[0].Y, rectangles[0].Width, rectangles[0].Height));

	GpRectF bounds = { 0, 0, 0, 0 };
	ASSERT_EQ(0, GdipGetPathWorldBounds(path, &bounds, NULL, NULL));
	ASSERT_EQ(bounds.X, rectangles[0].X);
	ASSERT_EQ(bounds.Y, rectangles[0].Y);
	ASSERT_EQ(bounds.Width, rectangles[0].Width);
	ASSERT_EQ(bounds.Height, rectangles[0].Height);

	SHUTDOWN
}

TEST(GraphicsTest, FromHdcInternal_GetDC_ReturnsExpected) {
	STARTUP

		HDC hdc = GetDC(NULL);

	GpGraphics* graphics = NULL;
	ASSERT_EQ(0, GdipCreateFromHDC(hdc, &graphics));

	SHUTDOWN
}

TEST(GraphicsTest, CustomPixelFormat_GetPixels_ReturnsExpected) {
	STARTUP

	GpBitmap* bitmap = NULL;
	ASSERT_EQ(0, GdipCreateBitmapFromScan0(2, 1, 0, PixelFormat24bppRGB, NULL, &bitmap));

	GpRect rect = { 0, 0, 2, 1 };
	BitmapData data;

	ASSERT_EQ(0, GdipBitmapLockBits(bitmap, &rect, ImageLockModeRead, PixelFormat24bppRGB, &data));
	ASSERT_EQ(0, GdipBitmapUnlockBits(bitmap, &data));

	ASSERT_EQ(0, GdipDisposeImage(bitmap));

	SHUTDOWN
}

TEST(GraphicsTest, FromHicon_InvalidHandle_ThrowsArgumentException) {
	STARTUP

	GpBitmap* bitmap = NULL;
	HICON icon = (HICON)10;
	ASSERT_EQ(NotImplemented, GdipCreateBitmapFromHICON(icon, &bitmap));

	SHUTDOWN
}

TEST(ImageAttributesTests, Clone_Success) {
	STARTUP

	GpRect rect = { 0, 0, 64, 64 };
	GpBitmap *bitmap = NULL;
	GpGraphics* graphics = NULL;
	GpImageAttributes* attributes = NULL;

	ColorMatrix matrix =
	{{
		{ 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0 },
		{ 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0 },
	}};

	ASSERT_EQ(0, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(0, GdipGetImageGraphicsContext(bitmap, &graphics));
	ASSERT_EQ(0, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(0, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, true, &matrix, NULL, ColorMatrixFlagsDefault));

	GpImageAttributes* attributesClone = NULL;
	ASSERT_EQ(0, GdipCloneImageAttributes(attributes, &attributesClone));

	ASSERT_EQ(0, GdipDisposeImageAttributes(attributesClone));
	ASSERT_EQ(0, GdipDisposeImageAttributes(attributes));
	ASSERT_EQ(0, GdipDeleteGraphics(graphics));
	ASSERT_EQ(0, GdipDisposeImage(bitmap));
	
	SHUTDOWN
}

TEST(InstalledFontCollectionTests, Ctor_Default) {
	STARTUP

	GpFontCollection* fontCollection = NULL;
	ASSERT_EQ(0, GdipNewInstalledFontCollection(&fontCollection));

	INT numFound = 0;
	ASSERT_EQ(0, GdipGetFontCollectionFamilyCount(fontCollection, &numFound));
	ASSERT_NE(0, numFound);

	SHUTDOWN
}

TEST(MatrixTests, Ctor_FloatingPointBoundsInElements) {
	STARTUP

	float values[3] = { NAN, INFINITY, -INFINITY };

	for (int i = 0; i < 3; i++)
	{
		float f = values[i];
		GpMatrix* matrix = NULL;

		ASSERT_EQ(0, GdipCreateMatrix2(f, 0, 0, 1, 0, 0, &matrix));

		BOOL result;
		ASSERT_EQ(0, GdipIsMatrixIdentity(matrix, &result));
		ASSERT_EQ(FALSE, result);

		ASSERT_EQ(0, GdipIsMatrixInvertible(matrix, &result));
		ASSERT_EQ(FALSE, result);

		REAL* elements = (REAL*)malloc(8 * sizeof(float));;
		GdipGetMatrixElements(matrix, elements);
		ASSERT_EQ(0, elements[4]);
		ASSERT_EQ(0, elements[5]);
	}

	SHUTDOWN
}

TEST(MatrixTests, Invert_FloatBounds_ThrowsArgumentException) {
	STARTUP

	float values[3] = { NAN, INFINITY, -INFINITY };

	for (int i = 0; i < 3; i++)
	{
		float f = values[i];
		GpMatrix* matrix = NULL;

		ASSERT_EQ(0, GdipCreateMatrix2(f, 0, 0, 1, 0, 0, &matrix));

		ASSERT_EQ(InvalidParameter, GdipInvertMatrix(matrix));
	}

	SHUTDOWN
}

TEST(MatrixTests, Multiply_Matrix_Success) {
	STARTUP

	GpMatrix* matrix = NULL;
	ASSERT_EQ(0, GdipCreateMatrix2(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, &matrix));

	GpMatrix* multiple = NULL;
	ASSERT_EQ(0, GdipCreateMatrix2(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, &multiple));

	ASSERT_EQ(0, GdipMultiplyMatrix(matrix, multiple, MatrixOrderAppend));
	

	REAL* elements = (REAL*)malloc(6 * sizeof(float));;
	GdipGetMatrixElements(matrix, elements);

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(FLT_MAX, elements[i]);
	}

	SHUTDOWN
}

TEST(MatrixTests, Multiply_Matrix_Success2) {
	STARTUP

	GpMatrix* matrix = NULL;
	ASSERT_EQ(0, GdipCreateMatrix2(10, 20, 30, 40, 50, 60, &matrix));

	GpMatrix* multiple = NULL;
	ASSERT_EQ(0, GdipCreateMatrix2(0, 0, 0, 0, 0, 0, &multiple));

	ASSERT_EQ(0, GdipMultiplyMatrix(matrix, multiple, MatrixOrderAppend));


	REAL* elements = (REAL*)malloc(6 * sizeof(float));
	GdipGetMatrixElements(matrix, elements);

	for (int i = 0; i < 6; i++)
	{
		ASSERT_EQ(0, elements[i]);
	}

	SHUTDOWN
}

TEST(MatrixTests, Multiply_Matrix_Success3) {
	STARTUP

	GpMatrix* matrix = NULL;
	ASSERT_EQ(0, GdipCreateMatrix2(10, 20, 30, 40, 50, 60, &matrix));

	GpMatrix* multiple = NULL;
	ASSERT_EQ(0, GdipCreateMatrix2(0, 0, 0, 0, 0, 0, &multiple));

	ASSERT_EQ(0, GdipMultiplyMatrix(matrix, multiple, MatrixOrderPrepend));

	REAL* elements = (REAL*)malloc(6 * sizeof(float));
	GdipGetMatrixElements(matrix, elements);

	for (int i = 0; i < 4; i++)
	{
		ASSERT_EQ(0, elements[i]);
	}
	ASSERT_EQ(50, elements[4]);
	ASSERT_EQ(60, elements[5]);

	SHUTDOWN
}

TEST(LinearGradientBrushTests, Ctor_EqualPoints_ThrowsOutOfMemoryException) {
	STARTUP

	GpPoint point1 = { 0, 1 };
	GpPoint point2 = { 0, 1 };
	GpLineGradient* gradient = NULL;

	ASSERT_EQ(OutOfMemory, GdipCreateLineBrushI(&point1, &point2, 0x00FF00, 0x0000FF, WrapModeTile, &gradient));

	SHUTDOWN
}

TEST(CustomLineCapTests, Ctor_InvalidLineCap_ReturnsFlat) {
	STARTUP

	LineCap caps[2] = { (LineCap)(LineCapFlat - 1), (LineCap)(LineCapCustom + 1) };

	for (int i = 0; i < 2; i++)
	{
		LineCap cap = caps[i];

		GpPath* fillPath;
		GpPath* strokePath;
		ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &fillPath));
		ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &strokePath));

		GpCustomLineCap* lineCap = NULL;
		ASSERT_EQ(Ok, GdipCreateCustomLineCap(fillPath, strokePath, cap, 0, &lineCap));

		LineCap baseCap;
		ASSERT_EQ(Ok, GdipGetCustomLineCapBaseCap(lineCap, &baseCap));
		ASSERT_EQ(LineCapFlat, baseCap);
	}

	SHUTDOWN
}

TEST(CustomLineCapTests, Ctor_Path_Path_LineCap_Float) {
	STARTUP

	LineCap caps[19] = 
	{
		(LineCap)(LineCapFlat - 1),
		LineCapFlat,
		LineCapSquare,
		LineCapRound,
		LineCapTriangle,
		(LineCap)(LineCapTriangle + 1),
		(LineCap)(LineCapNoAnchor - 1),
		LineCapNoAnchor,
		LineCapSquareAnchor,
		LineCapRoundAnchor,
		LineCapDiamondAnchor,
		LineCapArrowAnchor,
		(LineCap)(LineCapArrowAnchor + 1),
		(LineCap)(LineCapAnchorMask - 1),
		LineCapAnchorMask,
		(LineCap)(LineCapAnchorMask + 1),
		(LineCap)(LineCapCustom - 1),
		LineCapCustom,
		(LineCap)(LineCapCustom + 1) 
	};

	LineCap expectedCaps[19] =
	{
		LineCapFlat,
		LineCapFlat,
		LineCapSquare,
		LineCapRound,
		LineCapTriangle,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat,
		LineCapFlat
	};

	for (int i = 0; i < 19; i++)
	{
		LineCap cap = caps[i];
		LineCap expectedCap = expectedCaps[i];

		GpPath* fillPath;
		GpPath* strokePath;
		ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &fillPath));
		ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &strokePath));

		GpCustomLineCap* lineCap = NULL;
		ASSERT_EQ(Ok, GdipCreateCustomLineCap(fillPath, strokePath, cap, 0, &lineCap));

		LineCap baseCap;
		ASSERT_EQ(Ok, GdipGetCustomLineCapBaseCap(lineCap, &baseCap));
		ASSERT_EQ(expectedCap, baseCap);
	}

	SHUTDOWN
}

TEST(GraphicsPathTests, AddArc_ZeroWidthHeight_ThrowsArgumentException) {
	STARTUP

	GpPath* path;
	ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &path));

	ASSERT_EQ(InvalidParameter, GdipAddPathArcI(path, 0, 0, 0, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathArcI(path, 0, 0, 1, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathArcI(path, 0, 0, 0, 1, 3.14, 3.14));

	ASSERT_EQ(InvalidParameter, GdipAddPathArc(path, 0, 0, 0, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathArc(path, 0, 0, 1, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathArc(path, 0, 0, 0, 1, 3.14, 3.14));

	ASSERT_EQ(Ok, GdipDeletePath(path));
	SHUTDOWN
}

TEST(GraphicsPathTests, AddPie_ZeroWidthHeight_ThrowsArgumentException) {
	STARTUP

	GpPath* path;
	ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &path));

	ASSERT_EQ(InvalidParameter, GdipAddPathPieI(path, 0, 0, 0, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathPieI(path, 0, 0, 1, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathPieI(path, 0, 0, 0, 1, 3.14, 3.14));

	ASSERT_EQ(InvalidParameter, GdipAddPathPie(path, 0, 0, 0, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathPie(path, 0, 0, 1, 0, 3.14, 3.14));
	ASSERT_EQ(InvalidParameter, GdipAddPathPie(path, 0, 0, 0, 1, 3.14, 3.14));

	ASSERT_EQ(Ok, GdipDeletePath(path));
	SHUTDOWN
}

TEST(ImageAttributesTests, ClearColorMatrix_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));

	ColorMatrix colorMatrix = { {
		{1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	} };

	ColorMatrix grayMatrix = { {
		{1, 0, 0, 0, 0},
		{0, 2, 0, 0, 0},
		{0, 0, 3, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	} };

	ASSERT_EQ(Ok, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault));
	ASSERT_EQ(Ok, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault));
	ASSERT_EQ(Ok, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, false, NULL, NULL, ColorMatrixFlagsDefault));

	ARGB green = 0xFF00FF00;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(green, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, ClearNoOp_Type_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesGamma(attributes, ColorAdjustTypeDefault, true, 2.2));

	ColorMatrix colorMatrix = { {
		{1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	} };

	ASSERT_EQ(Ok, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault));
	ASSERT_EQ(Ok, GdipSetImageAttributesNoOp(attributes, ColorAdjustTypeDefault, true));
	ASSERT_EQ(Ok, GdipSetImageAttributesNoOp(attributes, ColorAdjustTypeDefault, false));

	ARGB green = 0xFF64FF00;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(0xFF210000, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, SetGamma_Gamma_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesGamma(attributes, ColorAdjustTypeDefault, true, 2.2));

	ARGB green = 0xFF64FF00;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(0xFF21FF00, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, ClearColorKey_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesColorKeys(attributes, ColorAdjustTypeDefault, true, 0x00323232, 0x00969696));
	ASSERT_EQ(Ok, GdipSetImageAttributesColorKeys(attributes, ColorAdjustTypeDefault, false, 0x00323232, 0x00969696));

	ARGB green = 0xFF646464;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(green, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, ClearGamma_Type_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesGamma(attributes, ColorAdjustTypeDefault, true, 2.2));
	ASSERT_EQ(Ok, GdipSetImageAttributesGamma(attributes, ColorAdjustTypeDefault, false, 0));

	ARGB green = 0xFF64FF00;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(green, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, ClearOutputChannelColorProfile_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesOutputChannel(attributes, ColorAdjustTypeDefault, true, ColorChannelFlagsC));
	ASSERT_EQ(Ok, GdipSetImageAttributesOutputChannel(attributes, ColorAdjustTypeDefault, false, ColorChannelFlagsC));

	ARGB green = 0xFF64FF00;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(green, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, ClearRemapTable_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	ColorMap colorMap[1] = {
		{ 0xFFFFFF00, 0xFFFF0000 }
	};

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesRemapTable(attributes, ColorAdjustTypeDefault, true, 1, colorMap));
	ASSERT_EQ(Ok, GdipSetImageAttributesRemapTable(attributes, ColorAdjustTypeDefault, false, 0, NULL));

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, colorMap[0].oldColor.Argb));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(colorMap[0].oldColor.Argb, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, ClearThreshold_ThresholdTypeI_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesThreshold(attributes, ColorAdjustTypeDefault, true, 0.7));
	ASSERT_EQ(Ok, GdipSetImageAttributesThreshold(attributes, ColorAdjustTypeDefault, false, 0));

	ARGB green = 0xFF64FF00;
	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(green, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, GetAdjustedPalette_Disposed_ThrowsArgumentException) {
	STARTUP

	GpBitmap* bitmap = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipDisposeImageAttributes(attributes));
	
	int size = 0;
	ASSERT_EQ(Ok, GdipGetImagePaletteSize(bitmap, &size));

	ColorPalette* palette = (ColorPalette*)malloc(size);

	ASSERT_EQ(Ok, GdipGetImagePalette(bitmap, palette, size));

	ASSERT_EQ(InvalidParameter, GdipGetImageAttributesAdjustedPalette(attributes, palette, ColorAdjustTypeDefault));

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));

	SHUTDOWN
}

TEST(ImageAttributesTests, SetColorKey_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesColorKeys(attributes, ColorAdjustTypeDefault, true, 0x00323232, 0x00969696));

	ARGB green = 0xFF646464;
	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(0x00000000, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, SetColorMatrices_InvalidFlags_ThrowsArgumentException) {
	STARTUP

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));

	ColorMatrix colorMatrix = { {
		{1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	} };

	ColorMatrixFlags flags[4] = {
		(ColorMatrixFlags)(ColorMatrixFlagsDefault - 1),
		(ColorMatrixFlags)(ColorMatrixFlagsAltGray + 1),
		(ColorMatrixFlags)INT32_MAX,
		(ColorMatrixFlags)INT32_MIN
	};

	for (int i = 0; i < 4; i++)
	{
		ASSERT_EQ(InvalidParameter, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, true, &colorMatrix, NULL, flags[i]));
	}

	ASSERT_EQ(Ok, GdipDisposeImageAttributes(attributes));

	SHUTDOWN
}

TEST(ImageAttributesTests, SetNoOp_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesGamma(attributes, ColorAdjustTypeDefault, true, 2.2));

	ColorMatrix colorMatrix = { {
		{1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	} };

	ASSERT_EQ(Ok, GdipSetImageAttributesColorMatrix(attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault));
	ASSERT_EQ(Ok, GdipSetImageAttributesNoOp(attributes, ColorAdjustTypeDefault, true));

	ARGB green = 0xFF64FF00;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));
	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(green, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));


	SHUTDOWN
}

TEST(ImageAttributesTests, SetThreshold_Threshold_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesThreshold(attributes, ColorAdjustTypeDefault, true, 0.7));

	ARGB green = 0xFFE632DC;
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));

	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(0xFFFF00FF, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, SetOutputChannel_FlagType_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(64, 64, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	GpImageAttributes* attributes;
	ASSERT_EQ(Ok, GdipCreateImageAttributes(&attributes));
	ASSERT_EQ(Ok, GdipSetImageAttributesOutputChannel(attributes, ColorAdjustTypeDefault, true, ColorChannelFlagsC));

	ARGB green = 0xFF646464;
	ARGB actualColor = 0;

	ASSERT_EQ(Ok, GdipBitmapSetPixel(bitmap, 0, 0, green));

	ASSERT_EQ(Ok, GdipDrawImageRectRectI(graphics, bitmap, 0, 0, 64, 64, 0, 0, 64, 64, UnitPixel, attributes, NULL, NULL));

	ASSERT_EQ(Ok, GdipBitmapGetPixel(bitmap, 0, 0, &actualColor));
	ASSERT_EQ(0xFFC6C6C6, actualColor);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(ImageAttributesTests, Ctor_IntPtrRectangleFMetafileFrameUnit_Success) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpGraphics* graphics = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(10, 10, 0, PixelFormat32bppARGB, NULL, &bitmap));
	ASSERT_EQ(Ok, GdipGetImageGraphicsContext(bitmap, &graphics));

	HDC hdc = NULL;
	ASSERT_EQ(Ok, GdipGetDC(graphics, &hdc));

	GpRectF rect = { 0, 0, 64, 64 };
	GpMetafile* metafile = NULL;
	ASSERT_EQ(Ok, GdipRecordMetafile(hdc, EmfTypeEmfPlusDual, &rect, MetafileFrameUnitDocument, NULL, &metafile));

	MetafileHeader header;

	ASSERT_EQ(0, GdipGetMetafileHeaderFromMetafile(metafile, &header));

	ASSERT_EQ(0, header.Width);
	ASSERT_EQ(0, header.Height);
	ASSERT_EQ(0, header.X);
	ASSERT_EQ(0, header.Y);

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	ASSERT_EQ(Ok, GdipDeleteGraphics(graphics));

	SHUTDOWN
}

TEST(BitmapTests, Clone_RectangleF_ReturnsExpected) {
	STARTUP

	GpBitmap* bitmap = NULL;
	GpBitmap* clone = NULL;

	ASSERT_EQ(Ok, GdipCreateBitmapFromScan0(3, 3, 0, PixelFormat32bppARGB, NULL, &bitmap));

	PixelFormat pixelFormats[2] = {
		PixelFormat32bppARGB,
		PixelFormat24bppRGB
	};

	PixelFormat pixelFormat;

	for (int i = 0; i < 2; i++)
	{
		ASSERT_EQ(Ok, GdipCloneBitmapArea(0, 0, 3, 3, pixelFormats[i], bitmap, &clone));
		ASSERT_EQ(Ok, GdipGetImagePixelFormat(bitmap, &pixelFormat));

		ASSERT_EQ(pixelFormats[i], pixelFormat);
		ASSERT_EQ(Ok, GdipDisposeImage(clone));
	}

	ASSERT_EQ(Ok, GdipDisposeImage(bitmap));
	SHUTDOWN
}

TEST(FontFamilyTests, Ctor_NoSuchFontName_ThrowsArgumentException) {
	STARTUP

	GpFontFamily* fontFamily;

	WCHAR* name = L"XYZ";

	ASSERT_EQ(Ok, GdipCreateFontFamilyFromName(name, NULL, &fontFamily));

	GpFont* font;
	ASSERT_EQ(Ok, GdipCreateFont(fontFamily, 12, FontStyleRegular, (Unit)(UnitMillimeter + 1), &font));

	GdipDeleteFont(font);
	GdipDeleteFontFamily(fontFamily);
	SHUTDOWN
}

TEST(GraphicsPathTests, AddArc_Rectangle_Success) {
	STARTUP

	GpPath* path = NULL;
	ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &path));

	GdipAddPathArc(path, 1, 1, 2, 2, M_PI / 4.0, M_PI / 4.0);

	int pointCount = 0;
	ASSERT_EQ(Ok, GdipGetPointCount(path, &pointCount));
	ASSERT_EQ(4, pointCount);

	GpPointF* points = (GpPointF*)malloc(sizeof(GpPointF) * pointCount);
	BYTE* types = (BYTE*)malloc(sizeof(BYTE) * pointCount);

	ASSERT_EQ(Ok, GdipGetPathPoints(path, points, pointCount));
	ASSERT_EQ(Ok, GdipGetPathTypes(path, types, pointCount));

	GpRectF bounds;
	ASSERT_EQ(Ok, GdipGetPathWorldBounds(path, &bounds, NULL, NULL));

	ASSERT_NEAR(2.99990582, points[0].X, 0.001);
	ASSERT_NEAR(2.01370716, points[0].Y, 0.001);

	ASSERT_NEAR(2.99984312, points[1].X, 0.001);
	ASSERT_NEAR(2.018276, points[1].Y, 0.001);

	ASSERT_NEAR(2.99974918, points[2].X, 0.001);
	ASSERT_NEAR(2.02284455, points[2].Y, 0.001);

	ASSERT_NEAR(2.999624, points[3].X, 0.001);
	ASSERT_NEAR(2.027412, points[3].Y, 0.001);

	ASSERT_EQ(0, types[0]);
	ASSERT_EQ(3, types[1]);
	ASSERT_EQ(3, types[2]);
	ASSERT_EQ(3, types[3]);

	ASSERT_NEAR(2.99962401, bounds.X, 0.001);
	ASSERT_NEAR(2.01370716, bounds.Y, 0.001);
	ASSERT_NEAR(0, bounds.Width, 0.001);
	ASSERT_NEAR(0.0137047768, bounds.Height, 0.001);

	SHUTDOWN
}

TEST(GraphicsPathTests, AddClosedCurve_Points_Success) {
	STARTUP

	GpPath* path = NULL;
	ASSERT_EQ(Ok, GdipCreatePath(FillModeAlternate, &path));

	GpPoint _points[3] = {
		{ 1, 1 },
		{ 2, 2 },
		{ 3, 3 }
	};

	ASSERT_EQ(Ok, GdipAddPathClosedCurveI(path, _points, 3));

	int pointCount = 0;
	ASSERT_EQ(Ok, GdipGetPointCount(path, &pointCount));
	ASSERT_EQ(10, pointCount);

	GpPointF* points = (GpPointF*)malloc(sizeof(GpPointF) * pointCount);
	BYTE* types = (BYTE*)malloc(sizeof(BYTE) * pointCount);

	ASSERT_EQ(Ok, GdipGetPathPoints(path, points, pointCount));
	ASSERT_EQ(Ok, GdipGetPathTypes(path, types, pointCount));

	GpRectF bounds;
	ASSERT_EQ(Ok, GdipGetPathWorldBounds(path, &bounds, NULL, NULL));

	ASSERT_NEAR(0.8333333, bounds.X, 0.001);
	ASSERT_NEAR(0.8333333, bounds.Y, 0.001);
	ASSERT_NEAR(2.33333278, bounds.Width, 0.001);
	ASSERT_NEAR(2.33333278, bounds.Height, 0.001);

	SHUTDOWN
}