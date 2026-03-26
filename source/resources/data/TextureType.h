#pragma once
enum class TextureType {
    Albedo,        // sRGB
    Normal,        // linear
	AO,			   // linear
    ORM,           // linear packed
    Emissive,      // usually linear
    Height,        // linear
    Mask,          // linear
    Unknown
};
