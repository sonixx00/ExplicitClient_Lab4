#include <iostream>
#include <fstream>
#include <windows.h>

using ChecksumFn = long long (*)(const char*);
using VerifyFn = int (*)(const char*, long long);
using SizeFn = long long (*)(const char*);
using CompareFn = int (*)(const char*, const char*);

int main()
{
    std::cout << "=== EXPLICIT LINKING TEST ===\n\n";

    HMODULE dll = LoadLibraryW(L"FileIntegrityDynamic.dll");

    if (!dll)
    {
        std::cout << "DLL loading failed\n";
        return 1;
    }

    std::cout << "DLL loaded successfully\n\n";

    auto checksum = reinterpret_cast<ChecksumFn>(
        GetProcAddress(dll, "checksum"));

    auto verifyChecksum = reinterpret_cast<VerifyFn>(
        GetProcAddress(dll, "verifyChecksum"));

    auto fileSize = reinterpret_cast<SizeFn>(
        GetProcAddress(dll, "fileSize"));

    auto compareFiles = reinterpret_cast<CompareFn>(
        GetProcAddress(dll, "compareFiles"));

    if (!checksum or !verifyChecksum or
        !fileSize or !compareFiles)
    {
        std::cout << "Function loading failed\n";
        FreeLibrary(dll);
        return 1;
    }

    std::ofstream("file1.txt") << "123456789";
    std::ofstream("file2.txt") << "123456789";
    std::ofstream("file3.txt") << "Different data";
    std::ofstream("empty.txt").close();

    int passed = 0;
    int failed = 0;

    auto test = [&](const char* name, bool result)
        {
            std::cout << name << ": "
                << (result ? "PASS" : "FAIL") << '\n';

            if (result)
                passed++;
            else
                failed++;
        };

    test("CRC32 checksum",
        checksum("file1.txt") == 3421780262LL);

    test("Correct checksum",
        verifyChecksum("file1.txt", 3421780262LL) == 1);

    test("Incorrect checksum",
        verifyChecksum("file1.txt", 123) == 0);

    test("File size", fileSize("file1.txt") == 9);

    test("Identical files",
        compareFiles("file1.txt", "file2.txt") == 1);

    test("Different files",
        compareFiles("file1.txt", "file3.txt") == 0);

    test("Missing file", checksum("missing.txt") == -1);

    test("Null pointer", checksum(nullptr) == -1);

    test("Empty file checksum", checksum("empty.txt") == 0);

    test("Empty file size", fileSize("empty.txt") == 0);

    test("Invalid checksum",
        verifyChecksum("file1.txt", -5) == -1);

    std::cout << "\nPassed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';

    if (!FreeLibrary(dll))
    {
        std::cout << "DLL unloading failed\n";
        return 1;
    }

    std::cout << "DLL unloaded successfully\n";

    return failed == 0 ? 0 : 1;
}