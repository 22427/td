td - TextureDatahttps://github.com/22427/td/subscription
======================================================
This is a small library/tool containing all features to convert images (png, bmp, tga, ...) into decent looking textures
in formats supported by OpenGL|ES 2.0.
The supported formats are 
- UNSIGNED_SHORT_5_6_5
- UNSIGNED_SHORT_4_4_4_4
- UNSIGNED_SHORT_5_5_5_1
- UNSIGNED_BYTE (1-4 channel)

Dithering
-------------------------------------------------------
In order to compensate for color-banding effects a simple Floyd-Steinberg-Dithering is applied to the images during
quantization. This way even UNSIGNED_SHORT_4_4_4_4 generates well looking textures.

![Images][ex]

MipMaps
------------------------------------------------------
td can also create the corresponding MipMap-levels, applying the dithering for each level individually.
![Texture with MipMap-levels using 4444][mip_maps]

FileFormat
------------------------------------------------------
The .td format is a simple binary dump of the textures data (including mip-map-levels).
Neither compression nor handling for endianness is implemented yet.


td uses the libaries stb_image to load, stb_image_write to store images as well as  stb_image_resize to generate MipMap-levels.


[mip_maps]: https://github.com/22427/td/blob/master/examples/4444_mip_mapping.png "MipMapLevels using 4444" 
[ex]: https://github.com/22427/td/blob/master/examples/examples.png ""
