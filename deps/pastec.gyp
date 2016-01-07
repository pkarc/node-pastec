{
    "targets": [{
        "target_name": 'pastec-c',
        "type": "static_library",
        "direct_dependent_settings": {
            "include_dirs": [ "." ],
        },
        "sources": [
            "./pastec/src/imageloader.cpp",
            "./pastec/src/imagereranker.cpp",
            "./pastec/src/imagererankerransac.cpp",
            "./pastec/src/orb/orbfeatureextractor.cpp",
            "./pastec/src/orb/orbindex.cpp",
            "./pastec/src/orb/orbwordindex.cpp",
            "./pastec/src/orb/orbsearcher.cpp",
        ],
        "libraries": [
            "<!@(pkg-config --libs opencv)"
        ],
        "cflags!" : [ "-fno-exceptions"],
        "cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"]
    }]
}
