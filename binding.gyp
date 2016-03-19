{
    "targets": [{
        "target_name": "node-pastec",
        "include_dirs": [
            "<!(node -e \"require('nan')\")",
            "deps/pastec/include",
            "deps/pastec/include/orb"
        ],
        "sources": [
            "src/pastec.cc"
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
        ],
        "dependencies": [
            "deps/pastec.gyp:pastec-c"
        ]
    }]
}
