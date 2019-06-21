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