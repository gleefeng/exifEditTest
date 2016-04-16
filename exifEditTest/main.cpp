#include "wincodecsdk.h"
#include "Propvarutil.h"
#include "iostream"
#include "assert.h"

#define XP_TITLE  (L"/app1/ifd/{ushort=40091}")
#define XP_COMMENT  (L"/app1/ifd/{ushort=40092}")
#define XP_AUTHOR  (L"/app1/ifd/{ushort=40093}")
#define XP_KEYWORDS (L"/app1/ifd/{ushort=40094}")
#define XP_SUBJECT  (L"/app1/ifd/{ushort=40095}")
#pragma comment(lib,"Shlwapi.lib")
bool getTheValue(IWICMetadataQueryReader *pQueryReader, wchar_t *name, char *dest, int size)
{
	PROPVARIANT value;
	HRESULT hr;
	PropVariantInit(&value);
	hr = pQueryReader->GetMetadataByName(name, &value);
	if (SUCCEEDED(hr))
	{
		if (value.vt == ((VT_VECTOR) | (VT_UI1)))
		{
			//PropVariantToBuffer
			//std::cout << "get the true yes!!!!" << std::endl;
			//printf("%d %s\n", value.caub.cElems, value.caub.pElems);
			wchar_t * pUni = (wchar_t*)value.caub.pElems;
			int wStringNum = wcslen(pUni);
			//std::wcout << pUni << std::endl;
			WideCharToMultiByte(CP_OEMCP, 0, pUni, -1, dest, size, NULL, FALSE);
		}
		return true;
	}
	else
	{
		printf("not found the key!\n");
		return false;
	}
	//PropVariantClear(&value);
}

void getJpegXpItem( char *jpegPath)
{
	assert(strlen(jpegPath) < 1024);
	wchar_t jpegname[1024];
	memset(jpegname, 0, sizeof(jpegname));
	MultiByteToWideChar(CP_ACP, 0, jpegPath, strlen(jpegPath), jpegname, sizeof(jpegname));

	IWICImagingFactory *pFactory = NULL;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pFrameDecode = NULL;
	IWICMetadataQueryReader *pQueryReader = NULL;
	IWICMetadataQueryReader *pEmbedReader = NULL;
	PROPVARIANT value;

	// Initialize COM
	CoInitialize(NULL);

	// Initialize PROPVARIANT
	PropVariantInit(&value);

	//Create the COM imaging factory
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&pFactory);

	// Create the decoder
	if (SUCCEEDED(hr))
	{
		hr = pFactory->CreateDecoderFromFilename(
			jpegname,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&pDecoder);
	}

	// Get a single frame from the image
	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(
			0,  //JPEG has only one frame.
			&pFrameDecode);
	}

	// Get the query reader
	if (SUCCEEDED(hr))
	{
		hr = pFrameDecode->GetMetadataQueryReader(&pQueryReader);
	}


	if (SUCCEEDED(hr))
	{
		char dest[256];
		if (getTheValue(pQueryReader, XP_TITLE,dest,sizeof(dest)))
		{
			printf("XP_TITLE : %s\n",dest);
		}
		if (getTheValue(pQueryReader, XP_AUTHOR, dest, sizeof(dest)))
		{
			printf("XP_AUTHOR : %s\n", dest);
		}
		if (getTheValue(pQueryReader, XP_COMMENT, dest, sizeof(dest)))
		{
			printf("XP_COMMENT : %s\n", dest);
		}
		if (getTheValue(pQueryReader, XP_KEYWORDS, dest, sizeof(dest)))
		{
			printf("XP_KEYWORDS : %s\n", dest);
		}
		if (getTheValue(pQueryReader, XP_SUBJECT, dest, sizeof(dest)))
		{
			printf("XP_SUBJECT : %s\n", dest);
		}
	}
	if (pFactory)
	{
		pFactory->Release();
	}

	if (pDecoder)
	{
		pDecoder->Release();
	}
	if (pFrameDecode)
	{
		pFrameDecode->Release();
	}
	CoUninitialize();
	
}
void setWPathAndDstPath(char *jpegPath,  wchar_t *wdestPath,int wdestPathSize)
{
	char destPath[1024 + 10];
	char *pName = NULL;
	memset(destPath, 0, sizeof(destPath));
	memset(wdestPath, 0, sizeof(wdestPath));
	pName = strrchr(jpegPath, '/');
	if (pName)
	{
		pName++;
		int pos = pName - jpegPath;
		memcpy(destPath, jpegPath, pos);
		strcat(destPath, "temp/");
	}
	else
	{
		pName = strrchr(jpegPath, '\\');
		if (pName == NULL)
		{
			printf("cannot find the img path! please use path '\' or '\\' \n");
			assert(0);
		}
		pName++;
		int pos = pName - jpegPath;
		memcpy(destPath, jpegPath, pos);
		strcat(destPath, "temp\\");
	}
	bool flag = PathFileExistsA(destPath);
	if (flag == false)
	{
		printf("creat path %s\n", destPath);
		CreateDirectoryA(destPath, 0);
	}
	strcat(destPath, pName);
	MultiByteToWideChar(CP_ACP, 0, destPath, strlen(destPath), wdestPath, sizeof(wdestPath));
}

