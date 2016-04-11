{
    "variables" : {
		"boost_dir": "D:/boost_1_60_0",
	},
    "targets": [{
        "target_name": "node-pastec",
        "sources": [
            "src/node.cpp",
            "src/pastec.cpp"
        ],
        "libraries": [
            "<!@(node utils/find-opencv.js --libs)"
        ],
        "include_dirs": [
            "<!@(node utils/find-opencv.js --cflags)",
            "deps/pastec/include",
            "deps/pastec/include/orb"
        ],
        "cflags!" : [ "-fno-exceptions"],
        "cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"],
        "conditions": [
            [ "OS==\"linux\"", {
                "cflags": [
                    "<!@(pkg-config --cflags \"opencv >= 2.3.1\" )",
                    "-Wall"
                ]
            }],
            [ "OS==\"win\"", {
                "include_dirs": [
                    "<(boost_dir)"
                ],
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
                }
            }],
        ],
        "dependencies": [
            "deps/pastec.gyp:pastec-c"
        ],
    }]
}
