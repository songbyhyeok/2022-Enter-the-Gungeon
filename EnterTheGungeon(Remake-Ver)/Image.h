#pragma once

/*
* BitBlt -> ��Ʈ���� ����ϱ� ���� ���� �⺻���� �Լ��̴�.
* -> �ش� hdc ���� ����� ���� ��ġ�������� �����ؼ� ���� ��ġ�� �ٿ� �־��
* TransparentBlt ���� -> GdiTransparentBlt -> RGB ������ ������ �ش� ��Ʈ�ʿ��� ����(����ȭ) ����� �� �ִ�.
* GdiAlphaBlend -> ����, ������ ����� ����
* StretchBlt -> �̹����� ũ�⸦ �����Ͽ� ȭ�鿡 ����� �ִ� �Լ�(�������� �ս��� ����)(Ȯ��/��� ����)
*/

#include "pch.h"

struct IMAGE_INFO
{
public:
	IMAGE_INFO()
	{
		hMemDC = nullptr;
		hBitmap = nullptr;
		hOldBit = nullptr;

		hTempDC = nullptr;
		hTempBitmap = nullptr;
		hTempOldBit = nullptr;

		hBlendDC = nullptr;
		hBlendBitmap = nullptr;
		hBlendOldBit = nullptr;

		hRotateDC = nullptr;
		hRotateBitmap = nullptr;
		hRotateOldBit = nullptr;
		rotateMaxSize = 0;

		scale.x = scale.y = 0.0f;
		startPosition.x = startPosition.y = 0;
		size.x = size.y = 0;
		printSize.x = printSize.y = 0;
		curKeyFrame.x = curKeyFrame.y = 0;
		maxKeyFrame.x = maxKeyFrame.y = 0;
		keyFrameSize.x = keyFrameSize.y = 0;
		resID = 0;
		loadType = 0;
		source = nullptr;
	}

public:
	HDC			hMemDC;				// �׸��⸦ �ְ��ϴ� �ڵ�(�޸� ����)
	HBITMAP		hBitmap;			// �̹��� ����
	HBITMAP		hOldBit;			// ���� �̹��� ����

	HDC			hTempDC;			// �׸��⸦ �ְ��ϴ� �ڵ�(�޸� ����)
	HBITMAP		hTempBitmap;		// �̹��� ����
	HBITMAP		hTempOldBit;		// ���� �̹��� ����

	HDC			hBlendDC;			// �׸��⸦ �ְ��ϴ� �ڵ�(�޸� ����)
	HBITMAP		hBlendBitmap;		// �̹��� ����
	HBITMAP		hBlendOldBit;		// ���� �̹��� ����

	HDC			hRotateDC;			// �׸��⸦ �ְ��ϴ� �ڵ�(�޸� ����)
	HBITMAP		hRotateBitmap;		// �̹��� ����
	HBITMAP		hRotateOldBit;		// ���� �̹��� ����
	int32		rotateMaxSize;

	FPOINT		scale;				// ������
	POINT		startPosition;		// ���� ����
	POINT		size;				// �̹��� ũ��
	POINT		printSize;			// scale * �̹��� ���
	POINT		curKeyFrame;		// ���� Ű������ �ּ�
	POINT		maxKeyFrame;		// �ִ� Ű������ �ּ�
	POINT		keyFrameSize;		// Ű������ ũ��
	DWORD		resID;				// ���ҽ��� ������ ���̵�
	BYTE		loadType;			// �ε� Ÿ��
	const char* source;
};

struct REVERSE_DATA
{
	bool x;
	bool y;
};

constexpr POINT BigSize{ 3000,3000 };
class Image
{
public:
	Image()
	{
		_transColor = RGB(0, 0, 0);
		_blendFunc.AlphaFormat = 0;
		_blendFunc.BlendFlags = 0;
		_blendFunc.BlendOp = AC_SRC_OVER;
		_blendFunc.SourceConstantAlpha = 0;
		_identityNumber = -1;
		_isBigSize = false;
		_isTrans = false;
		_reverseData = { false, false};
	}

	void						init(POINT sp, FPOINT scale, POINT size);
	void						init(const char* fileName, POINT sp, FPOINT scale, POINT size, bool isTrans = false, REVERSE_DATA reverseData = { false, false }, COLORREF transColor = 0);
	void						init(const char* fileName, POINT sp, FPOINT scale, POINT size, POINT keyFrame, bool isTrans = false, REVERSE_DATA reverseData = { false, false }, COLORREF transColor = 0);
	void						render(const HDC& hdc, const POINT& dest) const;
	void						release(int16 identityNumber);