void setJpegXpItem(char *jpegPath, wchar_t *XPitem, char* itemV)
{
	assert(strlen(jpegPath) < 1024);
	assert(strlen(itemV) < 1024);
	wchar_t jpegname[1024];
	memset(jpegname, 0, sizeof(jpegname));
	MultiByteToWideChar(CP_ACP, 0, jpegPath,strlen(jpegPath),jpegname,sizeof(jpegname));

	wchar_t itemValue[1024];
	memset(itemValue, 0, sizeof(itemValue));
	MultiByteToWideChar(CP_ACP, 0, itemV, strlen(itemV), itemValue, sizeof(itemValue));
	
	wchar_t wdestPath[1024 + 10];
	setWPathAndDstPath(jpegPath, wdestPath, sizeof(wdestPath));
	

	
	// Initialize COM.
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	IWICImagingFactory *piFactory = NULL;
	IWICBitmapDecoder *piDecoder = NULL;

	// Create the COM imaging factory.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory,
			NULL, CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&piFactory));
	}

	// Create the decoder.
	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateDecoderFromFilename(jpegname, NULL, GENERIC_READ,
			WICDecodeMetadataCacheOnDemand, //For JPEG lossless decoding/encoding.
			&piDecoder);
	}

	// Variables used for encoding.
	IWICStream *piFileStream = NULL;
	IWICBitmapEncoder *piEncoder = NULL;
	IWICMetadataBlockWriter *piBlockWriter = NULL;
	IWICMetadataBlockReader *piBlockReader = NULL;

	WICPixelFormatGUID pixelFormat = { 0 };
	UINT count = 0;
	double dpiX, dpiY = 0.0;
	UINT width, height = 0;

	// Create a file stream.
	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateStream(&piFileStream);
	}

	// Initialize our new file stream.
	if (SUCCEEDED(hr))
	{
		hr = piFileStream->InitializeFromFilename(wdestPath, GENERIC_WRITE);
	}

	// Create the encoder.
	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &piEncoder);
	}
	// Initialize the encoder
	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Initialize(piFileStream, WICBitmapEncoderNoCache);
	}

	if (SUCCEEDED(hr))
	{
		hr = piDecoder->GetFrameCount(&count);
	}

	if (SUCCEEDED(hr))
	{
		// Process each frame of the image.
		for (UINT i = 0; i < count && SUCCEEDED(hr); i++)
		{
			// Frame variables.
			IWICBitmapFrameDecode *piFrameDecode = NULL;
			IWICBitmapFrameEncode *piFrameEncode = NULL;
			IWICMetadataQueryReader *piFrameQReader = NULL;
			IWICMetadataQueryWriter *piFrameQWriter = NULL;

			// Get and create the image frame.
			if (SUCCEEDED(hr))
			{
				hr = piDecoder->GetFrame(i, &piFrameDecode);
			}
			if (SUCCEEDED(hr))
			{
				hr = piEncoder->CreateNewFrame(&piFrameEncode, NULL);
			}

			// Initialize the encoder.
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->Initialize(NULL);
			}
			// Get and set the size.
			if (SUCCEEDED(hr))
			{
				hr = piFrameDecode->GetSize(&width, &height);
			}
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->SetSize(width, height);
			}
			// Get and set the resolution.
			if (SUCCEEDED(hr))
			{
				piFrameDecode->GetResolution(&dpiX, &dpiY);
			}
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->SetResolution(dpiX, dpiY);
			}
			// Set the pixel format.
			if (SUCCEEDED(hr))
			{
				piFrameDecode->GetPixelFormat(&pixelFormat);
			}
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->SetPixelFormat(&pixelFormat);
			}

			// Check that the destination format and source formats are the same.
			bool formatsEqual = FALSE;
			if (SUCCEEDED(hr))
			{
				GUID srcFormat;
				GUID destFormat;

				hr = piDecoder->GetContainerFormat(&srcFormat);
				if (SUCCEEDED(hr))
				{
					hr = piEncoder->GetContainerFormat(&destFormat);
				}
				if (SUCCEEDED(hr))
				{
					if (srcFormat == destFormat)
						formatsEqual = true;
					else
						formatsEqual = false;
				}
			}

			if (SUCCEEDED(hr) && formatsEqual)
			{
				// Copy metadata using metadata block reader/writer.
				if (SUCCEEDED(hr))
				{
					piFrameDecode->QueryInterface(IID_PPV_ARGS(&piBlockReader));
				}
				if (SUCCEEDED(hr))
				{
					piFrameEncode->QueryInterface(IID_PPV_ARGS(&piBlockWriter));
				}
				if (SUCCEEDED(hr))
				{
					piBlockWriter->InitializeFromBlockReader(piBlockReader);
				}
			}

			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);
			}
			if (SUCCEEDED(hr))
			{
				// Add additional metadata.
				//PROPVARIANT    value;
				//value.vt = VT_LPWSTR;
				//value.pwszVal = L"Metadata Test Image.";
				//hr = piFrameQWriter->SetMetadataByName(L"/xmp/dc:title", &value);

				PROPVARIANT value;
				PropVariantInit(&value);
				hr = InitPropVariantFromBuffer(itemValue, wcslen(itemValue)*2+2, &value);
				if (SUCCEEDED(hr))
				{
					hr = piFrameQWriter->SetMetadataByName(XPitem, &value);
					PropVariantClear(&value);
				}
			}
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->WriteSource(
					static_cast<IWICBitmapSource*> (piFrameDecode),
					NULL); // Using NULL enables JPEG loss-less encoding.
			}

			// Commit the frame.
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->Commit();
			}

			if (piFrameDecode)
			{
				piFrameDecode->Release();
			}

			if (piFrameEncode)
			{
				piFrameEncode->Release();
			}

			if (piFrameQReader)
			{
				piFrameQReader->Release();
			}

			if (piFrameQWriter)
			{
				piFrameQWriter->Release();
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		piEncoder->Commit();
	}

	if (SUCCEEDED(hr))
	{
		piFileStream->Commit(STGC_DEFAULT);
	}

	if (piFileStream)
	{
		piFileStream->Release();
	}
	if (piEncoder)
	{
		piEncoder->Release();
	}
	if (piBlockWriter)
	{
		piBlockWriter->Release();
	}
	if (piBlockReader)
	{
		piBlockReader->Release();
	}
}

void main()
{
	setJpegXpItem(".\\111.jpg", XP_AUTHOR,"zhangkaichipi");
	getJpegXpItem(".\\temp\\111.jpg");
	system("pause");
}