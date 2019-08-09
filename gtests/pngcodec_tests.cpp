#include "pch.h"

TEST(BitmapTest, Load16bppPngTest) {
	STARTUP

		GpBitmap* bitmap = NULL;
	ASSERT_EQ(Ok, GdipCreateBitmapFromFile(L"16bpp.png", &bitmap));

	GdipDisposeImage(bitmap);

	SHUTDOWN
}

TEST(BitmapTest, Load48bppPngTest1) {
	STARTUP

		GpBitmap* bitmap = NULL;
	PixelFormat format;
	ASSERT_EQ(Ok, GdipCreateBitmapFromFile(L"48bpp_1.png", &bitmap));
	ASSERT_EQ(Ok, GdipGetImagePixelFormat(bitmap, &format));
	ASSERT_EQ(PixelFormat24bppRGB, format);

	GdipDisposeImage(bitmap);

	SHUTDOWN
}

TEST(BitmapTest, Load48bppPngTest2) {
	STARTUP

		GpBitmap* bitmap = NULL;
	PixelFormat format;
	ASSERT_EQ(Ok, GdipCreateBitmapFromFile(L"48bpp_2.png", &bitmap));
	ASSERT_EQ(Ok, GdipGetImagePixelFormat(bitmap, &format));
	ASSERT_EQ(PixelFormat32bppARGB, format);

	GdipDisposeImage(bitmap);

	SHUTDOWN
}

TEST(BitmapTest, Load2bppPngTest) {
	STARTUP

		const CLSID jpegEncoderClsId = { 0x557cf401, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };

	GpBitmap* bitmap = NULL;
	PixelFormat format;
	ASSERT_EQ(Ok, GdipCreateBitmapFromFile(L"2bpp.png", &bitmap));
	ASSERT_EQ(Ok, GdipGetImagePixelFormat(bitmap, &format));
	ASSERT_EQ(PixelFormat8bppIndexed, format);

	// Saving an image with pixel format 'undefined' would crash the JPEG codec.
	ASSERT_EQ(Ok, GdipSaveImageToFile(bitmap, L"2bpp.jpg", &jpegEncoderClsId, NULL));

	GdipDisposeImage(bitmap);

	SHUTDOWN
}