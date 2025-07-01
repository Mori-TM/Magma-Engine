/*
void ImageToRGB(unsigned char** RGBA, int Width, int Height)
{
	size_t Size = Width * Height * 4;
	unsigned char* Data = (unsigned char*)malloc(Width * Height * 3);

	int j = 0;
	for (int i = 0; i < Size; i += 4)
	{
	//	unsigned int* Color = (unsigned int*)(Data + j);
	//	if (Image->Data[i + 3] == 0)
	//	{
	//		memcpy(Color, &AlphaColor, 3);
	//	}
	//	else
		{
			Data[j	  ] = (*RGBA)[i];
			Data[j + 1] = (*RGBA)[i + 1];
			Data[j + 2] = (*RGBA)[i + 2];
		}

		j += 3;
	}

	free(*RGBA);
	*RGBA = Data;
}
*/

bool CompressImage(unsigned char* rgba_data, int width, int height, unsigned char** bc1_data, size_t* bc1_size, uint32_t Format, bool TextureCompressedHQ)
{
	const int num_blocks = (width / 4) * (height / 4);
	const size_t block_size = sizeof(unsigned char) * 64;

	// Allocate memory for compressed image
	*bc1_size = num_blocks * block_size;
	*bc1_data = (unsigned char*)malloc(*bc1_size);
	if (!*bc1_data)
		return false;

//	unsigned char* Data = stbi__convert_format(rgba_data, 4, 3, width, height);
	if (Format == OPENVK_FORMAT_BC4_RGBA)
		return compress_pixelsBC4(*bc1_data, rgba_data, width, height, 0);
	else
		return compress_pixels(*bc1_data, rgba_data, width, height, 0, TextureCompressedHQ);
	/*
	const int num_blocks = (width / 4) * (height / 4);
    const size_t block_size = sizeof(unsigned char) * 8;

    // Allocate memory for compressed image
    *bc1_size = num_blocks * block_size;
    *bc1_data = (unsigned char*)malloc(*bc1_size);

 //   ImageToRGB(rgba_data, width, height);

//	size_t Size = width * height * 4;
	unsigned char* Data = stbi__convert_format(rgba_data, 4, 3, width, height);

//	int j = 0;
//	for (int i = 0; i < Size; i += 4)
//	{
//		//	unsigned int* Color = (unsigned int*)(Data + j);
//		//	if (Image->Data[i + 3] == 0)
//		//	{
//		//		memcpy(Color, &AlphaColor, 3);
//		//	}
//		//	else
//		{
//			Data[j] = rgba_data[i];
//			Data[j + 1] = rgba_data[i + 1];
//			Data[j + 2] = rgba_data[i + 2];
//		}
//
//		j += 3;
//	}


    // Compress each 4x4 block of the image using BC1/DXT1
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            const int block_index = (y / 4) * (width / 4) + (x / 4);
            unsigned char* block = *bc1_data + block_index * block_size;

        //    stb_compress_bc4_block(block, rgba_data + (y * width + x) * 4);
            stb_compress_dxt_block(block, Data + (y * width + x) * 3, 0, STB_DXT_NORMAL);
        }
    }

	free(Data);
	*/

	return true;
}