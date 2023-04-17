#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	const auto id0 = pReader->Read<char>();
	const auto id1 = pReader->Read<char>();
	const auto id2 = pReader->Read<char>();
	const auto version = pReader->Read<char>();

	if (id0 != 66 && id1 != 77 && id2 != 70)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	if (version != 3)
	{
		Logger::LogError(L"SpriteFontLoader::LOadContent > Only version 3 .fnt files are supported");
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the FontSize [fontDesc.fontSize]
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [fontDesc.fontName]

	auto blockId = pReader->Read<char>();
	auto blockSize = pReader->Read<int>();

	fontDesc.fontSize = pReader->Read<short>();
	pReader->SetBufferPosition(23);
	fontDesc.fontName = pReader->ReadNullString();
	
	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	//Retrieve PageCount
	//> if pagecount > 1
	//	> Log Error (Only one texture per font is allowed!)
	//Advance to Block2 (Move Reader)

	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();

	pReader->SetBufferPosition(33 + static_cast<int>(fontDesc.fontName.length()));
	fontDesc.textureWidth = pReader->Read<short>();
	fontDesc.textureHeight = pReader->Read<short>();
	const auto pageCount = pReader->Read<short>();
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only one texture per font is allowed!");
		return nullptr;
	}
	pReader->SetBufferPosition(29 + static_cast<int>(fontDesc.fontName.length()) + blockSize);

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the PageName (BinaryReader::ReadNullString)
	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();

	const auto pageName = pReader->ReadNullString();
	fontDesc.pTexture = ContentManager::Load<TextureData>(loadInfo.assetFullPath.parent_path().append(pageName));

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Create loop for Character Count, and:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Create instance of FontMetric (struct)
	//	> Set Character (CharacterId) [FontMetric::character]
	//	> Retrieve Xposition (store Local)
	//	> Retrieve Yposition (store Local)
	//	> Retrieve & Set Width [FontMetric::width]
	//	> Retrieve & Set Height [FontMetric::height]
	//	> Retrieve & Set OffsetX [FontMetric::offsetX]
	//	> Retrieve & Set OffsetY [FontMetric::offsetY]
	//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
	//	> Retrieve & Set Page [FontMetric::page]
	//	> Retrieve Channel (BITFIELD!!!) 
	//		> See documentation for BitField meaning [FontMetrix::channel]
	//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
	//
	//> Insert new FontMetric to the metrics [font.metrics] map
	//	> key = (wchar_t) charId
	//	> value = new FontMetric
	//(loop restarts till all metrics are parsed)

	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	const int numChars = blockSize / 20;
	for (int i = 0; i < numChars; i++)
	{
		const uint32_t charId = pReader->Read<uint32_t>();
		FontMetric metric{};
		metric.character = static_cast<wchar_t>(charId);
		const uint32_t xPosition = pReader->Read<short>();
		const uint32_t yPosition = pReader->Read<short>();
		metric.width = pReader->Read<short>();
		metric.height = pReader->Read<short>();
		metric.offsetX = pReader->Read<short>();
		metric.offsetY = pReader->Read<short>();
		metric.advanceX = pReader->Read<short>();
		metric.page = pReader->Read<char>();
		const auto channel = pReader->Read<char>();
		switch (channel)
		{
		case 1:
			metric.channel = 2;
			break;
		case 2:
			metric.channel = 1;
			break;
		case 4:
			metric.channel = 0;
			break;
		case 8:
			metric.channel = 3;
			break;
		default:
			metric.channel = 0;
			break;
		}
		metric.texCoord = XMFLOAT2(float(xPosition) / float(fontDesc.textureWidth), float(yPosition) / float(fontDesc.textureHeight));
		fontDesc.metrics.insert(std::pair<wchar_t, FontMetric>(metric.character, metric));
	}

	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
