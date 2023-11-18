/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace Resources
{
    extern const char*   null_png;
    const int            null_pngSize = 857;

    extern const char*   zero_png;
    const int            zero_pngSize = 264116;

    extern const char*   one_png;
    const int            one_pngSize = 238783;

    extern const char*   two_png;
    const int            two_pngSize = 302507;

    extern const char*   three_png;
    const int            three_pngSize = 264778;

    extern const char*   four_png;
    const int            four_pngSize = 250870;

    extern const char*   five_png;
    const int            five_pngSize = 253158;

    extern const char*   six_png;
    const int            six_pngSize = 243929;

    extern const char*   seven_png;
    const int            seven_pngSize = 228619;

    extern const char*   eight_png;
    const int            eight_pngSize = 267331;

    extern const char*   January_png;
    const int            January_pngSize = 102001;

    extern const char*   February_png;
    const int            February_pngSize = 106204;

    extern const char*   March_png;
    const int            March_pngSize = 92413;

    extern const char*   April_png;
    const int            April_pngSize = 99082;

    extern const char*   May_png;
    const int            May_pngSize = 97520;

    extern const char*   June_png;
    const int            June_pngSize = 102866;

    extern const char*   July_png;
    const int            July_pngSize = 101773;

    extern const char*   August_png;
    const int            August_pngSize = 83910;

    extern const char*   September_png;
    const int            September_pngSize = 89375;

    extern const char*   October_png;
    const int            October_pngSize = 91406;

    extern const char*   November_png;
    const int            November_pngSize = 99845;

    extern const char*   December_png;
    const int            December_pngSize = 105086;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 22;

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
