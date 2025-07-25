cmake_minimum_required(VERSION 3.4.1)
project("ddgcrypto")

macro(add_sodium_arch _KEY _VALUE)
    set("SODIUM_ARCH_${_KEY}" "${_VALUE}")
endmacro(add_sodium_arch)

add_sodium_arch("armeabi-v7a" "armeabi-v7a")
add_sodium_arch("arm64-v8a" "arm64-v8a")
add_sodium_arch("x86" "x86")
add_sodium_arch("x86_64" "x86_64")
set(SODIUM_ARCH "${SODIUM_ARCH_${ANDROID_ABI}}")

add_library(sodium STATIC IMPORTED)
set_property(TARGET sodium PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../native_lib/third-party/android.${SODIUM_ARCH}/libsodium.a")

add_library( # Sets the name of the library.
        ddgcrypto
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ddg-sync-jni.c
        ../../../../../native_lib/DDGSyncCrypto.c
        ../../../../../native_lib/DDGSyncCrypto.h
        )
target_include_directories(ddgcrypto PRIVATE ${CMAKE_SOURCE_DIR}/../../../../../native_lib/third-party/android.${SODIUM_ARCH}/include)
target_link_libraries(ddgcrypto
    -Wl,--whole-archive sodium -Wl,--no-whole-archive
)


# Make sure ddgcrypto is 16KB compatible
target_link_options(
  ddgcrypto
  PRIVATE
  "-Wl,-z,common-page-size=16384"
  "-Wl,-z,max-page-size=16384")
