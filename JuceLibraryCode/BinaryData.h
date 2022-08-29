/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   loud_w_png;
    const int            loud_w_pngSize = 18951;

    extern const char*   soft_w_png;
    const int            soft_w_pngSize = 10190;

    extern const char*   treble_w_png;
    const int            treble_w_pngSize = 24020;

    extern const char*   play_png;
    const int            play_pngSize = 14359;

    extern const char*   bass_w_png;
    const int            bass_w_pngSize = 1615;

    extern const char*   tohp_logo_pngCopy1_png;
    const int            tohp_logo_pngCopy1_pngSize = 14736;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
