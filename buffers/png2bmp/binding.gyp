{
    "targets": [
        {
            "target_name": "png2bmp",
            "sources": ["./cpp/png2bmp.cpp", "./cpp/lodepng.cpp"],
            "cflags": ["-Wall", "-Wextra", "-pedantic", "-ansi", "-O3"],
            "include_dirs" : ["<!(node -e \"require('nan')\")"]
        }
    ]
}