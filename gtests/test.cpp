#include "pch.h"

// Define WIN32 to get a correct definition of ULONG_PTR
#define WIN32
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