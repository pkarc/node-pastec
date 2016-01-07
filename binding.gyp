{
    "targets": [{
        "target_name": "pastec",
        "sources": [
            "src/node.cpp",
            "src/pastec.cpp"
        ],
        "libraries": [
            "<!@(pkg-config --libs opencv)"
        ],
        "dependencies": [
            "deps/pastec.gyp:pastec-c"
        ],
    }]
}
