{
  "targets": [{
    "target_name": "pastec-c",
    "type": "static_library",
    "include_dirs": [
      "./pastec/src/orb",
      "./pastec/include",
      "./pastec/include/orb"
    ],
    "sources": [
      "./pastec/src/imageloader.cpp",
      "./pastec/src/imagereranker.cpp",
      "./pastec/src/imagererankerransac.cpp",
      "./pastec/src/orb/orbfeatureextractor.cpp",
      "./pastec/src/orb/orbindex.cpp",
      "./pastec/src/orb/orbwordindex.cpp",
      "./pastec/src/orb/orbsearcher.cpp"
    ],
    "conditions": [
      [ "OS==\"linux\"", {
        "libraries": [
          "<!@(pkg-config --libs opencv)"
        ],
        "cflags!" : [
          "-fno-exceptions"
        ],
        "cflags_cc!": [
          "-fno-rtti",
          "-fno-exceptions"
        ]
      }],
      [ "OS==\"win\"", {
        "cflags": [
          "-Wall"
        ],
        "defines": [
          "WIN"
        ],
        "msvs_settings": {
          "VCCLCompilerTool": {
            "ExceptionHandling": "2",
            "DisableSpecificWarnings": [ "4530", "4506", "4244" ]
          }
        }
      }],
      [ "OS==\"mac\"", {
        "xcode_settings": {
          "OTHER_CFLAGS": [
            "-mmacosx-version-min=10.7",
            "-std=c++11",
            "-stdlib=libc++",
            "<!@(pkg-config --cflags opencv)"
          ],
          "GCC_ENABLE_CPP_RTTI": "YES",
          "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
        },
        "libraries": [
          "<!@(pkg-config --libs opencv)"
        ]
      }]
    ]
  }]
}
