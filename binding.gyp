{
    "targets": [{
        "target_name": "pastec",
        "include_dirs": [
            "deps/pastec/include",
            "deps/pastec/include/orb"
        ],
        "sources": [
            "src/node.cpp",
            "src/pastec.cpp"
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
                        "DisableSpecificWarnings": [ "4530", "4506", "4244" ],
                    },
                }
            }],
            [ # cflags on OS X are stupid and have to be defined like this
                "OS==\"mac\"", {
                    "xcode_settings": {
                    "OTHER_CFLAGS": [
                        "-mmacosx-version-min=10.7",
                        "-std=c++11",
                        "-stdlib=libc++",
                        "<!@(pkg-config --cflags opencv)"
                    ],
                    "GCC_ENABLE_CPP_RTTI": "YES",
                    "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
                }
            }]
        ],
        "dependencies": [
            "deps/pastec.gyp:pastec-c"
        ],
    }]
}