	uint16						getIdentityNumber() const { return _identityNumber; }
	const HDC&					getMemDC() const { return _imageInfo.hMemDC; }
	const POINT&				getImagePrintSize() const { return _imageInfo.printSize; }
	const POINT&				getMaxFrame() const { return _imageInfo.maxKeyFrame; }
	const POINT&				getFrameSize() const { return _imageInfo.keyFrameSize; }
	void						setCurrentKeyFrameY(uint16 y) { _imageInfo.curKeyFrame.y = y; }
	const POINT&				getCurrentKeyFrame() const { return _imageInfo.curKeyFrame; }
	void						setStartPosition(POINT startPosition) { _imageInfo.startPosition = move(startPosition); }
	const POINT&				getStartPosition() const { return _imageInfo.startPosition; }
	const char*					getSource() const { return _imageInfo.source; }
	void						setPrintSize(POINT printSize) { _imageInfo.printSize = move(printSize); }
	const POINT&				getSize() const { return _imageInfo.size; }
	void						setReverseData(REVERSE_DATA reverseData) { _reverseData = move(reverseData); }
	const REVERSE_DATA&			getReverseData() const { return _reverseData; }
	void						setTransparency(BYTE alpha) { _blendFunc.SourceConstantAlpha = alpha; }

	void						initDatas(POINT sp, FPOINT scale, POINT size);
	void						initDatas(FPOINT scale, POINT size, POINT sp, bool isTrans = false, REVERSE_DATA reverseData = { false, false }, COLORREF transColor = 0);
	void						initDatas(FPOINT scale, POINT size, POINT sp, POINT keyFrame, bool isTrans = false, REVERSE_DATA reverseData = { false, false }, COLORREF transColor = 0);
	void						renderFrame(const HDC& hdc, const POINT& dest) const;
	void						renderFrame(const HDC& hdc, const POINT& dest, const POINT& keyFrame) const;
	void						renderFrame(const HDC& hdc, const POINT& dest, const POINT& keyFrame, const POINT& size, const POINT& printSize) const;
	void						renderAnimation(const HDC& hdc, const POINT& dest, const POINT& sourcePoint) const;
	void						renderTransparentAnimation(const HDC& hdc, const POINT& dest, const POINT& sourcePoint) const;
	void						renderTransparent(const HDC& hdc, const POINT& dest) const;
	void						renderTransparentFrame(const HDC& hdc, const POINT& dest, const POINT& keyFrame) const;
	void						renderTransparentFrame(const HDC& hdc, const POINT& dest, const POINT& keyFrame, const POINT& size, const POINT& printSize, const BYTE alpha);
	void						renderRotation(const HDC& hdc, const POINT& dest, uint16 rotationAngle) const;
	void						renderRotatingFrame(const HDC& hdc, const POINT& dest, const POINT& keyFrame, int16 rotationAngle) const;
	void						renderTransparentRotatingFrame(const HDC& hdc, const POINT& dest, const POINT& keyFrame, int16 rotationAngle) const;
	void						resetSize(POINT startPosition, POINT size, POINT printSize);
	void						resetSpAndPrintSizeOfImage(POINT startPosition, POINT printSize);
	void						resetSpAndScaleOfImage(POINT startPosition, FPOINT scale);

private:
	void						setMemDC(const HDC& hdc, const char* fileName, const POINT& size);
	void						setTempDC(const HDC& hdc, const POINT& size);
	void						setBlendDC(const HDC& hdc, const POINT& size);
	void						setRotateDC(const HDC& hdc, const POINT& size);
	void						setHDCs(const HDC& hdc, const char* fileName, const POINT& size);
	void						setSize(POINT size) { _imageInfo.size = move(size); }
	void						setPrintSize();

	bool						isBigSize() const { return _isBigSize; }

	void						processHDCAndDCs(const char* fileName, POINT size);
	void						generateIdentityNumber(int16& identityN);
	void						checkBigSize(uint16 width, uint16 height) { _isBigSize = _imageInfo.size.x >= width || _imageInfo.size.y >= height; } // ���ں��� ���ų� Ŭ ��� bool true	
	void						checkNullptrOfDCs() const;

private:
	IMAGE_INFO					_imageInfo;							// �̹��� ���� ����ü PTR
	BLENDFUNCTION				_blendFunc;
	COLORREF					_transColor;						// ����ȭ�� ��
	int16						_identityNumber;	
	REVERSE_DATA				_reverseData;
	bool						_isBigSize;							
	bool						_isTrans;	
};

